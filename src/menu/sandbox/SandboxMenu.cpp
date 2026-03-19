#include "SandboxMenu.h"
#include <filesystem>
#include <iostream>

void SandboxMenu::HandleEvent(sf::Event event) { // для инкапсуляции вынес
	gui.handleEvent(event);
}

SandboxMenu::SandboxMenu(sf::RenderWindow& window_, std::function<void(std::string, std::string, int)> playBtnClick)
	: window(window_), gui(window_) // не знаю как, но работает только такой init
{
	this->playBtnClick = playBtnClick;

	// Сначала та, что прописана отдельными методами, потом те, котопым места в них не нашлось (не вижу смысла дробить на нанометоды)
	InitLevelsList();
	InitLevelsCards();
	InitButtons();
	InitDifficultyBlock();

	auto searchBox = tgui::EditBox::create();
	searchBox->setPosition(10, 10);
	searchBox->setSize("80% - 10", 30);
	searchBox->setDefaultText("Search levels...");
	searchBox->getRenderer()->setBackgroundColor(tgui::Color::Black);
	searchBox->getRenderer()->setTextColor(tgui::Color(200, 200, 200));
    searchBox->onTextChange([&, searchBox]() {
        searchText = searchBox->getText().toStdString();
		TriggerSearch(); 
	});
	gui.add(searchBox);

	auto shaderSelect = tgui::ComboBox::create();
	shaderSelect->setPosition("80% + 5", "100% - 160");
	shaderSelect->setSize("20% - 20", 20);
	for (const auto& entry : std::filesystem::directory_iterator(std::filesystem::current_path() / "graphics/styles", std::filesystem::directory_options::skip_permission_denied)) {
		if (entry.is_directory()) {
			shaderSelect->addItem(entry.path().filename().u8string());
		}
	}
	shaderSelect->onItemSelect([&, shaderSelect]() {
		selectedShaderFilename = shaderSelect->getSelectedItem().toStdString();
		std::cout << selectedShaderFilename << "\n";
	});
	shaderSelect->setSelectedItemByIndex(0);

	gui.add(shaderSelect);
}
