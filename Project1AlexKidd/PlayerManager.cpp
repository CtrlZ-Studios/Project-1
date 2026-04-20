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
    lastMoveKeyPressed = 0;
    
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

void PlayerManager::Update(float deltaTime, const MapManager& map) {
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

    // Last Input Priority Logic
    if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) lastMoveKeyPressed = 1;
    if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) lastMoveKeyPressed = 2;
    
    // If holding both, resolve to the last pressed. If holding one, resolve to that one.
    int activeDirection = 0; // 0 = none, 1 = left, 2 = right
    if (moveLeftInput && moveRightInput) {
        activeDirection = lastMoveKeyPressed;
    } else if (moveLeftInput) {
        activeDirection = 1;
    } else if (moveRightInput) {
        activeDirection = 2;
    }

    // 3. Movement Logic
    
    // Check for Blocked Crouch conditions
    bool wasCrouching = (state == PlayerState::CROUCHING || state == PlayerState::BLOCKED_CROUCH);
    Rectangle headSpace = { position.x - 5, position.y, 10, (float)(frameHeight - crouchHitboxHeight) };
    bool isCeilingBlocked = map.CheckCollision(headSpace);
    bool shouldBeBlockedCrouch = wasCrouching && !crouchInput && isCeilingBlocked && isGrounded;

    // Crouch Lock: Cannot build speed, but can change facing direction
    if (crouchInput) {
        if (activeDirection == 1) facingRight = false;
        if (activeDirection == 2) facingRight = true;
        // Friction still applies while crouching so we can slide
        if (velocity.x > 0) {
            velocity.x -= groundFriction * deltaTime;
            if (velocity.x < 0) velocity.x = 0;
        } else if (velocity.x < 0) {
            velocity.x += groundFriction * deltaTime;
            if (velocity.x > 0) velocity.x = 0;
        }
    } else if (shouldBeBlockedCrouch) {
        // Blocked Crouch Movement (Slow)
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
        // Normal Movement (Allowed if not attacking on ground)
        if (attackTimer <= 0 || !isGrounded) {
            if (activeDirection == 1) {
                velocity.x -= moveAcceleration * deltaTime;
                facingRight = false;
            } else if (activeDirection == 2) {
                velocity.x += moveAcceleration * deltaTime;
                facingRight = true;
            } else {
                // Apply Ground Friction when no keys are held
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

    // Clamp horizontal velocity
    if (velocity.x > maxMoveSpeed) velocity.x = maxMoveSpeed;
    if (velocity.x < -maxMoveSpeed) velocity.x = -maxMoveSpeed;
    
    // 4. X Collision (Moved UP so it stops speed BEFORE jump check)
    position.x += velocity.x * deltaTime;
    if (map.CheckCollision(GetHitbox())) {
        position.x -= velocity.x * deltaTime;
        velocity.x = 0;
    }

    // Gravity
    velocity.y += gravity * deltaTime;
    if (velocity.y > terminalVelocity) velocity.y = terminalVelocity;

    // Jumping and Attack Trigger (Only if NOT already attacking or crouching)
    if (attackTimer <= 0 && !crouchInput && !shouldBeBlockedCrouch) {
        // Jumping
        if (isGrounded && (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W))) {
            if (fabs(velocity.x) > 0.5f) {
                velocity.y = jumpForceMoving;
            } else {
                velocity.y = jumpForceStanding;
            }
            isGrounded = false;
        }

        // Variable Jump Height (Jump Cut)
        if (!isGrounded && velocity.y < 0) {
            if (IsKeyReleased(KEY_SPACE) || IsKeyReleased(KEY_W)) {
                velocity.y *= jumpCutMultiplier;
            }
        }

        // Trigger Attack
        if (attackInput) {
            attackTimer = 0.2f;
            if (isGrounded) velocity.x = 0; // Stop horizontal movement on ground when attacking
        }
    }

// 5. Y Collision (With Thinner Hitbox, Ceiling Snap, AND Sprite Offset Fix)
    isGrounded = false;
    position.y += velocity.y * deltaTime;
    
    // Thinner hitbox for Y-checks to ignore side walls
    Rectangle yHitbox = GetHitbox();
    yHitbox.x += 2;
    yHitbox.width -= 4;

    // --- NEW: Account for the 2-pixel transparent gap in the jumping sprite! ---
    // If he's in the air, shrink the top of the hitbox by 2 pixels.
    int topOffset = (!isGrounded) ? 2 : 0; 
    yHitbox.y += topOffset;
    yHitbox.height -= topOffset;

    if (map.CheckCollision(yHitbox)) {
        if (velocity.y > 0) { // Falling down onto a floor
            isGrounded = true;
            // Snap perfectly to the grid and STAY there
            position.y = floorf((position.y + frameHeight) / TILE_SIZE) * TILE_SIZE - frameHeight;
        } else if (velocity.y < 0) { // Jumping up into a ceiling
            // Snap perfectly to the bottom of the tile above, minus the empty sprite space!
            position.y = floorf((position.y + topOffset) / TILE_SIZE) * TILE_SIZE + TILE_SIZE - topOffset;
        }
        velocity.y = 0; // Stop vertical momentum
    }

    // World Boundaries
    if (position.x - 8 < 0) position.x = 8;
    if (position.x + 16 > WORLD_WIDTH) position.x = (float)WORLD_WIDTH - 16;
    if (position.y < 0) position.y = 0;
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
        float hitboxX = facingRight ? position.x + 4 : position.x - 16;
        attackHitbox = { hitboxX, position.y + 7, 12, 10 };
    } else {
        attackHitbox = { 0, 0, 0, 0 };
    }

    // 8. Animation Logic
    switch (state) {
        case PlayerState::ATTACKING:
            currentFrame = 5;
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
        
        DrawTextureRec(spriteSheet, source, drawPos, WHITE);
    } else {
        // Fallback
        Rectangle hb = GetHitbox();
        if (!isGrounded) {
            hb.y += 2;
            hb.height -= 2;
        }
        DrawRectangleRec(hb, BLUE);
    }

    if (showDebug) {
        // Get the base hitbox
        Rectangle debugBox = GetHitbox();
        
        // Shrink the top by 2 pixels when in the air, exactly like the physics check!
        if (!isGrounded) {
            debugBox.y += 2;
            debugBox.height -= 2;
        }
        
        DrawRectangleRec(debugBox, { 0, 255, 0, 150 });
        
        if (state == PlayerState::ATTACKING) {
            DrawRectangleRec(attackHitbox, { 255, 0, 0, 150 });
        }
    }
}

Vector2 PlayerManager::GetPosition() const {
    return position;
}

Rectangle PlayerManager::GetHitbox() const {
    // Note: This needs to be consistent during Update, but state might lag.
    // However, in Alex Kidd, the crouch hitbox is smaller.
    // If we are currently holding crouch and grounded, we use crouch hitbox.
    // To be safe, let's use the actual state if it's already set to CROUCHING.
    int currentHitboxHeight = (state == PlayerState::CROUCHING || state == PlayerState::BLOCKED_CROUCH) ? crouchHitboxHeight : frameHeight;
    return { 
        position.x - 5, 
        position.y + (frameHeight - currentHitboxHeight), 
        10, 
        (float)currentHitboxHeight 
    };
}

Rectangle PlayerManager::GetAttackHitbox() const {
    return attackHitbox;
}
