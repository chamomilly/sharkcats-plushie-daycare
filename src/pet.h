#pragma once
#include <raylib.h>
#include <string>
#include "animation.h"

struct Ball;      // Forward declaration

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
    Animation idle;
    Animation walk;
    Animation sleep;
    Animation pouncing;
};

void UpdatePet(Pet &pet, float deltaTime, Ball &Ball);
void DrawPet(const Pet &pet);
