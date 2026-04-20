#include "Enemy.h"
#include "MapManager.h"
#include <cmath>

// --- Bird Implementation ---

Bird::Bird(Vector2 pos) : Enemy(pos), facingRight(true), animTimer(0), frame(0) {
    texture = LoadTexture("Sprites/bird.png");
}

Bird::~Bird() {
    UnloadTexture(texture);
}

void Bird::Update(float deltaTime, const MapManager& map) {
    if (dead) return;

    // Movement
    float moveX = (facingRight ? speed : -speed) * deltaTime;
    position.x += moveX;

    // Wall collision
    if (map.CheckCollision(GetHitbox())) {
        position.x -= moveX;
        facingRight = !facingRight;
    }

    // Animation
    animTimer += deltaTime;
    if (animTimer >= 0.2f) {
        animTimer = 0;
        frame = (frame == 0) ? 1 : 0;
    }
}

void Bird::Draw(bool showDebug) {
    if (dead) return;

    float width = 24.0f;
    if (!facingRight) width = -width;

    Rectangle source = { (float)frame * 24.0f, 0, width, 16.0f };
    DrawTextureRec(texture, source, position, WHITE);

    if (showDebug) {
        DrawRectangleLinesEx(GetHitbox(), 1, RED);
    }
}

Rectangle Bird::GetHitbox() const {
    return { position.x, position.y, 24, 16 };
}

// --- Scorpion Implementation ---

Scorpion::Scorpion(Vector2 pos) : Enemy(pos), facingLeft(true), animTimer(0), frame(0) {
    // Align to bottom of tile if needed, but for now just use spawn pos
    texture = LoadTexture("Sprites/scorpion.png");
    // Ensure it's on the ground (spawn might be at top-left of tile)
    position.y += (16 - 14); 
}

Scorpion::~Scorpion() {
    UnloadTexture(texture);
}

void Scorpion::Update(float deltaTime, const MapManager& map) {
    if (dead) return;

    // Movement
    float moveX = (facingLeft ? -speed : speed) * deltaTime;
    position.x += moveX;

    bool shouldReverse = false;

    // Wall collision
    if (map.CheckCollision(GetHitbox())) {
        shouldReverse = true;
    } else {
        // Edge detection
        // Check a point slightly ahead and below
        float checkX = facingLeft ? position.x : position.x + 16;
        Rectangle edgeCheck = { checkX, position.y + 14 + 1, 1, 1 };
        if (!map.CheckCollision(edgeCheck)) {
            shouldReverse = true;
        }
    }

    if (shouldReverse) {
        position.x -= moveX;
        facingLeft = !facingLeft;
    }

    // Animation
    animTimer += deltaTime;
    if (animTimer >= 0.2f) {
        animTimer = 0;
        frame = (frame == 0) ? 1 : 0;
    }
}

void Scorpion::Draw(bool showDebug) {
    if (dead) return;

    float width = 16.0f;
    if (!facingLeft) width = -width; // Assuming default is facing Left, so negative width flips to Right

    Rectangle source = { (float)frame * 16.0f, 0, width, 14.0f };
    DrawTextureRec(texture, source, position, WHITE);

    if (showDebug) {
        DrawRectangleLinesEx(GetHitbox(), 1, RED);
    }
}

Rectangle Scorpion::GetHitbox() const {
    return { position.x, position.y, 16, 14 };
}
