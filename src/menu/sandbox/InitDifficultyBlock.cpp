#include "UI/SandboxMenu.h"

void SandboxMenu::InitDifficultyBlock() {

    auto difficultySlider = tgui::Slider::create(1, 5);
    difficultySlider->setPosition("80% + 5", "100% - 80");
    difficultySlider->setSize("20% - 20", 20);
    difficultySlider->setValue(3); // Значение по умолчанию
    difficultySlider->setStep(1);  // Только целые значения
    difficultySlider->getRenderer()->setTrackColor(tgui::Color(0, 150, 230));
    difficultySlider->getRenderer()->setThumbColor(tgui::Color(0, 150, 230));

    auto difficultyLabel = tgui::Label::create("Difficulty: ★★★");
    difficultyLabel->setPosition("80% + 5", "100% - 50");
    difficultyLabel->setTextSize(24);
    difficultyLabel->getRenderer()->setTextColor(tgui::Color(200, 200, 200));

    // Обработчик изменения значения ползунка
    difficultySlider->onValueChange([&, difficultyLabel](float value) {

        selectedDifficulty = static_cast<int>(value);
        std::string text = "Difficulty: ";
        for (int i = 0; i < selectedDifficulty; i++) {
            text += "★";
        }

        difficultyLabel->setText(text);
    });

    gui.add(difficultyLabel);
    gui.add(difficultySlider);
}