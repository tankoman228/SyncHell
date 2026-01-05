#include <SFML/Graphics.hpp>
#include <Game.hpp>
#include <filesystem>

namespace fs = std::filesystem;

struct LevelOption {
    sf::Text control;
    std::string filename;
};

bool endsWith(const std::string& fullString, const std::string& ending) {
    if (ending.length() > fullString.length()) {
        return false;
    }
    return fullString.compare(fullString.length() - ending.length(), ending.length(), ending) == 0;
}

int main()
{
    // Получаем разрешение экрана
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    
    // Создаем окно размером с экран (можно чуть меньше для удобства)
    sf::RenderWindow window(sf::VideoMode(desktopMode.width, desktopMode.height), 
                           "SyncHell");
    GameScene* scene = nullptr;

    // Для измерения времени
    sf::Clock clock;
    float deltaTime = 0.0f;
    
    // Настройка FPS
    window.setFramerateLimit(75); // SFML сам ограничит частоту

    std::vector<LevelOption> Levels;
    sf::Font font;
    font.loadFromFile("assets/sansation.ttf");
    int selectedLevelIndex = 0;
    sf::Text hint;
    hint.setFont(font);
    hint.setPosition(10,10);
    hint.setString("ESC: Menu\nW/S Select Level\nEnter: Play");

    try {
        int i = 0;
        for (const auto& entry : fs::directory_iterator("levels", fs::directory_options::skip_permission_denied)) {
            try {
                if (entry.is_directory()) continue;

                auto fullPath = entry.path().u8string();
                auto filename = entry.path().filename().u8string();

                if (endsWith(filename, ".ogg") ||
                    endsWith(filename, ".wav") ||
                    endsWith(filename, ".flac")) 
                {
                    LevelOption option;
                    option.control = sf::Text();
                    option.control.setFont(font);
                    option.control.setFillColor(sf::Color::White);
                    
                    option.control.setString(filename);
                    option.filename = filename;

                    Levels.push_back(option);
                }

                std::cout << filename << std::endl;
            }
            catch (const fs::filesystem_error& e) {
                std::cerr << "Skipping file: " << e.what() << std::endl;
            }
        }
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << "\n"
            << "path1: " << e.path1() << "\n"
            << "path2: " << e.path2() << std::endl;
    }
    
    while (window.isOpen())
    {
        deltaTime = clock.restart().asSeconds();
        
        // Обработка событий
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        
        // Обновление и отрисовка
        window.clear();

        if (scene != nullptr) {
            scene->Cycle(deltaTime); // Передаем время в секундах

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
                delete scene;
                scene = nullptr;
            }
        }
        else {

            window.draw(hint);

            for (int i = 0; i < Levels.size(); i++) {    
                Levels[i].control.setPosition(i == selectedLevelIndex ? 100 : 50, 150 + i * 30);
                window.draw(Levels[i].control);
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
                selectedLevelIndex--;
                if (selectedLevelIndex < 0) selectedLevelIndex = Levels.size() - 1;

                while (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {}
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                selectedLevelIndex++;
                if (selectedLevelIndex >= Levels.size()) selectedLevelIndex = 0;

                while (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {}
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) && Levels.size() > 0) {
                scene = new GameScene(&window, Levels[selectedLevelIndex].filename);
            }
        }

        window.display();
    }
    
    return 0;
}