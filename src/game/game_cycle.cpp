#include <Game.hpp>
#include <EIF.hpp>

char strFormatBuf[64]; // ТОЛЬКО ДЛЯ ОСНОВНОГО ПОТОКА, МЕЛКИЕ СТРОКИ

void GameScene::ProjectilesCycle() {

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

        Projectiles[i]->Cycle(dt);
    }

    // Чтобы перераспределение памяти не привело к ошыбке
    for (int i = 0; i < spawnDamageAura; i++) {
        Projectiles.push_back(new ProjectileAura(player, 255, 10, 10, 0.1f));
    }
    for (int i = 0; i < spawnHealAura; i++) {
        Projectiles.push_back(new ProjectileAura(player, 10, 255, 10, 0.5f));
    }
}

void GameScene::FeaturesCycleResolve() {

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

void GameScene::VisualCycle() {

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
        shader.setUniform("deltaTime", dt);
        static float tt = 0;
        tt += dt;
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
    snprintf(strFormatBuf, sizeof(strFormatBuf), "%d FPS", int(1.f / dt));
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

void GameScene::Cycle(float dt) {

    this->dt = dt;
    HandleInput(); // WASD

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
            dynamics = 0,  // суммарная дельта по рецепторам за секунду
            bass = 0,      // суммарная энергия по рецепторам баса
            melody = 0,    // суммарная энергия поделить на энергию баса
            
            timer = 0;
        static float prev[256] = {0};

        // Мамой клянусь, код в этом блоке никто, кроме меня не писао, нейронок не использовал
        // это мой личный говнокодик. ЧТЕНИЕ ИЛИ ПОПЫТКА ИЗМЕНИТЬ ДАННЫЙ КУСОК КОДА ОПАСНА ДЛЯ ПСИХИКИ

        bool alreadyTriggered = false; // НЕ ТРОГАТЬ ЛОГИКУ ВООБЩЕ НИКАК! КОЭФФИЦИЕНТЫ ЗАНОВО ПОДГОНЯТЬ ПРИДЁТСЯ
        for (int i = 0; i < 256; i++) {
            float value = EIF::OUT_Etalon[i]; // значенеи с музыкального рецептора

            if (value > 230.f) {
                // чтобы подсчитать количество пиков надо найти те моменты, когда есть "яркая вспышка"
                // считаются именно очень яркие пики. Полосатость, тигринный коэффициент, его бы довести до ума
                if (!alreadyTriggered) {
                    diversity += dt * 64.f;
                    alreadyTriggered = true;
                }

                if (i < 64) bass += value * value * dt;
                melody += value * value * dt;
            }
            else {
                alreadyTriggered = false; 
                ambience += std::sqrt(value) * dt; // насыщеннее звук = больше чувствуется фон, больше шумов
            }

            dynamics += std::abs(prev[i] - value) * dt;
            prev[i] = value;
        }

        timer += dt;
        if (timer > 1.f) {

            // Нормализайция
            ambience  /= 220 * 16; 
            diversity /= 32 * 256;
            dynamics  /= 256 * 128;
            
            melody /= bass + 256.f;
            melody = std::sqrt(melody); // иначе в космос улетает
            bass   /= 262144;
            bass = std::sqrt(bass); // иначе в космос улетает

            // LOG
            // std::cout << ambience << "\t" << diversity << "\t" << dynamics << "\t" << bass << "\t" << melody << "\n";

            // Значимость параметра для выбора режима, коэффициенты натягивания совы на глобус
            const float K[5] = { -8.00, -8.00, -8.00, 2.00, -2.00 }; // Считал эмпирически

            // Триггер - значение, напрямую двигающая в сторону нужного режима
            float trigger[5] = { ambience, diversity, dynamics, bass, melody }; // N :)
            for (int i = 0; i < 5; i++) {
                trigger[i] *= K[i];
                trigger[i] = std::min(7.f, trigger[i]);
                trigger[i] = std::max(0.f, trigger[i]);
            }

            // Суммируем триггерящие моменты, это уже почти токен
            float triggerSum = 0;
            for (int i = 0; i < 5; i++) {
                if (trigger[i] > 1.f) triggerSum += trigger[i];
            }
            triggerSum = std::min(7.f, triggerSum);
            triggerSum = std::max(0.f, triggerSum);

            // стабилизирующая компонента, немного сглаживает прыжки, оттягивает от центра внимание
            float mediator = std::pow(triggerSum / 7.f, 0.7355);

            // Выбор режима
            switch ((int)std::round(mediator * triggerSum))
            {
                case 0: 
                    FeatureTriggerCurrentMode = &GameScene::FeatureTriggerMode0; 
                    
                    minTriggeredValue = 253;
                    cooldownTime = 0.6;

                    break;
                case 1: 
                    FeatureTriggerCurrentMode = &GameScene::FeatureTriggerMode1;     
                    
                    minTriggeredValue = 253;
                    cooldownTime = 0.5; 

                    break;
                case 2: 
                    FeatureTriggerCurrentMode = &GameScene::FeatureTriggerMode2;    
                    
                    minTriggeredValue = 254;
                    cooldownTime = 0.9; 

                    break;
                case 3: 
                    FeatureTriggerCurrentMode = &GameScene::FeatureTriggerMode3;     
                    
                    minTriggeredValue = 254;
                    cooldownTime = 0.7; 

                    break;
                case 4: 
                    FeatureTriggerCurrentMode = &GameScene::FeatureTriggerMode4;     
                    
                    minTriggeredValue = 245;
                    cooldownTime = 0.6; 

                    break;
                case 5: 
                    FeatureTriggerCurrentMode = &GameScene::FeatureTriggerMode5;     
                    
                    minTriggeredValue = 253;
                    cooldownTime = 0.7; 

                    break;
                case 6: 
                    FeatureTriggerCurrentMode = &GameScene::FeatureTriggerMode6;     
                    
                    minTriggeredValue = 253;
                    cooldownTime = 0.6; 

                    break;
                case 7: 
                    FeatureTriggerCurrentMode = &GameScene::FeatureTriggerMode7;     
                    
                    minTriggeredValue = 253;
                    cooldownTime = 0.5; 

                    break;
            }

            // Следующая секунда тоже начинает с нуля!
            ambience = 0; 
            diversity = 0; 
            dynamics = 0;
            bass = 0;
            melody = 0;
            timer = 0;
        }
    }


    FeaturesCycleResolve(); // спавнит снаряды, но только когда проверит, что динамика трека подходит
    ProjectilesCycle();     // цикл самих снарядов, их деспавна и движения

    // Просто изменения динамических параметров
    healingReload -= dt;

    if (healingReload < 0) {
        healingReload = 10;

        float Radius = 300.0f;
        float AngleRad = Rotator * (3.14159f / 180.0f);

        Vector2f TangentDir = Vector2f(cos(AngleRad), sin(AngleRad));
        Vector2f RadialDir = Vector2f(-TangentDir.y, TangentDir.x);
        Vector2f ContactPoint = barrierCenter + RadialDir * Radius;

        auto hl = new ProjectileHealingLazer(0.8f * ContactPoint + 0.2f * player.getPosition(), Rotator * 320.f, 8, 0);
        Projectiles.push_back(hl);
    }

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
            playerShield += dt * 20;
            playerShield = playerShield > 64 ? 64 : playerShield;
        }
    }
    else {
        playerHealth = 0;
        playerShield = 0;

        std::cout << "restart await counter: " << delayBeforeRestartCounter << "\n";
        
        delayBeforeRestartCounter -= dt;

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

    VisualCycle();
}