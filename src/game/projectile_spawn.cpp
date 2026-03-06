#include <Game.hpp>

void GameScene::FeatureTrigger(float value, int feature)
{
    // Защита от шумов и резких перепадов
    static float TriggeredAlready[256] = { 0 };
    TriggeredAlready[feature] -= dt;

    if (value > minTriggeredValue) {

        if (TriggeredAlready[feature] <= 0) {
            TriggeredAlready[feature] = cooldownTime; // сколько перезарядка для следующего триггера           
        }
        else {
            TriggeredAlready[feature] = cooldownTime; // чтобы после кулдауна на той же ноте не вызвать
            return;
        }
    }
    else {
        return; // не триггерится же
    }

    Rotator += dt / 256.f; // 1 градус в секунду 

    // сама логика спавна снаряда
    (this->*FeatureTriggerCurrentMode)(value, feature);
}