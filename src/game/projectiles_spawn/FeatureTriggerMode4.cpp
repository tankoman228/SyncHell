#include <Game/Game.hpp>
#include <Analysis/EIF.hpp>

// Активность средняя, есть и басы, и другое. Умеренный шум
void GameScene::FeatureTriggerMode4(float value, int feature) {

    txtDebug.setString("Mode 4");

    if (feature < 20) {

        static bool alreadySpawned;
        if (feature == 0) alreadySpawned = 0;
        else if (alreadySpawned) goto pentagons; // защита от спама зонами смерти
        alreadySpawned = 1;

        auto projectile = new ProjectileDeathZone(feature);
        projectile->startPos = player.getPosition();
        projectile->width = feature * 2 + 60;
        projectile->startAngleDeg = Rotator + feature * 10;
        projectile->damage = feature + 50;

        projectile->Build();
        Projectiles.push_back(projectile);
    }
    else if (feature < 55) {
        
        auto projectile = new ProjectileLazer(feature);

        projectile->startPos = player.getPosition();
        projectile->startAngleDeg = Rotator + feature;
        projectile->rotationSpeed = feature % 7 - 3;
        projectile->size = 100;

        projectile->Build();
        Projectiles.push_back(projectile);
    }
    else if (feature < 70) { // 55-69

        // Шестиугольники для того, чтобы жизнь малиной не казалась
        float y = windowHeight / 2 - (feature - 62) * 30;
        float x = -40.f + (feature % 50) * 4.0f;

        sf::Vector2f toPlayer = player.getPosition() - sf::Vector2f(x, y);
        sf::Vector2f velocity = (toPlayer * float(1.f / sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y))) * 600.0f;

        sf::Vector2f speed = sf::Vector2f(feature % 20 - 10 + 1200, feature % 20 + 10) * (feature / 61.0f);

        auto projectile = new ProjectileHexagon(feature);

        projectile->startPos = sf::Vector2f(x, y);
        projectile->speed = (speed * 0.7f + velocity * 0.3f);
        projectile->radius = 17.0f + (feature % 25);
        projectile->startAngleDeg = Rotator * 4 + feature;

        projectile->Build();
        Projectiles.push_back(projectile);
    }
    else if (feature < 120) {

        pentagons:

        // Пятиугольники для большего веселья
        float x = windowWidth / 2 - (feature - 95) * 39;
        float y = windowHeight - 40.f + (feature % 50) * 4.0f;

        sf::Vector2f toPlayer = player.getPosition() - sf::Vector2f(x, y);
        sf::Vector2f velocity = (toPlayer * float(1.0 / sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y))) * 600.0f;

        sf::Vector2f speed = sf::Vector2f(feature % 20 - 10, -700) * (feature / 71.0f);

        auto projectile = new ProjectilePentagon(feature);

        projectile->startPos = sf::Vector2f(x, y);
        projectile->speed = speed * 0.7f + velocity * 0.3f;
        projectile->radius = 27.0f + (feature % 15);
        projectile->startAngleDeg = Rotator * 4 + feature;

        projectile->Build();
        Projectiles.push_back(projectile);
    }
    else {
        // узкий поток
        float angleRad = (feature / 256.f * 60.f) * 3.14159265f / 180.0f;
        float distance = barrierRadius + 180.0f;

        sf::Vector2f st(
            barrierCenter.x + distance * cos(angleRad),
            barrierCenter.y + distance * sin(angleRad)
        );

        // Направление
        sf::Vector2f direction = player.getPosition() - st;

        float length = sqrt(direction.x * direction.x + direction.y * direction.y);
        sf::Vector2f velocity = (direction / length) * 240.f;

        auto projectile = new ProjectileTriangle(feature);
        projectile->startPos = st;
        projectile->speed = velocity;
        projectile->startAngleDeg = feature / 256.f * 360.f;
        projectile->color = sf::Color(feature % 2 == 0 ? 255 : 0, (feature - 100) * 10, feature % 2 == 1 ? 255 : 0);
        projectile->radius = 7 * 4;

        projectile->Build();
        Projectiles.push_back(projectile);
    }
}