#include <Game.hpp>

void GameScene::FeatureTriggered(float value, int feature)
{
    // около 0 равно инфразвук, триггерится при этом не только басами, около 255 ультразвук, он почти никогда не триггерится
    // TODO: полностью переписать логику их спавна, прям полностью

    // перебалансю наоборот мб
    feature = 255 - feature; // 255 теперь 0, а 0 теперь 255

    //return; // временно отключил вообще

    if (feature < 20) {
        // мелкие
        float angleRad = (angleAtack / 5.f + feature * 3.f) * 3.14159265f / 180.0f ;
        float distance = barrierRadius + 240.0f;

        sf::Vector2f position(
            barrierCenter.x + distance * cos(angleRad),
            barrierCenter.y + distance * sin(angleRad)
        );

        // Направление к центру
        sf::Vector2f direction = barrierCenter - position;
        float length = sqrt(direction.x * direction.x + direction.y * direction.y);
        sf::Vector2f velocity = (direction / length) * speedAtack * 70.f;

        auto p = new ProjectileTriangle(position, velocity, sf::Color::Cyan, angleAtack + 90);
        p->damage = 2;
        Projectiles.push_back(p);
    }
    else if (feature < 40) {
        // Большие, красно синие
        float angleRad = (-angleAtack + feature * 12.f + value / 20000.f) * 3.14159265f / 180.0f ;
        float distance = barrierRadius + 180.0f;

        sf::Vector2f position(
            barrierCenter.x + distance * cos(angleRad),
            barrierCenter.y + distance * sin(angleRad)
        );

        // Направление
        sf::Vector2f direction = player.getPosition() - position;

        float length = sqrt(direction.x * direction.x + direction.y * direction.y);
        sf::Vector2f velocity = (direction / length) * speedAtack * 80.f;

        Projectile* projectile = nullptr;

        float rotateDegrees = std::atan2(velocity.y, velocity.x) * 180.0f / 3.14159265f - 90;

        if (tension < 900 || tension > 1500) {
            projectile = new ProjectileTriangle(position, velocity, sf::Color(feature % 2 == 0 ? 255 : 0, (feature - 100) * 10, feature % 2 == 1 ? 255 : 0), rotateDegrees);
            projectile->shape.setScale(4, 4);
        }
        else if (int(tension) % 4 == 0) { // иначе с меньшим шансом

            projectile = new ProjectileLazer(position, rotateDegrees, 10, int(tension) % 2 == 0 ? 10 : -10);
            projectile->speed = velocity * 1.3f;
            angleAtack -= 30;
        }

        if (projectile != nullptr) Projectiles.push_back(projectile);
    }
    else if (feature < 60) {

        // Прыгающие круги
        float angleRad = (-angleAtack / 50.f + feature * 15.f + 90 + (int(value) % 30 - 15) + (feature % 2) * 180) * 3.14159265f / 180.0f ;
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
        sf::Vector2f velocity = (direction / length) * speedAtack * 350.f;

        auto projectile = new ProjectileRound(
            position, 
            positiong,
            velocity, 
            sf::Color(feature % 2 == 0 ? 255 : 0, (feature - 100) * 10, feature % 2 == 1 ? 255 : 0)
            , (feature % 20 + 6) * 2.45);
        Projectiles.push_back(projectile);

    }
    else if (feature < 80) {
        if (tension > 550) {

            float r = std::max(decoreR * 3, 250.f);
            float g = std::max(decoreG * 3, 250.f);
            int b = 255;

            auto projectile = new ProjectileSpiralMove(barrierCenter, sf::Color(r, g, b), 10, barrierRadius + 100, angleAtack + feature / 10.f);
            Projectiles.push_back(projectile);

            return;
        }

        tension += 5;

        int variation = feature - 200;
        auto x = barrierCenter.x;
        x += (variation - 5) * 100 + int(tension) % 128;
        auto radius = std::max(float(int(tension) % 30), 25.f);

        auto projectile = new ProjectilePentagon(
            sf::Vector2f(x, window->getSize().y), 
            Vector2f((int(tension) % 10) - 5, -900), 
            radius, 
            tension);       

        Projectiles.push_back(projectile);
    }
    else if (feature < 100) {
        if (int(tension) % 15 == 5 && tension > 50 && tension < 1000) {
            auto projectile = new ProjectileLazer(player.getPosition(), tension + angleAtack + feature, 10, feature % 7 - 3);
            Projectiles.push_back(projectile);
        }
    }
    // Далее особо высокие звуки
    else if (feature < 120) {
        if (healingReload < 0 && feature % 5 == 2) {
            
            auto projectile = new ProjectileHealingLazer(player.getPosition(), tension + angleAtack + feature, 10, (feature % 7 - 3) * 13);
            float mv = feature - 90.f;
            projectile->shape.move(mv * 50.f, int(tension) % 500 - 250);

            Projectiles.push_back(projectile);

            healingReload = 10;
        }
        else if (tension < 500) {

            tension += 5; // фича значительно повышает напряжённость

            int variation = feature - 80;
            auto y = barrierCenter.y;
            y += (variation - 10) * 100 + int(tension) % 128;

            auto radius = std::max(float(int(tension) % 50), 50.f);

            auto projectile = new ProjectileHexagon(
                sf::Vector2f(300, y), 
                Vector2f(900, ((int(tension) % 10) - 5) * 15), 
                radius, 
                tension * 19.f);       

            Projectiles.push_back(projectile);
        }
    }
    else if (feature < 140) {
        if (int(tension) % 20 == 5 && tension > 1000) {
            auto projectile = new ProjectileLazer(barrierCenter * 0.8f + player.getPosition() * 0.2f, tension + angleAtack + feature, 100, 0);
            Projectiles.push_back(projectile);

            tension -= 500; // чтобы не спамило большим лазером
        }
        else {
            int variation = feature - 80;
            auto y = barrierCenter.y;
            y += (variation - 10) * 100 + int(tension) % 128;

            auto radius = std::max(float(int(tension) % 50), 50.f);

            auto projectile = new ProjectilePentagon(
                sf::Vector2f(300, y), 
                Vector2f(900, ((int(tension) % 10) - 5) * 15), 
                radius, 
                tension * 19.f);       

            Projectiles.push_back(projectile);
        }
    }
    else if (feature < 160) {
        barrierRadius *= 1.005;
        barrierRadius = std::min(barrierRadius, barrierRadiusInitial * 1.3f);
    }
    else if (feature < 180) {
        barrierRadius *= 0.995;
        barrierRadius = std::max(barrierRadius, barrierRadiusInitial / 1.1f);

        while (!IsInsideBarrier(player.getPosition()))
        {
            player.move((player.getPosition() - barrierCenter) * -0.05f);
        } 
    }
    else if (feature < 200) { 
        angleAtack -= 0.8 * feature / 55.f; 

        if (tension > 1200) {
            auto pos = player.getPosition();
            auto projectile = new ProjectileDeathZone(pos, 100, tension, 40);
            Projectiles.push_back(projectile);
            tension -= 400;
        }
    }
    else if (feature < 220) {
        angleAtack += 0.8 * feature / 55.f; 
    }
    else if (feature < 240) {
        angleAtack -= 0.3;
        tension++; // пусть просто повысит напряжённость
    }
    else {
        int r = 255;
        float g = std::max(decoreG * 3, 250.f);
        float b = std::max(decoreB * 3, 250.f);

        auto projectile = new ProjectileSpiralMove(barrierCenter, sf::Color(r, g, b), 10, barrierRadius + 100, angleAtack + (feature - 10) / 10.f + 180);
        projectile->angleSpeed *= -1;
        Projectiles.push_back(projectile);
    }

    // Декор
    if (feature < 80) {
        // от центра
        float angleRad = (angleAtack + feature * 360 / 80) * 3.14159265f / 180.0f ;
        float distance = barrierRadius + 10;

        sf::Vector2f position(
            barrierCenter.x + distance * cos(angleRad),
            barrierCenter.y + distance * sin(angleRad)
        );

        // Направление от центра
        sf::Vector2f direction = barrierCenter - position;
        float length = sqrt(direction.x * direction.x + direction.y * direction.y);
        sf::Vector2f velocity = (direction / length) * speedAtack * -0.9f * (value + 10) / 255.f;

        auto p = new ProjectileTriangle(position, velocity, sf::Color::White, angleAtack + 90);
        p->shape.setScale(0.3f, 0.3f);
        p->isCollidable = false; // это декор
        Projectiles.push_back(p);
    }
}