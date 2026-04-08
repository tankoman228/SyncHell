#include <Game/Game.hpp>
#include <Analysis/EIF.hpp>

sf::RenderTexture bufferA, bufferB;
sf::RenderTexture* prevFrame;
sf::RenderTexture* nextFrame;
bool buffersInitialized = false;
char strFormatBuf2[64]; // ТОЛЬКО ДЛЯ ОСНОВНОГО ПОТОКА, МЕЛКИЕ СТРОКИ


void GameScene::VisualCycle() {

    // барьер поля
    barrier.setOutlineColor(sf::Color(255, 0, 254.f));
    barrier.setRadius(barrierRadius);
    barrier.setOrigin(barrierRadius, barrierRadius); // Центрируем
    barrier.setPosition(barrierCenter);

    // шкалы
    healthbar.setFillColor(sf::Color(255 - playerHealth, playerHealth, 0));
    healthbar.setScale(playerHealth / 255.f, 1);
    shielbar.setScale(playerShield / 64.f, 1);

    // фон TODO: вынести инициализацию
    if (!buffersInitialized) {
        auto size = window->getSize();
        bufferA.create(size.x, size.y);
        bufferB.create(size.x, size.y);
        prevFrame = &bufferA;
        nextFrame = &bufferB;
        buffersInitialized = true;
    }

    // В цикле:
    nextFrame->clear();
    shader.setUniform("previousTexture", prevFrame->getTexture()); // Передаем старый кадр
    shader.setUniformArray("spectrum",    &EIF::OUT_Etalon[0], 256);
    shader.setUniformArray("spectrumSum", &SpectrumTriggerSum[0], 256);
    shader.setUniform("deltaTime", dt);
        
    static float tt = 0; tt += dt; // TODO: вынести тикатель
    shader.setUniform("time", tt);

    nextFrame->clear();
    nextFrame->draw(background, &shader);
    nextFrame->display();

    sf::Sprite resultSprite(nextFrame->getTexture());
    window->draw(resultSprite);

    std::swap(prevFrame, nextFrame);
    window->draw(barrier);

    // Снаряды
    for (int i = Projectiles.size() - 1; i >= 0; i--)
    {
        if (Projectiles[i]->DoOutineFlash) {
            if (Projectiles[i]->lifeTime < 0.34f) {
                Projectiles[i]->shape.setOutlineThickness((0.4f - Projectiles[i]->lifeTime) * 3.f);
            }
            else Projectiles[i]->shape.setOutlineThickness(0.f);
        }
        
        Projectiles[i]->featureValue = EIF::OUT_Etalon[Projectiles[i]->featureIndex];
        // TODO: немного жмякать по scale

        if (Projectiles[i]->damage > 0 && Projectiles[i]->DoOutineFlash) {        
            projectileShader.setUniform("projectileColor", sf::Glsl::Vec4(
                Projectiles[i]->shape.getFillColor().r / 255.f,
                Projectiles[i]->shape.getFillColor().g / 255.f,
                Projectiles[i]->shape.getFillColor().b / 255.f,
                Projectiles[i]->shape.getFillColor().a / 255.f
            ));
            projectileShader.setUniform("spectroValue",                 
                std::max(Projectiles[i]->featureValue - 120.f, 0.f) / 135.f           
            );

            window->draw(Projectiles[i]->shape, &projectileShader);
        }
        else window->draw(Projectiles[i]->shape);
    }

    // счётчик FPS
    snprintf(strFormatBuf2, sizeof(strFormatBuf2), "%d FPS", int(1.f / dt));
    txtFpsCounter.setString(strFormatBuf2);

    // Отрисовка
    window->draw(player);
    window->draw(healthbar);
    window->draw(shielbar);
    window->draw(txtFpsCounter);
    window->draw(txtProgress);
    window->draw(txtDebug);
}