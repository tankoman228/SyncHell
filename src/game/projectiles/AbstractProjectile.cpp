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

    this->convVerticesCache = std::vector<sf::Vector2f>(shape.getPointCount());
}

void AbstractProjectile::UpdateConvVerticesCache() {

    sf::Transform transform = shape.getTransform();

    for (size_t i = 0; i < this->convVerticesCache.size(); i++) {
        convVerticesCache[i] = transform.transformPoint(shape.getPoint(i));
    }
}