#include "pet.h"
#include <cstdlib>
#include <raymath.h>
#include "items/ball.h"

void UpdatePet(Pet &pet, float deltaTime, Ball &ball)
{
    pet.stateTimer += deltaTime;

    // Needs decay
    pet.hunger -= 0.2 * deltaTime;
    pet.energy -= 0.1 * deltaTime;

    if (pet.hunger < 30)
        pet.happiness -= 4.0f * deltaTime; // sadness from hunger

    if (pet.hunger < 10 && pet.state != PLAYING && pet.state != POUNCING && pet.state != PUSHING)
        pet.state = SLEEPING;

    if (pet.hunger == 0)
        pet.happiness -= 10 * deltaTime;

    pet.hunger = Clamp(pet.hunger, 0.0f, 100.0f);
    pet.happiness = Clamp(pet.happiness, 0.0f, 100.0f);
    pet.energy = Clamp(pet.energy, 0.0f, 100.0f);

    // Randomise the Pet State
    if (pet.stateTimer > (5.0f + (rand() % 3)))
    {

        // if energy gets too low, make it more likely for
        // the pet to fall asleep (but not while playing)
        if (pet.energy < 20 && rand() % 5 == 0 && pet.state != PLAYING && pet.state != POUNCING && pet.state != PUSHING)
        {
            pet.state = SLEEPING;
            pet.velocity = {0, 0};
            return;
        }

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
        if (pet.position.x > 310)
            pet.position.x = 310;
    }

    Vector2 mouse = GetMousePosition();
    float d = Vector2Distance(mouse, pet.position);
    if (d < 25 && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        pet.happiness += 5;
        pet.state = PLAYING;
    };

    if (pet.state == SLEEPING)
    {
        pet.energy += 10 * deltaTime;
        if (pet.energy > 100)
        {
            pet.energy = 100;
            pet.state = IDLE;
        }
    }

    // make pet chase ball
    if (pet.state == PLAYING)
    {
        if (ball.active && !ball.carried)
        {
            Vector2 toBall = Vector2Subtract(ball.position, pet.position);
            float distance = Vector2Length(toBall);
            float dt = deltaTime;

            if (distance < 20)
            {
                pet.state = POUNCING;
                pet.stateTimer = 0.0f;
                pet.velocity = {0, 0};
                pet.hasPounced = true;
                pet.pounceTarget = ball.position;
                return;
            }

            // chase after ball
            if (distance > 20)
            {
                if (ball.position.x > pet.position.x)
                    pet.position.x += 50 * deltaTime;
                else if (ball.position.x < pet.position.x)
                    pet.position.x -= 50 * deltaTime;
            }
            else
            {
                // reached ball
                pet.happiness += 10;
                pet.happiness = Clamp(pet.happiness, 0, 100);
                ball.active = false;
                ball.position = {-100, -100};
                pet.state = IDLE;
            }
        };
    }

    if (pet.state == POUNCING)
    {
        pet.stateTimer += deltaTime;

        float duration = 0.35f;
        float time = pet.stateTimer / duration;
        if (time > 1.0f)
            time = 1.0f;

        Vector2 start = pet.position;
        Vector2 target = pet.pounceTarget;

        pet.position.x = start.x + (target.x - start.x) * time;

        float jumpHeight = 10.0f * 4 * time * (1 - time); // jump arc parabola
        pet.jumpOffset = jumpHeight;

        if (pet.stateTimer > duration)
        {
            pet.state = PUSHING;
            pet.stateTimer = 0.0f;
            pet.jumpOffset = 0;
        }
    }

    if (pet.state == PUSHING)
    {
        pet.stateTimer += deltaTime;

        // Stop pushing after 5 seconds
        if (pet.stateTimer > 5.0f)
        {
            pet.state = IDLE;
            pet.velocity = {0, 0};
            ball.active = false;
            pet.hasPounced = false;
            return;
        }

        // Jump effect
        pet.jumpOffset = 0;

        // Nudge the ball if close
        Vector2 toBall = Vector2Subtract(ball.position, pet.position);
        float distance = Vector2Length(toBall);

        if (distance < 35 && ball.active) // increased range for nudges
        {
            Vector2 pushDirection = Vector2Normalize(toBall);

            ball.velocity.x += pushDirection.x * 200 * deltaTime; // increased force
            ball.velocity.y += pushDirection.y * 100 * deltaTime;
        }
    }
}

void DrawPet(const Pet &pet)
{
    float y = pet.position.y - pet.jumpOffset;

    // Draw pet based on state
    switch (pet.state)
    {
    case IDLE:
        DrawCircle(pet.position.x, y, 20, pet.color);
        DrawText("...", pet.position.x - 10, y - 40, 16, GRAY);
        break;
    case WALKING:
        DrawCircle(pet.position.x, y, 20, pet.color);
        DrawText("!", pet.position.x - 5, y - 35, 20, BLACK);
        break;
    case SLEEPING:
        DrawEllipse(pet.position.x, y, 25, 15, pet.color);
        DrawText("zzz", pet.position.x - 15, y - 30, 12, DARKGRAY);
        break;
    case PLAYING:
        DrawCircle(pet.position.x, y, 20, pet.color);
        DrawText("♪", pet.position.x - 8, y - 35, 20, GREEN);
        break;
    case POUNCING:
        DrawCircle(pet.position.x, y, 20, pet.color);
        DrawText("!", pet.position.x - 5, y - 35, 20, BLACK);
    case PUSHING:
        DrawCircle(pet.position.x, y, 20, pet.color);
        DrawText("!!", pet.position.x - 5, y - 35, 20, BLACK);
    }

    // Draw eyes
    if (pet.state != SLEEPING)
    {
        DrawCircle(pet.position.x - 8, y - 5, 3, BLACK);
        DrawCircle(pet.position.x + 8, y - 5, 3, BLACK);
    }
}