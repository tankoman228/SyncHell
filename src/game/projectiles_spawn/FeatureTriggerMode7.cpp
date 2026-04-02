#include <Game.hpp>
#include <EIF.hpp>

// Пики активности, часто встречается в металле
void GameScene::FeatureTriggerMode7(float value, int feature) {
    
    txtDebug.setString("Mode 7"); // TODO: ревью, написан ИИ
    
    float angleRad = (feature / 256.f * 360.f) * 3.14159265f / 180.0f;
    float distance = barrierRadius + 180.0f;

    sf::Vector2f st(
        barrierCenter.x + distance * cos(angleRad),
        barrierCenter.y + distance * sin(angleRad)
    );
    sf::Vector2f direction = player.getPosition() - st;

    float length = sqrt(direction.x * direction.x + direction.y * direction.y);
    sf::Vector2f velocity = (direction / length) * 540.f;

    AbstractProjectile* projectile;
    if (feature % 64 > 8) {

        // большие треугольники
        auto projectile = new ProjectileTriangle(feature);

        projectile->startPos = st;
        projectile->speed = velocity;
        projectile->startAngleDeg = feature / 256.f * 360.f;
        projectile->color = sf::Color(feature % 2 == 0 ? 255 : 0, (feature - 100) * 10, feature % 2 == 1 ? 255 : 0);
        projectile->radius = 17 * 4;

        projectile->Build();
        Projectiles.push_back(projectile);
    }
    else {
        // лазеры
        auto projectile = new ProjectileLazer(feature);

        projectile->startPos = st * 0.2f + player.getPosition() * 0.8f;
        projectile->startAngleDeg = feature / 256.f * 360.f + 90;
        projectile->rotationSpeed = feature % 2 == 9 ? 255 : -9;
        projectile->size = 20;

        projectile->Build();
        Projectiles.push_back(projectile);
    }
}