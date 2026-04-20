#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include <vector>

class MapManager;

class Enemy {
public:
    Enemy(Vector2 pos) : position(pos), dead(false) {}
    virtual ~Enemy() {}
    virtual void Update(float deltaTime, const MapManager& map) = 0;
    virtual void Draw(bool showDebug) = 0;
    virtual Rectangle GetHitbox() const = 0;
    bool IsDead() const { return dead; }
    void Die() { dead = true; }

protected:
    Vector2 position;
    bool dead;
};

class Bird : public Enemy {
public:
    Bird(Vector2 pos);
    ~Bird();
    void Update(float deltaTime, const MapManager& map) override;
    void Draw(bool showDebug) override;
    Rectangle GetHitbox() const override;

private:
    Texture2D texture;
    bool facingRight;
    float animTimer;
    int frame;
    float speed = 60.0f;
};

class Scorpion : public Enemy {
public:
    Scorpion(Vector2 pos);
    ~Scorpion();
    void Update(float deltaTime, const MapManager& map) override;
    void Draw(bool showDebug) override;
    Rectangle GetHitbox() const override;

private:
    Texture2D texture;
    bool facingLeft;
    float animTimer;
    int frame;
    float speed = 40.0f;
};

#endif
