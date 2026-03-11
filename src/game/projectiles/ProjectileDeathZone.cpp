#include "Projectile.hpp"

void ProjectileDeathZone::Cycle(float dt) {

    shape.move(speed * dt);
    shape.setOutlineThickness(2);

    if (lifeTime < 0.5) {
        shape.setScale(lifeTime * 2.f, lifeTime * 2.f);

        shape.setFillColor(sf::Color(128, 0, 0, lifeTime * 340));
        shape.setOutlineColor(sf::Color(255, 255, 255, lifeTime * 511.f));
    }
    else if (lifeTime < 0.75) { shape.setOutlineColor(sf::Color::White); }
    else if (lifeTime < 1) {
        shape.setFillColor(sf::Color(255, 0, 0, 255));
        isCollidable = true;
    }
    else {
        isCollidable = false;
        shape.scale(1 - dt * 19.f, 1 - dt * 19.f);
        shape.setFillColor(sf::Color(64, 0, 0, 200));

        if (shape.getScale().y < 0.08) {
            lifeTime = 9999999;
        }
    }

    lifeTime += dt;
}

void ProjectileDeathZone::Build() {

    shape.setPointCount(4);

    // Точки должны идти последовательно по контуру
    // По часовой стрелке, начиная с левого нижнего:
    shape.setPoint(0, sf::Vector2f(-width / 2, width / 2));   // левый низ
    shape.setPoint(1, sf::Vector2f(-width / 2, -width / 2));  // левый верх
    shape.setPoint(2, sf::Vector2f(width / 2, -width / 2));   // правый верх  
    shape.setPoint(3, sf::Vector2f(width / 2, width / 2));    // правый низ

    AbstractProjectile::Build();
    shape.setFillColor(sf::Color::Transparent);
    shape.setOutlineColor(sf::Color::White);
    shape.setOutlineThickness(3);

    isCollidable = false;
}