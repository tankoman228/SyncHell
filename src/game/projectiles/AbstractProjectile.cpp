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

    static sf::Texture* whiteTexture = nullptr; // TODO: вынести инициализацию
    if (whiteTexture == nullptr) {
        sf::Image img;
        img.create(64, 64, sf::Color::White); // Наша константа 64
        whiteTexture = new sf::Texture();
        whiteTexture->loadFromImage(img);
    }

    shape.setTexture(whiteTexture); // Теперь SFML создаст UV-сетку
    shape.setTextureRect(sf::IntRect(0, 0, 64, 64)); // TODO: вынести в константу куда-нибудь
}

void AbstractProjectile::UpdateConvVerticesCache() {

    sf::Transform transform = shape.getTransform();

    for (size_t i = 0; i < this->convVerticesCache.size(); i++) {
        convVerticesCache[i] = transform.transformPoint(shape.getPoint(i));
    }
}