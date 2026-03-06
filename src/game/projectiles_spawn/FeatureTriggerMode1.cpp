#include <Game.hpp>
#include <EIF.hpp>

// Характерен для умеренных мелодий без басов
void GameScene::FeatureTriggerMode1(float value, int feature) {
  
    txtDebug.setString("Mode 1");

    if (feature < 55) {

        float angleRad = (-Rotator / 50.f + feature * 5.f + 90 + (int(value) % 30 - 15) + (feature % 2) * 180) * 3.14159265f / 180.0f;
        float distance = barrierRadius + 200.0f;

        sf::Vector2f position(
            barrierCenter.x + distance * cos(angleRad),
            barrierCenter.y + distance * sin(angleRad)
        );
        sf::Vector2f positiong(
            barrierCenter.x + distance * cos(angleRad) * 4.f,
            barrierCenter.y + distance * sin(angleRad) * 4.f
        );

        // Направление
        sf::Vector2f direction = (player.getPosition() + barrierCenter) * 0.5f - position;

        float length = sqrt(direction.x * direction.x + direction.y * direction.y);
        sf::Vector2f velocity = (direction / length) * 850.f;

        auto projectile = new ProjectileRound(
            position,
            positiong,
            velocity,
            sf::Color(feature % 2 == 0 ? 255 : 0, (feature - 100) * 10, feature % 2 == 1 ? 255 : 0)
            , (feature % 20 + 6) * 2.45);
        Projectiles.push_back(projectile);

    }
    else if (feature < 70) {

        // Лазеры
        auto projectile = new ProjectileLazer(player.getPosition(), Rotator + feature, 10, feature % 7 - 3);
        Projectiles.push_back(projectile);
    }
    else if (feature < 120) {
        // Пятиугольники для большего веселья
        bool left = feature % 2 == 0;
        float x = left ? -10.0f : windowWidth + 10.0f;
        float y = 50.0f - (feature % 50) * 4.0f;

        sf::Vector2f toPlayer = player.getPosition() - sf::Vector2f(x, y);
        sf::Vector2f velocity = (toPlayer / sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y)) * 600.0f;

        sf::Vector2f speed = sf::Vector2f(left ? 600.0f : -600.0f, feature % 20) * (feature / 71.0f);

        Projectiles.push_back(new ProjectilePentagon(
            sf::Vector2f(x, y),
            (speed * 0.5f + velocity * 0.5f),
            27.0f + (feature % 15),
            Rotator * 3 + feature
        ));
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
        projectile->shape.setScale(4, 4);

        Projectiles.push_back(projectile);
    }
}