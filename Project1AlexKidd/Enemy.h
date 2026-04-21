#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include <vector>

class MapManager;

enum class EnemyType { BIRD, SCORPION, LAVA, PLANT };

class Enemy {
public:
    Enemy(Vector2 pos) : position(pos), velocity({0,0}), isGrounded(false), dead(false) {}
    virtual ~Enemy() {}
    virtual void Update(float deltaTime, const MapManager& map) = 0;
    virtual void Draw(bool showDebug) = 0;
    virtual Rectangle GetHitbox() const = 0;
    virtual EnemyType GetType() const = 0;
    bool IsDead() const { return dead; }
    void Die() { dead = true; }

protected:
    Vector2 position;
    Vector2 velocity;
    bool isGrounded;
    bool dead;
};

class Bird : public Enemy {
public:
    Bird(Vector2 pos);
    ~Bird();
    void Update(float deltaTime, const MapManager& map) override;
    void Draw(bool showDebug) override;
    Rectangle GetHitbox() const override;
    EnemyType GetType() const override { return EnemyType::BIRD; }

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
    EnemyType GetType() const override { return EnemyType::SCORPION; }

private:
    Texture2D texture;
    bool facingLeft;
    float animTimer;
    int frame;
    float speed = 40.0f;
};

class Lava : public Enemy {
public:
    Lava(Vector2 pos);
    ~Lava();
    void Update(float deltaTime, const MapManager& map) override;
    void Draw(bool showDebug) override;
    Rectangle GetHitbox() const override;
    EnemyType GetType() const override { return EnemyType::LAVA; }

private:
    Texture2D texture;
    float animTimer;
    int frame;
};

class Plant : public Enemy {
public:
    Plant(Vector2 pos);
    ~Plant();
    void Update(float deltaTime, const MapManager& map) override;
    void Draw(bool showDebug) override;
    Rectangle GetHitbox() const override;
    EnemyType GetType() const override { return EnemyType::PLANT; }

private:
    Texture2D texture;
    Vector2 startPos;
    float moveTimer;
};

#endif
