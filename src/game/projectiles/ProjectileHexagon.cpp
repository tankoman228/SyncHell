#include "Projectile.hpp"

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
        float x = radius * std::cos(angle);
        float y = radius * std::sin(angle);
        shape.setPoint(i, sf::Vector2f(x, y));
    }

    damage = radius / 3;

    AbstractProjectile::Build();
}