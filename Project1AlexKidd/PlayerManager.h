#ifndef PLAYER_MANAGER_H
#define PLAYER_MANAGER_H

#include "raylib.h"

class PlayerManager {
public:
    PlayerManager();
    ~PlayerManager();

    void Update(float deltaTime, Rectangle floor);
    void Draw();
    Vector2 GetPosition() const;

private:
    Vector2 position;
    Vector2 velocity;
    bool isGrounded;
    bool facingRight;

    Texture2D spriteSheet;
    bool spriteLoaded;

    // Animation variables
    int currentFrame;
    float frameTimer;
    float frameDuration;

    // Constants
    const float moveSpeed = 150.0f;
    const float jumpForce = -350.0f;
    const float gravity = 1000.0f;
    const int frameWidth = 15;
    const int frameHeight = 24;
};

#endif // PLAYER_MANAGER_H
