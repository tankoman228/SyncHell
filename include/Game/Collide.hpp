#include <SFML/Graphics.hpp>

using namespace sf;

namespace GameCollider {

	// реинициализация позиции игрока
	void ReinitByRectangleShape(sf::RectangleShape& rect);

	// Основная функция проверки коллизии
	bool HasCollision(sf::ConvexShape& conv, std::vector<Vector2f>& convVertices);
}

