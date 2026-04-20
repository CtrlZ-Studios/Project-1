#ifndef PLAYER_MANAGER_H
#define PLAYER_MANAGER_H

#include "raylib.h"

enum class PlayerState {
    IDLE,
    WALKING,
    JUMPING,
    CROUCHING,
    ATTACKING,
    BLOCKED_CROUCH
};

class MapManager; // Forward declaration

class PlayerManager {
public:
    PlayerManager();
    PlayerManager(Vector2 spawnPos = {100, 150});
    ~PlayerManager();

    void Update(float deltaTime, const MapManager& map);
    void Draw(bool showDebug = false);
    Vector2 GetPosition() const;
    Rectangle GetHitbox() const;
    Rectangle GetAttackHitbox() const;
    PlayerState GetState() const { return state; }

    // Constants
    static const int frameWidth = 24;
    static const int frameHeight = 24;
    static const int spineOffset = 8;
    static const int crouchHitboxHeight = 16;

private:
    Vector2 position;
    Vector2 velocity;
    bool isGrounded;
    bool facingRight;

    Texture2D spriteSheet;
    bool spriteLoaded;

    // Last input priority tracking
    int lastMoveKeyPressed; // 0: None, 1: Left (A), 2: Right (D)

    // Animation and State variables
    PlayerState state;
    int currentFrame;
    float frameTimer;
    float frameDuration;
    float attackTimer;
    Rectangle attackHitbox;

    // Movement Constants
    const float maxMoveSpeed = 120.0f;
    const float moveAcceleration = 1000.0f;
    const float groundFriction = 480.0f;
    const float gravity = 800.0f;
    const float terminalVelocity = 350.0f;
    const float jumpForceStanding = -277.13f; // Height: 3 tiles (48px) -> sqrt(2 * 1600 * 48)
    const float jumpForceMoving = -320.0f;   // Height: 4 tiles (64px) -> sqrt(2 * 1600 * 64)
    const float jumpCutMultiplier = 0.5f;
    const float blockedCrouchSpeed = 5.0f;
};

#endif // PLAYER_MANAGER_H
