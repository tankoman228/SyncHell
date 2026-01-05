#include <Game.hpp>

void GameScene::FeatureTriggered(float value, int feature)
{
    // диапазон    описание                                         значения в песне Beast In Black

    // от  0 до 19 энергия (громкость?)                             0-1
    // от 20 до 39 разброс                                          0-240
    // от 40 до 59 дельта усиления                                  0-3900
    // от 60 до 79 дельта ослабления                                0-4000
    // от 80 до 99 энергия 1-й доминирующей частоты                 0-4000
    // от 100 энергия 2-й доминирующей частоты                      0-3800
    // от 120 индекс первой доминирующей                            0-7
    // от 140 индекс второй доминирующей                            0-18
    // от 160 резкость волны по производной                         0-1362
    // от 180 альт. резкость волны по производной                   0-1362
    // от 200 резкость волны по второй производной                  0-2568
    // от 220 альт. резкость волны по второй производной            0-13000
    // от 240 до 251 суммы фич по всем полосам                      дофига
    // 252 - обобщённая громкость                                   0-255
    // 253 - число горизонтальных рёбер (резкие перепады)           0-2222
    // 254 - число вертикальных рёбер (резкие перепады)             0-3923
    // 255 - отношение числа вертикальных к (горизонтальным + 1)    0-15.8

    // На практике почти всегда нули: 140-145, 120-123, 100-105 (низкие частоты менее разнообразны)

    // Декор вокруг центра отобр. громкость
    if (feature < 20) {

        if (value < 0.6) return;

        // от центра
        float angleRad = (angleAtack + feature * 18) * 3.14159265f / 180.0f ;
        float distance = barrierRadius + 10;

        sf::Vector2f position(
            barrierCenter.x + distance * cos(angleRad),
            barrierCenter.y + distance * sin(angleRad)
        );

        // Направление от центра
        sf::Vector2f direction = barrierCenter - position;
        float length = sqrt(direction.x * direction.x + direction.y * direction.y);
        sf::Vector2f velocity = (direction / length) * speedAtack * -10.f * (value + 1.f);

        auto p = new ProjectileTriangle(position, velocity, sf::Color::White, angleAtack + 90);
        p->shape.setScale(0.3f, 0.3f);
        p->isCollidable = false; // это декор
        Projectiles.push_back(p);

        return;
    }

    // Мелкие надоедливые треугольнички
    if (feature < 40 && feature >= 20) {

        if (tension > 1000) return; 

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

    else if (feature < 60 && feature >= 40) {

        angleAtack -= 0.8 * feature / 55.f; 

        if (tension > 1200) {
            auto pos = player.getPosition();
            auto projectile = new ProjectileDeathZone(pos, 100, tension, 40);
            Projectiles.push_back(projectile);
            tension -= 400;
        }
    }
    else if (feature < 80 && feature >= 60) {

        angleAtack += 0.8 * feature / 55.f; 
    }

    else if (feature < 100 && feature >= 80) {
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

    else if (feature < 114 && feature >= 100)
    {
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

    else if (feature < 120 && feature >= 114)
    {
        // Большие, красно синие
        float angleRad = (-angleAtack / 50.f + feature * 15.f + 90 + (int(value) % 30 - 15) + (feature % 2) * 180) * 3.14159265f / 180.0f ;
        float distance = barrierRadius + 200.0f;

        sf::Vector2f position(
            barrierCenter.x + distance * cos(angleRad),
            barrierCenter.y + distance * sin(angleRad)
        );

        // Направление
        sf::Vector2f direction = (player.getPosition() + barrierCenter) * 0.5f - position;

        float length = sqrt(direction.x * direction.x + direction.y * direction.y);
        sf::Vector2f velocity = (direction / length) * speedAtack * 400.f;

        auto projectile = new ProjectileRound(position, velocity, sf::Color(feature % 2 == 0 ? 255 : 0, (feature - 100) * 10, feature % 2 == 1 ? 255 : 0), (feature % 110 + 6) * 2.25, float(feature % 110) / 7.f + 2.3f);
        Projectiles.push_back(projectile);
    }

    else if (feature < 140 && feature >= 120) {
        barrierRadius *= 1.005;
        barrierRadius = std::min(barrierRadius, barrierRadiusInitial * 1.3f);
    }
    
    else if (feature < 160 && feature >= 140) {
        barrierRadius *= 0.995;
        barrierRadius = std::max(barrierRadius, barrierRadiusInitial / 1.1f);

        while (!IsInsideBarrier(player.getPosition()))
        {
            player.move((player.getPosition() - barrierCenter) * -0.05f);
        } 
    }

    else if (feature < 180 && feature >= 160) {
        if (int(tension) % 15 == 5 && tension > 50 && tension < 1000) {
            auto projectile = new ProjectileLazer(player.getPosition(), tension + angleAtack + feature, 10, feature % 7 - 3);
            Projectiles.push_back(projectile);
        }
    }

    else if (feature < 200 && feature >= 180) {
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

    else if (feature < 210 && feature >= 200) {

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
    else if (feature < 220 && feature >= 210) {
        if (tension > 550) {
            int r = 255;
            float g = std::max(decoreG * 3, 250.f);
            float b = std::max(decoreB * 3, 250.f);

            auto projectile = new ProjectileSpiralMove(barrierCenter, sf::Color(r, g, b), 10, barrierRadius + 100, angleAtack + (feature - 10) / 10.f + 180);
            projectile->angleSpeed *= -1;
            Projectiles.push_back(projectile);
        }
    }
    else if (feature < 240 && feature >= 220) {
        angleAtack -= 0.3;
        tension++; // пусть просто повысит напряжённость
    }

    else if (feature < 252 && feature >= 240) {

        #define DecoreIncreaseCoef 0.97
        #define DecoreAntiCoef 0.03

        #define DecoreIncreaseCoefGreen 0.99
        #define DecoreAntiCoefGreen 0.01

        // преобладание энергии разных частот потихоньку будет перетягивать цвет декора в свою сторону
        switch ((feature / 2) % 10) {
            case 0: decoreR = decoreR * DecoreIncreaseCoef + 255 * DecoreAntiCoef; break;
            case 1: decoreG = decoreG * DecoreIncreaseCoefGreen + 255 * DecoreAntiCoefGreen; break;
            case 2: decoreB = decoreB * DecoreIncreaseCoef + 255 * DecoreAntiCoef; break;

            case 3: decoreR = decoreR * DecoreIncreaseCoef; break;  
            case 4: decoreG = decoreG * DecoreIncreaseCoefGreen; break;
            case 5: decoreB = decoreB * DecoreIncreaseCoef; break;

            case 6: 
                decoreR = decoreR * DecoreIncreaseCoef;
                decoreB = decoreB * DecoreIncreaseCoef; 
                decoreG = decoreG * DecoreIncreaseCoef + 255 * DecoreAntiCoef;  
                break;

            case 7: 
            case 8: 

                decoreR = decoreR * DecoreIncreaseCoef + 255 * DecoreAntiCoef; 
                decoreG = decoreG * DecoreIncreaseCoefGreen + 255 * DecoreAntiCoefGreen; 
                decoreB = decoreB * DecoreIncreaseCoef + 255 * DecoreAntiCoef; 
                break;
            
            case 9: 

                decoreR = decoreR * DecoreIncreaseCoef;
                decoreG = decoreG * DecoreIncreaseCoefGreen;
                decoreB = decoreB * DecoreIncreaseCoef;

                break;

            default: break;
        }     

    }

}