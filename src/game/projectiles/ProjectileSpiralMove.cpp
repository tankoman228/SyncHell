#include "Game/Projectile.hpp"

void ProjectileSpiralMove::Cycle(float dt) {

    spiralRadius -= dt * 225; // пикселей в секунду
    angleMove += dt * angleSpeed;

    if (spiralRadius < 30) {
        shape.setScale(spiralRadius / 30.f, spiralRadius / 30.f);
    }
    if (spiralRadius < 0) {
        lifeTime = 99999999;
    }

    auto mv = sf::Vector2f(spiralRadius * cos(angleMove), spiralRadius * sin(angleMove));
    shape.setPosition(positionStart + mv);

    this->lifeTime += dt;
}

void ProjectileSpiralMove::Build() {

    shape.setPointCount(20);

    // Создаем точки для круга в локальных координатах (относительно центра фигуры)
    for (int i = 0; i < 20; ++i) {
        float angle = 2 * M_PI * i / 20.f;
        float x = 32.f * std::cos(angle) + 32.f;
        float y = 32.f * std::sin(angle) + 32.f;
        shape.setPoint(i, sf::Vector2f(x, y));
    }

    damage = radius / 1.5f;
    shape.setScale(radius / 32.f, radius / 32.f);
    shape.setOrigin(32, 32);

    AbstractProjectile::Build();
}