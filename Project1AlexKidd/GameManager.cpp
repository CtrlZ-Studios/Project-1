#include "GameManager.h"
#include <algorithm>
#include <iostream>
#include <string>

GameManager::GameManager() {
    map = new MapManager();   // Map first, so spawn point is ready
    map->LoadLevel(0);        // Task: Start at Level 0 (Main Menu)
    player = new PlayerManager(map->GetSpawnPosition());
    sound = new SoundManager();
    
    // Initialize Camera
    camera = { 0 };
    camera.offset = { 256 / 2.0f, 192 / 2.0f };
    UpdateCamera();
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    
    maxCameraX = camera.target.x;
    
    playerMoney = 0;
    droppedItems.clear();

    SpawnEnemies();

    // Menu state initialization
    menuTimer = 0.0f;
    menuColorVariant = 0;
    menuFlickerTimer = 0.0f;
}

GameManager::~GameManager() {
    delete player;
    delete map;
    delete sound;
    ClearEnemies();
}

void GameManager::ClearEnemies(bool returningFromShop) {
    if (returningFromShop || map->GetCurrentLevel() == 3) return;
    for (auto enemy : enemies) {
        delete enemy;
    }
    enemies.clear();
}

void GameManager::SpawnEnemies(bool returningFromShop) {
    if (returningFromShop || map->GetCurrentLevel() == 3) return;
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
        } else if (spawn.type == -6) {
            enemies.push_back(new Quicksand(spawn.position));
        } else if (spawn.type == -7) {
            enemies.push_back(new Frog(spawn.position));
        } else if (spawn.type == -8) {
            // Frog with 0.5 block (8px) offset to the right
            Vector2 frogPos = { spawn.position.x + 8.0f, spawn.position.y };
            enemies.push_back(new Frog(frogPos));
        }
    }
}

void GameManager::EnterShop() {
    previousLevel = map->GetCurrentLevel();
    // Don't ClearEnemies() here to allow them to persist (we just stop updating/drawing them)
    map->LoadLevel(3);
    player->GetPosition() = { 1 * 16.0f, 9 * 16.0f }; // 2nd block, 3rd row from bottom
    camera.target = { 128.0f, 96.0f }; // Center of shop
}

void GameManager::ExitShop() {
    map->LoadLevel(previousLevel, true); // true = returning from shop
    player->GetPosition() = { 5 * 16.0f, (map->GetMapRows() - 3) * 16.0f }; // 6th block, 3rd row from bottom
}

void GameManager::RestartLevel() {
    delete player;
    ClearEnemies();
    
    // Reload the map data from template for the CURRENT level
    map->LoadLevel(map->GetCurrentLevel());

    player = new PlayerManager(map->GetSpawnPosition());
    
    // Reset Camera
    camera.target = { player->GetPosition().x, 144.0f };
    maxCameraX = camera.target.x;
    
    // Clear items for a fresh reload
    droppedItems.clear();

    SpawnEnemies();
}

void GameManager::PlayerDied() {
    if (player->GetState() == PlayerState::DYING) return; // Prevent double-death
    lives--;
    std::cout << "Lost a life! " << lives << " remaining." << std::endl;
    player->TriggerDeath();
}

void GameManager::UpdateCamera() {
    if (map->GetCurrentLevel() == 3 || map->GetCurrentLevel() == 0) {
        // Lock camera perfectly to the center of the 256x192 room
        camera.target.x = 128.0f;
        camera.target.y = 96.0f; 
    } else {
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
        float maxX = map->GetWorldWidth() - camera.offset.x;
        
        if (camera.target.x < minX) camera.target.x = minX;
        if (camera.target.x > maxX) camera.target.x = maxX;
    }
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
    if (map->GetCurrentLevel() == 0) {
        float dt = GetFrameTime();
        if (dt > 0.05f) dt = 0.05f;

        menuTimer += dt;
        
        const float MENU_STEP_INTERVAL = 0.2f;
        const float MENU_FLICKER_RATE  = 0.05f;

        if (menuTimer >= 0.5f + 6 * MENU_STEP_INTERVAL) {
            menuFlickerTimer += dt;
            if (menuFlickerTimer >= MENU_FLICKER_RATE) {
                menuFlickerTimer = 0;
                menuColorVariant = (menuColorVariant + 1) % 3; // red, green, yellow
            }
        }

        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_J)) {
            map->LoadLevel(1);
            RestartLevel();
        }
        UpdateCamera();
        return;
    }

    if (isGameOver) {
        if (IsKeyPressed(KEY_SPACE)) {
            lives = 3;
            score = 0;
            playerMoney = 0;
            shop1UpBought = false; // Reset shop item availability
            isGameOver = false;
            // Hard Reset / Load Level 1 (F2 logic)
            map->LoadLevel(1);
            RestartLevel();
        }
        return; // Skip the rest of the update
    }

    float dt = GetFrameTime();
    if (dt > 0.05f) dt = 0.05f;
    
    if (player->GetState() == PlayerState::DYING) {
        float topCameraY = camera.target.y - (192.0f / 2.0f);
        if (player->GetPosition().y + 24.0f < topCameraY) { // Toes off screen
            if (lives <= 0) {
                isGameOver = true;
            } else {
                float leftEdgeX = camera.target.x - (256.0f / 2.0f);
                Vector2 newPos = map->GetSafeRespawnPosition(leftEdgeX, enemies);
                player->TriggerRespawn(newPos);
            }
        }
    }

    if (IsKeyPressed(KEY_F1)) {
        showDebugHitboxes = !showDebugHitboxes;
    }

    // Task 2: Debug Keybinds
    if (IsKeyPressed(KEY_F2)) {
        playerMoney = 0;
        map->LoadLevel(1);
        RestartLevel();
    }
    if (IsKeyPressed(KEY_F3)) {
        RestartLevel();
    }
    if (IsKeyPressed(KEY_F4)) {
        map->LoadLevel(1);
        RestartLevel();
    }
    if (IsKeyPressed(KEY_F5)) {
        map->LoadLevel(2);
        RestartLevel();
    }
    if (IsKeyPressed(KEY_F6)) {
        PlayerDied();
    }
    if (IsKeyPressed(KEY_F7)) {
        player->TogglePermInvincibility();
    }

    // Update Sound (Music Stream)
    sound->Update();
    
    // Update Player with Map collision
    player->Update(dt, *map);
    
    // Check for falling off map (Y > WORLD_HEIGHT)
    if (player->GetPosition().y > 192) { // 192 is the screen/world height based on camera logic
        PlayerDied();
        return;
    }

    // Task 1: Left-Screen Boundary for Player
    float leftEdgeX = camera.target.x - camera.offset.x;
    Rectangle playerHb = player->GetHitbox();
    if (playerHb.x < leftEdgeX) {
        player->GetPosition().x = leftEdgeX + 5; 
    }

    UpdateCamera();
    CullOffscreen();

    // Update Enemies (Only if NOT in shop)
    if (map->GetCurrentLevel() != 3) {
        for (int i = (int)enemies.size() - 1; i >= 0; i--) {
            enemies[i]->Update(dt, *map);
            
            // Collision: Player Punch vs Enemy (Plant, Lava, and Quicksand are invincible)
            if (player->GetState() == PlayerState::ATTACKING && player->IsAttackHitboxActive()) {
                if (enemies[i]->GetType() != EnemyType::LAVA && enemies[i]->GetType() != EnemyType::PLANT && enemies[i]->GetType() != EnemyType::QUICKSAND) {
                    if (CheckCollisionRecs(player->GetAttackHitbox(), enemies[i]->GetHitbox())) {
                        score += 200; 
                        std::cout << "You gained 200 points by killing an enemy! Total points: " << score << std::endl;
                        enemies[i]->Die();
                        player->DeactivateAttackHitbox();
                    }
                }
            }

            // Collision: Player Body vs Enemy (if enemy not dead)
            if (i < (int)enemies.size() && !enemies[i]->IsDead()) {
                if (CheckCollisionRecs(player->GetHitbox(), enemies[i]->GetHitbox())) {
                    if (enemies[i]->GetType() == EnemyType::LAVA || 
                        enemies[i]->GetType() == EnemyType::QUICKSAND || 
                        enemies[i]->GetType() == EnemyType::PLANT) {
                        PlayerDied();
                        return; 
                    } else if (player->IsInvincible()) {
                        // Player is invincible, ignore normal enemies
                        continue; 
                    } else {
                        PlayerDied();
                        return;
                    }
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
                if (droppedItems.size() > 50) { 
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
                int amount = 0;
                // Based on MapManager definitions: 3 is Big (+20), 21 is Small (+10)
                if (droppedItems[i].tileID == 3) amount = 20;
                if (droppedItems[i].tileID == 21) amount = 10;
                playerMoney += amount;
                score += 100;
                std::cout << "Picked up " << amount << " money. Total: " << playerMoney << std::endl;
                std::cout << "You gained 100 points by collecting money! Total points: " << score << std::endl;
                droppedItems.erase(droppedItems.begin() + i);
            }
        }
    }

    // Collectible interaction (always check body hitbox for static tiles)
    InteractionResult collectRes = map->InteractWithMap(player->GetHitbox(), 3);
    if (collectRes.tileID != 0) {
        // Task: Map-Placed Money (Tiles 3, 21)
        if (collectRes.tileID == 3 || collectRes.tileID == 21) {
            // 3 is Big (+20), 21 is Small (+10)
            int amount = (collectRes.tileID == 3) ? 20 : 10;
            playerMoney += amount;
            score += 100;
            std::cout << "Picked up " << amount << " money from map. Total: " << playerMoney << std::endl;
            std::cout << "You gained 100 points by collecting money! Total points: " << score << std::endl;
        }
        
        // Onigiri (Tile 25) Advances to next level
        if (collectRes.tileID == 25) {
            score += 1000;
            std::cout << "You gained 1000 points by eating onigiri! Total points: " << score << std::endl;
            int nextLevel = map->GetCurrentLevel() + 1;
            if (nextLevel > 2) nextLevel = 1;
            map->LoadLevel(nextLevel);
            RestartLevel();
            return;
        }
    }

    // Shop Interactions
    for (int r = 0; r < map->GetMapRows(); r++) {
        for (int c = 0; c < map->GetMapCols(); c++) {
            int tid = map->GetTile(r, c);
            if (tid == 40 || tid == 42 || tid == 43) {
                float tx = (float)c * TILE_SIZE;
                float ty = (float)r * TILE_SIZE;
                Rectangle hb = { tx, ty, (float)TILE_SIZE, (float)TILE_SIZE };
                
                if (tid == 40) { // Enter Door
                    hb.x += map->doorOffsetX;
                    hb.y += map->doorOffsetY;
                    hb.height = 32; // Make hitbox tall enough to reach the ground
                    if (CheckCollisionRecs(player->GetHitbox(), hb) && player->IsGrounded() && IsKeyPressed(KEY_SPACE)) {
                        EnterShop();
                        return; // Level changed, stop updating this frame
                    }
                } else if (tid == 42) { // 1UP Item
                    hb.x += map->oneUpOffsetX;
                    hb.y += map->oneUpOffsetY;
                    if (!shop1UpBought && CheckCollisionRecs(player->GetHitbox(), hb) && IsKeyPressed(KEY_SPACE)) {
                        if (playerMoney >= 100) {
                            playerMoney -= 100;
                            lives++;
                            shop1UpBought = true;
                            std::cout << "Bought 1UP! Lives: " << lives << " Money: " << playerMoney << std::endl;
                        }
                    }
                } else if (tid == 43) { // Exit Sign
                    hb.x += map->exitOffsetX;
                    hb.y += map->exitOffsetY;
                    hb.width = 24;
                    hb.height = 60;
                    if (CheckCollisionRecs(player->GetHitbox(), hb) && player->IsGrounded() && IsKeyPressed(KEY_SPACE)) {
                        ExitShop();
                        return; // Level changed, stop updating this frame
                    }
                }
            }
        }
    }
}

void GameManager::Draw() {
    if (map->GetCurrentLevel() == 0) {
        map->DrawMainMenu(menuTimer, menuColorVariant);
        return;
    }

    if (isGameOver) {
        ClearBackground(BLACK);

        const char* gameOverText = "GAME OVER";
        int goWidth = MeasureText(gameOverText, 20);
        
        // Use camera.target to draw exactly in the center of the current screen view
        float centerX = camera.target.x;
        float centerY = camera.target.y;

        DrawText(gameOverText, centerX - (goWidth / 2), centerY - 20, 20, WHITE);

        std::string scoreStr = "Score: " + std::to_string(score);
        int scoreWidth = MeasureText(scoreStr.c_str(), 10);
        DrawText(scoreStr.c_str(), centerX - (scoreWidth / 2), centerY + 10, 10, WHITE);

        return; 
    }

    // 1. Sky Layer
    map->DrawBackground();

    // 2. Enemies (Only if NOT in shop)
    if (map->GetCurrentLevel() != 3) {
        // Plant Enemies (Only the Plants)
        for (auto enemy : enemies) {
            if (enemy->GetType() == EnemyType::PLANT) {
                enemy->Draw(showDebugHitboxes);
            }
        }

        // Static hazards drawn behind player/foreground
        for (auto enemy : enemies) {
            if (enemy->GetType() == EnemyType::LAVA || enemy->GetType() == EnemyType::QUICKSAND) {
                enemy->Draw(showDebugHitboxes);
            }
        }
    }

    // 3. Main Map Tiles
    map->DrawTiles(shop1UpBought);
    
    // Draw Dropped Items
    for (const auto& item : droppedItems) {
        map->DrawTile(item.tileID, item.position, shop1UpBought);
    }

    // 4. Moving Enemies (Only if NOT in shop)
    if (map->GetCurrentLevel() != 3) {
        for (auto enemy : enemies) {
            if (enemy->GetType() == EnemyType::BIRD || enemy->GetType() == EnemyType::SCORPION || enemy->GetType() == EnemyType::FROG) {
                enemy->Draw(showDebugHitboxes);
            }
        }
    }

    player->Draw(showDebugHitboxes);
    
    // Foreground Pass (Tile 12 Tall Grass)
    map->DrawForeground();
}
