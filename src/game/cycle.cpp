#include <Game.hpp>

int currentStfIndex;   // актуальный индекс для фич
char strFormatBuf[32]; // ТОЛЬКО ДЛЯ ОСНОВНОГО ПОТОКА, МЕЛКИЕ СТРОКИ

void GameScene::ProjectilesCycle(float t) {

    for (int i = Projectiles.size() - 1; i >= 0; i--)
    {
        if (Projectiles[i]->lifeTime > PROJECTILES_LIFE_TIME)
        {
            delete Projectiles[i]; 
            Projectiles.erase(Projectiles.begin() + i);
        }
        else if (Projectiles[i]->isCollidable && HasCollision(player, Projectiles[i]->shape))
        {
            if (Projectiles[i]->damage < 0) {
                Projectiles.push_back(new ProjectileAura(player, 10, 255, 10, 0.5f));
                playerShield = 64;
                playerHealth -= Projectiles[i]->damage;
                if (playerHealth > 255) playerHealth = 255;
            }
            else 
            {
                playerShield -= Projectiles[i]->damage;
                if (playerShield < 0) {
                    playerHealth += playerShield;
                    playerShield = 0;
                }
                Projectiles.push_back(new ProjectileAura(player, 255, 10, 10, 0.1f));
            }

            delete Projectiles[i]; 
            Projectiles.erase(Projectiles.begin() + i);
        }
        else if (Projectiles[i]->lifeTime < 0.3f) {
            Projectiles[i]->shape.setOutlineThickness((0.4f - Projectiles[i]->lifeTime) * 3.f);
        }
        else Projectiles[i]->shape.setOutlineThickness(0.f);

        Projectiles[i]->Cycle(t);
        window->draw(Projectiles[i]->shape);
    }
}

void GameScene::FeaturesCycleResolve(float t) {

    if (awaitingRestart) return; // при остановленной музыке проверок быть не должно

    static float averages[256] = {0};       
    static float averagesDeltasAbs[256] = {0};    
    static int prevIndex = 0;
    if (prevIndex != currentStfIndex)
    {
        for (int feature = 0; feature < 20; feature++)
        {
            FeatureTriggered(stf.spectro[currentStfIndex][feature], feature);
        }
        for (int feature = 20; feature < 256; feature++)
        {
            float value = stf.spectro[currentStfIndex][feature];
            float delta = abs(value - averages[feature]);

            // перепад больше обычного, порог снижается с ростом громкости и сложностью
            if (delta > averagesDeltasAbs[feature] * 2.3 / std::pow(currentVolume / 256.f, 6) / difficulty) {
                FeatureTriggered(value, feature);
                tension += 1.6 * (currentVolume / 256.f);
            }
            averages[feature] = averages[feature] * 0.85 + value * 0.15;
            averagesDeltasAbs[feature] = averagesDeltasAbs[feature] * 0.85 + delta * 0.15;
        }
    }
    prevIndex = currentStfIndex;
    if (tension > 1) {
        tension -= t * tension / 2.f; // за 2 секунды при отсутствии триггеров напряжение упадёт до нуля
    }
}

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

    // Прыгающие декорации по краям
    if (decoreR + decoreG + decoreB < 50) {
        decoreG += t * 10.f;
    }
    for (int i = 0; i < 12; i++) {

        float feature = currentStfIndex < stf.timeLength ? stf.spectro[currentStfIndex][240 + i] / bouncyJumpersMaxVals[i] : 0;
        
        // скользящее среднее от текущего масштаба и требуемой высоты. Условие с "?" нужно, чтобы степень не обнуляла колебания на младших диапазонах
        float scaler = (
            (std::pow(feature * 1.7f, 3) + feature * 20.f + 0.2f) * 0.1f 
            + bouncyJumpers->getScale().x * 0.9f);
        
        float shader = (feature / bouncyJumpersMaxVals[i]) * 0.3 + 0.69999f;

        bouncyJumpers[i].setScale(scaler, 1.f);
        bouncyJumpers[i].setFillColor(sf::Color(shader * decoreR, shader * decoreG, shader * decoreB));

        window->draw(bouncyJumpers[i]);
    }

    // счётчик FPS
    snprintf(strFormatBuf, sizeof(strFormatBuf), "%d FPS", int(1.f / t));
    txtFpsCounter.setString(strFormatBuf);

    // Отрисовка
    window->draw(barrier);
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

    // Вычисляем текущий индекс для фич, время STF-граммы
    currentStfIndex = static_cast<int>((music.getPlayingOffset().asSeconds() / musicDurationSeconds) * (stf.timeLength - 1));
    if (currentStfIndex >= stf.timeLength) currentStfIndex = 0; // на всякий случай

    currentVolume = stf.spectro[currentStfIndex][252];
    
    FeaturesCycleResolve(t); // спавнит снаряды, но только когда проверит, что динамика трека подходит
    ProjectilesCycle(t);     // цикл самих снарядов, их деспавна и движения

    // Просто изменения динамических параметров
    angleAtack += t * 9;
    healingReload -= t * std::pow(currentVolume / avgVolume, 3) * 1.5;

    // если не запущен счётчик ожидания рестарта уровня
    if (!awaitingRestart) {

        if (currentStfIndex != 0) {
            snprintf(strFormatBuf, sizeof(strFormatBuf), "%d%%", int(float(currentStfIndex) / float(stf.timeLength) * 100.f) + 1); // а то 99 из-за округлений остаётся
            txtProgress.setString(strFormatBuf);
        }

        if (playerHealth < 0) {
            delayBeforeRestartCounter = delayBeforeRestartSeconds - 0.1f;
            player.setPosition(999999,999999); // просто спрячем за карту
            awaitingRestart = true;
        }
        else if (playerShield < 64) {
            playerShield += t * 20;
            playerShield = playerShield > 64 ? 64 : playerShield;
        }
    }
    else {
        playerHealth = 0;
        playerShield = 0;
        
        delayBeforeRestartCounter -= t;

        if (delayBeforeRestartCounter < 4) {
            music.setVolume(delayBeforeRestartCounter * 10.f);
        }

        if (delayBeforeRestartCounter < 0) {
            delayBeforeRestartCounter = 999999; // деактивация счётчика
            music.setVolume(100);
            SetupLevel(); // реально рестарт уровня
        }
    }

    VisualCycle(t);
}