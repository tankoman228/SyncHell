#include <Game.hpp>
#include <EIF.hpp>

void GameScene::FeatureTriggerMode7(float value, int feature) {
    
    // TODO: удалить затычку и сделать реальную логику
    
    static bool TriggeredAlready[256] = {0};
    if (value > 250) {
        if (!TriggeredAlready[feature]) {
            TriggeredAlready[feature] = 1; tension += value;
        }
        else {
            return;
        }
    }
    else {
        TriggeredAlready[feature] = 0;
        return; // не триггерится же
    }

    txtDebug.setString("Mode 7");

    // Большие, красно синие
    float angleRad = (feature / 256.f * 360.f) * 3.14159265f / 180.0f ;
    float distance = barrierRadius + 180.0f;

    sf::Vector2f position(
        barrierCenter.x + distance * cos(angleRad),
        barrierCenter.y + distance * sin(angleRad)
    );

    // Направление
    sf::Vector2f direction = player.getPosition() - position;

    float length = sqrt(direction.x * direction.x + direction.y * direction.y);
    sf::Vector2f velocity = (direction / length) * 240.f;

    Projectile* projectile = nullptr;
    projectile = new ProjectileTriangle(position, velocity, sf::Color(feature % 2 == 0 ? 255 : 0, (feature - 100) * 10, feature % 2 == 1 ? 255 : 0), feature / 256.f * 360.f);
    projectile->shape.setScale(8, 8);

    if (projectile != nullptr) Projectiles.push_back(projectile);
}