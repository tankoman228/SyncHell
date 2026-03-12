#include "Projectile.hpp"

sf::Vector2f Towards(const sf::Vector2f& from, const sf::Vector2f& to, float strength) {
    
    sf::Vector2f direction = to - from;
    float length = sqrt(direction.x * direction.x + direction.y * direction.y);
    return (direction / length) * strength;
}

sf::Vector2f Around(const sf::Vector2f& center, float angleDeg, float length) {

    float angleRad = angleDeg * M_PI / 180.0f;
    return sf::Vector2f(
        center.x + length * cos(angleRad),
        center.y + length * sin(angleRad)
    );
}