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
    
    playerMoney = 0;
    droppedItems.clear();

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
        } else if (spawn.type == -4) {
            enemies.push_back(new Lava(spawn.position));
        } else if (spawn.type == -5) {
            // Plant Spawn Offset: +8 X, +48 Y (total 3 blocks)
            Vector2 plantPos = { spawn.position.x + 8.0f, spawn.position.y + 48.0f };
            enemies.push_back(new Plant(plantPos));
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
    
    playerMoney = 0;
    droppedItems.clear();

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
        
        // Collision: Player Punch vs Enemy (Plant and Lava are invincible)
        if (player->GetState() == PlayerState::ATTACKING && player->IsAttackHitboxActive()) {
            if (enemies[i]->GetType() != EnemyType::LAVA && enemies[i]->GetType() != EnemyType::PLANT) {
                if (CheckCollisionRecs(player->GetAttackHitbox(), enemies[i]->GetHitbox())) {
                    enemies[i]->Die();
                    player->DeactivateAttackHitbox();
                }
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

    // Map Interaction (Punch)
    if (player->GetState() == PlayerState::ATTACKING && player->IsAttackHitboxActive()) {
        InteractionResult res = map->InteractWithMap(player->GetAttackHitbox(), 2);
        if (res.tileID != 0) {
            player->DeactivateAttackHitbox(); // FIX A: Deactivate immediately
            
            // Task 2: Star Block (Tile 22)
            if (res.tileID == 22) {
                int newMoneyType = (GetRandomValue(0, 1) == 0) ? 3 : 21; // 3 = Big, 21 = Small
                
                DroppedItem item;
                item.tileID = newMoneyType;
                item.position = { (float)res.col * TILE_SIZE, (float)res.row * TILE_SIZE };
                
                if (DYNAMIC_MONEY_DROPS) {
                    // Check tile above for ceiling clipping
                    bool ceilingSolid = false;
                    int tileAbove = map->GetTile(res.row - 1, res.col);
                    if ((tileAbove >= 1 && tileAbove <= 15) || (tileAbove >= 21 && tileAbove <= 23)) {
                        ceilingSolid = true;
                    }

                    item.velocity = { 0, ceilingSolid ? 0.0f : -100.0f }; // Only pop if ceiling is clear
                    item.pickupCooldown = 0.15f;
                    item.isGrounded = false;
                } else {
                    item.velocity = { 0, 0 };
                    item.pickupCooldown = 0.15f;
                    item.isGrounded = true;    // Don't apply gravity
                }
                
                droppedItems.push_back(item);
                if (droppedItems.size() > 2) {
                    droppedItems.erase(droppedItems.begin()); // Remove oldest
                }
            }
            
            // Task 3: Stun Block (Tile 23)
            if (res.tileID == 23) {
                player->TriggerStun();
            }
        }
    }
    
    // Update Dropped Items
    for (int i = (int)droppedItems.size() - 1; i >= 0; i--) {
        droppedItems[i].pickupCooldown -= dt;
        
        if (!droppedItems[i].isGrounded) {
            droppedItems[i].velocity.y += 400.0f * dt; // Gravity
            droppedItems[i].position.y += droppedItems[i].velocity.y * dt;
            
            Rectangle itemHb = { droppedItems[i].position.x, droppedItems[i].position.y, 16, 16 };
            if (map->CheckCollision(itemHb)) {
                droppedItems[i].isGrounded = true;
                droppedItems[i].velocity.y = 0;
                // Snap to tile
                int r = (int)floorf((itemHb.y + itemHb.height) / TILE_SIZE);
                
                // Tile 12 snap for items too
                bool hitTile12 = false;
                if (map->GetTile(r, (int)floorf(itemHb.x / TILE_SIZE)) == 12 || 
                    map->GetTile(r, (int)floorf((itemHb.x + 15) / TILE_SIZE)) == 12) {
                    hitTile12 = true;
                }

                if (hitTile12) {
                    droppedItems[i].position.y = (float)r * TILE_SIZE + 6 - 16;
                } else {
                    droppedItems[i].position.y = (float)r * TILE_SIZE - 16;
                }
            }
        }

        // Collection check
        if (droppedItems[i].pickupCooldown <= 0) {
            Rectangle itemHb = { droppedItems[i].position.x + 1, droppedItems[i].position.y, 14, 16 };
            if (CheckCollisionRecs(player->GetHitbox(), itemHb)) {
                if (droppedItems[i].tileID == 3) playerMoney += 20;
                if (droppedItems[i].tileID == 21) playerMoney += 10;
                droppedItems.erase(droppedItems.begin() + i);
            }
        }
    }

    // Collectible interaction (always check body hitbox for static tiles)
    InteractionResult collectRes = map->InteractWithMap(player->GetHitbox(), 3);
    if (collectRes.tileID != 0) {
        if (collectRes.tileID == 3) playerMoney += 20;
        if (collectRes.tileID == 21) playerMoney += 10;
    }
}

void GameManager::Draw() {
    // 1. Sky Layer
    map->DrawBackground();

    // 2. Plant Enemies (Only the Plants)
    for (auto enemy : enemies) {
        if (enemy->GetType() == EnemyType::PLANT) {
            enemy->Draw(showDebugHitboxes);
        }
    }

    // 3. Main Map Tiles & Lava (Lava is a non-moving enemy, but functionally a tile here)
    // Actually, Lava is in the 'enemies' list. We need to draw it here.
    for (auto enemy : enemies) {
        if (enemy->GetType() == EnemyType::LAVA) {
            enemy->Draw(showDebugHitboxes);
        }
    }
    map->DrawTiles();
    
    // Draw Dropped Items
    for (const auto& item : droppedItems) {
        map->DrawTile(item.tileID, item.position);
    }

    // 4. Player & Other Enemies (Bird, Scorpion)
    for (auto enemy : enemies) {
        if (enemy->GetType() == EnemyType::BIRD || enemy->GetType() == EnemyType::SCORPION) {
            enemy->Draw(showDebugHitboxes);
        }
    }

    player->Draw(showDebugHitboxes);
    
    // Foreground Pass (Tile 12 Tall Grass)
    map->DrawForeground();
}
