#include <SFML/Graphics.hpp>
#include <Game.hpp>
#include <filesystem>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/Widgets/Group.hpp>
#include <TGUI/Widgets/Panel.hpp>
#include <TGUI/Widgets/HorizontalWrap.hpp>


namespace fs = std::filesystem;

struct LevelOption {
    sf::Text control;
    std::string filename;
    std::string author = "?";
    std::string duration = "?";
};

bool endsWith(const std::string& fullString, const std::string& ending) {
    if (ending.length() > fullString.length()) {
        return false;
    }
    return fullString.compare(fullString.length() - ending.length(), ending.length(), ending) == 0;
}

tgui::Panel::Ptr createLevelCardWithBottomSpacing(tgui::Panel::Ptr content, float bottomSpacing) {
    auto card = tgui::Panel::create();
    card->getRenderer()->setBackgroundColor(tgui::Color(50, 50, 50));
    
    // Основной контент карточки
    content->setPosition(0, 0);
    content->setSize("100%", "100% - " + tgui::String(bottomSpacing));
    
    // "Фейковый" отступ внизу (пустая панель)
    auto spacer = tgui::Panel::create();
    spacer->setPosition(0, "100% - " + tgui::String(bottomSpacing));
    spacer->setSize("100%", bottomSpacing);
    spacer->getRenderer()->setBackgroundColor(tgui::Color::Transparent);
    
    card->add(content);
    card->add(spacer);
    
    return card;
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
    hint.setPosition(10,20);
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


    // !Файлы просканированы, инициализируем сам GUI!

    tgui::Gui gui{window};  // Создаем GUI, привязанный к окну
    auto scrollPanel = tgui::ScrollablePanel::create();
    scrollPanel->setSize("80%", "99%");

    auto scrollbarRenderer = scrollPanel->getRenderer();
    auto scrollbarData = tgui::RendererData::create();
  
    scrollbarData->propertyValuePairs["ThumbColor"] = tgui::Color(0,50,100);
    scrollbarData->propertyValuePairs["TrackColor"] = tgui::Color::Black;
    scrollbarData->propertyValuePairs["ArrowBackgroundColor"] = tgui::Color::Black;
    scrollbarData->propertyValuePairs["ArrowColor"] = tgui::Color(0,50,100);

    scrollbarRenderer->setProperty("Scrollbar", scrollbarData);


    // Рассчитываем общую высоту всех карточек
    const float CARD_HEIGHT = 110;
    float totalHeight = Levels.size() * CARD_HEIGHT;

    // Создаем контейнер для карточек
    auto cardContainer =  tgui::VerticalLayout::create();
    cardContainer->setSize("100%", totalHeight); // Начальная высота 50

    // Функция создания карточки
    auto createLevelCard = [&](const LevelOption& level, int index) {
        // Панель как карточка
        auto card = tgui::Panel::create();
        card->setSize("95%", CARD_HEIGHT); 
        
        // Градиентный фон
        card->getRenderer()->setBackgroundColor(tgui::Color(30, 33, 40));
        
        // Внешний вид карточки
        card->getRenderer()->setRoundedBorderRadius(12);
        card->getRenderer()->setBorders(2);
        card->getRenderer()->setBorderColor(tgui::Color(70, 130, 180));
        
        // Название уровня (БОЛЬШОЕ)
        auto title = tgui::Label::create(level.filename);
        title->setPosition(20, 10);
        title->setSize("100% - 100", 40);
        title->setTextSize(26);
        title->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Left);
        title->getRenderer()->setTextColor(tgui::Color::White);
        title->getRenderer()->setTextStyle(tgui::TextStyle::Bold);
        card->add(title);
        
        auto authorLabel = tgui::Label::create(level.author);
        authorLabel->setPosition(20, 65);
        authorLabel->setTextSize(16);
        authorLabel->getRenderer()->setTextColor(tgui::Color::White);
        card->add(authorLabel);
        
        auto durationValue = tgui::Label::create(level.duration);
        durationValue->setPosition("50%", 65);
        durationValue->setTextSize(16);
        durationValue->getRenderer()->setTextColor(tgui::Color::White);
        card->add(durationValue);

        auto playButton = tgui::Button::create(" ▶");
        playButton->setSize(170, "80%");
        playButton->setPosition("100% - 180", 10);
        playButton->setTextSize(38);
        playButton->onClick([&, index]() {
            selectedLevelIndex = index;
            scene = new GameScene(&window, Levels[index].filename);
        });
        playButton->getRenderer()->setBackgroundColor(tgui::Color(70, 130, 180));
        playButton->getRenderer()->setBackgroundColorHover(tgui::Color(100, 149, 237));
        playButton->getRenderer()->setTextColor(tgui::Color::White);
        playButton->getRenderer()->setRoundedBorderRadius(8);
        playButton->getRenderer()->setTextStyle(tgui::TextStyle::Bold);
        card->add(playButton);

        return card;
    };

    // Заполняем карточками
    for (int i = 0; i < Levels.size(); i++) {
        // УБИРАЕМ setSize из createLevelCard, если он там есть
        // Вместо этого создаем обертку для карточки
        auto cardWrapper = tgui::Panel::create();
        cardWrapper->setHeight(CARD_HEIGHT); // ТОЛЬКО здесь задаем высоту!
        
        auto levelCard = createLevelCard(Levels[i], i);
        levelCard = createLevelCardWithBottomSpacing(levelCard, 5);

        levelCard->setSize("100%", "100%");
        cardWrapper->add(levelCard);
        
        cardContainer->add(cardWrapper);
    }

    scrollPanel->add(cardContainer);
    gui.add(scrollPanel);


    while (window.isOpen())
    {
        deltaTime = clock.restart().asSeconds();
        
        // Обработка событий
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            gui.handleEvent(event);
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

            //window.draw(hint);


            gui.draw();  // Отрисовываем GUI поверх всего

            /*
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
            }*/
        }

        window.display();
    }
    
    return 0;
}