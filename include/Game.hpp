#ifndef Game
#define Game 1

#define PROJECTILES_LIFE_TIME 4

#include <SFML/Audio.hpp>
#include <Projectile.hpp>
#include <Spectro.hpp>
#include <STF.hpp>
#include <STFGram.hpp>
#include <iostream>
#include <algorithm> 
#include <filesystem>
#include <Collide.hpp>
#include <fstream>  // For file streams

// Реализацию см. в src/game, тут только поля и объявления

class GameScene
{
    sf::RenderWindow *window; // окно игры

    // Объекты игры
    sf::CircleShape barrier;
    sf::RectangleShape player;
    sf::RectangleShape healthbar;
    sf::RectangleShape shielbar;
    sf::Font font;
    sf::Text txtFpsCounter;
    sf::Text txtProgress;
    sf::RectangleShape background;
    sf::Shader shader;

    // Константы в рамках уровня или вообще всей игры
    const float barrierRadiusInitial = 300;
    const float playerSpeed = 500;
    const float playerSize = 20;
    const int spectroAccuracy = 512;        // примерно кадров на секунду
    const int spectroFramesInStfFrame = 16; // т.е. если секунда делится на 512 сегментов, число делим на 16 и получаем 32 кадра в секунду
    const int spectroHeight = 384; // Не трогать, а если и трогать, то поменять один массив в cl файле!
    
    // временно отключено для теста EIF float avgVolume = 0; // от 0 до 255
    float musicDurationSeconds = 0; 
    float difficulty = 1; // преобразованное уже конструктором до дроби, всегда больше нуля

    // Параметры игры (динамические)
    float barrierRadius = barrierRadiusInitial;
    float playerHealth = 255;
    float playerShield = 64;
    float healingReload = 0;
    bool awaitingRestart = false;

    // Векторные объекты
    sf::Vector2f barrierCenterInitial;
    sf::Vector2f barrierCenter;
    std::vector<Projectile *> Projectiles;
    // Spectro::Spectrogram stf; временно отключено для теста EIF
    Spectro::Spectrogram spectro;

    std::vector<float> rawSound; // сырой звук 44 100 ГЦ значение волны от -100 до 100
    float rawSoundPrevIndex = 0;

    // Звуки SFML
    sf::Music music;
    //sf::Sound soundDamage; TODO: добавить
    //sf::Sound soundHeal;

    // Динамические параметры уровня
    float angleAtack = 0;
    float speedAtack = 3; // множитель скоростей атаки
    float decoreR = 128, decoreG = 128, decoreB = 128; // для покраски декораций
    float tension = 0; // Напряжение, влияет на типы атак. Может колебаться в районе 50, но в жёстких метал партиях и выше 1000, а свыше 1500 сходу достигает только Cynthony
    float currentVolume = 0;

    // инициализация при начале и перерождении
    void SetupLevel();

public:
    /* инициализация сцены */
    GameScene(sf::RenderWindow *window_, std::string level, int difficulty_);
    ~GameScene() {
        spectro.FreeMemory();
        // временно отключено для теста EIF stf.FreeMemory();
    }

    /* вызывается до display и после clear*/
    void Cycle(float t);

private:
    void HandleInput(float t);

    bool IsInsideBarrier(const sf::Vector2f &point);

    void FeatureTriggered(float value, int feature);

    // cycle.cpp
    void ProjectilesCycle(float t);
    void FeaturesCycleResolve(float t);
    void VisualCycle(float t);

    // game_init.cpp
    void LoadFromFile(std::string level);
};

#endif
