#include <Game.hpp>

void GameScene::ProjectilesCycle() {

    int spawnDamageAura = 0;
    int spawnHealAura = 0;

    GameCollider::ReinitByRectangleShape(player);

    for (int i = Projectiles.size() - 1; i >= 0; i--)
    {
        if (Projectiles[i]->lifeTime > PROJECTILES_LIFE_TIME)
        {
            // оптимизация: ранее было delete Projectiles[i]; Projectiles.erase(Projectiles.begin() + i);
            std::swap(Projectiles[i], Projectiles.back());
            delete Projectiles.back();
            Projectiles.pop_back(); // чтобы индексы не пересчитывало

            continue;
        }

        Projectiles[i]->UpdateConvVerticesCache();

        if (
            Projectiles[i]->isCollidable && 
            GameCollider::HasCollision(Projectiles[i]->shape, Projectiles[i]->convVerticesCache)
        )
        {
            if (Projectiles[i]->damage < 0) {
                spawnHealAura++;
                playerShield = 64;
                playerHealth -= Projectiles[i]->damage / difficulty;
                if (playerHealth > 255) playerHealth = 255;
            }
            else
            {
                spawnDamageAura++;
                playerShield -= Projectiles[i]->damage * difficulty;
                if (playerShield < 0) {
                    playerHealth += playerShield;
                    playerShield = 0;
                }
            }

            delete Projectiles[i];
            Projectiles.erase(Projectiles.begin() + i);

            continue;
        }

        Projectiles[i]->Cycle(dt);
    }

    // Чтобы перераспределение памяти не привело к ошыбке
    for (int i = 0; i < spawnDamageAura; i++) {

        auto projectile = new ProjectileAura();

        projectile->TargetedShape = &player;
        projectile->R = 255;
        projectile->G = 10;
        projectile->B = 10;
        projectile->InitFadeKoef = 0.1f;

        projectile->Build();
        Projectiles.push_back(projectile);
    }
    for (int i = 0; i < spawnHealAura; i++) {

        auto projectile = new ProjectileAura();

        projectile->TargetedShape = &player;
        projectile->R = 10;
        projectile->G = 255;
        projectile->B = 10;
        projectile->InitFadeKoef = 0.5f;

        projectile->Build();
        Projectiles.push_back(projectile);
    }
}