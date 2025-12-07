#include "pet.h"
#include <cstdlib>
#include <raymath.h>
#include "items/ball.h"
#include "animation.h"

void UpdatePet(Pet &pet, float deltaTime, Ball &ball, Animation &idle, Animation &walk, Animation &sleeping, Animation &pouncing)
{
    pet.stateTimer += deltaTime;

    if (pet.state == IDLE)
    {
        UpdateAnimation(idle);
    }
    else if (pet.state == SLEEPING)
    {
        UpdateAnimation(sleeping);
    }
    else if (pet.state == POUNCING)
    {
        UpdateAnimation(pouncing);
    }
    else
    {
        UpdateAnimation(walk);
    }

    // Needs decay
    pet.hunger -= 0.2 * deltaTime;
    if (pet.state == SLEEPING)
    {
        pet.energy += 0.5 * deltaTime;
    }
    else
    {
        pet.energy -= 0.1 * deltaTime;
    }

    if (pet.hunger < 30)
        pet.happiness -= 4.0f * deltaTime; // sadness from hunger

    if (pet.hunger < 10 && pet.state != PLAYING && pet.state != POUNCING && pet.state != PUSHING)
        pet.state = SLEEPING;

    if (pet.hunger == 0)
        pet.happiness -= 10 * deltaTime;

    Vector2 mousePosition = GetMousePosition();
    float distance = Vector2Distance(mousePosition, pet.position);
    if (pet.state == SLEEPING && (pet.energy < 50) && (distance < 80) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        pet.happiness += 5;
        pet.state = PLAYING;
        pet.jumpOffset = 15.0f;
        walk.currentFrame = 0;
        walk.timer = 0;
    };
    if (pet.state == SLEEPING && pet.energy < 50)
    {
        return;
    }

    if (pet.jumpOffset > 0)
    {
        pet.jumpOffset -= 80.0f * deltaTime;
        if (pet.jumpOffset < 0)
            pet.jumpOffset = 0;
    }

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

        PetState oldState = pet.state;
        pet.state = (PetState)(rand() % 4);
        pet.stateTimer = 0.0f;

        // Reset animations when state changes
        if (oldState != pet.state)
        {
            idle.currentFrame = 0;
            idle.timer = 0;
            walk.currentFrame = 0;
            walk.timer = 0;
            sleeping.currentFrame = 0;
            sleeping.timer = 0;
        }

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
    Vector2 petCenter2 = {pet.position.x, pet.position.y};
    float d = Vector2Distance(mouse, petCenter2);
    if (d < 80 && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        pet.happiness += 5;
        pet.state = PLAYING;
        walk.currentFrame = 0;
        walk.timer = 0;
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
                {
                    pet.position.x += 50 * deltaTime;
                    pet.velocity.x = 50;
                    pet.facingLeft = false;
                }
                else if (ball.position.x < pet.position.x)
                {
                    pet.position.x -= 50 * deltaTime;
                    pet.velocity.x = -50;
                    pet.facingLeft = true;
                }
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

        if (pouncing.currentFrame >= pouncing.frameCount - 1)
        {
            pet.state = PUSHING;
            pet.stateTimer = 0.0f;
            pet.jumpOffset = 0;
            pouncing.currentFrame = 0;
            pouncing.timer = 0;
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

void DrawPet(const Pet &pet, const Animation &idle, const Animation &walk, const Animation &sleeping, const Animation &pouncing)
{
    Vector2 position = {pet.position.x - 16, pet.position.y - pet.jumpOffset - 16};
    const Animation *anim;
    if (pet.state == IDLE)
    {
        anim = &idle;
    }
    else if (pet.state == WALKING || pet.state == PLAYING)
    {
        anim = &walk;
    }
    else if (pet.state == SLEEPING)
    {
        anim = &sleeping;
    }
    else if (pet.state == POUNCING)
    {
        anim = &pouncing;
    }
    else if (pet.state == PUSHING)
    {
        anim = &idle;
    }
    else
    {
        anim = &idle;
    }
    int frameIndex = anim->startFrame + anim->currentFrame;
    int frameX = (frameIndex % 8) * 32;
    int frameY = (frameIndex / 8) * 32;

    Rectangle src = {(float)frameX, (float)frameY, 32, 32};
    Rectangle dest = {pet.position.x, pet.position.y - pet.jumpOffset, 128, 128};

    if (pet.facingLeft)
    {
        src.width = -32;
    }

    DrawTexturePro(pet.sprite, src, dest, {64, 64}, 0, WHITE);
}