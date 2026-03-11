#include "Projectile.hpp"

void ProjectileHealingLazer::Cycle(float dt) {

    shape.setOutlineThickness(2);

    if (lifeTime < 0.5) {
        shape.setFillColor(sf::Color(0, 64, 0, lifeTime * 340));
        shape.setOutlineColor(sf::Color(255, 255, 255, lifeTime * 255));
    }
    else if (lifeTime < 1) {
        shape.setOutlineColor(sf::Color(255, 255, 255, lifeTime * 255));
    }
    else if (lifeTime < 3) {
        shape.setFillColor(sf::Color(0, 255, 0, 255));
        isCollidable = true;
    }
    else {
        isCollidable = false;
        shape.scale(1, 1 - dt * 7.f);
        shape.setFillColor(sf::Color(0, 64, 0, 200));

        if (shape.getScale().y < 0.08) {
            lifeTime = 9999999;
        }
    }

    lifeTime += dt;
}

void ProjectileHealingLazer::Build() {

    shape.setPointCount(4);

    // Точки должны идти последовательно по контуру
    // По часовой стрелке, начиная с левого нижнего:
    shape.setPoint(0, sf::Vector2f(-3000, size));   // левый низ
    shape.setPoint(1, sf::Vector2f(-3000, -size));  // левый верх
    shape.setPoint(2, sf::Vector2f(3000, -size));   // правый верх  
    shape.setPoint(3, sf::Vector2f(3000, size));    // правый низ

    AbstractProjectile::Build();

    shape.setFillColor(sf::Color::Transparent);
    shape.setOutlineColor(sf::Color::White);
    shape.setOutlineThickness(3);
    isCollidable = false;
    damage = -40;

    this->speed = speed;
}