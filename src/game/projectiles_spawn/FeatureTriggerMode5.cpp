#include <Game.hpp>
#include <EIF.hpp>

// Активность выше среднего, басов хватает
void GameScene::FeatureTriggerMode5(float value, int feature) {
    
    txtDebug.setString("Mode 5"); // TODO: ревью, написан ИИ

    if (feature < 30) {
        // Быстрые лазеры-кресты
        for (int i = 0; i < 3; i++) {
            float angle = Rotator + feature * 2 + i * 120;
            auto projectile = new ProjectileLazer(
                player.getPosition(),
                angle,
                feature % 15 + 5,
                feature % 5 - 2
            );
            Projectiles.push_back(projectile);
        }
    }
    else if (feature < 60) {
        // Взрывные пятиугольники с ускорением
        float baseAngle = (feature / 256.f * 360.f) * M_PI / 180.0f;

        for (int i = -1; i <= 1; i++) {
            float angle = baseAngle + i * 0.3f;
            float distance = barrierRadius + 150.0f;

            sf::Vector2f position(
                barrierCenter.x + distance * cos(angle),
                barrierCenter.y + distance * sin(angle)
            );

            sf::Vector2f toPlayer = player.getPosition() - position;
            float length = sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);
            sf::Vector2f velocity = (toPlayer / length) * (200.0f + feature * 2);

            auto projectile = new ProjectilePentagon(
                position,
                velocity,
                15.0f + (feature % 10),
                Rotator * 2 + feature * 5
            );
            Projectiles.push_back(projectile);
        }
    }
    else if (feature < 100) {
        // Шестиугольники-волны сверху и снизу
        bool fromTop = feature % 2 == 0;
        float y = fromTop ? -50.0f : windowHeight + 50.0f;
        float x = (feature % 100) * (windowWidth / 100.0f);

        sf::Vector2f toPlayer = player.getPosition() - sf::Vector2f(x, y);
        float length = sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);
        sf::Vector2f baseVelocity = (toPlayer / length) * 900.0f;

        // Добавляем горизонтальное колебание
        sf::Vector2f velocity = baseVelocity +
            sf::Vector2f(sin(feature * 0.1f) * 200.0f, 0);

        auto projectile = new ProjectileHexagon(
            sf::Vector2f(x, y),
            velocity,
            22.0f + (feature % 12),
            Rotator * 3 + feature * 2
        );
        Projectiles.push_back(projectile);
    }
    else {
        // Спиральные с изменяющимся радиусом (быстрая спираль)
        float baseAngle = (Rotator / 20.0f + feature * 3) * M_PI / 180.0f;
        float startRadius = barrierRadius + 100.0f;

        auto projectile = new ProjectileSpiralMove(
            barrierCenter,
            sf::Color(255, 100 + feature % 155, 50),
            8 + (feature % 10),
            startRadius,
            baseAngle
        );
        Projectiles.push_back(projectile);
    }
}