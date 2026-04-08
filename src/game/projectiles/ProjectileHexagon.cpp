#include "Game/Projectile.hpp"

void ProjectileHexagon::Cycle(float dt) {
    shape.setFillColor(sf::Color(255, 255 - lifeTime * 50, 255 - lifeTime * 50));

    shape.move(speed * dt);
    speed.x -= dt * 230.f;

    lifeTime += dt * 0.3f; // пусть живёт дольше
}

void ProjectileHexagon::Build() {

    shape.setPointCount(6);

    // Создаем точки для круга в локальных координатах (относительно центра фигуры)
    for (int i = 0; i < 6; ++i) {
        float angle = 2 * M_PI * i / 6.f; // Полный круг 360 градусов
        float x = 32.f * std::cos(angle) + 32.f;
        float y = 32.f * std::sin(angle) + 32.f;
        shape.setPoint(i, sf::Vector2f(x, y));
    }

    shape.setScale(radius / 32.f, radius / 32.f);
    shape.setOrigin(32, 32);
    damage = radius / 3;

    AbstractProjectile::Build();
    shape.setOutlineColor(sf::Color(0, 0, 0));
}