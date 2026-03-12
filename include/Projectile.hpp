#ifndef DEF_PROJECTILE
#define DEF_PROJECTILE 1

#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

#ifndef M_PI
#define M_PI 3.141592f
#endif

// Абстрактная структура снаряда
struct AbstractProjectile
{
    // То, что 99% надо задавать перед Build() после создания объекта и до добавления в физику
    sf::Vector2f    speed;    // вектор движения
    sf::Vector2f    startPos; // стартовая позиция
    sf::Color       color;    // цвет заливки
    float           startAngleDeg; // только для Build!

    bool isCollidable = true;  // взаимодействует ли с игроком (нужна для лазеров и декоративных Projectile)
    bool DoOutineFlash = true; // мигает ли обводка при появлении (для рендера, статична в рамках объекта после build)

    sf::ConvexShape shape;    // моделька для рендера
    float lifeTime = 0;       // сколько уже существует, при превышении PROJECTILES_LIFE_TIME игра уничтожит частицу
    int damage = 6;           // урон, наносимый игроку

    std::vector<sf::Vector2f> convVerticesCache;

    AbstractProjectile() {} // Конструктор всегда пустой

    // Цикл физики (в плане перемещений и изменений параметров)
    virtual void Cycle(float dt);

    // Сначала задать поля из начала структуры, а потом вызывать
    virtual void Build(); 

    void UpdateConvVerticesCache();
};

// Вспомогательные функции
sf::Vector2f Towards(const sf::Vector2f& from, const sf::Vector2f& to, float strength);
sf::Vector2f Around(const sf::Vector2f& center, float angleDeg, float length);

// Далее просто оъявления разных типов частиц. Ура, я отошёл от конструкторов вида ProjectilePentagon(sf::Vector2f position, sf::Vector2f speed, float radius, float angle_), их больше не будет!

struct ProjectileTriangle : AbstractProjectile
{
    virtual void Build();
};

struct ProjectilePentagon : AbstractProjectile
{
    float radius = 1;

    virtual void Build();
    virtual void Cycle(float t);
};

struct ProjectileHexagon : AbstractProjectile
{
    float radius = 1;

    virtual void Build();
    virtual void Cycle(float t);
};


struct ProjectileRound : AbstractProjectile
{
    sf::Vector2f gravityTargetPoint;
    float gravity = 1;
    float radius = 1;

    virtual void Build();
    virtual void Cycle(float t);
};

struct ProjectileSpiralMove : AbstractProjectile
{
    sf::Vector2f positionStart;
    float radius = 1;
    float spiralRadius = 1;
    float angleMove = 0;
    float angleSpeed = 2.0;

    virtual void Build();
    virtual void Cycle(float t);
};

struct ProjectileLazer : AbstractProjectile
{
    float rotationSpeed = 1;
    float size = 1;

    virtual void Build();
    virtual void Cycle(float t);
};

// Похож на лазер
struct ProjectileDeathZone : AbstractProjectile
{
    float width = 1;

    virtual void Build();
    virtual void Cycle(float t);
};

struct ProjectileHealingLazer : AbstractProjectile
{
    float rotationSpeed = 1;
    float size = 1;

    virtual void Build();
    virtual void Cycle(float t);
};

struct ProjectileAura : AbstractProjectile
{
    int R = 0, G = 0, B = 0; 
    float InitFadeKoef = 1;

    sf::RectangleShape *TargetedShape;

    virtual void Build();
    virtual void Cycle(float t);
};

#endif