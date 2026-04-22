#include "PlayerManager.h"
#include "MapManager.h"
#include "raymath.h"
#include <cmath>

PlayerManager::PlayerManager(Vector2 spawnPos) {
    position = spawnPos;
    velocity = { 0, 0 };
    isGrounded = false;
    facingRight = true;
    state = PlayerState::IDLE;
    attackTimer = 0.0f;
    attackHitbox = { 0, 0, 0, 0 };
    attackHitboxActive = false;
    lastMoveKeyPressed = 0;
    
    stunTimer = 0.0f;
    pendingStun = false;
    stunStartPosition = { 0, 0 };
    stunWithPunchSprite = false;

    spriteSheet = LoadTexture("Sprites/alex.png");
    SetTextureFilter(spriteSheet, TEXTURE_FILTER_POINT);
    spriteLoaded = (spriteSheet.id != 0);

    deathSpriteSheet = LoadTexture("Sprites/death.png");
    invencibleSpriteSheet = LoadTexture("Sprites/invencible.png");
    deathSpriteLoaded = (deathSpriteSheet.id != 0);
    invencibleSpriteLoaded = (invencibleSpriteSheet.id != 0);

    currentFrame = 4; // Start at idle frame
    frameTimer = 0.0f;
    frameDuration = 0.1f;
}

PlayerManager::~PlayerManager() {
    if (spriteLoaded) {
        UnloadTexture(spriteSheet);
    }
    if (deathSpriteLoaded) {
        UnloadTexture(deathSpriteSheet);
    }
    if (invencibleSpriteLoaded) {
        UnloadTexture(invencibleSpriteSheet);
    }
}

void PlayerManager::TriggerStun() {
    if (isGrounded) {
        state = PlayerState::STUNNED;
        stunTimer = 0.7f;
        stunStartPosition = position;
        stunWithPunchSprite = true;
        velocity = { 0, 0 };
    } else {
        pendingStun = true;
    }
}

Vector2& PlayerManager::GetPosition() {
    return position;
}

Vector2 PlayerManager::GetPosition() const {
    return position;
}

Rectangle PlayerManager::GetHitbox() const {
    int currentHitboxHeight = (state == PlayerState::CROUCHING || state == PlayerState::BLOCKED_CROUCH) ? crouchHitboxHeight : frameHeight;
    return { 
        position.x - 5, 
        position.y + (frameHeight - currentHitboxHeight), 
        10, 
        (float)currentHitboxHeight 
    };
}

void PlayerManager::Update(float deltaTime, const MapManager& map) {
    if (state == PlayerState::DYING) {
        position.y += velocity.y * deltaTime; // Float upwards
        frameTimer += deltaTime;
        if (frameTimer >= 0.15f) { // Cycle angel animation
            frameTimer = 0.0f;
            currentFrame = (currentFrame + 1) % 3;
        }
        return; // SKIP ALL PHYSICS AND INPUTS
    }

    if (invincibilityTimer > 0) {
        invincibilityTimer -= deltaTime;
    }
    if (IsInvincible()) {
        flickerTimer += deltaTime;
        if (flickerTimer >= invincibilityFlickerRate) {
            flickerTimer = 0;
            currentColorRow = (currentColorRow == 0) ? 1 : 0;
        }
    }

    wasGrounded = isGrounded;

    // 0. Handle Stun Logic
    if (state == PlayerState::STUNNED) {
        stunTimer -= deltaTime;
        
        // Oscillate X coordinate rapidly (e.g., 30 times per second)
        int frameOffset = ((int)(stunTimer * 30.0f)) % 2 == 0 ? 1 : -1;
        position.x = stunStartPosition.x + frameOffset;

        if (stunTimer <= 0) {
            stunTimer = 0;
            position.x = stunStartPosition.x;
            state = PlayerState::IDLE;
        } else {
            // Lock movement and animation
            currentFrame = stunWithPunchSprite ? 5 : 4;
            return; // Skip normal update
        }
    }

    // 1. Handle Attack Timer
    if (attackTimer > 0) {
        attackTimer -= deltaTime;
        if (attackTimer <= 0) {
            attackTimer = 0;
        }
    }

    // 2. Determine Input
    bool attackInput = IsKeyPressed(KEY_J);
    bool moveLeftInput = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT);
    bool moveRightInput = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT);
    bool crouchInput = (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) && isGrounded;

    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_D) || IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT)) {
        applyLandingFriction = false;
    }

    if (state != PlayerState::ATTACKING) {
        attackHitboxActive = false;
    }

    if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) lastMoveKeyPressed = 1;
    if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) lastMoveKeyPressed = 2;
    
    int activeDirection = 0;
    if (moveLeftInput && moveRightInput) {
        activeDirection = lastMoveKeyPressed;
    } else if (moveLeftInput) {
        activeDirection = 1;
    } else if (moveRightInput) {
        activeDirection = 2;
    }

    // 3. Movement Logic
    bool wasCrouching = (state == PlayerState::CROUCHING || state == PlayerState::BLOCKED_CROUCH);
    Rectangle headSpace = { position.x - 5, position.y, 10, (float)(frameHeight - crouchHitboxHeight) };
    bool isCeilingBlocked = map.CheckCollision(headSpace);
    bool shouldBeBlockedCrouch = wasCrouching && !crouchInput && isCeilingBlocked && isGrounded;

    if (crouchInput) {
        if (activeDirection == 1) facingRight = false;
        if (activeDirection == 2) facingRight = true;
        if (velocity.x > 0) {
            velocity.x -= groundFriction * deltaTime;
            if (velocity.x < 0) velocity.x = 0;
        } else if (velocity.x < 0) {
            velocity.x += groundFriction * deltaTime;
            if (velocity.x > 0) velocity.x = 0;
        }
    } else if (shouldBeBlockedCrouch) {
        if (activeDirection == 1) {
            velocity.x = -blockedCrouchSpeed;
            facingRight = false;
        } else if (activeDirection == 2) {
            velocity.x = blockedCrouchSpeed;
            facingRight = true;
        } else {
            velocity.x = 0;
        }
    } else {
        if (attackTimer <= 0 || !isGrounded) {
            if (activeDirection == 1) {
                velocity.x -= moveAcceleration * deltaTime;
                facingRight = false;
            } else if (activeDirection == 2) {
                velocity.x += moveAcceleration * deltaTime;
                facingRight = true;
            } else {
                if (isGrounded) {
                    // Step B: Decelerate using Anti-Ice logic
                    float currentFriction = applyLandingFriction ? stoppingFriction : groundFriction;
                    
                    if (velocity.x > 0) {
                        velocity.x -= currentFriction * deltaTime;
                        if (velocity.x <= 0) {
                            velocity.x = 0;
                            applyLandingFriction = false; // Reset once stopped
                        }
                    } else if (velocity.x < 0) {
                        velocity.x += currentFriction * deltaTime;
                        if (velocity.x >= 0) {
                            velocity.x = 0;
                            applyLandingFriction = false; // Reset once stopped
                        }
                    }
                }
            }
        }
    }

    if (velocity.x > maxMoveSpeed) velocity.x = maxMoveSpeed;
    if (velocity.x < -maxMoveSpeed) velocity.x = -maxMoveSpeed;
    
    // --- TASK 2: RESOLVE Y COLLISION BEFORE X COLLISION ---
    velocity.y += gravity * deltaTime;
    if (velocity.y > terminalVelocity) velocity.y = terminalVelocity;

    if (attackTimer <= 0 && !crouchInput && !shouldBeBlockedCrouch) {
        // Step C: The 3-Tier Jump
        if (isGrounded && (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))) {
            float absVx = fabs(velocity.x);
            if (absVx < maxMoveSpeed * 0.20f) {
                velocity.y = jumpForceStanding;
            } else if (absVx > maxMoveSpeed * 0.80f) {
                velocity.y = jumpForceMoving;
            } else {
                velocity.y = (jumpForceStanding + jumpForceMoving) / 2.0f;
            }
            isGrounded = false;
        }
        if (!isGrounded && velocity.y < 0) {
            if (IsKeyReleased(KEY_SPACE) || IsKeyReleased(KEY_W)) {
                velocity.y *= jumpCutMultiplier;
            }
        }
        if (attackInput) {
            attackTimer = 0.2f;
            attackHitboxActive = true;
            if (isGrounded) velocity.x = 0;
        }
    }

    // --- Y Movement and Collision ---
    bool wasGrounded = isGrounded;
    isGrounded = false;
    position.y += velocity.y * deltaTime;
    
    Rectangle yHitbox = GetHitbox();
    yHitbox.x += 2.0f;       // Shave sides to avoid wall catching
    yHitbox.width -= 4.0f;

    int topOffset = (!wasGrounded) ? 2 : 0; 
    yHitbox.y += (float)topOffset;
    yHitbox.height -= (float)topOffset;

    if (map.CheckCollision(yHitbox)) {
        if (velocity.y > 0) { // Falling/Landing
            isGrounded = true;

            if (!wasGrounded && isGrounded && !IsKeyDown(KEY_A) && !IsKeyDown(KEY_D) && !IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_RIGHT)) {
                applyLandingFriction = true;
            }
            if (IsKeyDown(KEY_A) || IsKeyDown(KEY_D) || IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT)) {
                applyLandingFriction = false;
            }

            int r = (int)floorf((yHitbox.y + yHitbox.height) / TILE_SIZE);
            
            bool hitTile12 = false;
            bool hitSolid = false;
            int startCol = (int)floorf(yHitbox.x / TILE_SIZE);
            int endCol = (int)floorf((yHitbox.x + yHitbox.width) / TILE_SIZE);
            
            // Check all columns the hitbox spans to prioritize solid blocks over tall grass/low tiles
            for (int c = startCol; c <= endCol; c++) {
                int tid = map.GetTile(r, c);
                if (tid == 12 || tid == 27) {
                    hitTile12 = true;
                } else if ((tid >= 1 && tid <= 15 && tid != 3) || (tid >= 21 && tid <= 24 && tid != 21)) {
                    hitSolid = true;
                }
            }

            // Normal solid tiles MUST win over the lowered grass tile, or you clip into the seams!
            if (hitSolid) {
                position.y = (float)r * TILE_SIZE - frameHeight;
            } else if (hitTile12) {
                position.y = (float)r * TILE_SIZE + 6 - frameHeight;
            } else {
                position.y = (float)r * TILE_SIZE - frameHeight; // Fallback
            }
            
            // Check for pending stun when landing
            if (pendingStun) {
                state = PlayerState::STUNNED;
                stunTimer = 0.7f;
                stunStartPosition = position;
                stunWithPunchSprite = false;
                velocity = { 0, 0 };
                pendingStun = false;
                return;
            }
        } else if (velocity.y < 0) { // Hitting head on ceiling
            int r = (int)floorf((yHitbox.y) / TILE_SIZE);
            position.y = (float)r * TILE_SIZE + TILE_SIZE - topOffset;
        }
        velocity.y = 0;
    }

    // --- X Movement and Collision ---
    position.x += velocity.x * deltaTime;
    
    Rectangle xHitbox = GetHitbox();
    // THE MAGIC FIX: Shave the top and bottom so horizontal checks don't scrape the floor or ceiling!
    xHitbox.y += 2.0f;
    xHitbox.height -= 4.0f;
    
    // Keep your slight side shaving
    xHitbox.x += 0.5f;
    xHitbox.width -= 1.0f;

    if (map.CheckCollision(xHitbox)) {
        if (velocity.x > 0) { // Moving Right
            int tileCol = (int)floorf((xHitbox.x + xHitbox.width) / TILE_SIZE);
            position.x = (float)tileCol * TILE_SIZE - 5.0f;
        } else if (velocity.x < 0) { // Moving Left
            int tileCol = (int)floorf(xHitbox.x / TILE_SIZE);
            position.x = (float)tileCol * TILE_SIZE + TILE_SIZE + 5.0f;
        }
        velocity.x = 0;
    }

    // Map Boundaries Check
    if (position.x - 8 < 0) position.x = 8;
    if (position.x + 16 > map.GetWorldWidth()) position.x = (float)map.GetWorldWidth() - 16;
    if (position.y > WORLD_HEIGHT - frameHeight) position.y = (float)WORLD_HEIGHT - frameHeight;

    // 6. State Determination
    if (attackTimer > 0) {
        state = PlayerState::ATTACKING;
    } else if (isGrounded && crouchInput) {
        state = PlayerState::CROUCHING;
    } else if (isGrounded && shouldBeBlockedCrouch) {
        state = PlayerState::BLOCKED_CROUCH;
    } else if (!isGrounded) {
        state = PlayerState::JUMPING;
    } else if (fabs(velocity.x) > 0.5f) {
        state = PlayerState::WALKING;
    } else {
        state = PlayerState::IDLE;
    }

    // 7. Attack Hitbox Logic
    if (state == PlayerState::ATTACKING) {
        float baseHitboxX = facingRight ? position.x + 4 : position.x - 16;
        attackHitbox = { baseHitboxX + 1, position.y + 8, 10, 8 };
    } else {
        attackHitbox = { 0, 0, 0, 0 };
    }

    // 8. Animation Logic
    switch (state) {
        case PlayerState::ATTACKING:
            currentFrame = 5;
            break;
        case PlayerState::STUNNED:
            currentFrame = stunWithPunchSprite ? 5 : 4;
            break;
        case PlayerState::CROUCHING:
        case PlayerState::BLOCKED_CROUCH:
            currentFrame = 7;
            break;
        case PlayerState::JUMPING:
            currentFrame = 6;
            break;
        case PlayerState::WALKING:
            frameTimer += deltaTime;
            if (frameTimer >= frameDuration) {
                frameTimer = 0;
                currentFrame++;
                if (currentFrame > 3) currentFrame = 0;
            }
            break;
        case PlayerState::IDLE:
        default:
            currentFrame = 4;
            frameTimer = 0;
            break;
    }
}

void PlayerManager::Draw(bool showDebug) {
    if (state == PlayerState::DYING && deathSpriteLoaded) {
        Rectangle source = { currentFrame * 16.0f, 0, 16.0f, 24.0f };
        DrawTextureRec(deathSpriteSheet, source, position, WHITE);
    } 
    else if (IsInvincible() && invencibleSpriteLoaded) {
        int invFrame = 4; // Default Idle
        if (state == PlayerState::WALKING) invFrame = currentFrame % 4;
        else if (state == PlayerState::ATTACKING || state == PlayerState::STUNNED) invFrame = 5;
        else if (state == PlayerState::JUMPING) invFrame = 6;
        else if (state == PlayerState::CROUCHING || state == PlayerState::BLOCKED_CROUCH) invFrame = 7;

        Rectangle source = { invFrame * 24.0f, currentColorRow * 24.0f, facingRight ? 24.0f : -24.0f, 24.0f };
        Vector2 drawPos = { (float)(int)roundf(position.x - spineOffset), (float)(int)roundf(position.y) };
        DrawTextureRec(invencibleSpriteSheet, source, drawPos, WHITE);
    } 
    else if (spriteLoaded) {
        float width = (float)frameWidth;
        if (!facingRight) width = -width;
        
        Rectangle source = { (float)((int)currentFrame * frameWidth), 0, width, (float)frameHeight };
        Vector2 drawPos = { 
            (float)(facingRight ? (int)roundf(position.x) - 8 : (int)roundf(position.x) - 16), 
            (float)(int)roundf(position.y) 
        };
        
        // During stun, we used position.x for the oscillation.
        // The draw logic uses position.x so it should be fine.
        
        DrawTextureRec(spriteSheet, source, drawPos, WHITE);
    } else {
        Rectangle hb = GetHitbox();
        if (!isGrounded && state != PlayerState::STUNNED) {
            hb.y += 2;
            hb.height -= 2;
        }
        DrawRectangleRec(hb, BLUE);
    }

    if (showDebug) {
        // DRAW ACCURATE CROSS HITBOX
        
        // 1. X-Axis Hitbox (The one that hits walls)
        Rectangle xHb = GetHitbox();
        xHb.y += 2.0f;
        xHb.height -= 4.0f;
        xHb.x += 0.5f;
        xHb.width -= 1.0f;
        DrawRectangleLinesEx(xHb, 1.0f, { 0, 255, 0, 200 }); // Green for X

        // 2. Y-Axis Hitbox (The one that hits floors/ceilings)
        Rectangle yHb = GetHitbox();
        yHb.x += 2.0f;
        yHb.width -= 4.0f;
        bool wasGrounded = isGrounded; // Approximate or use actual state
        int topOffset = (!wasGrounded) ? 2 : 0; 
        yHb.y += (float)topOffset;
        yHb.height -= (float)topOffset;
        DrawRectangleLinesEx(yHb, 1.0f, { 0, 0, 255, 200 }); // Blue for Y

        if (state == PlayerState::ATTACKING && attackHitboxActive) {
            DrawRectangleRec(attackHitbox, { 255, 0, 0, 150 });
        }
    }
}

Rectangle PlayerManager::GetAttackHitbox() const {
    return attackHitbox;
}

void PlayerManager::TriggerDeath() {
    state = PlayerState::DYING;
    velocity = { 0, deathAscentSpeed };
    currentFrame = 0;
    frameTimer = 0.0f;
}

void PlayerManager::TriggerRespawn(Vector2 newPos) {
    position = newPos;
    state = PlayerState::IDLE;
    velocity = { 0, 0 };
    invincibilityTimer = invincibilityDuration;
}