#include <Game.hpp>
#include <EIF.hpp>

// Характерен для умеренных мелодий без явных басов
void GameScene::FeatureTriggerMode2(float value, int feature) {
    
    txtDebug.setString("Mode 2");

    if (feature < 20) {

        auto projectile = new ProjectileLazer(feature);

        projectile->startPos = player.getPosition();
        projectile->startAngleDeg = Rotator + feature * 3;
        projectile->rotationSpeed = feature % 7 - 3;
        projectile->size = feature == 10 ? 100 : 10;

        projectile->Build();
        Projectiles.push_back(projectile);
    }
    else if (feature < 55) {

        // Прыгающие круги для ритма
        float angleRad = (-Rotator / 50.f + feature * 2.f + 90 + (int(value) % 30 - 15) + (feature % 2) * 180) * 3.14159265f / 180.0f;
        float distance = barrierRadius + 200.0f;

        sf::Vector2f st(
            barrierCenter.x + distance * cos(angleRad),
            barrierCenter.y + distance * sin(angleRad)
        );
        sf::Vector2f positiong(
            barrierCenter.x + distance * cos(angleRad) * 4.f,
            barrierCenter.y + distance * sin(angleRad) * 4.f
        );

        // Направление
        sf::Vector2f direction = (player.getPosition() + barrierCenter) * 0.5f - st;

        float length = sqrt(direction.x * direction.x + direction.y * direction.y);
        sf::Vector2f velocity = (direction) * (850.f / length);

        auto projectile = new ProjectileRound(feature);

        projectile->color = sf::Color(feature % 2 == 0 ? 255 : 0, (feature - 100) * 10, feature % 2 == 1 ? 255 : 0);
        projectile->startPos = st;
        projectile->gravityTargetPoint = positiong;
        projectile->speed = velocity;
        projectile->radius = (feature % 20 + 6) * 2.45;

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
        projectile->radius = 47.0f + (feature % 25);
        projectile->startAngleDeg = Rotator * 4 + feature;

        projectile->Build();
        Projectiles.push_back(projectile);
    }
    else if (feature < 120) {

        // Пятиугольники для большего веселья
        float x = windowWidth / 2 - (feature - 95) * 39;
        float y = windowHeight - 40.f + (feature % 50) * 4.0f;

        sf::Vector2f toPlayer = player.getPosition() - sf::Vector2f(x, y);
        sf::Vector2f velocity = (toPlayer * float(1.f / sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y))) * 600.0f;

        sf::Vector2f speed = sf::Vector2f(feature % 20 - 10, -700) * (feature / 71.0f);

        auto projectile = new ProjectilePentagon(feature);

        projectile->startPos = sf::Vector2f(x, y);
        projectile->speed = (speed * 0.7f + velocity * 0.3f);
        projectile->radius = 27.0f + (feature % 15);
        projectile->startAngleDeg = Rotator * 4 + feature;

        projectile->Build();
        Projectiles.push_back(projectile);
    }
    else {
        // красно синие
        ProjectileTriangle* projectile = new ProjectileTriangle(feature);

        sf::Vector2f start = Around(barrierCenter, feature / 256.f * 360.f, barrierRadius + 180.0f);

        projectile->startPos = start;
        projectile->speed = Towards(start, player.getPosition(), 240);
        projectile->startAngleDeg = feature / 256.f * 360.f;
        projectile->color = sf::Color(feature % 2 == 0 ? 255 : 0, (feature - 100) * 10, feature % 2 == 1 ? 255 : 0);
        projectile->radius = 8 * 4;

        projectile->Build();
        Projectiles.push_back(projectile);
    }
}