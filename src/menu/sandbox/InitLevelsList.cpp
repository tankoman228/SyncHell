#include "UI/SandboxMenu.h"

#include <TGUI/Widgets/Group.hpp>
#include <TGUI/Widgets/Panel.hpp>
#include <TGUI/Widgets/HorizontalWrap.hpp>
#include <cctype> 
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

bool endsWith(const std::string& fullString, const std::string& ending) {
    if (ending.length() > fullString.length()) {
        return false;
    }
    return fullString.compare(fullString.length() - ending.length(), ending.length(), ending) == 0;
}

void SandboxMenu::InitLevelsList() {

    // туториалы
    Levels.push_back(LevelOption("tutorial/Investigations.ogg", "Kevin MacLeod (CC BY 3.0)", true));
    Levels.push_back(LevelOption("tutorial/Jingle-Bells-3.ogg", "Kevin MacLeod (CC BY 3.0)", true));
    Levels.push_back(LevelOption("tutorial/Run-Amok.ogg", "Kevin MacLeod (CC BY 3.0)", true));
    Levels.push_back(LevelOption("tutorial/We-Wish-You.ogg", "Kevin MacLeod (CC BY 3.0)", true));

    // Работа с файлами
    try {
        std::vector<fs::directory_entry> entries;
        for (const auto& entry : fs::directory_iterator(fs::current_path() / "levels", fs::directory_options::skip_permission_denied)) {
            if (entry.is_regular_file())  entries.push_back(entry);
        }

        std::sort(entries.begin(), entries.end(), [](const auto& a, const auto& b) {
            return fs::last_write_time(a) > fs::last_write_time(b);
            });

        // 3. Используем отсортированный список
        for (const auto& entry : entries) {
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
}