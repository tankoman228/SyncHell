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
    sf::Text txtDebug;
    sf::RectangleShape background;
    sf::Shader shader;

    // Константы в рамках уровня или вообще всей игры
    const float barrierRadiusInitial = 300;
    const float playerSpeed = 500;
    const float playerSize = 20;
    const int spectroAccuracy = 512;        // примерно кадров на секунду
    const int spectroFramesInStfFrame = 16; // т.е. если секунда делится на 512 сегментов, число делим на 16 и получаем 32 кадра в секунду
    const int spectroHeight = 384; // Не трогать, а если и трогать, то поменять один массив в cl файле!
    
    float windowWidth;
    float windowHeight;

    // временно отключено для теста EIF float avgVolume = 0; // от 0 до 255
    float musicDurationSeconds = 0; 
    float difficulty = 1; // преобразованное уже конструктором до дроби, всегда больше нуля

    // Параметры игры (динамические)
    float barrierRadius = barrierRadiusInitial;
    float playerHealth = 255;
    float playerShield = 64;
    float healingReload = 0;
    bool awaitingRestart = false;
    float dt = 0;
    float Rotator = 0; // вращение углов атаки спавнеров частиц

    // параметры режима
    float minTriggeredValue = 255;
    float cooldownTime = 0;

    // Векторные объекты
    sf::Vector2f barrierCenterInitial;
    sf::Vector2f barrierCenter;
    std::vector<AbstractProjectile *> Projectiles;
    // Spectro::Spectrogram stf; временно отключено для теста EIF
    Spectro::Spectrogram spectro;

    std::vector<float> rawSound; // сырой звук 44 100 ГЦ значение волны от -100 до 100
    float rawSoundPrevIndex = 0;

    // Звуки SFML
    sf::Music music;
    //sf::Sound soundDamage; TODO: добавить
    //sf::Sound soundHeal;

    // инициализация при начале и перерождении
    void SetupLevel();

public:
    /* инициализация сцены */
    GameScene(sf::RenderWindow *window_, std::string level, int difficulty_, std::string shader_filename);
    ~GameScene() {
        spectro.FreeMemory();
        // временно отключено для теста EIF stf.FreeMemory();
    }

    /* вызывается до display и после clear*/
    void Cycle(float dt);

private:
    void HandleInput();

    bool IsInsideBarrier(const sf::Vector2f &point);

    void FeatureTrigger(float value, int feature);

    // Актуальные режимы для игры
    void (GameScene::*FeatureTriggerCurrentMode)(float value, int feature) = &GameScene::FeatureTriggerMode0;
    void FeatureTriggerMode0(float value, int feature);
    void FeatureTriggerMode1(float value, int feature);
    void FeatureTriggerMode2(float value, int feature);
    void FeatureTriggerMode3(float value, int feature);
    void FeatureTriggerMode4(float value, int feature);
    void FeatureTriggerMode5(float value, int feature);
    void FeatureTriggerMode6(float value, int feature);
    void FeatureTriggerMode7(float value, int feature);

    // cycle.cpp
    void ProjectilesCycle();
    void FeaturesCycleResolve();
    void VisualCycle();

    // game_init.cpp
    void LoadFromFile(std::string level);
};

#endif
