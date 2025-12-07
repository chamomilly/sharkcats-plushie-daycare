#pragma once
#include <raylib.h>
#include <string>

struct Ball;      // Forward declaration
struct Animation; // Forward declaration

enum PetState
{
    IDLE,
    WALKING,
    SLEEPING,
    PLAYING,
    POUNCING,
    PUSHING
};

struct Pet
{
    char name[32];
    Vector2 position;
    Vector2 velocity;
    PetState state;
    float stateTimer;
    Color color;
    float hunger;
    float happiness;
    float energy;
    float jumpOffset;
    bool hasPounced;
    Vector2 pounceTarget;
    Texture2D sprite;
    bool facingLeft;
};

void UpdatePet(Pet &pet, float deltaTime, Ball &ball, Animation &idle, Animation &walk, Animation &sleeping, Animation &pouncing);
void DrawPet(const Pet &pet, const Animation &idle, const Animation &walk, const Animation &sleeping, const Animation &pouncing);
