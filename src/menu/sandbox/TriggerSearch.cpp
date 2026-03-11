#include "SandboxMenu.h"

void SandboxMenu::TriggerSearch() {
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

                widgetStates.push_back({ panel, matches });
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
}