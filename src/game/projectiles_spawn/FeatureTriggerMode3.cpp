#include <Game.hpp>
#include <EIF.hpp>

// Редко встречается, нестабильна, энергии меньше среднего
void GameScene::FeatureTriggerMode3(float value, int feature) {

    txtDebug.setString("Mode 3");

    
    float angleRad = (feature / 256.f * 360.f) * 3.14159265f / 180.0f;
    float distance = barrierRadius + 180.0f;

    sf::Vector2f position(
        barrierCenter.x + distance * cos(angleRad),
        barrierCenter.y + distance * sin(angleRad)
    );
    sf::Vector2f direction = player.getPosition() - position;

    float length = sqrt(direction.x * direction.x + direction.y * direction.y);
    sf::Vector2f velocity = (direction / length) * 240.f;

    Projectile* projectile;
    if (feature % 64 > 32) {

        // большие треугольники
        projectile = new ProjectileTriangle(position, velocity, sf::Color(feature % 2 == 0 ? 255 : 0, (feature - 100) * 10, feature % 2 == 1 ? 255 : 0), feature / 256.f * 360.f);
        projectile->shape.setScale(5, 5);
    }
    else {
        // движущиеся лазеры
        projectile = new ProjectileLazer(position, feature / 256.f * 360.f + 90, 10, feature % 2 == 4 ? 255 : -4);
        projectile->speed = velocity * 1.35f;
    }
    Projectiles.push_back(projectile);
}