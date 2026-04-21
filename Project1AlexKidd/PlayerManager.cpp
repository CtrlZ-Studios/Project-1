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

    currentFrame = 4; // Start at idle frame
    frameTimer = 0.0f;
    frameDuration = 0.1f;
}

PlayerManager::~PlayerManager() {
    if (spriteLoaded) {
        UnloadTexture(spriteSheet);
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
                    if (velocity.x > 0) {
                        velocity.x -= groundFriction * deltaTime;
                        if (velocity.x < 0) velocity.x = 0;
                    } else if (velocity.x < 0) {
                        velocity.x += groundFriction * deltaTime;
                        if (velocity.x > 0) velocity.x = 0;
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
        if (isGrounded && (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W))) {
            if (fabs(velocity.x) > 0.5f) {
                velocity.y = jumpForceMoving;
            } else {
                velocity.y = jumpForceStanding;
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

    // Y Movement and Collision
    isGrounded = false;
    position.y += velocity.y * deltaTime;
    
    Rectangle yHitbox = GetHitbox();
    yHitbox.x += 2;
    yHitbox.width -= 4;

    int topOffset = (state == PlayerState::JUMPING) ? 2 : 0; 
    yHitbox.y += topOffset;
    yHitbox.height -= topOffset;

    if (map.CheckCollision(yHitbox)) {
        if (velocity.y > 0) {
            isGrounded = true;
            int r = (int)floorf((yHitbox.y + yHitbox.height) / TILE_SIZE);
            
            // Check if we hit Tile 12 in the columns covered by the hitbox
            bool hitTile12 = false;
            int startCol = (int)floorf(yHitbox.x / TILE_SIZE);
            int endCol = (int)floorf((yHitbox.x + yHitbox.width) / TILE_SIZE);
            for (int c = startCol; c <= endCol; c++) {
                if (map.GetTile(r, c) == 12) {
                    hitTile12 = true;
                    break;
                }
            }

            if (hitTile12) {
                position.y = (float)r * TILE_SIZE + 6 - frameHeight;
            } else {
                position.y = (float)r * TILE_SIZE - frameHeight;
            }
            
            // Check for pending stun when landing
            if (pendingStun) {
                state = PlayerState::STUNNED;
                stunTimer = 0.7f;
                stunStartPosition = position;
                stunWithPunchSprite = false;
                velocity = { 0, 0 };
                pendingStun = false;
                // Since we just set state to STUNNED, we should return to avoid further processing
                return;
            }
        } else if (velocity.y < 0) {
            int r = (int)floorf((yHitbox.y) / TILE_SIZE);
            position.y = (float)r * TILE_SIZE + TILE_SIZE - topOffset;
        }
        velocity.y = 0;
    }

    // --- X Movement and Collision ---
    position.x += velocity.x * deltaTime;
    
    // TASK 2: Fix Vertical Gap Clipping
    // Use a slightly narrower hitbox for X checks to avoid catching on seams
    Rectangle xHitbox = GetHitbox();
    xHitbox.x += 0.5f;
    xHitbox.width -= 1.0f;

    if (map.CheckCollision(xHitbox)) {
        Rectangle stepUpHitbox = xHitbox;
        stepUpHitbox.y -= 2.0f; 
        
        if (!map.CheckCollision(stepUpHitbox)) {
            // Re-apply grounding logic for step-up to handle Tile 12 correctly
            int r = (int)floorf((position.y + frameHeight) / TILE_SIZE);
            bool hitTile12 = false;
            int startCol = (int)floorf(xHitbox.x / TILE_SIZE);
            int endCol = (int)floorf((xHitbox.x + xHitbox.width) / TILE_SIZE);
            for (int c = startCol; c <= endCol; c++) {
                if (map.GetTile(r, c) == 12) {
                    hitTile12 = true;
                    break;
                }
            }

            if (hitTile12) {
                position.y = (float)r * TILE_SIZE + 6 - frameHeight;
            } else {
                position.y = (float)r * TILE_SIZE - frameHeight;
            }

            isGrounded = true;
            velocity.y = 0;
            
            // Check for pending stun when landing via step-up
            if (pendingStun) {
                state = PlayerState::STUNNED;
                stunTimer = 0.5f;
                stunStartPosition = position;
                stunWithPunchSprite = false;
                velocity = { 0, 0 };
                pendingStun = false;
                return;
            }
        } else {
            position.x -= velocity.x * deltaTime;
            velocity.x = 0;
        }
    }

    if (position.x - 8 < 0) position.x = 8;
    if (position.x + 16 > WORLD_WIDTH) position.x = (float)WORLD_WIDTH - 16;
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
    if (spriteLoaded) {
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
        Rectangle debugBox = GetHitbox();
        if (!isGrounded && state != PlayerState::STUNNED) {
            debugBox.y += 2;
            debugBox.height -= 2;
        }
        DrawRectangleRec(debugBox, { 0, 255, 0, 150 });
        if (state == PlayerState::ATTACKING && attackHitboxActive) {
            DrawRectangleRec(attackHitbox, { 255, 0, 0, 150 });
        }
    }
}

Rectangle PlayerManager::GetAttackHitbox() const {
    return attackHitbox;
}
