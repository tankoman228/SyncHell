#include <Game.hpp>
#include <EIF.hpp>

// Характерен для умеренных мелодий без басов
void GameScene::FeatureTriggerMode1(float value, int feature) {
  
    txtDebug.setString("Mode 1");

    if (feature < 55) {

        float angleRad = (-Rotator / 50.f + feature * 5.f + 90 + (int(value) % 30 - 15) + (feature % 2) * 180) * 3.14159265f / 180.0f;
        float distance = barrierRadius + 200.0f;

        sf::Vector2f start(
            barrierCenter.x + distance * cos(angleRad),
            barrierCenter.y + distance * sin(angleRad)
        );
        sf::Vector2f positiong(
            barrierCenter.x + distance * cos(angleRad) * 4.f,
            barrierCenter.y + distance * sin(angleRad) * 4.f
        );

        // Направление
        sf::Vector2f direction = (player.getPosition() + barrierCenter) * 0.5f - start;

        float length = sqrt(direction.x * direction.x + direction.y * direction.y);
        sf::Vector2f velocity = (direction / length) * 850.f;

        auto projectile = new ProjectileRound();

        projectile->color = sf::Color(feature % 2 == 0 ? 255 : 0, (feature - 100) * 10, feature % 2 == 1 ? 255 : 0);
        projectile->startPos = start;
        projectile->gravityTargetPoint = positiong;
        projectile->speed = velocity;
        projectile->radius = (feature % 20 + 6) * 2.45;

        projectile->Build();
        Projectiles.push_back(projectile);
    }
    else if (feature < 70) {

        auto projectile = new ProjectileLazer();

        projectile->startPos      = player.getPosition();
        projectile->startAngleDeg = Rotator + feature;
        projectile->rotationSpeed = feature % 7 - 3;
        projectile->size          = 10;

        projectile->Build();
        Projectiles.push_back(projectile);
    }
    else if (feature < 120) {
        // Пятиугольники для большего веселья
        bool left = feature % 2 == 0;
        float x = left ? -10.0f : windowWidth + 10.0f;
        float y = 50.0f - (feature % 50) * 4.0f;

        sf::Vector2f toPlayer = player.getPosition() - sf::Vector2f(x, y);
        sf::Vector2f velocity = (toPlayer * float(1.f / sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y))) * 600.0f;

        sf::Vector2f speed = sf::Vector2f(left ? 600.0f : -600.0f, feature % 20) * (feature / 71.0f);

        auto projectile = new ProjectilePentagon();

        projectile->startPos = sf::Vector2f(x, y);
        projectile->speed = speed * 0.5f + velocity * 0.5f;
        projectile->radius = 27.0f + (feature % 15);
        projectile->startAngleDeg = Rotator * 3 + feature;

        projectile->Build();
        Projectiles.push_back(projectile);
    }
    else {
        // Большие, красно синие
        float angleRad = (feature / 256.f * 360.f) * 3.14159265f / 180.0f;
        float distance = barrierRadius + 180.0f;

        sf::Vector2f start(
            barrierCenter.x + distance * cos(angleRad),
            barrierCenter.y + distance * sin(angleRad)
        );

        // Направление
        sf::Vector2f direction = player.getPosition() - start;

        float length = sqrt(direction.x * direction.x + direction.y * direction.y);
        sf::Vector2f velocity = (direction / length) * 240.f;

        AbstractProjectile* projectile = new ProjectileTriangle();

        projectile->startPos = start;
        projectile->speed = velocity;
        projectile->startAngleDeg = feature / 256.f * 360.f;
        projectile->color = sf::Color(feature % 2 == 0 ? 255 : 0, (feature - 100) * 10, feature % 2 == 1 ? 255 : 0);

        projectile->Build();
        projectile->shape.setScale(4.5f, 4.5f);

        Projectiles.push_back(projectile);
    }
}