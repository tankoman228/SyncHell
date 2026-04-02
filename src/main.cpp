#include <SFML/Graphics.hpp>
#include <Game.hpp>
#include <EIF.hpp> 

#include "SandboxMenu.h"

#include <chrono>
using namespace std::chrono;

int main()
{
    // Инициализация модели уха
    EIF::InitParams();

    // Окошко
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(sf::VideoMode(desktopMode.width, desktopMode.height), 
                           "SyncHell", 
                            sf::Style::None);
    window.setPosition(sf::Vector2i(0, 0));

    // FPS
    sf::Clock clock;
    float deltaTime = 0.0f;
    window.setFramerateLimit(75); // TODO: сцена настроек, вообще сделать возможность наличия множества меню

    // Сцены для рендера
    GameScene* scene = nullptr; // если нуль, то мы в меню
    SandboxMenu menu(window,
        [&window, &scene](std::string levelName, std::string shaderName, int diff) {
            std::cout << "new GameScene\n";
            scene = new GameScene(&window, levelName, diff, shaderName);

            //window.setMouseCursorVisible(0);
            window.setMouseCursorGrabbed(0);
        }
    );

    // Основной цикл программы
    while (window.isOpen())
    {
        deltaTime = clock.restart().asSeconds(); // FPS

        // Обработка событий
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (scene == nullptr) menu.HandleEvent(event);
        }
        
        // Обновление и отрисовка
        window.clear();
        if (scene != nullptr) {
            scene->Cycle(deltaTime);

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
                delete scene;
                scene = nullptr;
                window.setMouseCursorVisible(1);
                window.setMouseCursorGrabbed(1);
            }
        }
        else {
            menu.Cycle();
        }

        window.display();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::F2) && sf::Keyboard::LShift) {

            // Создаем текстуру размером с окно
            sf::Texture texture;
            texture.create(window.getSize().x, window.getSize().y);

            // Копируем содержимое активного окна в текстуру
            texture.update(window);

            auto ms = duration_cast<std::chrono::milliseconds>(system_clock::now().time_since_epoch()).count();

            // Сохраняем в файл
            sf::Image screenshot = texture.copyToImage();
            if (screenshot.saveToFile("screenshots/screenshot_" + std::to_string(ms) + ".png")) {
                // Успешно сохранено
            }
            else {
                std::cout << "Save Error of screenshot\n";
            }

            while (sf::Keyboard::isKeyPressed(sf::Keyboard::F1)) {}
        }
    }
    
    return 0;
}