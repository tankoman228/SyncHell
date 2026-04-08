#include <Game/Game.hpp>
#include <Analysis/EIF.hpp>

// Характерен для тихих и ненасыщенных мелодий, звуков. Количество триггеров минимально
void GameScene::FeatureTriggerMode0(float value, int feature) {
    
    txtDebug.setString("Mode 0");

    if (feature < 35) {

        auto projectile = new ProjectileLazer(feature);

        projectile->startPos      = player.getPosition();
        projectile->startAngleDeg = Rotator + feature;
        projectile->rotationSpeed = feature % 7 - 3;
        projectile->size          = 10;

        projectile->Build();
        Projectiles.push_back(projectile);
    }
    if (feature < 55) {

        // Спиральные
        float r = std::max((feature - 25) * 5.f, 250.f);
        float g = std::max((feature - 25) * 5.f, 250.f);
        int b = feature % 2 == 0 ? 128 : 255;

        auto projectile = new ProjectileSpiralMove(feature);

        projectile->positionStart = barrierCenter;
        projectile->color = sf::Color(r, g, b);
        projectile->radius = 10;
        projectile->spiralRadius = barrierRadius + 100;
        projectile->angleMove = Rotator + feature;

        projectile->Build();
        Projectiles.push_back(projectile);
    }
    else {
        // Красно синие
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