#include <Game/Game.hpp>
#include <Analysis/EIF.hpp>

// Редко встречается, нестабильна, энергии меньше среднего
void GameScene::FeatureTriggerMode3(float value, int feature) {

    txtDebug.setString("Mode 3");
    
    float angleRad = (feature / 256.f * 360.f) * 3.14159265f / 180.0f;
    float distance = barrierRadius + 180.0f;

    sf::Vector2f startPos = Around(barrierCenter, (feature / 256.f * 360.f), barrierRadius + 180.0f);
    sf::Vector2f velocity = Towards(startPos, player.getPosition(), 240);

    if (feature % 64 > 32) {

        // большие треугольники
        auto projectile = new ProjectileTriangle(feature);
        
        projectile->startPos = startPos;
        projectile->speed = velocity;
        projectile->color = sf::Color(feature % 2 == 0 ? 255 : 0, (feature - 100) * 10, feature % 2 == 1 ? 255 : 0);
        projectile->startAngleDeg = feature / 256.f * 360.f;
        projectile->radius = 14 * 4;

        projectile->Build();
        Projectiles.push_back(projectile);
    }
    else {
        auto projectile = new ProjectileLazer(feature);

        projectile->startPos = startPos;
        projectile->startAngleDeg = feature / 256.f * 360.f + 90;
        projectile->rotationSpeed = feature % 2 == 4 ? 255 : -4;
        projectile->size = 10;
        projectile->speed = velocity * 1.45f;

        projectile->Build();
        Projectiles.push_back(projectile);
    }
}