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

void GameScene::HandleInput()
{
    // управление мышью
    auto cursorPosI = sf::Mouse::getPosition(*window); // типы разные, зараза
    auto cursorPos  = sf::Vector2f(cursorPosI.x, cursorPosI.y);
    auto diff = player.getPosition() - cursorPos;
    auto l = std::sqrt(diff.x * diff.x + diff.y * diff.y);

    // защита от деления на 0 в Towards
    if (l > 0.01) {
        auto dir = Towards(player.getPosition(), cursorPos, l * dt * playerSpeed / 20.0);
        player.move(dir);

        while (!IsInsideBarrier(player.getPosition()))
        {
            player.move(Towards(player.getPosition(), barrierCenter, 1.0f));
        }
        window->setMouseCursorVisible(!IsInsideBarrier(cursorPos));
    }

    return; // Отключил, будет щя другой режим


    auto playerNewC = player.getPosition();

    // Проверяем нажатые клавиши
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::InitFadeKoef))
    {
        playerNewC.x -= playerSpeed * dt;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        playerNewC.x += playerSpeed * dt;
    }

    if (IsInsideBarrier(playerNewC))
    {
        player.setPosition(playerNewC);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        playerNewC.y -= playerSpeed * dt;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        playerNewC.y += playerSpeed * dt;
    }

    if (IsInsideBarrier(playerNewC))
    {
        player.setPosition(playerNewC);
    }
    
}
