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

int controlMode = 0;

void GameScene::HandleInput()
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num0)) {
        controlMode = 0;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) {
        controlMode = 1;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) {
        controlMode = 2;
    }

    switch (controlMode)
    {
        case 0:
        {
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

            player.setPosition(playerNewC);

            break;
        }

        case 1:
        {
            // управление мышью
            auto cursorPosI = sf::Mouse::getPosition(*window); // типы разные, зараза
            auto cursorPos = sf::Vector2f(cursorPosI.x, cursorPosI.y);
            auto diff = player.getPosition() - cursorPos;
            auto l = std::sqrt(diff.x * diff.x + diff.y * diff.y);

            // защита от деления на 0 в Towards
            if (l > 0.01) {
                auto dir = Towards(player.getPosition(), cursorPos, l * dt * playerSpeed / 20.0);
                player.move(dir);
                window->setMouseCursorVisible(!IsInsideBarrier(cursorPos));
            }

            break;
        }

        case 2: 
        {
            sf::Vector2f moveVector(0.f, 0.f);

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad1)) {
                moveVector += sf::Vector2f(-0.7f, 0.7f);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad2)) {
                moveVector += sf::Vector2f(0.f, 1.f);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad3)) {
                moveVector += sf::Vector2f(0.7f, 0.7f);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad4)) {
                moveVector += sf::Vector2f(-1.f, 0.f);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad6)) {
                moveVector += sf::Vector2f(1.f, 0.f);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad7)) {
                moveVector += sf::Vector2f(-0.7f, -0.7f);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad8)) {
                moveVector += sf::Vector2f(0.f, -1.f);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad9)) {
                moveVector += sf::Vector2f(0.7f, -0.7f);
            }

            player.move(moveVector * playerSpeed * dt);

            break;
        }

        default: break;
    }

    while (!IsInsideBarrier(player.getPosition()))
    {
        player.move(Towards(player.getPosition(), barrierCenter, 1.0f));
    }
}
