#include <Game.hpp>
#include <EIF.hpp>

// Активность средняя, есть и басы, и другое. Умеренный шум
void GameScene::FeatureTriggerMode4(float value, int feature) {

    txtDebug.setString("Mode 4");

    if (feature < 20) {

        static bool alreadySpawned;
        if (feature == 0) alreadySpawned = 0;
        else if (alreadySpawned) goto pentagons; // защита от спама зонами смерти
        alreadySpawned = 1;

        auto projectile = new ProjectileDeathZone(player.getPosition(), feature + 90, Rotator + feature * 9, feature + 50);
        Projectiles.push_back(projectile);
    }
    else if (feature < 55) {
        
        // Лазеры
        auto projectile = new ProjectileLazer(player.getPosition(), Rotator + feature, 100, feature % 7 - 3);
        Projectiles.push_back(projectile);
    }
    else if (feature < 70) { // 55-69

        // Шестиугольники для того, чтобы жизнь малиной не казалась
        float y = windowHeight / 2 - (feature - 62) * 30;
        float x = -40.f + (feature % 50) * 4.0f;

        sf::Vector2f toPlayer = player.getPosition() - sf::Vector2f(x, y);
        sf::Vector2f velocity = (toPlayer * float(1.f / sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y))) * 600.0f;

        sf::Vector2f speed = sf::Vector2f(feature % 20 - 10 + 1200, feature % 20 + 10) * (feature / 61.0f);

        Projectiles.push_back(new ProjectileHexagon(
            sf::Vector2f(x, y),
            (speed * 0.7f + velocity * 0.3f),
            17.0f + (feature % 25),
            Rotator * 4 + feature
        ));
    }
    else if (feature < 120) {

        pentagons:

        // Пятиугольники для большего веселья
        float x = windowWidth / 2 - (feature - 95) * 39;
        float y = windowHeight - 40.f + (feature % 50) * 4.0f;

        sf::Vector2f toPlayer = player.getPosition() - sf::Vector2f(x, y);
        sf::Vector2f velocity = (toPlayer * float(1.0 / sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y))) * 600.0f;

        sf::Vector2f speed = sf::Vector2f(feature % 20 - 10, -700) * (feature / 71.0f);

        Projectiles.push_back(new ProjectilePentagon(
            sf::Vector2f(x, y),
            (speed * 0.7f + velocity * 0.3f),
            27.0f + (feature % 15),
            Rotator * 4 + feature
        ));
    }
    else {
        // узкий поток
        float angleRad = (feature / 256.f * 60.f) * 3.14159265f / 180.0f;
        float distance = barrierRadius + 180.0f;

        sf::Vector2f position(
            barrierCenter.x + distance * cos(angleRad),
            barrierCenter.y + distance * sin(angleRad)
        );

        // Направление
        sf::Vector2f direction = player.getPosition() - position;

        float length = sqrt(direction.x * direction.x + direction.y * direction.y);
        sf::Vector2f velocity = (direction / length) * 240.f;

        Projectile* projectile = new ProjectileTriangle(position, velocity, sf::Color(feature % 2 == 0 ? 255 : 0, (feature - 100) * 10, feature % 2 == 1 ? 255 : 0), feature / 256.f * 360.f);
        projectile->shape.setScale(2, 2);

        Projectiles.push_back(projectile);
    }
}