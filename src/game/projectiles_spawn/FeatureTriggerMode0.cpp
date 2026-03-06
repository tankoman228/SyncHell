#include <Game.hpp>
#include <EIF.hpp>

// Характерен для тихих и ненасыщенных мелодий, звуков. Количество триггеров минимально
void GameScene::FeatureTriggerMode0(float value, int feature) {
    
    txtDebug.setString("Mode 0");

    if (feature < 35) {
        // Лазеры
        auto projectile = new ProjectileLazer(player.getPosition(), Rotator + feature, 10, (feature % 7 - 3));
        Projectiles.push_back(projectile);
    }
    if (feature < 55) {

        // Спиральные
        float r = std::max((feature - 25) * 5.f, 250.f);
        float g = std::max((feature - 25) * 5.f, 250.f);
        int b = feature % 2 == 0 ? 128 : 255;

        auto projectile = new ProjectileSpiralMove(barrierCenter, sf::Color(r, g, b), 10, barrierRadius + 100, Rotator + feature / 20.f);
        Projectiles.push_back(projectile);
    }
    else {
        // Большие, красно синие
        float angleRad = (feature / 256.f * 360.f) * 3.14159265f / 180.0f;
        float distance = barrierRadius + 180.0f;

        sf::Vector2f position(
            barrierCenter.x + distance * cos(angleRad),
            barrierCenter.y + distance * sin(angleRad)
        );

        // Направление
        sf::Vector2f direction = player.getPosition() - position;

        float length = sqrt(direction.x * direction.x + direction.y * direction.y);
        sf::Vector2f velocity = (direction / length) * 240.f;

        Projectile* projectile = new ProjectileTriangle(position, velocity, sf::Color(feature % 2 == 0 ? 255 : 0, (feature - 100) * 10, feature % 2 == 1 ? 255 : 0), feature / 256.f * 360.f);
        projectile->shape.setScale(3, 3);

        Projectiles.push_back(projectile);
    }
}