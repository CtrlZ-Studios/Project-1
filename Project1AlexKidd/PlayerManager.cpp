#include "PlayerManager.h"
#include "MapManager.h"
#include "raymath.h"

PlayerManager::PlayerManager() {
    position = { 100, 300 };
    velocity = { 0, 0 };
    isGrounded = false;
    facingRight = true;
    
    spriteSheet = LoadTexture("Sprites/alex.png");
    spriteLoaded = (spriteSheet.id != 0);

    currentFrame = 0; // Start at first frame to ensure visibility
    frameTimer = 0.0f;
    frameDuration = 0.1f;
}

PlayerManager::~PlayerManager() {
    if (spriteLoaded) {
        UnloadTexture(spriteSheet);
    }
}

void PlayerManager::Update(float deltaTime, Rectangle floor) {
    // Horizontal Movement
    velocity.x = 0;
    if (IsKeyDown(KEY_A)) {
        velocity.x = -moveSpeed;
        facingRight = false;
    }
    if (IsKeyDown(KEY_D)) {
        velocity.x = moveSpeed;
        facingRight = true;
    }

    // Gravity
    velocity.y += gravity * deltaTime;

    // Jumping
    if (isGrounded && IsKeyPressed(KEY_SPACE)) {
        velocity.y = jumpForce;
        isGrounded = false;
    }

    // Apply Velocity
    position.x += velocity.x * deltaTime;
    position.y += velocity.y * deltaTime;

    // Clamp position within world bounds
    if (position.x < 0) position.x = 0;
    if (position.x > WORLD_WIDTH - PlayerManager::frameWidth) position.x = WORLD_WIDTH - PlayerManager::frameWidth;
    
    if (position.y < 0) position.y = 0;
    if (position.y > WORLD_HEIGHT - PlayerManager::frameHeight) position.y = WORLD_HEIGHT - PlayerManager::frameHeight;

    // Floor Collision
    isGrounded = false;
    if (CheckCollisionRecs({ position.x, position.y, (float)PlayerManager::frameWidth, (float)PlayerManager::frameHeight }, floor)) {
        if (velocity.y > 0) {
            position.y = floor.y - PlayerManager::frameHeight;
            velocity.y = 0;
            isGrounded = true;
        }
    }

    // Animation Logic
    if (!isGrounded) {
        // Simple jump frame if needed, but requirements say walk 0-3, idle 4.
        // We'll stick to those.
        currentFrame = 0; // Use first walk frame for jumping for now
    } else if (velocity.x != 0) {
        // Walk Animation (0-3)
        frameTimer += deltaTime;
        if (frameTimer >= frameDuration) {
            frameTimer = 0;
            currentFrame++;
            if (currentFrame > 3) currentFrame = 0;
        }
    } else {
        // Idle Animation (4)
        currentFrame = 4;
        frameTimer = 0;
    }
}

void PlayerManager::Draw() {
    if (spriteLoaded) {
        Rectangle source = { 
            (float)currentFrame * PlayerManager::frameWidth, 
            0, 
            facingRight ? (float)PlayerManager::frameWidth : -(float)PlayerManager::frameWidth, 
            (float)PlayerManager::frameHeight 
        };
        DrawTextureRec(spriteSheet, source, position, WHITE);
    } else {
        // Fallback
        DrawRectangleV(position, { (float)PlayerManager::frameWidth, (float)PlayerManager::frameHeight }, BLUE);
    }
}

Vector2 PlayerManager::GetPosition() const {
    return position;
}
