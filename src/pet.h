#pragma once
#include <raylib.h>

enum PetState
{
    IDLE,
    WALKING,
    SLEEPING,
    PLAYING
};

struct Pet
{
    Vector2 position;
    Vector2 velocity;
    PetState state;
    float stateTimer;
    Color color;
    float hunger;
    float happiness;
    float energy;
};

void UpdatePet(Pet &pet, float deltaTime);
void DrawPet(const Pet &pet);
