#include "GameManager.h"

GameManager::GameManager() {
    map = new MapManager();   // Map first, so spawn point is ready
    player = new PlayerManager(map->GetSpawnPosition());
    sound = new SoundManager();
}

GameManager::~GameManager() {
    delete player;
    delete map;
    delete sound;
}

void GameManager::Update() {
    float dt = GetFrameTime();
    if (dt > 0.05f) dt = 0.05f;
    
    if (IsKeyPressed(KEY_F1)) {
        showDebugHitboxes = !showDebugHitboxes;
    }

    // Update Sound (Music Stream)
    sound->Update();
    
    // Update Player with Map collision
    player->Update(dt, *map);

    // Map Interaction
    if (player->GetState() == PlayerState::ATTACKING) {
        map->InteractWithMap(player->GetAttackHitbox(), 2); // 2 = Destructible
    }
    
    // Collectible interaction (always check body hitbox)
    map->InteractWithMap(player->GetHitbox(), 3); // 3 = Collectible
}

void GameManager::Draw() {
    map->Draw();
    player->Draw(showDebugHitboxes);
}
