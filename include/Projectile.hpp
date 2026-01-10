#ifndef DEF_PROJECTILE
#define DEF_PROJECTILE 1

#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

struct Projectile
{
    sf::ConvexShape shape;
    sf::Vector2f speed;
    float lifeTime = 0;
    bool isCollidable = true;
    int damage = 7;

    Projectile() {
    }

    virtual void Cycle(float t) {
       lifeTime += t;
    }
};

struct ProjectileTriangle : Projectile
{
    ProjectileTriangle(sf::Vector2f position, sf::Vector2f speed, sf::Color color, int Angle) {

        shape.setPointCount(3); // Треугольник имеет 3 вершины
        shape.setPoint(0, sf::Vector2f(10, 10)); // Первая вершина
        shape.setPoint(1, sf::Vector2f(20, 10)); // Вторая вершина
        shape.setPoint(2, sf::Vector2f(15, 20)); // Третья вершина

        shape.setFillColor(color);
        shape.setRotation(Angle);
        shape.setPosition(position);
        shape.setOutlineColor(sf::Color(240,240,240));

        damage = 10;
        this->speed = speed;
    }

    virtual void Cycle(float t) {
        shape.move(speed * t);
        lifeTime += t;
    }
};

struct ProjectilePentagon : Projectile
{
    ProjectilePentagon(sf::Vector2f position, sf::Vector2f speed, float radius, float angle_) {

        shape.setPointCount(5); 

        // Создаем точки для круга в локальных координатах (относительно центра фигуры)
        for (int i = 0; i < 5; ++i) {
            float angle = 2 * M_PI * i / 5.f; // Полный круг 360 градусов
            float x = radius * std::cos(angle);
            float y = radius * std::sin(angle);
            shape.setPoint(i, sf::Vector2f(x, y));
        }

        damage = radius / 2;

        shape.setRotation(angle_);
        shape.setPosition(position);
        shape.setOutlineColor(sf::Color(255,255,255));

        this->speed = speed;
    }

    virtual void Cycle(float t) {

        auto pos = shape.getPosition();
        shape.setFillColor(sf::Color(int(pos.x) % 128 + 128, 255 - lifeTime * 50, 255 - lifeTime * 50));

        shape.move(speed * t);
        speed.y += t * 600;

        lifeTime += t;
    }
};

struct ProjectileHexagon : Projectile
{
    ProjectileHexagon(sf::Vector2f position, sf::Vector2f speed, float radius, float angle_) {

        shape.setPointCount(6); 

        // Создаем точки для круга в локальных координатах (относительно центра фигуры)
        for (int i = 0; i < 6; ++i) {
            float angle = 2 * M_PI * i / 6.f; // Полный круг 360 градусов
            float x = radius * std::cos(angle);
            float y = radius * std::sin(angle);
            shape.setPoint(i, sf::Vector2f(x, y));
        }

        damage = radius / 3;

        shape.setRotation(angle_);
        shape.setPosition(position);
        shape.setOutlineColor(sf::Color(255,255,255));

        this->speed = speed;
    }

    virtual void Cycle(float t) {

        shape.setFillColor(sf::Color(255, 255 - lifeTime * 50, 255 - lifeTime * 50));

        shape.move(speed * t);
        speed.x -= t * 230.f;

        lifeTime += t * 0.3f; // пусть живёт дольше
    }
};


struct ProjectileRound : Projectile
{
    sf::Vector2f positionStart;
    float gravity;

    ProjectileRound(sf::Vector2f position, sf::Vector2f speed, sf::Color color, int radius, float gravityMultiply) {
        positionStart = position;
        shape.setPointCount(20);
        gravity = gravityMultiply * 0.03f;
        
        // Создаем точки для круга в локальных координатах (относительно центра фигуры)
        for (int i = 0; i < 20; ++i) {
            float angle = 2 * M_PI * i / 20.f; // Полный круг 360 градусов
            float x = radius * std::cos(angle);
            float y = radius * std::sin(angle);
            shape.setPoint(i, sf::Vector2f(x, y));
        }

        shape.setFillColor(color);
        shape.setPosition(position); // Устанавливаем позицию фигуры
        shape.setOutlineColor(sf::Color(190,190,190));
        shape.setOutlineThickness(3);

        damage = radius / 3;
        
        this->speed = speed;
    }

    virtual void Cycle(float t) {

        shape.move(speed * t);
        speed += (positionStart - shape.getPosition()) * gravity;
        gravity -= 0.005f * t;

        if (t > 3) {
            gravity = 0;
        }

        this->lifeTime += t;
    }
};

struct ProjectileSpiralMove : Projectile
{
    sf::Vector2f positionStart;
    float spiralRadius;
    float angleMove = 0;
    float angleSpeed = 2;

    ProjectileSpiralMove(sf::Vector2f position, sf::Color color, int radius, float radiusSpiral, float angle_) {
        positionStart = position;
        shape.setPointCount(20);
        spiralRadius = radiusSpiral;
        angleMove = angle_;
        
        // Создаем точки для круга в локальных координатах (относительно центра фигуры)
        for (int i = 0; i < 20; ++i) {
            float angle = 2 * M_PI * i / 20.f;
            float x = radius * std::cos(angle);
            float y = radius * std::sin(angle);
            shape.setPoint(i, sf::Vector2f(x, y));
        }

        shape.setFillColor(color);
        shape.setPosition(position); 
        shape.setOutlineThickness(3);

        damage = radius / 1.5f;
        
        this->speed = speed;
    }

    virtual void Cycle(float t) {

        spiralRadius -= t * 325; // пикселей в секунду
        angleMove += t * angleSpeed;

        if (spiralRadius < 30) {
            shape.setScale(spiralRadius / 30.f, spiralRadius / 30.f);
        }
        if (spiralRadius < 0) {
            lifeTime = 99999999;
        }

        auto mv = sf::Vector2f(spiralRadius * cos(angleMove), spiralRadius * sin(angleMove));
        shape.setPosition(positionStart + mv);

        this->lifeTime += t;
    }
};

struct ProjectileLazer : Projectile
{
    float rSpeed;

    ProjectileLazer(sf::Vector2f position, float angle, float size, float rotateSpeed) {

        shape.setPointCount(4);
        rSpeed = rotateSpeed;
        
        // Точки должны идти последовательно по контуру
        // По часовой стрелке, начиная с левого нижнего:
        shape.setPoint(0, sf::Vector2f(-3000, size));   // левый низ
        shape.setPoint(1, sf::Vector2f(-3000, -size));  // левый верх
        shape.setPoint(2, sf::Vector2f(3000, -size));   // правый верх  
        shape.setPoint(3, sf::Vector2f(3000, size));    // правый низ

        shape.setPosition(position);
        shape.setOutlineColor(sf::Color::White);
        shape.setOutlineThickness(3);
        shape.rotate(angle);

        damage = size;

        shape.setFillColor(sf::Color::Transparent);

        isCollidable = false;
        
        this->speed = speed;
    }

    virtual void Cycle(float dt) {

        shape.move(speed * dt);
        shape.setOutlineThickness(2);
        shape.rotate(dt * rSpeed);

        if (lifeTime < 0.5) {
            shape.setFillColor(sf::Color(64, 0, 0, lifeTime * 340));
            shape.setOutlineColor(sf::Color(255, 255, 255, lifeTime * 255));
        }
        else if (lifeTime < 1) {
            shape.setOutlineColor(sf::Color(255, 255, 255, lifeTime * 255));
        }
        else if (lifeTime < 1.25) {
            shape.setFillColor(sf::Color(255, 0, 0, 255));
            isCollidable = true;
        }
        else {
            isCollidable = false;
            shape.scale(1, 1 - dt * 7.f);
            shape.setFillColor(sf::Color(64, 0, 0, 200));
            shape.setOutlineColor(sf::Color(255, 255, 255, 254.f * shape.getScale().y));

            if (shape.getScale().y < 0.08) {
                lifeTime = 9999999;
            }
        }

        lifeTime += dt;
    }

    ~ProjectileLazer() {
        std::cout << "deleted lazer\n";
    }
};

// Похож на лазер
struct ProjectileDeathZone : Projectile
{
    ProjectileDeathZone(sf::Vector2f position, float width, float angle, float damage_) {

        shape.setPointCount(4);
        
        // Точки должны идти последовательно по контуру
        // По часовой стрелке, начиная с левого нижнего:
        shape.setPoint(0, sf::Vector2f(-width / 2, width / 2));   // левый низ
        shape.setPoint(1, sf::Vector2f(-width / 2, -width / 2));  // левый верх
        shape.setPoint(2, sf::Vector2f(width / 2, -width / 2));   // правый верх  
        shape.setPoint(3, sf::Vector2f(width / 2, width / 2));    // правый низ

        shape.setPosition(position);
        shape.setOutlineColor(sf::Color::White);
        shape.setOutlineThickness(3);
        shape.rotate(angle);

        damage = damage_;

        shape.setFillColor(sf::Color::Transparent);

        isCollidable = false;
        
        this->speed = speed;
    }

    virtual void Cycle(float dt) {

        shape.move(speed * dt);
        shape.setOutlineThickness(2);

        if (lifeTime < 0.5) {
            shape.setScale(lifeTime * 2.f, lifeTime * 2.f);

            shape.setFillColor(sf::Color(128, 0, 0, lifeTime * 340));
            shape.setOutlineColor(sf::Color(255, 255, 255, lifeTime * 511.f));
        }
        else if (lifeTime < 0.75) {shape.setOutlineColor(sf::Color::White);}
        else if (lifeTime < 1) {
            shape.setFillColor(sf::Color(255, 0, 0, 255));
            isCollidable = true;
        }
        else {
            isCollidable = false;
            shape.scale(1 - dt * 19.f, 1 - dt * 19.f);
            shape.setFillColor(sf::Color(64, 0, 0, 200));

            if (shape.getScale().y < 0.08) {
                lifeTime = 9999999;
            }
        }

        lifeTime += dt;
    }
};

struct ProjectileHealingLazer : Projectile
{
    float rSpeed;

    ProjectileHealingLazer(sf::Vector2f position, float angle, float size, float rotateSpeed) {

        shape.setPointCount(4);
        rSpeed = rotateSpeed;
        
        // Точки должны идти последовательно по контуру
        // По часовой стрелке, начиная с левого нижнего:
        shape.setPoint(0, sf::Vector2f(-3000, size));   // левый низ
        shape.setPoint(1, sf::Vector2f(-3000, -size));  // левый верх
        shape.setPoint(2, sf::Vector2f(3000, -size));   // правый верх  
        shape.setPoint(3, sf::Vector2f(3000, size));    // правый низ

        shape.setPosition(position); // Устанавливаем позицию фигуры
        shape.setOutlineColor(sf::Color::White);
        shape.setOutlineThickness(3);
        shape.rotate(angle);

        damage = -40;

        shape.setFillColor(sf::Color::Transparent);

        isCollidable = false;
        
        this->speed = speed;
    }

    virtual void Cycle(float dt) {

        shape.setOutlineThickness(2);

        if (lifeTime < 0.5) {
            shape.setFillColor(sf::Color(0, 64, 0, lifeTime * 340));
            shape.setOutlineColor(sf::Color(255, 255, 255, lifeTime * 255));
        }
        else if (lifeTime < 1) {
            shape.setOutlineColor(sf::Color(255, 255, 255, lifeTime * 255));
        }
        else if (lifeTime < 3) {
            shape.setFillColor(sf::Color(0, 255, 0, 255));
            isCollidable = true;
        }
        else {
            isCollidable = false;
            shape.scale(1, 1 - dt * 7.f);
            shape.setFillColor(sf::Color(0, 64, 0, 200));

            if (shape.getScale().y < 0.08) {
                lifeTime = 9999999;
            }
        }

        lifeTime += dt;
    }
};

struct ProjectileAura : Projectile
{
    int R, G, B; float A;
    sf::RectangleShape *position;

    ProjectileAura(sf::RectangleShape& position_, int r, int g, int b, float a) {

        R = r; G = g; B = b; A = a;
        float radius = 1;
        shape.setPointCount(40);

        position = &position_;
        
        // Создаем точки для круга в локальных координатах (относительно центра фигуры)
        for (int i = 0; i < 40; ++i) {
            float angle = 2 * M_PI * i / 40.f; // Полный круг 360 градусов
            float x = radius * std::cos(angle);
            float y = radius * std::sin(angle);
            shape.setPoint(i, sf::Vector2f(x, y));
        }

        shape.setFillColor(sf::Color::Transparent);
        shape.setOutlineThickness(5);

        damage = 0;
        isCollidable = false;
        
        this->speed = speed;
    }

    virtual void Cycle(float t) {

        shape.setOutlineThickness(3);
        shape.setPosition(position->getPosition()); 

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

        shape.setOutlineColor(sf::Color(R, G, B, alpha * A));
        shape.setScale(lifeTime * 21.5, lifeTime * 21.5);

        this->lifeTime += t;
    }
};

#endif