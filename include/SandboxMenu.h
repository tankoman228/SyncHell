#pragma once

// Это меню режима песочницы. Основной и первый режим, где игрок САМ загружает музыку и получает уровни
// Для сюжета (если он есть/будет), другие классы. Я клянусь, что комментарии тут к ИИ отношения не имеют!

// системные
#include <vector>
#include <functional>


// TGUI и SFML, я накосчил с библиотеками походу, но так компилится на всех ОС, потому сойдёт
#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>

#ifdef _WIN32
#include <TGUI/Backend.hpp> // у иеня почему-то вот этот заголовок адекватно пашет, а тот не хотит брать
#include <Windows.h>
#endif

#ifdef linux
#include <TGUI/Backend/SFML-Graphics.hpp> // а тут надо указать конкретный бекенд
#endif

// TODO: считывать атрибуты файла, а не забивать затычками
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

// Просто игровая менюшка. TODO: дизайн
class SandboxMenu {

	// Константы для рендера
	const float CARD_HEIGHT = 110;

	// Статичные поля
	sf::RenderWindow& window;
	tgui::Gui gui; 
	std::vector<LevelOption> Levels;
	std::vector<std::string> shaderFilenames;
	std::function<void(std::string, std::string, int)> playBtnClick;

	// Динамичные поля
	int selectedDifficulty = 2;
	std::string searchText;
	std::string selectedShaderFilename;

	// Интерфейс
	tgui::Panel::Ptr cardContainer;

	// реализация в .cpp в директории menu, строит интерфейс из конструктора
	void InitLevelsList();
	void InitLevelsCards();
	void InitButtons();
	void InitDifficultyBlock();

	// для поиска уровней по имени
	void TriggerSearch();

public:
	SandboxMenu(sf::RenderWindow& window_, std::function<void(std::string, std::string, int)> playBtnClick);

	void HandleEvent(sf::Event event);
	void Cycle(); // цикл меню, вызывать только когда оно уже открыто
};