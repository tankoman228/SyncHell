#include <Game.hpp>
#include <EIF.hpp>

// Характерен для умеренных мелодий без басов
void GameScene::FeatureTriggerMode1(float value, int feature) {
  
    txtDebug.setString("Mode 1");

    if (feature < 55) {

        float distance = barrierRadius + 200.0f;
        float a = -Rotator / 50.f + feature * 5.f + 90 + (int(value) % 30 - 15) + (feature % 2) * 180;

        sf::Vector2f start     = Around(barrierCenter, a, barrierRadius + 200.0f);
        sf::Vector2f positiong = Around(barrierCenter, a, barrierRadius + 800.0f);
        sf::Vector2f velocity  = Towards(start, player.getPosition() * 0.3f + barrierCenter * 0.7f, 850.f);

        auto projectile = new ProjectileRound(feature);

        projectile->color = sf::Color(feature % 2 == 0 ? 255 : 0, (feature - 100) * 10, feature % 2 == 1 ? 255 : 0);
        projectile->startPos = start;
        projectile->gravityTargetPoint = positiong;
        projectile->speed = velocity;
        projectile->radius = (feature % 20 + 6) * 2.45;

        projectile->Build();
        Projectiles.push_back(projectile);
    }
    else if (feature < 70) {

        auto projectile = new ProjectileLazer(feature);

        projectile->startPos      = player.getPosition();
        projectile->startAngleDeg = Rotator + feature;
        projectile->rotationSpeed = feature % 7 - 3;
        projectile->size          = 10;

        projectile->Build();
        Projectiles.push_back(projectile);
    }
    else if (feature < 120) {

        // Пятиугольники, сыпятся сверху
        bool left = feature % 2 == 0;
        float x = left ? -10.0f : windowWidth + 10.0f;
        float y = 50.0f - (feature % 50) * 4.0f;

        sf::Vector2f toPlayer = player.getPosition() - sf::Vector2f(x, y);
        sf::Vector2f velocity = (toPlayer * float(1.f / sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y))) * 600.0f;
        sf::Vector2f speed = sf::Vector2f(left ? 600.0f : -600.0f, feature % 20) * (feature / 71.0f);

        auto projectile = new ProjectilePentagon(feature);

        projectile->startPos = sf::Vector2f(x, y);
        projectile->speed = speed * 0.5f + velocity * 0.5f;
        projectile->radius = 27.0f + (feature % 15);
        projectile->startAngleDeg = Rotator * 3 + feature;

        projectile->Build();
        Projectiles.push_back(projectile);
    }
    else {
        // Большие, красно синие
        ProjectileTriangle* projectile = new ProjectileTriangle(feature);
        sf::Vector2f start = Around(barrierCenter, feature / 256.f * 360.f, barrierRadius + 180.0f);

        projectile->startPos = start;
        projectile->speed = Towards(start, player.getPosition(), 240);;
        projectile->startAngleDeg = feature / 256.f * 360.f;
        projectile->color = sf::Color(feature % 2 == 0 ? 255 : 0, (feature - 100) * 10, feature % 2 == 1 ? 255 : 0);
        projectile->radius = 12 * 4;

        projectile->Build();
        Projectiles.push_back(projectile);
    }
}