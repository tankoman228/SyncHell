#include <Game.hpp>
#include <EIF.hpp>

// Активность выше среднего, басов хватает
void GameScene::FeatureTriggerMode5(float value, int feature) {
    
    txtDebug.setString("Mode 5"); // TODO: ревью, написан ИИ

    if (feature < 30) {
        // Быстрые лазеры-кресты
        for (int i = 0; i < 3; i++) {
            float angle = Rotator + feature * 3 + i * 120;

            auto projectile = new ProjectileLazer();

            projectile->startPos = player.getPosition();
            projectile->startAngleDeg = angle;
            projectile->rotationSpeed = feature % 5 - 2;
            projectile->size = feature % 15 + 5;

            projectile->Build();
            Projectiles.push_back(projectile);
        }
    }
    else if (feature < 60) {
        // Взрывные пятиугольники с ускорением
        float baseAngle = (feature / 256.f * 360.f) * M_PI / 180.0f;

        for (int i = -1; i <= 1; i++) {
            float angle = baseAngle + i * 0.3f;
            float distance = barrierRadius + 150.0f;

            sf::Vector2f st(
                barrierCenter.x + distance * cos(angle),
                barrierCenter.y + distance * sin(angle)
            );

            sf::Vector2f toPlayer = player.getPosition() - st;
            float length = sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);
            sf::Vector2f velocity = (toPlayer / length) * (200.0f + feature * 2);

            auto projectile = new ProjectilePentagon();

            projectile->startPos = st;
            projectile->speed = velocity;
            projectile->radius = 15.0f + (feature % 10);
            projectile->startAngleDeg = Rotator * 2 + feature * 5;

            projectile->Build();
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

        auto projectile = new ProjectileHexagon();

        projectile->startPos = sf::Vector2f(x, y);
        projectile->speed = velocity;
        projectile->radius = 22.0f + (feature % 12);
        projectile->startAngleDeg = Rotator * 3 + feature * 2;

        projectile->Build();
        Projectiles.push_back(projectile);
    }
    else {
        // Спиральные с изменяющимся радиусом (быстрая спираль)
        float baseAngle = (Rotator / 20.0f + feature * 3) * M_PI / 180.0f;
        float startRadius = barrierRadius + 100.0f;

        auto projectile = new ProjectileSpiralMove();

        projectile->positionStart = barrierCenter;
        projectile->color = sf::Color(255, 100 + feature % 155, 50);
        projectile->radius = 8 + (feature % 10);
        projectile->spiralRadius = startRadius;
        projectile->angleMove = baseAngle;

        projectile->Build();
        Projectiles.push_back(projectile);
    }
}