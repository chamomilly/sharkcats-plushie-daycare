#include "pet.h"
#include <cstdlib>

void UpdatePet(Pet &pet, float deltaTime)
{
    pet.stateTimer += deltaTime;

    if (pet.stateTimer > (5.0f + (rand() % 3)))
    {
        pet.state = (PetState)(rand() % 4);
        pet.stateTimer = 0.0f;

        if (pet.state == WALKING)
        {
            pet.velocity.x = (rand() % 3 - 1) * 50;
            pet.velocity.y = 0;
        }
        else
        {
            pet.velocity = {0, 0};
        }
    }

    // Update position
    if (pet.state == WALKING)
    {
        pet.position.x += pet.velocity.x * deltaTime;

        // Keep on screen
        if (pet.position.x < 20)
            pet.position.x = 20;
        if (pet.position.x > 350)
            pet.position.x = 350;
    }
}

void DrawPet(const Pet &pet)
{
    // Draw pet based on state
    switch (pet.state)
    {
    case IDLE:
        DrawCircle(pet.position.x, pet.position.y, 20, pet.color);
        DrawText("zzz", pet.position.x - 10, pet.position.y - 40, 16, GRAY);
        break;
    case WALKING:
        DrawCircle(pet.position.x, pet.position.y, 20, pet.color);
        DrawText("!", pet.position.x - 5, pet.position.y - 35, 20, BLACK);
        break;
    case SLEEPING:
        DrawEllipse(pet.position.x, pet.position.y, 25, 15, pet.color);
        DrawText("ZZZ", pet.position.x - 15, pet.position.y - 30, 12, DARKGRAY);
        break;
    case PLAYING:
        DrawCircle(pet.position.x, pet.position.y, 20, pet.color);
        DrawText("♪", pet.position.x - 8, pet.position.y - 35, 20, GREEN);
        break;
    }

    // Draw eyes
    if (pet.state != SLEEPING)
    {
        DrawCircle(pet.position.x - 8, pet.position.y - 5, 3, BLACK);
        DrawCircle(pet.position.x + 8, pet.position.y - 5, 3, BLACK);
    }
}