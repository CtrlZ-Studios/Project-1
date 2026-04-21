#include "GameManager.h"
#include <algorithm>

GameManager::GameManager() {
    map = new MapManager();   // Map first, so spawn point is ready
    player = new PlayerManager(map->GetSpawnPosition());
    sound = new SoundManager();
    
    // Initialize Camera
    camera = { 0 };
    camera.offset = { 256 / 2.0f, 192 / 2.0f };
    camera.target = { player->GetPosition().x, 144.0f }; // Centered on ground rows (13, 14)
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    
    maxCameraX = camera.target.x;
    
    SpawnEnemies();
}

GameManager::~GameManager() {
    delete player;
    delete map;
    delete sound;
    ClearEnemies();
}

void GameManager::ClearEnemies() {
    for (auto enemy : enemies) {
        delete enemy;
    }
    enemies.clear();
}

void GameManager::SpawnEnemies() {
    const auto& spawns = map->GetEnemySpawns();
    for (const auto& spawn : spawns) {
        if (spawn.type == -2) {
            enemies.push_back(new Bird(spawn.position));
        } else if (spawn.type == -3) {
            enemies.push_back(new Scorpion(spawn.position));
        }
    }
}

void GameManager::RestartLevel() {
    delete player;
    delete map;
    ClearEnemies();
    map = new MapManager();
    player = new PlayerManager(map->GetSpawnPosition());
    
    // Reset Camera
    camera.target = { player->GetPosition().x, 144.0f };
    maxCameraX = camera.target.x;
    
    SpawnEnemies();
}

void GameManager::UpdateCamera() {
    Vector2 playerPos = player->GetPosition();
    float targetX = (float)playerPos.x;
    
    // One-way scrolling logic
    if (targetX > maxCameraX) {
        maxCameraX = targetX;
    }
    
    camera.target.x = floorf(maxCameraX);
    camera.target.y = 144.0f; // Lock Y camera to show ground at bottom

    // Camera Clamping
    float minX = camera.offset.x;
    float maxX = WORLD_WIDTH - camera.offset.x;
    
    if (camera.target.x < minX) camera.target.x = minX;
    if (camera.target.x > maxX) camera.target.x = maxX;
}

void GameManager::CullOffscreen() {
    float leftEdgeX = camera.target.x - camera.offset.x;
    
    // Cull Tiles
    map->CullOffscreen(leftEdgeX);
    
    // Cull Enemies (if 100% off-screen to the left)
    for (int i = (int)enemies.size() - 1; i >= 0; i--) {
        Rectangle hb = enemies[i]->GetHitbox();
        if (hb.x + hb.width < leftEdgeX) {
            delete enemies[i];
            enemies.erase(enemies.begin() + i);
        }
    }
}

void GameManager::Update() {
    float dt = GetFrameTime();
    if (dt > 0.05f) dt = 0.05f;
    
    if (IsKeyPressed(KEY_F1)) {
        showDebugHitboxes = !showDebugHitboxes;
    }

    if (IsKeyPressed(KEY_F2)) {
        RestartLevel();
    }

    // Update Sound (Music Stream)
    sound->Update();
    
    // Update Player with Map collision
    player->Update(dt, *map);
    
    // Task 1: Left-Screen Boundary for Player
    float leftEdgeX = camera.target.x - camera.offset.x;
    Rectangle playerHb = player->GetHitbox();
    if (playerHb.x < leftEdgeX) {
        player->GetPosition().x = leftEdgeX + 5; 
    }

    UpdateCamera();
    CullOffscreen();

    // Update Enemies
    for (int i = (int)enemies.size() - 1; i >= 0; i--) {
        enemies[i]->Update(dt, *map);
        
        // Collision: Player Punch vs Enemy
        if (player->GetState() == PlayerState::ATTACKING && player->IsAttackHitboxActive()) {
            if (CheckCollisionRecs(player->GetAttackHitbox(), enemies[i]->GetHitbox())) {
                enemies[i]->Die();
                player->DeactivateAttackHitbox();
            }
        }

        // Collision: Player Body vs Enemy (if enemy not dead)
        if (i < (int)enemies.size() && !enemies[i]->IsDead()) {
            if (CheckCollisionRecs(player->GetHitbox(), enemies[i]->GetHitbox())) {
                RestartLevel();
                return; // Level restarted, exit update
            }
        }
    }

    // Cleanup dead enemies
    for (auto it = enemies.begin(); it != enemies.end(); ) {
        if ((*it)->IsDead()) {
            delete *it;
            it = enemies.erase(it);
        } else {
            ++it;
        }
    }

    // Map Interaction
    if (player->GetState() == PlayerState::ATTACKING && player->IsAttackHitboxActive()) {
        if (map->InteractWithMap(player->GetAttackHitbox(), 2)) { // 2 = Destructible
            player->DeactivateAttackHitbox();
        }
    }
    
    // Collectible interaction (always check body hitbox)
    map->InteractWithMap(player->GetHitbox(), 3); // 3 = Collectible
}

void GameManager::Draw() {
    map->Draw();
    for (auto enemy : enemies) {
        enemy->Draw(showDebugHitboxes);
    }
    player->Draw(showDebugHitboxes);
}
