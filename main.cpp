#include <iostream>
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

    InitWindow(screenW, screenH, "Pong");
    SetWindowState(FLAG_VSYNC_HINT);

    Ball ball;
    ball.x = GetScreenWidth() / 2.0f;
    ball.y = GetScreenHeight() / 2.0f;
    ball.speedX = 190;
    ball.speedY = 290;
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
            if(ball.speedX < 0)
            {
                ball.speedX *= -1;
            }
        }

        if(CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, rightBar.GetRect()))
        {
            if(ball.speedX > 0)
            {
                ball.speedX *= -1;
            }
        }

        //Set Winner
        if(ball.x < 20)
        {
            winner = "BLUE Wins!";
        }

        if(ball.x > 780)
        {
            winner = "RED Wins!";
        }

        //Restart
        if(winner && IsKeyPressed(KEY_SPACE))
        {
            ball.x = GetScreenWidth() / 2;
            ball.y = GetScreenHeight() / 2;
            leftBar.y = GetScreenHeight() / 2;
            rightBar.y = GetScreenHeight() / 2;
            winner = nullptr;
        }


        BeginDrawing();

            ClearBackground(BLACK);

            DrawFPS(5, 5);
            //draw ball
            ball.Draw();
            //Draw the bars
            leftBar.Draw(RED);
            rightBar.Draw(BLUE);

            //Declare winner
            if(winner)
            {
                int textW = MeasureText(winner, 40);
                DrawText(winner, GetScreenWidth() / 2 - textW / 2, GetScreenHeight() / 2 - 20, 40, GetColor(winner));
                DrawText("Press SPACE ro restart", 270, 400, 20, WHITE);
            }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}