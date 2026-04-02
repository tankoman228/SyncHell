#include "Projectile.hpp"

void ProjectileTriangle::Build() {

    shape.setPointCount(3);

    for (int i = 0; i < 3; ++i) {

        float angle = 2 * M_PI * i / 3.f - 1.5708f;

        float x = 32.f * std::cos(angle);
        float y = 32.f * std::sin(angle);
        shape.setPoint(i, sf::Vector2f(x, y));
    }

    shape.setScale(radius / 32.f, radius / 32.f);

    AbstractProjectile::Build();
}