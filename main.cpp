#include <iostream>
#include <string>
#include "raylib.h"

struct Ball
{
    float x, y;
    float speedX, speedY;
    float radius;

    void Draw()
    {
        DrawCircle(x, y, radius, WHITE);
    }
};

struct Bar
{
    float x, y;

    Rectangle GetRect()
    {
        return Rectangle {x - 10 / 2, y - 100 / 2, 10, 100};
    }

    void Draw(Color color) {
        DrawRectangleRec(GetRect(), color);
    }
};

Color GetColor(const char* text) {

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
    ball.x = GetScreenWidth() / 2;
    ball.y = GetScreenHeight() / 2;
    //ball.speedX = 200;

    //randomize ball direction at start (towards blue or towards red)
    if (randBool)
    {
        ball.speedX = 200;
    }
    else
    {
        ball.speedX = -200;
    }

    ball.speedY = 300;
    ball.radius = 7.0f;

    Bar leftBar;
    leftBar.x = 40;
    leftBar.y = GetScreenHeight() / 2;

    Bar rightBar;
    rightBar.x = GetScreenWidth() - 40;
    rightBar.y = GetScreenHeight() / 2;

    while (!WindowShouldClose())
    {
        //update ball position
        ball.x += ball.speedX * GetFrameTime();
        ball.y += ball.speedY * GetFrameTime();

        if ((ball.y + ball.radius) < 0)
		{
			ball.y = 0;
			ball.speedY*= -1;
		}

		if ((ball.y - ball.radius) > screenH)
		{
			ball.y = screenH;
			ball.speedY *= -1;
		}

        //Control left bar
        if(IsKeyDown(KEY_W))
        {
            leftBar.y -= barSpeed * GetFrameTime();
        }

        if(IsKeyDown(KEY_S))
        {
            leftBar.y += barSpeed * GetFrameTime();
        }

        //Control right bar
        if(IsKeyDown(KEY_UP))
        {
            rightBar.y -= barSpeed * GetFrameTime();
        }

        if(IsKeyDown(KEY_DOWN))
        {
            rightBar.y += barSpeed * GetFrameTime();
        }

        //Collision detection
        if(CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, leftBar.GetRect()))
        {
            PlaySound(hit);
            if(ball.speedX < 0)
            {
                ball.speedX *= -1;
            }
        }

        if(CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, rightBar.GetRect()))
        {
            PlaySound(hit);
            if(ball.speedX > 0)
            {
                ball.speedX *= -1;
            }
        }

        //Set Winner
        if(ball.x < 20)
        {
            winner = "BLUE Wins!";
            if ((ball.x < 20) && scoreUpdate)
            {
                blueS += 1;
                scoreUpdate = false;
            }
        }

        if(ball.x > 780)
        {
            winner = "RED Wins!";
            if ((ball.x > 780) && scoreUpdate)
            {
                redS += 1;
                scoreUpdate = false;
            }
        }

        //Restart
        if(winner && IsKeyPressed(KEY_SPACE))
        {
            ball.x = GetScreenWidth() / 2;
            ball.y = GetScreenHeight() / 2;
            leftBar.y = GetScreenHeight() / 2;
            rightBar.y = GetScreenHeight() / 2;
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