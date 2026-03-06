#include <Game.hpp>

void GameScene::FeatureTrigger(float value, int feature)
{
    (this->*FeatureTriggerCurrentMode)(value, feature); // Мы переехали, теперь логика триггеров разная в разных режимах
}