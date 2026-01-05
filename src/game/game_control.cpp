#include <Game.hpp>

bool GameScene::IsInsideBarrier(const sf::Vector2f &point)
{
    // Вычисляем расстояние от точки до центра барьера
    float dx = point.x - barrierCenter.x;
    float dy = point.y - barrierCenter.y;
    float distance = std::sqrt(dx * dx + dy * dy);

    // Проверяем, находится ли точка внутри барьера
    // Учитываем половину размера игрока для более точной коллизии
    float playerHalfSize = playerSize / 2;
    return distance < (barrierRadius - playerHalfSize);
}

void GameScene::HandleInput(float t)
{
    auto playerNewC = player.getPosition();

    // Проверяем нажатые клавиши
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        playerNewC.x -= playerSpeed * t;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        playerNewC.x += playerSpeed * t;
    }

    if (IsInsideBarrier(playerNewC))
    {
        player.setPosition(playerNewC);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        playerNewC.y -= playerSpeed * t;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        playerNewC.y += playerSpeed * t;
    }

    if (IsInsideBarrier(playerNewC))
    {
        player.setPosition(playerNewC);
    }
}
