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

    static float averages[256] = {0};       
    static float averagesDeltasAbs[256] = {0};    

    // TODO: переписать логику появления снарядов по уху
    for (int feature = 0; feature < 256; feature++)
    {
        float value = EIF::OUT_Pendulum[feature];
        float delta = abs(value - averages[feature]);

        // перепад больше обычного, порог снижается с ростом громкости и сложностью
        if (delta > averagesDeltasAbs[feature] * 2.3 / std::pow(currentVolume / 256.f, 6) / difficulty) {
            FeatureTriggered(value, feature);
            tension += 1.6 * (currentVolume / 256.f);
        }
        averages[feature] = averages[feature] * 0.85 + value * 0.15;
        averagesDeltasAbs[feature] = averagesDeltasAbs[feature] * 0.85 + delta * 0.15;
    }
    
    if (tension > 1) {
        tension -= t * tension / 2.f; // за 2 секунды при отсутствии триггеров напряжение упадёт до нуля
    }
}

sf::RenderTexture bufferA, bufferB;
sf::RenderTexture* prevFrame;
sf::RenderTexture* nextFrame;
bool buffersInitialized = false;

void GameScene::VisualCycle(float t) {

    // барьер поля
    barrier.setOutlineColor(sf::Color(255, 0, 255.f - currentVolume));
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
        shader.setUniformArray("spectrum", &EIF::OUT_Pendulum[0], 256);
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
}

const int delayBeforeRestartSeconds = 5;
float delayBeforeRestartCounter = 9999; // время до рестарта уровня, при активации в delayBeforeRestartSeconds частично останавливает игру

void GameScene::Cycle(float t) {

    HandleInput(t); // WASD

    // Вычисляем текущий индекс
    int rawSoundIndexEnd = std::min(rawSound.size() - 1, ulong(music.getPlayingOffset().asSeconds() * 44100.f));
    if (rawSoundPrevIndex < rawSoundIndexEnd) EIF::Cycle(&rawSound, rawSoundPrevIndex + 1, rawSoundIndexEnd);
    rawSoundPrevIndex = rawSoundIndexEnd;
    std::cout << EIF::OUT_Pendulum[20] << "\n";

    currentVolume = 128.f; // TODO: прописать
    
    FeaturesCycleResolve(t); // спавнит снаряды, но только когда проверит, что динамика трека подходит
    ProjectilesCycle(t);     // цикл самих снарядов, их деспавна и движения

    // Просто изменения динамических параметров
    angleAtack += t * 9;
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