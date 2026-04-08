#include "UI/SandboxMenu.h"

void SandboxMenu::InitButtons() {

    auto closeButton = tgui::Button::create("Close game");
    closeButton->setPosition("80% + 7", 20);
    closeButton->getRenderer()->setBackgroundColor(tgui::Color(10, 10, 10));
    closeButton->getRenderer()->setTextColor(tgui::Color(200, 200, 200));
    closeButton->setSize("20% - 15", 40);
    closeButton->onClick([&]() {
        window.close();
    });
    gui.add(closeButton);

    auto aboutButton = tgui::Button::create("About / Copyright");
    aboutButton->setPosition("80% + 7", 80);
    aboutButton->getRenderer()->setBackgroundColor(tgui::Color(10, 10, 10));
    aboutButton->getRenderer()->setTextColor(tgui::Color(200, 200, 200));
    aboutButton->setSize("20% - 15", 40);
    aboutButton->onClick([]() {

        std::string filePath = "README.md";
        #ifdef _WIN32
            ShellExecuteA(NULL, "open", filePath.c_str(), NULL, NULL, SW_SHOWNORMAL);
        #else
            // Linux
            std::string command = "xdg-open \"" + filePath + "\"";
            system(command.c_str());
        #endif
    });
    gui.add(aboutButton);
}