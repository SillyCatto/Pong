#include <iostream>
#include <string>
#include "raylib.h"

struct Ball
{
    Vector2 position;
    Vector2 speed;
    float radius;

    void Draw()
    {
        DrawCircle(position.x, position.y, radius, WHITE);
    }
};

struct Bar
{
    Vector2 position;

    Rectangle GetRect()
    {
        return Rectangle {position.x - 10 / 2, position.y - 100 / 2, 10, 100};
    }

    void Draw(Color color)
    {
        DrawRectangleRec(GetRect(), color);
    }
};

Color GetColor(const char* text)
{

    std::string str(text);
    if(str == "RED Wins!") {
        return RED;
    }
    if(str == "BLUE Wins!") {
        return BLUE;
    }
    return BLACK;
}

int main()
{
    const int screenW = 800;
    const int screenH = 600;
    const int barSpeed = 300;
    const char* winner = nullptr;
    int redS = 0;
    int blueS = 0;
    bool scoreUpdate = true;

    //random bool for ball direction
    std::srand(time(0));
    bool randBool{};
    randBool = (std::rand() % 2);

    //init window
    InitWindow(screenW, screenH, "Pong");
    SetWindowState(FLAG_VSYNC_HINT);

    //init and load sound
    InitAudioDevice();
    Sound hit = LoadSound("./assets/sound/hit.mp3");
    SetMasterVolume(0.9f);

    Ball ball;
    ball.position.x = GetScreenWidth() / 2;
    ball.position.y = GetScreenHeight() / 2;

    //randomize ball direction at start (towards blue or towards red)
    if (randBool)
    {
        ball.speed.x = 230;
    }
    else
    {
        ball.speed.x = -230;
    }

    ball.speed.y = 300;
    ball.radius = 6.0f;

    Bar leftBar;
    leftBar.position.x = 40;
    leftBar.position.y = GetScreenHeight() / 2;

    Bar rightBar;
    rightBar.position.x = GetScreenWidth() - 40;
    rightBar.position.y = GetScreenHeight() / 2;

    while (!WindowShouldClose())
    {
        //update ball position
        ball.position.x += ball.speed.x * GetFrameTime();
        ball.position.y += ball.speed.y * GetFrameTime();

        if ((ball.position.y - ball.radius) < 0)
        {
            ball.position.y = 6.0f;
            ball.speed.y *= -1;
        }

        if ((ball.position.y + ball.radius) > screenH)
        {
            ball.position.y = 594.0f;
            ball.speed.y *= -1;
        }

        //Control left bar
        if(IsKeyDown(KEY_W))
        {
            leftBar.position.y -= barSpeed * GetFrameTime();
            if (leftBar.position.y < 50)
            {
                leftBar.position.y = 50;
            }
        }

        if(IsKeyDown(KEY_S))
        {
            leftBar.position.y += barSpeed * GetFrameTime();
            if (leftBar.position.y > 550)
            {
                leftBar.position.y = 550;
            }
        }

        //Control right bar
        if(IsKeyDown(KEY_UP))
        {
            rightBar.position.y -= barSpeed * GetFrameTime();
            if (rightBar.position.y < 50)
            {
                rightBar.position.y = 50;
            }
        }

        if(IsKeyDown(KEY_DOWN))
        {
            rightBar.position.y += barSpeed * GetFrameTime();
            if (rightBar.position.y > 550)
            {
                rightBar.position.y = 550;
            }
        }

        //Collision: ball with bars
        if(CheckCollisionCircleRec(Vector2{ball.position.x, ball.position.y}, ball.radius, leftBar.GetRect()))
        {
            PlaySound(hit);
            if(ball.speed.x < 0)
            {
                ball.speed.x *= -1;
            }
        }

        if(CheckCollisionCircleRec(Vector2{ball.position.x, ball.position.y}, ball.radius, rightBar.GetRect()))
        {
            PlaySound(hit);
            if(ball.speed.x > 0)
            {
                ball.speed.x *= -1;
            }
        }

        //Set Winner
        if(ball.position.x < 20)
        {
            winner = "BLUE Wins!";
            if ((ball.position.x < 20) && scoreUpdate)
            {
                blueS += 1;
                scoreUpdate = false;
            }
        }

        if(ball.position.x > 780)
        {
            winner = "RED Wins!";
            if ((ball.position.x > 780) && scoreUpdate)
            {
                redS += 1;
                scoreUpdate = false;
            }
        }

        //Restart
        if(winner && IsKeyPressed(KEY_SPACE))
        {
            ball.position.x = GetScreenWidth() / 2;
            ball.position.y = GetScreenHeight() / 2;
            leftBar.position.y = GetScreenHeight() / 2;
            rightBar.position.y = GetScreenHeight() / 2;
            winner = nullptr;
            scoreUpdate = true;
            randBool = (std::rand() % 2);
        }

        BeginDrawing();

            ClearBackground(BLACK);

            //draw ball
            ball.Draw();
            //Draw the bars
            leftBar.Draw(RED);
            rightBar.Draw(BLUE);

            //Declare winner and show score
            if(winner)
            {
                std::string redScore = std::to_string(redS);
                std::string blueScore = std::to_string(blueS);

                DrawText(winner, 305, GetScreenHeight() / 2 - 20, 30, GetColor(winner));

                DrawText(redScore.c_str(), 320, 200, 50, RED);
                DrawText(" - ", 350, 200, 50, WHITE);
                DrawText(blueScore.c_str(), 420, 200, 50, BLUE);

                DrawText("Press SPACE ro restart", 250, 400, 20, WHITE);
            }

        EndDrawing();
    }

    UnloadSound(hit);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
