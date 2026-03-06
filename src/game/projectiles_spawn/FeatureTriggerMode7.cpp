#include <Game.hpp>
#include <EIF.hpp>

// Пики активности, часто встречается в металле
void GameScene::FeatureTriggerMode7(float value, int feature) {
    
    txtDebug.setString("Mode 7"); // TODO: ревью, написан ИИ

    if (feature < 15) {
        // Массовый спавн лазеров со всех сторон
            float angle = (feature * 360 / 15 + Rotator) * M_PI / 180.0f;
            float distance = 300.0f;

            sf::Vector2f pos(
                barrierCenter.x + distance * cos(angle),
                barrierCenter.y + distance * sin(angle)
            );

            auto projectile = new ProjectileLazer(
                pos,
                (Rotator + feature * 360 / 15),
                8 + feature % 12,
                feature % 10 - 5
            );
            Projectiles.push_back(projectile);
    }
    else if (feature < 40) {
        // Комбо: центр притяжения + взрывные треугольники
        static bool centerSpawned = false;

        if (!centerSpawned && feature == 20) {
            // Создаём спираль в центре как магнита
            auto spiral = new ProjectileSpiralMove(
                barrierCenter,
                sf::Color(255, 0, 0),
                5,
                barrierRadius + 50,
                Rotator
            );
            Projectiles.push_back(spiral);
            centerSpawned = true;
        }

        // Треугольники, летящие в центр
        float baseAngle = (feature * 25.0f) * M_PI / 180.0f;
        float radius = std::max(windowWidth, windowHeight) * 0.8f;

        for (int i = 0; i < 4; i++) {
            float angle = baseAngle + i * M_PI / 2;
            sf::Vector2f pos(
                barrierCenter.x + radius * cos(angle),
                barrierCenter.y + radius * sin(angle)
            );

            sf::Vector2f toCenter = barrierCenter - pos;
            float length = sqrt(toCenter.x * toCenter.x + toCenter.y * toCenter.y);
            sf::Vector2f velocity = (toCenter / length) * 200.0f;

            auto projectile = new ProjectileTriangle(
                pos,
                velocity,
                sf::Color(255, 50, 50),
                atan2(velocity.y, velocity.x) * 180 / M_PI
            );
            projectile->shape.setScale(4.0f, 4.0f);
            Projectiles.push_back(projectile);
        }
    }
    else if (feature < 70) {
        // Хаотичные шестиугольники с переменной скоростью
        for (int i = 0; i < 3; i++) {
            float x = (rand() % (int)windowWidth);
            float y = (rand() % (int)windowHeight);

            sf::Vector2f toPlayer = player.getPosition() - sf::Vector2f(x, y);
            float length = sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);

            // Сумасшедшая скорость
            float speedMultiplier = 400.0f + (feature * 10);
            sf::Vector2f velocity = (toPlayer / length) * speedMultiplier;

            // Добавляем случайное отклонение
            velocity += sf::Vector2f(rand() % 200 - 100, rand() % 200 - 100);

            auto projectile = new ProjectileHexagon(
                sf::Vector2f(x, y),
                velocity,
                15.0f + (feature % 20),
                Rotator * 10 + feature * 20
            );
            Projectiles.push_back(projectile);
        }
    }
    else if (feature < 120) {
        // Волна смерти - несколько зон подряд
        if (feature % 5 == 0) {
            float offset = (feature - 70) * 30;
            sf::Vector2f pos(
                barrierCenter.x - 200 + offset,
                barrierCenter.y - 200 + sin(feature * 0.5f) * 100
            );

            auto projectile = new ProjectileDeathZone(
                pos,
                80 + feature % 40,
                Rotator + feature * 15,
                feature * 2
            );
            Projectiles.push_back(projectile);
        }
    }
    else {
        // ФИНАЛ: мега-спираль с треугольниками
        float baseAngle = (feature * 10.0f) * M_PI / 180.0f;

        // Центральная спираль
        auto mainSpiral = new ProjectileSpiralMove(
            barrierCenter,
            sf::Color(255, 200, 0),
            12,
            barrierRadius + 500,
            baseAngle
        );
        Projectiles.push_back(mainSpiral);

        // Треугольники-спутники
        for (int i = 0; i < 8; i++) {
            float angle = baseAngle + i * M_PI / 4;
            float dist = barrierRadius + 500 + 100 * sin(feature * 0.1f + i);

            sf::Vector2f pos(
                barrierCenter.x + dist * cos(angle),
                barrierCenter.y + dist * sin(angle)
            );

            sf::Vector2f toPlayer = player.getPosition() - pos;
            float length = sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);
            sf::Vector2f velocity = (toPlayer / length) * 400.0f;

            auto projectile = new ProjectileTriangle(
                pos,
                velocity,
                sf::Color(255, 100 + i * 20, 0),
                angle * 180 / M_PI
            );
            projectile->shape.setScale(3.0f, 3.0f);
            Projectiles.push_back(projectile);
        }
    }
}