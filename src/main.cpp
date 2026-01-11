#include <SFML/Graphics.hpp>
#include <Game.hpp>
#include <filesystem>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/Widgets/Group.hpp>
#include <TGUI/Widgets/Panel.hpp>
#include <TGUI/Widgets/HorizontalWrap.hpp>
#include <cctype> // Required for std::tolower

namespace fs = std::filesystem;

struct LevelOption {
    std::string filename;
    std::string author = "?";
    std::string duration = "?";
    bool isTutorial = false;

    LevelOption() {}
    LevelOption(
        std::string filename_, 
        std::string author_, 
        bool isTutorial_) {

            // TODO: убрать этот костыль и разделить туториальные от обычных
            filename = filename_; author = author_; isTutorial = isTutorial_;
            duration = "Difficulty is always ★★";
        }
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

    // FPS
    sf::Clock clock;
    float deltaTime = 0.0f;
    window.setFramerateLimit(75); 

    // Загрузка списка уровней
    GameScene* scene = nullptr; // если нуль, то мы в меню
    std::vector<LevelOption> Levels;

    Levels.push_back(LevelOption("tutorial/Investigations.ogg", "Kevin MacLeod (CC BY 3.0)", true));
    Levels.push_back(LevelOption("tutorial/Jingle-Bells-3.ogg", "Kevin MacLeod (CC BY 3.0)", true));
    Levels.push_back(LevelOption("tutorial/Run-Amok.ogg",       "Kevin MacLeod (CC BY 3.0)", true));
    Levels.push_back(LevelOption("tutorial/We-Wish-You.ogg",    "Kevin MacLeod (CC BY 3.0)", true));

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

    static int difficulty = 3; // по умолчанию 3, если трогать константу, то править код нижже

    tgui::Gui gui{window};  // Создаем GUI, привязанный к окну
    auto scrollPanel = tgui::ScrollablePanel::create();
    scrollPanel->setSize("80%", "100% - 50");
    scrollPanel->setPosition(5, 50);

    auto scrollbarRenderer = scrollPanel->getRenderer();
    auto scrollbarData = tgui::RendererData::create();
  
    scrollbarData->propertyValuePairs["ThumbColor"] = tgui::Color(0,50,100);
    scrollbarData->propertyValuePairs["TrackColor"] = tgui::Color::Black;
    scrollbarData->propertyValuePairs["ArrowBackgroundColor"] = tgui::Color::Black;
    scrollbarData->propertyValuePairs["ArrowColor"] = tgui::Color(0,50,100);
    scrollbarRenderer->setProperty("Scrollbar", scrollbarData);

    // Рассчитываем общую высоту всех карточек
    const float CARD_HEIGHT = 110;

    // Создаем контейнер для карточек
    auto cardContainer =  tgui::Panel::create();
    cardContainer->setSize("100%", Levels.size() * CARD_HEIGHT);
    cardContainer->getRenderer()->setBackgroundColor(tgui::Color::Black);

    // Функция создания карточки
    auto createLevelCard = [&](const LevelOption& level, int index) {
        // Панель как карточка
        auto card = tgui::Panel::create();
        card->setSize("100%", CARD_HEIGHT); 
        
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
            scene = new GameScene(&window, Levels[index].filename, Levels[index].isTutorial ? 2 : difficulty);
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
        cardWrapper->setPosition(0, CARD_HEIGHT * i);

        cardWrapper->add(levelCard);
        
        cardContainer->add(cardWrapper);
    }

    scrollPanel->add(cardContainer);
    gui.add(scrollPanel);

    // Создаем текстовое поле для поиска
    auto searchBox = tgui::EditBox::create();
    searchBox->setPosition(10, 10);
    searchBox->setSize("80% - 10", 30);
    searchBox->setDefaultText("Search levels...");
    searchBox->getRenderer()->setBackgroundColor(tgui::Color::Black);
    searchBox->getRenderer()->setTextColor(tgui::Color(200,200,200));

    // Добавляем обработчик ввода в текстовое поле
    searchBox->onTextChange([searchBox, cardContainer, Levels, CARD_HEIGHT]() {
        std::string searchText = searchBox->getText().toStdString();
        std::vector<std::pair<tgui::Panel::Ptr, bool>> widgetStates;

        // Проходим по всем оберткам карточек
        for (size_t i = 0; i < cardContainer->getWidgets().size(); i++) {
            auto wrapper = cardContainer->getWidgets()[i];
            auto panel = std::dynamic_pointer_cast<tgui::Panel>(wrapper);
            if (panel && !panel->getWidgets().empty()) {
                // Получаем уровень по индексу
                if (i < Levels.size()) {
                    const auto level = Levels[i];

                    // Проверяем совпадение
                    bool matches = searchText.empty() ||
                                level.filename.find(searchText) != std::string::npos ||
                                level.author.find(searchText) != std::string::npos;
                    
                    widgetStates.push_back({panel, matches});
                }
            }
        }

        // Затем применяем изменения
        int matchesCount = 0;
        for (const auto& [panel, matches] : widgetStates) {
            panel->setPosition(0, matches ? CARD_HEIGHT * matchesCount : (Levels.size() - 1) * CARD_HEIGHT);
            if (matches) {               
                matchesCount++;
            }
        }
    });

    gui.add(searchBox);

    // Создаем ползунок сложности справа внизу
    auto difficultySlider = tgui::Slider::create(1, 5);
    difficultySlider->setPosition("80% + 5", "100% - 80");
    difficultySlider->setSize("20% - 20", 20);
    difficultySlider->setValue(3); // Значение по умолчанию
    difficultySlider->setStep(1); // Только целые значения
    difficultySlider->getRenderer()->setTrackColor(tgui::Color(0,150,230));
    difficultySlider->getRenderer()->setThumbColor(tgui::Color(0,150,230));

    // Создаем Label для отображения текущей сложности
    auto difficultyLabel = tgui::Label::create("Difficulty: ★★★");
    difficultyLabel->setPosition("80% + 5", "100% - 50");
    difficultyLabel->setTextSize(24);
    difficultyLabel->getRenderer()->setTextColor(tgui::Color(200,200,200));

    // Обработчик изменения значения ползунка
    difficultySlider->onValueChange([difficultyLabel](float value) {
        difficulty = static_cast<int>(value);
        std::string text = "Difficulty: ";
        for (int i = 0; i < difficulty; i++) {
            text += "★";
        }

        difficultyLabel->setText(text);
    });

    gui.add(difficultyLabel);
    gui.add(difficultySlider);

    // Создаем кнопку About чуть выше регулятора сложности
    auto aboutButton = tgui::Button::create("About / Copyright");
    aboutButton->setPosition("80% + 5", 10);
    aboutButton->getRenderer()->setBackgroundColor(tgui::Color(10,10,10));
    aboutButton->getRenderer()->setTextColor(tgui::Color(200,200,200));
    aboutButton->setSize("20% - 15", 40);
    aboutButton->onClick([]() {

        std::string filePath = "README.md";
        #ifdef _WIN32
            // Windows
            ShellExecuteA(NULL, "open", filePath.c_str(), NULL, NULL, SW_SHOWNORMAL);
        #elif __APPLE__
            // macOS
            std::string command = "open \"" + filePath + "\"";
            system(command.c_str());
        #else
            // Linux
            std::string command = "xdg-open \"" + filePath + "\"";
            system(command.c_str());
        #endif
    });

    gui.add(aboutButton);

    while (window.isOpen())
    {
        deltaTime = clock.restart().asSeconds();
        
        // Обработка событий
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (scene == nullptr) gui.handleEvent(event);
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
            gui.draw();  // Отрисовываем GUI поверх всего
        }

        window.display();
    }
    
    return 0;
}