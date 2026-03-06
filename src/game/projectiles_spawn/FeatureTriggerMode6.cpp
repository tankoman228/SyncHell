#include <Game.hpp>
#include <EIF.hpp>

// Высокая активность, часто встречается в металле как основная
void GameScene::FeatureTriggerMode6(float value, int feature) {
    
    // TODO: удалить затычку и сделать реальную логику
    txtDebug.setString("Mode 6");  // TODO: ревью, написан ИИ

    if (feature < 20) {
        // Зоны смерти, но полезные? Нет, опасные!
        static bool deathSpawned = false;
        if (feature == 0) deathSpawned = false;

        if (!deathSpawned && feature % 10 == 0) {
            sf::Vector2f spawnPos(
                barrierCenter.x + (rand() % 400 - 200),
                barrierCenter.y + (rand() % 400 - 200)
            );

            auto projectile = new ProjectileDeathZone(
                spawnPos,
                feature + 120,
                Rotator + feature * 10,
                feature + 80
            );
            Projectiles.push_back(projectile);
            deathSpawned = true;
        }
        else goto lazer;
    }
    else if (feature < 45) {
        // Двойные лазеры крест-накрест
        lazer:
        float baseAngle = Rotator + feature * 4;

        for (int i = 0; i < 2; i++) {
            float angle = baseAngle + i * 90;
            auto projectile = new ProjectileLazer(
                player.getPosition() + sf::Vector2f(50, 50),
                angle,
                15 + feature % 10,
                feature % 8 - 4
            );
            Projectiles.push_back(projectile);
        }
    }
    else if (feature < 80) {
        // Треугольники с разных сторон с высокой скоростью
        int sides = 8; // Восьмиугольное размещение
        float baseAngle = (feature / 256.f * 360.f) * M_PI / 180.0f;

        for (int i = 0; i < sides; i++) {
            float angle = baseAngle + (2 * M_PI * i / sides);
            float distance = barrierRadius + 250.0f;

            sf::Vector2f position(
                barrierCenter.x + distance * cos(angle),
                barrierCenter.y + distance * sin(angle)
            );

            sf::Vector2f direction = player.getPosition() - position;
            float length = sqrt(direction.x * direction.x + direction.y * direction.y);
            sf::Vector2f velocity = (direction / length) * 550.0f;

            // Цвет меняется в зависимости от позиции
            sf::Color color(
                200 + 55 * sin(angle),
                100 + 155 * cos(angle * 2),
                255
            );

            auto projectile = new ProjectileTriangle(
                position,
                velocity,
                color,
                feature * 5 + i * 45
            );
            projectile->shape.setScale(2.5f, 2.5f);
            Projectiles.push_back(projectile);
        }
    }
    else {
        // Быстрые пятиугольники с гравитационным искажением
        float angle = (feature * 15.0f) * M_PI / 180.0f;
        float distance = barrierRadius + 300.0f;

        sf::Vector2f pos(
            barrierCenter.x + distance * cos(angle),
            barrierCenter.y + distance * sin(angle)
        );

        // Создаём "приманку" - точку не в игроке, а рядом
        sf::Vector2f fakeTarget = player.getPosition() +
            sf::Vector2f(100 * sin(feature), 100 * cos(feature));

        sf::Vector2f toFake = fakeTarget - pos;
        float length = sqrt(toFake.x * toFake.x + toFake.y * toFake.y);
        sf::Vector2f velocity = (toFake / length) * 700.0f;

        auto projectile = new ProjectilePentagon(
            pos,
            velocity,
            30.0f,
            Rotator * 5 + feature * 10
        );
        Projectiles.push_back(projectile);
    }
}