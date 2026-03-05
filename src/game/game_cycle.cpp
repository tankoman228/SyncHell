#include <Game.hpp>
#include <EIF.hpp>

char strFormatBuf[64]; // ТОЛЬКО ДЛЯ ОСНОВНОГО ПОТОКА, МЕЛКИЕ СТРОКИ

void GameScene::ProjectilesCycle(float t) {

    int spawnDamageAura = 0;
    int spawnHealAura = 0;

    for (int i = Projectiles.size() - 1; i >= 0; i--)
    {
        if (Projectiles[i]->lifeTime > PROJECTILES_LIFE_TIME)
        {
            delete Projectiles[i]; 
            Projectiles.erase(Projectiles.begin() + i);

            continue;
        }
        else if (Projectiles[i]->isCollidable && HasCollision(player, Projectiles[i]->shape))
        {
            if (Projectiles[i]->damage < 0) {
                spawnHealAura++;
                playerShield = 64;
                playerHealth -= Projectiles[i]->damage;
                if (playerHealth > 255) playerHealth = 255;
            }
            else 
            {
                spawnDamageAura++;
                playerShield -= Projectiles[i]->damage;
                if (playerShield < 0) {
                    playerHealth += playerShield;
                    playerShield = 0;
                }
            }

            delete Projectiles[i]; 
            Projectiles.erase(Projectiles.begin() + i);

            continue;
        }
        else if (Projectiles[i]->lifeTime < 0.3f) {
            Projectiles[i]->shape.setOutlineThickness((0.4f - Projectiles[i]->lifeTime) * 3.f);
        }
        else Projectiles[i]->shape.setOutlineThickness(0.f);

        Projectiles[i]->Cycle(t);
    }

    // Чтобы перераспределение памяти не привело к ошыбке
    for (int i = 0; i < spawnDamageAura; i++) {
        Projectiles.push_back(new ProjectileAura(player, 255, 10, 10, 0.1f));
    }
    for (int i = 0; i < spawnHealAura; i++) {
        Projectiles.push_back(new ProjectileAura(player, 10, 255, 10, 0.5f));
    }
}

void GameScene::FeaturesCycleResolve(float t) {

    if (awaitingRestart) return; // при остановленной музыке проверок быть не должно

    for (int feature = 0; feature < 256; feature++)
    {
        float value = EIF::OUT_Etalon[feature];
        FeatureTrigger(value, feature);
    }
}

sf::RenderTexture bufferA, bufferB;
sf::RenderTexture* prevFrame;
sf::RenderTexture* nextFrame;
bool buffersInitialized = false;

void GameScene::VisualCycle(float t) {

    // барьер поля
    barrier.setOutlineColor(sf::Color(255, 0, 254.f));
    barrier.setRadius(barrierRadius);
    barrier.setOrigin(barrierRadius, barrierRadius); // Центрируем
    barrier.setPosition(barrierCenter);

    // шкалы
    healthbar.setFillColor(sf::Color(255 - playerHealth, playerHealth, 0));
    healthbar.setScale(playerHealth / 255.f, 1);
    shielbar.setScale(playerShield / 64.f, 1);

    // фон
    {
        if (!buffersInitialized) {
            auto size = window->getSize();
            bufferA.create(size.x, size.y);
            bufferB.create(size.x, size.y);
            prevFrame = &bufferA;
            nextFrame = &bufferB;
            buffersInitialized = true;
        }

        // В цикле:
        nextFrame->clear();
        shader.setUniform("previousTexture", prevFrame->getTexture()); // Передаем старый кадр
        shader.setUniformArray("spectrum", &EIF::OUT_Etalon[0], 256);
        shader.setUniform("deltaTime", t);
        static float tt = 0;
        tt += t;
        shader.setUniform("time", tt);

        nextFrame->clear();
        nextFrame->draw(background, &shader);
        nextFrame->display();

        sf::Sprite resultSprite(nextFrame->getTexture());
        window->draw(resultSprite);

        std::swap(prevFrame, nextFrame);
    }
    window->draw(barrier);

    // снаряды
    for (int i = Projectiles.size() - 1; i >= 0; i--)
    {
        window->draw(Projectiles[i]->shape);
    }

    // счётчик FPS
    snprintf(strFormatBuf, sizeof(strFormatBuf), "%d FPS", int(1.f / t));
    txtFpsCounter.setString(strFormatBuf);

    // Отрисовка
    window->draw(player);
    window->draw(healthbar);
    window->draw(shielbar);
    window->draw(txtFpsCounter);
    window->draw(txtProgress);
    window->draw(txtDebug);
}

const int delayBeforeRestartSeconds = 5;
float delayBeforeRestartCounter = 9999; // время до рестарта уровня, при активации в delayBeforeRestartSeconds частично останавливает игру

void GameScene::Cycle(float t) {

    HandleInput(t); // WASD

    // Вычисляем текущий индекс и прогоняем рецепторы (реальное время)
    int rawSoundIndexEnd = std::min((long)rawSound.size() - 1, (long)(music.getPlayingOffset().asSeconds() * 44100.f));
    if (rawSoundPrevIndex < rawSoundIndexEnd) EIF::Cycle(&rawSound, rawSoundPrevIndex + 1, rawSoundIndexEnd);
    rawSoundPrevIndex = rawSoundIndexEnd;

    // логика переключения режимой
    { 
        // каждую секунду накапливается энергия в этих параметрах
        static double 
            ambience = 0,  // энергия из фона (не пиков)
            diversity = 0, // от количества пиков, а не их размера
            tension = 0,   // суммарный объём пиков
            dynamics = 0,  // суммарная дельта по рецепторам за секунду
            
            timer = 0;
        static float prev[256] = {0};

        bool alreadyTriggered = false; 
        for (int i = 0; i < 256; i++) {
            float value = EIF::OUT_Etalon[i]; // значенеи с музыкального рецептора

            if (value > 220.f) {
                // чтобы подсчитать количество пиков надо найти те моменты, когда есть "яркая вспышка"
                // считаются именно очень яркие пики. Полосатость, тигринный коэффициент, его бы довести до ума
                if (!alreadyTriggered) {
                    diversity += t * 64.f;
                    alreadyTriggered = true;
                }
                tension += std::sqrt(value) * t;
            }
            else {
                alreadyTriggered = false; 
                ambience += std::sqrt(value) * t; // насыщеннее звук = больше чувствуется фон, больше шумов
            }

            dynamics += std::abs(prev[i] - value) * t;
            prev[i] = value;
        }

        timer += t;
        if (timer > 1.f) {

            ambience  /= 220 * 16; 
            diversity /= 32 * 256;
            tension   /= 16 * 256;
            dynamics  /= 256 * 128;

            std::cout << ambience << "\t" << diversity << "\t" << tension << "\t" << dynamics << "\n";

            ambience = 0; 
            diversity = 0; 
            tension = 0; 
            dynamics = 0;
            timer = 0;
        }
    }

    // Теперь выбираем метод, который будет квадрилион раз в секунду вызывать. Это "режим" игры, т.е. как именно снаряды будут спанвиться
    {

        //bool TEN_SMALL = tension   < (avgTension   + maxTension  ) / 2.f;
        //bool DIV_SMALL = diversity < (avgDivercity + maxDivercity) / 2.f;
        //bool AMB_SMALL = ambience  < (avgAmbience  + maxAmbience ) / 2.f;


        /*
        
        // Режим не зависит от того, какая там мелодия, если только не совсем всё тихо. Главное - поймать дроп
        mode += (fmaxf(0, tension    - avgTension)    / (maxTension    - avgTension))    * 33.f;
        mode += (fmaxf(0, diversity  - avgDivercity)  / (maxDivercity  - avgDivercity))  * 33.f;
        mode += (fmaxf(0, ambience   - avgAmbience)   / (maxAmbience   - avgAmbience))   * 34.f;

        float t = mode / 100.f; // 0..1
        float result = std::pow(t, 0.01) * 7.f; // 0..7, но возрастает иначе. Да, 1^? = 1, степень позволит снижать
        */

        switch (0) // TODO: я так и не смог придумать, как правильно выбирать режимы
        {
            case 0: FeatureTriggerCurrentMode = &GameScene::FeatureTriggerMode0; break;
            case 1: FeatureTriggerCurrentMode = &GameScene::FeatureTriggerMode1; break;
            case 2: FeatureTriggerCurrentMode = &GameScene::FeatureTriggerMode2; break;
            case 3: FeatureTriggerCurrentMode = &GameScene::FeatureTriggerMode3; break;
            case 4: FeatureTriggerCurrentMode = &GameScene::FeatureTriggerMode4; break;
            case 5: FeatureTriggerCurrentMode = &GameScene::FeatureTriggerMode5; break; 
            case 6: FeatureTriggerCurrentMode = &GameScene::FeatureTriggerMode6; break;
            case 7: FeatureTriggerCurrentMode = &GameScene::FeatureTriggerMode7; break;
        }
        /*
        if (TEN_SMALL) {
            if (DIV_SMALL) {
                if (AMB_SMALL) {
                    FeatureTriggerCurrentMode = &GameScene::FeatureTriggerMode0;
                }
                else {
                    FeatureTriggerCurrentMode = &GameScene::FeatureTriggerMode1;
                }
            }
            else {
                if (AMB_SMALL) {
                    FeatureTriggerCurrentMode = &GameScene::FeatureTriggerMode2;
                }
                else {
                    FeatureTriggerCurrentMode = &GameScene::FeatureTriggerMode3;
                }
            }
        }
        else {
            if (DIV_SMALL) {
                if (AMB_SMALL) {
                    FeatureTriggerCurrentMode = &GameScene::FeatureTriggerMode4;
                }
                else {
                    FeatureTriggerCurrentMode = &GameScene::FeatureTriggerMode5;
                }
            }
            else {
                if (AMB_SMALL) {
                    FeatureTriggerCurrentMode = &GameScene::FeatureTriggerMode6;
                }
                else {
                    FeatureTriggerCurrentMode = &GameScene::FeatureTriggerMode7;
                }
            }
        }*/
    }


    FeaturesCycleResolve(t); // спавнит снаряды, но только когда проверит, что динамика трека подходит
    ProjectilesCycle(t);     // цикл самих снарядов, их деспавна и движения

    // Просто изменения динамических параметров
    healingReload -= t * 1.5;

    // если не запущен счётчик ожидания рестарта уровня
    if (!awaitingRestart) {

        if (rawSoundIndexEnd != 0) {
            snprintf(strFormatBuf, sizeof(strFormatBuf), "%d%%", int(double(rawSoundIndexEnd) / double(rawSound.size()) * 100.0f) + 1); // а то 99 из-за округлений остаётся
            txtProgress.setString(strFormatBuf);
        }

        if (playerHealth < 0) {
            delayBeforeRestartCounter = delayBeforeRestartSeconds - 0.1f;
            player.setPosition(window->getSize().x * 2.f, window->getSize().y * 2.f); // просто спрячем за карту
            awaitingRestart = true;
            
            std::cout << "Player is dead, await restart\n";
        }
        else if (playerShield < 64) {
            playerShield += t * 20;
            playerShield = playerShield > 64 ? 64 : playerShield;
        }
    }
    else {
        playerHealth = 0;
        playerShield = 0;

        std::cout << "restart await counter: " << delayBeforeRestartCounter << "\n";
        
        delayBeforeRestartCounter -= t;

        if (delayBeforeRestartCounter < 4) {
            music.setVolume(std::max(2.f, delayBeforeRestartCounter * 10.f));
        }

        if (delayBeforeRestartCounter < 0) {

            std::cout << "restart\n";
            delayBeforeRestartCounter = 999999; // деактивация счётчика
            music.setVolume(100);
            SetupLevel(); // реально рестарт уровня

            return;
        }
    }

    VisualCycle(t);
}