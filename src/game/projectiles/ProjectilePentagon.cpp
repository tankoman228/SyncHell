#include "Projectile.hpp"

void ProjectilePentagon::Cycle(float dt) {

    auto pos = shape.getPosition();

    // Периливается
    int dynamicRed = 128 + static_cast<int>(63.5f * (1.0f + std::sin(lifeTime * 4.0f)));

    // Синее смещение к концу жизни
    int green = std::max(0, 255 - static_cast<int>(lifeTime * 80));
    int blue = 255; 

    shape.setFillColor(sf::Color(dynamicRed, green, blue));

    shape.move(speed * dt);
    speed.y += dt * 600;

    lifeTime += dt;
}

void ProjectilePentagon::Build() {

    shape.setPointCount(5);

    // Создаем точки для круга в локальных координатах (относительно центра фигуры)
    for (int i = 0; i < 5; ++i) {
        float angle = 2 * M_PI * i / 5.f; // Полный круг 360 градусов
        float x = radius * std::cos(angle);
        float y = radius * std::sin(angle);
        shape.setPoint(i, sf::Vector2f(x, y));
    }

    damage = radius / 2;

    AbstractProjectile::Build();
    shape.setOutlineColor(sf::Color(0, 0, 0));
}