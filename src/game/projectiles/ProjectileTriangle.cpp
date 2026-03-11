#include "Projectile.hpp"

void ProjectileTriangle::Build() {

    shape.setPointCount(3);
    shape.setPoint(0, sf::Vector2f(10, 10));
    shape.setPoint(1, sf::Vector2f(20, 10));
    shape.setPoint(2, sf::Vector2f(15, 20));

    AbstractProjectile::Build();
}