#include "UI/SandboxMenu.h"

#include <TGUI/Widgets/Group.hpp>
#include <TGUI/Widgets/Panel.hpp>
#include <TGUI/Widgets/HorizontalWrap.hpp>

#include <cctype> 
#include <iostream>

// Чтобы отступы нормальные отступы в списке
tgui::Panel::Ptr createLevelCardWithBottomSpacing(tgui::Panel::Ptr content, float bottomSpacing) {
    auto card = tgui::Panel::create();
    card->getRenderer()->setBackgroundColor(tgui::Color(50, 50, 50));

    // Основной контент карточки
    content->setPosition(0, 0);
    content->setSize("100%", "100% - " + tgui::String(bottomSpacing));

    // Отступ внизу
    auto spacer = tgui::Panel::create();
    spacer->setPosition(0, "100% - " + tgui::String(bottomSpacing));
    spacer->setSize("100%", bottomSpacing);
    spacer->getRenderer()->setBackgroundColor(tgui::Color::Transparent);

    card->add(content);
    card->add(spacer);

    return card;
}


void SandboxMenu::InitLevelsCards() {

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
        playButton->onClick([&, filename = level.filename]() {
            std::cout << "playButton->onClick\n";
            playBtnClick(filename, selectedShaderFilename, selectedDifficulty);
        });
        playButton->getRenderer()->setBackgroundColor(tgui::Color(70, 130, 180));
        playButton->getRenderer()->setBackgroundColorHover(tgui::Color(100, 149, 237));
        playButton->getRenderer()->setTextColor(tgui::Color::White);
        playButton->getRenderer()->setRoundedBorderRadius(8);
        playButton->getRenderer()->setTextStyle(tgui::TextStyle::Bold);

        card->add(playButton);

        return card;
    };

    // Скроллер и его дизайн
    auto scrollPanel = tgui::ScrollablePanel::create();
    scrollPanel->setSize("80%", "100% - 50");
    scrollPanel->setPosition(5, 50);

    auto scrollbarRenderer = scrollPanel->getRenderer();
    auto scrollbarData = tgui::RendererData::create();

    scrollbarData->propertyValuePairs["ThumbColor"] = tgui::Color(0, 50, 100);
    scrollbarData->propertyValuePairs["TrackColor"] = tgui::Color::Black;
    scrollbarData->propertyValuePairs["ArrowBackgroundColor"] = tgui::Color::Black;
    scrollbarData->propertyValuePairs["ArrowColor"] = tgui::Color(0, 50, 100);
    scrollbarRenderer->setProperty("Scrollbar", scrollbarData);

    // Создаем контейнер, куда будут сложены все карточки
    cardContainer = tgui::Panel::create();
    cardContainer->setSize("100%", Levels.size() * CARD_HEIGHT);
    cardContainer->getRenderer()->setBackgroundColor(tgui::Color::Black);

    for (int i = 0; i < Levels.size(); i++) {

        auto cardWrapper = tgui::Panel::create();
        cardWrapper->setHeight(CARD_HEIGHT); 

        auto levelCard = createLevelCard(Levels[i], i);
        levelCard = createLevelCardWithBottomSpacing(levelCard, 5);

        levelCard->setSize("100%", "100%");
        cardWrapper->setPosition(0, CARD_HEIGHT * i);

        cardWrapper->add(levelCard);

        cardContainer->add(cardWrapper);
    }

    scrollPanel->add(cardContainer);

    gui.add(scrollPanel);
}