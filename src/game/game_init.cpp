#include <Game.hpp>

namespace fs = std::filesystem;

// инициализация при начале и перерождении
void GameScene::SetupLevel() {

    // Сброс динамических значений уровня
    playerHealth = 255;
    playerShield = 64;
    decoreR = 128; decoreG = 128; decoreB = 128; 
    angleAtack = 0;
    tension = 0;
    Projectiles.clear();
    barrierRadius = barrierRadiusInitial;
    speedAtack = 3;
    healingReload = 0;
    awaitingRestart = false;

    // Начальная позиция игрока (центр экрана)
    barrierCenter = barrierCenterInitial;

    player.setPosition(barrierCenter);

    barrier = sf::CircleShape(barrierRadius);
    barrier.setFillColor(sf::Color::Transparent);
    barrier.setOutlineColor(sf::Color(255,80,80));
    barrier.setOutlineThickness(2.0f);
    barrier.setOrigin(barrierRadius, barrierRadius); // Центрируем
    barrier.setPosition(barrierCenter);
    barrier.setPointCount(128);

    music.stop();

    musicDurationSeconds = music.getDuration().asSeconds();
    music.play();
}

/* инициализация сцены */
GameScene::GameScene(sf::RenderWindow *window_, std::string level, int difficulty_)
{
    this->window = window_;
    switch (difficulty_)
    {
        case 1: difficulty = 0.6; break;
        case 2: difficulty = 0.8; break;
        case 3: difficulty = 1; break;
        case 4: difficulty = 1.2; break;
        case 5: difficulty = 1.4; break;
    }

    // загрузка музыкального файла
    std::cout << "opening music file\n";
    LoadFromFile(level);
    music.openFromFile(fs::current_path() / ("levels/" + level));

    // Далее спрайты и т.п.
    sf::Vector2u windowSize = window->getSize();
    barrierCenterInitial = sf::Vector2f(windowSize.x / 2.0f, windowSize.y / 2.0f);

    // Инициализация игрока
    player = sf::RectangleShape(sf::Vector2f(playerSize, playerSize));
    player.setFillColor(sf::Color::Green);
    player.setOrigin(playerSize / 2, playerSize / 2); // Центрируем

    // Теперь засетапим декор, квадратики слева и справа, которые будут жмякаться в такт
    for (int t = 0; t < stf.timeLength; t++) {
        for (int i = 0; i < 12; i++) {
            bouncyJumpersMaxVals[i] = std::max(bouncyJumpersMaxVals[i], stf.spectro[t][i + 240]);
        }
        avgVolume += stf.spectro[t][252];
    }
    float jumperHeight = window->getSize().y / 6.f;
    for (int i = 0; i < 12; i++) {

        // Подсчитали для нормализации значений фич с 240-й (суммы фич по всем полосам), надо сами прямоугольники задать
        bouncyJumpers[i] = sf::RectangleShape(sf::Vector2f(10.f, jumperHeight));
        bouncyJumpers[i].setFillColor(sf::Color::White);

        if (i < 6) {
            bouncyJumpers[i].setPosition(0, i * jumperHeight);
        }
        else {
            bouncyJumpers[i].setOrigin(10, 0);
            bouncyJumpers[i].setPosition(window->getSize().x, (i - 6) * jumperHeight);
        }
    }

    avgVolume /= stf.timeLength;

    // Text
    font.loadFromFile("assets/sansation.ttf");

    txtFpsCounter.setFont(font);
    txtFpsCounter.setPosition(10, 10);
    txtFpsCounter.setFillColor(sf::Color::Green);

    txtProgress.setFont(font);
    txtProgress.setPosition(windowSize.x - 80, 10);
    txtProgress.setFillColor(sf::Color::Green);

    // TODO: придумать, что делать с диаграммой
    STF::GetSTFVisualized(stf);

    SetupLevel(); // тут заиграет музыка

    // Остатки спрайтов, зависимых от результатов SetupLevel
    healthbar = sf::RectangleShape(sf::Vector2f(500, 30));
    healthbar.setPosition(barrierCenter);
    healthbar.move(-300, barrierRadius + 100);

    shielbar = sf::RectangleShape(sf::Vector2f(500, 30));
    shielbar.setPosition(barrierCenter);
    shielbar.move(-300, barrierRadius + 150);
    shielbar.setFillColor(sf::Color(0, 255, 255));
}

void GameScene::LoadFromFile(std::string level) {

    // путь для кэш-файла
    std::filesystem::create_directory(fs::current_path() / ".cache");
    std::string cache_path = fs::current_path() / ".cache/" / (level + ".stf");

    // Пробуем загрузить из кэша
    bool loaded_from_cache = false;
    std::ifstream cache_file(cache_path, std::ios::binary);

    if (cache_file.is_open()) {
        // Читаем заголовок с размерами
        size_t outer_size, inner_size;
        
        if (cache_file.read(reinterpret_cast<char*>(&outer_size), sizeof(size_t)) &&
            cache_file.read(reinterpret_cast<char*>(&inner_size), sizeof(size_t))) {
            
            // Выделяем память
            stf.spectro = new float*[outer_size];
            stf.timeLength = outer_size;
            stf.heigth = inner_size;  // Используем сохраненный размер, а не 256

            // Выделяем память для каждой строки
            for (size_t i = 0; i < outer_size; i++) {
                stf.spectro[i] = new float[inner_size];
            }
            
            // Читаем данные
            for (size_t i = 0; i < outer_size; ++i) {
                // Читаем напрямую в массив stf.spectro[i]
                if (!cache_file.read(reinterpret_cast<char*>(stf.spectro[i]), 
                                    inner_size * sizeof(float))) {
                    // Ошибка чтения
                    std::cerr << "Error reading cache data at row " << i << "\n";
                    // Освобождаем уже выделенную память
                    for (size_t j = 0; j <= i; j++) {
                        delete[] stf.spectro[j];
                    }
                    delete[] stf.spectro;
                    stf.spectro = nullptr;
                    stf.timeLength = 0;
                    stf.heigth = 0;
                    break;
                }
            }

            if (stf.spectro != nullptr) {
                loaded_from_cache = true;
                std::cout << "Loaded STF from cache: " << outer_size << "x" << inner_size << "\n";
            }
        }
        cache_file.close();
    }

    // Если не удалось загрузить из кэша, вычисляем
    if (!loaded_from_cache) {
        std::cout << "Computing spectrogram...\n";
        spectro = Spectro::getSpectroFromOgg(fs::current_path() / ("levels/" + level), spectroAccuracy, spectroHeight);
        
        std::cout << "Computing STF (this may take a while)...\n";
        stf = STF::SpectroToStf(spectro, spectroFramesInStfFrame);
        
        // Сохраняем в кэш
        std::ofstream out_cache(cache_path, std::ios::binary);
        if (out_cache.is_open()) {
            // Сохраняем размеры
            size_t outer_size = stf.timeLength;
            size_t inner_size = outer_size > 0 ? stf.heigth : 0;
            
            out_cache.write(reinterpret_cast<const char*>(&outer_size), sizeof(size_t));
            out_cache.write(reinterpret_cast<const char*>(&inner_size), sizeof(size_t));
            
            // Сохраняем данные
            for (size_t chunk = 0; chunk < stf.timeLength; ++chunk) {
                // stf.spectro[chunk] - это массив из 256 float
                float* chunk_data = stf.spectro[chunk];
                out_cache.write(reinterpret_cast<const char*>(chunk_data), 
                            inner_size * sizeof(float));
            }
            
            out_cache.close();
            std::cout << "STF saved to cache: " << cache_path << "\n";
        } else {
            std::cerr << "Warning: Could not save cache to " << cache_path << "\n";
        }
    }

}