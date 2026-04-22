#ifndef PLAYER_MANAGER_H
#define PLAYER_MANAGER_H

#include "raylib.h"

enum class PlayerState {
    IDLE,
    WALKING,
    JUMPING,
    CROUCHING,
    ATTACKING,
    BLOCKED_CROUCH,
    STUNNED
};

class MapManager; // Forward declaration

class PlayerManager {
public:
    PlayerManager();
    PlayerManager(Vector2 spawnPos = {100, 150});
    ~PlayerManager();

    void Update(float deltaTime, const MapManager& map);
    void Draw(bool showDebug = false);
    Vector2& GetPosition();
    Vector2 GetPosition() const;
    Rectangle GetHitbox() const;
    Rectangle GetAttackHitbox() const;
    PlayerState GetState() const { return state; }
    bool IsAttackHitboxActive() const { return attackHitboxActive; }
    void DeactivateAttackHitbox() { attackHitboxActive = false; }
    void TriggerStun();
    bool IsGrounded() const { return isGrounded; }

    Vector2 velocity;

    // Constants
    static const int frameWidth = 24;
    static const int frameHeight = 24;
    static const int spineOffset = 8;
    static const int crouchHitboxHeight = 16;

private:
    Vector2 position;
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
    bool attackHitboxActive;

    // Stun Variables
    float stunTimer;
    bool pendingStun;
    Vector2 stunStartPosition;
    bool stunWithPunchSprite;

// Movement Constants
    const float maxMoveSpeed = 120.0f;
    const float moveAcceleration = 1000.0f;
    const float groundFriction = 480.0f;
    
    // -- UPDATED FOR FLOATY AIR TIME --
    const float gravity = 450.0f; // Lowered from 800 for more air-time
    const float terminalVelocity = 350.0f;
    const float jumpForceStanding = -207.85f; // Recalculated for exactly 3 tiles (48px)
    const float jumpForceMoving = -240.0f;    // Recalculated for exactly 4 tiles (64px)
    const float jumpCutMultiplier = 0.5f;
    const float blockedCrouchSpeed = 5.0f;

    float stoppingFriction = 1000.0f;
    bool applyLandingFriction = false;
    bool wasGrounded = false;
};

#endif // PLAYER_MANAGER_H
