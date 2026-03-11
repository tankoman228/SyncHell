#include <Game.hpp>
#include <EIF.hpp>

// Редко встречается, нестабильна, энергии меньше среднего
void GameScene::FeatureTriggerMode3(float value, int feature) {

    txtDebug.setString("Mode 3");
    
    float angleRad = (feature / 256.f * 360.f) * 3.14159265f / 180.0f;
    float distance = barrierRadius + 180.0f;

    sf::Vector2f startPos(
        barrierCenter.x + distance * cos(angleRad),
        barrierCenter.y + distance * sin(angleRad)
    );
    sf::Vector2f direction = player.getPosition() - startPos;

    float length = sqrt(direction.x * direction.x + direction.y * direction.y);
    sf::Vector2f velocity = (direction / length) * 240.f;

    if (feature % 64 > 32) {

        // большие треугольники
        auto projectile = new ProjectileTriangle();
        
        projectile->startPos = startPos;
        projectile->speed = velocity;
        projectile->color = sf::Color(feature % 2 == 0 ? 255 : 0, (feature - 100) * 10, feature % 2 == 1 ? 255 : 0);
        projectile->startAngleDeg = feature / 256.f * 360.f;

        projectile->Build();
        projectile->shape.setScale(5, 5);

        Projectiles.push_back(projectile);
    }
    else {
        auto projectile = new ProjectileLazer();

        projectile->startPos = startPos;
        projectile->startAngleDeg = feature / 256.f * 360.f + 90;
        projectile->rotationSpeed = feature % 2 == 4 ? 255 : -4;
        projectile->size = 10;
        projectile->speed = velocity * 1.45f;

        projectile->Build();
        Projectiles.push_back(projectile);
    }
}