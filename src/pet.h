#pragma once
#include <raylib.h>
#include <string>

struct Ball; // Forward declaration

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
};

void UpdatePet(Pet &pet, float deltaTime, Ball &ball);
void DrawPet(const Pet &pet);
