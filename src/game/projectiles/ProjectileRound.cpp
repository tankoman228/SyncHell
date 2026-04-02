#include "Projectile.hpp"

void ProjectileRound::Cycle(float dt) {
    shape.move(speed * dt);

    sf::Vector2f vectorGrav = (gravityTargetPoint - shape.getPosition());
    speed += gravity * vectorGrav * dt / std::sqrt(vectorGrav.x * vectorGrav.x + vectorGrav.y * vectorGrav.y);
    // скорость меняется пропорционально времени и вектору гравитации, делённому на свою длину (т.е. выйдет единичный вектор в нужную сторону)

    this->lifeTime += dt;
}

void ProjectileRound::Build() {

    shape.setPointCount(20);
    gravity = radius * 40.f;

    // Создаем точки для круга в локальных координатах (относительно центра фигуры)
    for (int i = 0; i < 20; ++i) {
        float angle = 2 * M_PI * i / 20.f; // Полный круг 360 градусов
        float x = 32.f * std::cos(angle);
        float y = 32.f * std::sin(angle);
        shape.setPoint(i, sf::Vector2f(x, y));
    }

    damage = radius / 6.f;
    shape.setScale(radius / 32.f, radius / 32.f);

    AbstractProjectile::Build();
}