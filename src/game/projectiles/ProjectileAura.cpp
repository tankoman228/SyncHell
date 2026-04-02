#include "Projectile.hpp"

void ProjectileAura::Cycle(float dt) {
    shape.setOutlineThickness(3);
    shape.setPosition(TargetedShape->getPosition());

    float alpha;
    if (lifeTime < 0.25f) {
        float x = lifeTime / 0.25f;
        alpha = 255.0f * std::sin(x * 3.14159f / 2.0f);
    }
    else if (lifeTime < 0.5f) {
        float x = (lifeTime - 0.25f) / 0.25f;
        alpha = 255.0f * std::cos(x * 3.14159f / 2.0f);
    }
    else {
        alpha = 0.0f;
    }

    // Ограничиваем значение в диапазоне [0, 255]
    alpha = std::max(0.0f, std::min(255.0f, alpha));

    shape.setOutlineColor(sf::Color(R, G, B, alpha * InitFadeKoef));
    shape.setScale(lifeTime * 21.5, lifeTime * 21.5);

    this->lifeTime += dt;
}

void ProjectileAura::Build() {

    float radius = 1;
    shape.setPointCount(40);

    // Создаем точки для круга в локальных координатах (относительно центра фигуры)
    for (int i = 0; i < 40; ++i) {
        float angle = 2 * M_PI * i / 40.f; // Полный круг 360 градусов
        float x = radius * std::cos(angle) + 32.f;
        float y = radius * std::sin(angle) + 32.f;
        shape.setPoint(i, sf::Vector2f(x, y));
    }

    shape.setFillColor(sf::Color::Transparent);
    shape.setOrigin(32, 32);
    shape.setOutlineThickness(5);

    damage = 0;
    isCollidable = false;
    DoOutineFlash = false;

    this->speed = speed;
}