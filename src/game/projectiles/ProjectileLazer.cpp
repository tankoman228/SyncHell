#include "Game/Projectile.hpp"

void ProjectileLazer::Cycle(float dt) {

    shape.move(speed * dt);
    shape.setOutlineThickness(2);
    shape.rotate(dt * rotationSpeed);

    if (lifeTime < 0.5) {
        shape.setFillColor(sf::Color(64, 0, 0, lifeTime * 340));
        shape.setOutlineColor(sf::Color(255, 255, 255, lifeTime * 255));
    }
    else if (lifeTime < 1) {
        shape.setOutlineColor(sf::Color(255, 255, 255, lifeTime * 255));
    }
    else if (lifeTime < 1.25) {
        shape.setFillColor(sf::Color(255, 0, 0, 255));
        isCollidable = true;
    }
    else {
        isCollidable = false;
        speed = sf::Vector2f(0, 0);
        rotationSpeed = 0;
        shape.scale(1, 1 - dt * 7.f);
        shape.setFillColor(sf::Color(64, 0, 0, 200));
        shape.setOutlineColor(sf::Color(255, 255, 255, 254.f * shape.getScale().y));

        if (shape.getScale().y < 0.08) {
            lifeTime = 9999999;
        }
    }

    lifeTime += dt;
}

void ProjectileLazer::Build() {

    shape.setPointCount(4);

    // Точки должны идти последовательно по контуру
    // По часовой стрелке, начиная с левого нижнего:
    shape.setPoint(0, sf::Vector2f(-3000, size));   // левый низ
    shape.setPoint(1, sf::Vector2f(-3000, -size));  // левый верх
    shape.setPoint(2, sf::Vector2f(3000, -size));   // правый верх  
    shape.setPoint(3, sf::Vector2f(3000, size));    // правый низ

    AbstractProjectile::Build();

    damage = size;
    shape.setFillColor(sf::Color::Transparent);
    isCollidable = false;
    DoOutineFlash = false;
}