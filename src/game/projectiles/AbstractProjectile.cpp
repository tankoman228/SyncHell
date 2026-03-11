#include "Projectile.hpp"

void AbstractProjectile::Cycle(float dt) {
    lifeTime += dt;
    shape.move(speed * dt);
}

void AbstractProjectile::Build() {
    shape.setFillColor(color);
    shape.setRotation(startAngleDeg);
    shape.setPosition(startPos);
    shape.setOutlineColor(sf::Color(240, 240, 240));
}