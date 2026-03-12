#include "Projectile.hpp"

void ProjectileTriangle::Build() {

    shape.setPointCount(3);
    shape.setPoint(0, sf::Vector2f(-5, -5)); 
    shape.setPoint(1, sf::Vector2f(5, -5));
    shape.setPoint(2, sf::Vector2f(0, 5));

    AbstractProjectile::Build();
}