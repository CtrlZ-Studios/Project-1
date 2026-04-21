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
        DrawRectangleLinesEx(GetHitbox(), 1, { 255, 0, 0, 200 }); // Red for enemy damage box
    }
}

Rectangle Bird::GetHitbox() const {
    return { position.x + 1, position.y, 22, 16 };
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

    const float gravity = 800.0f;
    const float terminalVelocity = 350.0f;

    // Apply Gravity
    if (!isGrounded) {
        velocity.y += gravity * deltaTime;
        if (velocity.y > terminalVelocity) velocity.y = terminalVelocity;
    }

    // Horizontal Movement
    float moveX = (facingLeft ? -speed : speed) * deltaTime;
    position.x += moveX;

    bool shouldReverse = false;

    // Wall collision (Horizontal)
    Rectangle hbx = GetHitbox();
    if (map.CheckCollision(hbx)) {
        position.x -= moveX;
        shouldReverse = true;
    } else if (isGrounded) {
        // Predictive Edge Detection ("Feeler" Logic)
        float feelerX = facingLeft ? hbx.x - 1.0f : hbx.x + hbx.width + 1.0f;
        float feelerY = hbx.y + hbx.height + 1.0f;
        Rectangle edgeCheck = { feelerX, feelerY, 1.0f, 1.0f };
        
        if (!map.CheckCollision(edgeCheck)) {
            position.x -= moveX; // Strictly prevent overhang
            shouldReverse = true;
        }
    }

    if (shouldReverse) {
        facingLeft = !facingLeft;
    }

    // Vertical Movement and Collision
    position.y += velocity.y * deltaTime;
    isGrounded = false;

    Rectangle hb = GetHitbox();
    if (map.CheckCollision(hb)) {
        if (velocity.y > 0) {
            isGrounded = true;
            int r = (int)floorf((hb.y + hb.height) / TILE_SIZE);
            
            // Check for Tile 12 snap
            bool hitTile12 = false;
            int startCol = (int)floorf(hb.x / TILE_SIZE);
            int endCol = (int)floorf((hb.x + hb.width) / TILE_SIZE);
            for (int c = startCol; c <= endCol; c++) {
                if (map.GetTile(r, c) == 12) {
                    hitTile12 = true;
                    break;
                }
            }

            if (hitTile12) {
                position.y = (float)r * TILE_SIZE + 6 - 14;
            } else {
                position.y = (float)r * TILE_SIZE - 14;
            }
        } else if (velocity.y < 0) {
            int r = (int)floorf(hb.y / TILE_SIZE);
            position.y = (float)r * TILE_SIZE + TILE_SIZE;
        }
        velocity.y = 0;
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
        DrawRectangleLinesEx(GetHitbox(), 1, { 255, 0, 0, 200 }); // Red for enemy damage box
    }
}

Rectangle Scorpion::GetHitbox() const {
    return { position.x + 1, position.y, 14, 14 };
}

// --- Lava Implementation ---

Lava::Lava(Vector2 pos) : Enemy(pos), animTimer(0), frame(0) {
    texture = LoadTexture("Sprites/lava.png");
}

Lava::~Lava() {
    UnloadTexture(texture);
}

void Lava::Update(float deltaTime, const MapManager& map) {
    if (dead) return;

    // Continuous animation
    animTimer += deltaTime;
    if (animTimer >= 0.15f) {
        animTimer = 0;
        frame = (frame + 1) % 4;
    }
}

void Lava::Draw(bool showDebug) {
    if (dead) return;

    Rectangle source = { (float)frame * 16.0f, 0, 16.0f, 16.0f };
    DrawTextureRec(texture, source, position, WHITE);

    if (showDebug) {
        DrawRectangleLinesEx(GetHitbox(), 1, { 255, 0, 0, 200 }); // Red for enemy damage box
    }
}

Rectangle Lava::GetHitbox() const {
    // 16x10, 6 pixels off the top
    return { position.x, position.y + 6, 16, 10 };
}

// --- Plant Implementation ---

Plant::Plant(Vector2 pos) : Enemy(pos), startPos(pos), moveTimer(0) {
    texture = LoadTexture("Sprites/plant.png");
}

Plant::~Plant() {
    UnloadTexture(texture);
}

void Plant::Update(float deltaTime, const MapManager& map) {
    if (dead) return;

    // Continuous up and down loop (48 pixels range)
    moveTimer += deltaTime;
    // Simple sine wave or triangle wave for smooth oscillation
    // Using sine for smoothness: center is startPos.y - 24, amplitude 24
    position.y = (startPos.y - 24.0f) + sinf(moveTimer * 2.0f) * 24.0f;
}

void Plant::Draw(bool showDebug) {
    if (dead) return;

    Rectangle source = { 0, 0, 16.0f, 32.0f };
    DrawTextureRec(texture, source, position, WHITE);

    if (showDebug) {
        DrawRectangleLinesEx(GetHitbox(), 1, { 255, 0, 0, 200 }); // Red for enemy damage box
    }
}

Rectangle Plant::GetHitbox() const {
    return { position.x, position.y, 16, 32 };
}

// --- Quicksand Implementation ---

Quicksand::Quicksand(Vector2 pos) : Enemy(pos), animTimer(0), frame(0) {
    texture = LoadTexture("Sprites/quicksand.png");
}

Quicksand::~Quicksand() {
    UnloadTexture(texture);
}

void Quicksand::Update(float deltaTime, const MapManager& map) {
    if (dead) return;

    // Continuous animation
    animTimer += deltaTime;
    if (animTimer >= 0.15f) {
        animTimer = 0;
        frame = (frame + 1) % 4;
    }
}

void Quicksand::Draw(bool showDebug) {
    if (dead) return;

    Rectangle source = { (float)frame * 16.0f, 0, 16.0f, 16.0f };
    DrawTextureRec(texture, source, position, WHITE);

    if (showDebug) {
        DrawRectangleLinesEx(GetHitbox(), 1, { 255, 0, 0, 200 }); // Red for enemy damage box
    }
}

Rectangle Quicksand::GetHitbox() const {
    // 16x10, 6 pixels off the top
    return { position.x, position.y + 6, 16, 10 };
}

// --- Frog Implementation ---

Frog::Frog(Vector2 pos) : Enemy(pos), animTimer(0), cooldownTimer(0.5f), frame(0) {
    texture = LoadTexture("Sprites/frog.png");
}

Frog::~Frog() {
    UnloadTexture(texture);
}

void Frog::Update(float deltaTime, const MapManager& map) {
    if (dead) return;

    const float gravity = 800.0f;
    const float terminalVelocity = 350.0f;
    const float jumpForce = -sqrtf(2.0f * gravity * 35.0f); // Apex of 35 pixels

    // Apply Gravity and Handle Animation States
    if (!isGrounded) {
        velocity.y += gravity * deltaTime;
        if (velocity.y > terminalVelocity) velocity.y = terminalVelocity;
        frame = 1; // Airborne
    } else {
        velocity.y = 0;
        frame = 0; // Grounded
        
        // Jump Logic
        cooldownTimer -= deltaTime;
        if (cooldownTimer <= 0) {
            velocity.y = jumpForce;
            isGrounded = false;
            frame = 1; 
            cooldownTimer = 0.5f;
        }
    }

    // Vertical Movement and Collision
    position.y += velocity.y * deltaTime;
    
    // We don't reset isGrounded here to prevent flicker; 
    // we only reset it if we actually moved away from the ground (which we do in the jump logic)
    // or if we fail the collision check below.
    
    Rectangle hb = GetHitbox();
    if (map.CheckCollision(hb)) {
        if (velocity.y > 0) {
            isGrounded = true;
            int r = (int)floorf((hb.y + hb.height) / TILE_SIZE);
            
            bool hitTile12 = false;
            int startCol = (int)floorf(hb.x / TILE_SIZE);
            int endCol = (int)floorf((hb.x + hb.width) / TILE_SIZE);
            for (int c = startCol; c <= endCol; c++) {
                if (map.GetTile(r, c) == 12) {
                    hitTile12 = true;
                    break;
                }
            }
            
            float snapY = hitTile12 ? ((float)r * TILE_SIZE + 6) : ((float)r * TILE_SIZE);
            position.y = snapY - 24.0f;
            velocity.y = 0;
            frame = 0;
        } else if (velocity.y < 0) {
            int r = (int)floorf(hb.y / TILE_SIZE);
            position.y = (float)r * TILE_SIZE + TILE_SIZE - (hb.y - position.y);
            velocity.y = 0;
        }
    } else {
        isGrounded = false;
    }
}

void Frog::Draw(bool showDebug) {
    if (dead) return;

    Rectangle source = { (float)frame * 16.0f, 0, 16.0f, 24.0f };
    DrawTextureRec(texture, source, position, WHITE);

    if (showDebug) {
        DrawRectangleLinesEx(GetHitbox(), 1, { 255, 0, 0, 200 }); // Red for enemy damage box
    }
}

Rectangle Frog::GetHitbox() const {
    if (frame == 0) {
        // Grounded: 12x16, +2 pixel X offset, +8 pixel Y offset
        return { position.x + 2, position.y + 8, 12, 16 };
    } else {
        // Airborne: 12x24, +2 pixel X offset
        return { position.x + 2, position.y, 12, 24 };
    }
}
