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
        return Rectangle{position.x - 10 / 2, position.y - 100 / 2, 10, 100};
    }

    void Draw(Color color)
    {
        DrawRectangleRec(GetRect(), color);
    }
};

enum GameScreen
{
    TITLE = 0,
    GAMEPLAY
};

Color GetColor(const char *text)
{

    std::string str(text);
    if (str == "RED Wins!")
    {
        return RED;
    }
    if (str == "BLUE Wins!")
    {
        return BLUE;
    }
    return BLACK;
}

void DeclareWinner(const char *winner, int redS, int blueS)
{
    std::string red_score = std::to_string(redS);
    std::string blue_score = std::to_string(blueS);

    DrawText(winner, 305, GetScreenHeight() / 2 - 20, 30, GetColor(winner));

    DrawText(red_score.c_str(), 320, 200, 50, RED);
    DrawText(" - ", 350, 200, 50, WHITE);
    DrawText(blue_score.c_str(), 420, 200, 50, BLUE);

    DrawText("Press SPACE to restart", 250, 400, 20, WHITE);
}

int main()
{
    const int screen_width = 800;
    const int screen_height = 600;
    const int bar_speed = 330;
    const char *winner = nullptr;
    int red_score = 0;
    int blue_score = 0;
    bool score_update = true;

    // random bool for ball direction
    std::srand(time(0));
    bool random_bool{};
    // division by 2 gives 0 or 1 which acts as true or false for ball direction
    random_bool = (std::rand() % 2);

    // init window
    InitWindow(screen_width, screen_height, "Pong");
    SetWindowState(FLAG_VSYNC_HINT);

    // init game state
    GameScreen current_screen = TITLE;

    // init and load sound
    InitAudioDevice();
    Sound hit = LoadSound("./assets/sound/hit.mp3");
    Music title = LoadMusicStream("./assets/sound/title.mp3");

    // set volume
    SetMasterVolume(0.9f);
    // play title screen music
    PlayMusicStream(title);

    // init ball
    Ball ball;
    ball.position.x = GetScreenWidth() / 2;
    ball.position.y = GetScreenHeight() / 2;
    ball.speed.y = 300;
    ball.radius = 6.0f;

    // randomize ball direction at start (towards blue or towards red)
    if (random_bool)
    {
        ball.speed.x = 200;
    }
    else
    {
        ball.speed.x = -200;
    }

    // init left player
    Bar leftBar;
    leftBar.position.x = 40;
    leftBar.position.y = GetScreenHeight() / 2;

    // init right player
    Bar rightBar;
    rightBar.position.x = GetScreenWidth() - 40;
    rightBar.position.y = GetScreenHeight() / 2;

    while (!WindowShouldClose())
    {
        // Update music buffer with new stream data
        UpdateMusicStream(title);

        // if ENTER key pressed, start gameplay
        if (current_screen == TITLE)
        {
            if (IsKeyPressed(KEY_ENTER))
            {
                current_screen = GAMEPLAY;
            }
        }

        if (current_screen == GAMEPLAY)
        {
            // stop title screen music
            StopMusicStream(title);

            // update ball position
            ball.position.x += ball.speed.x * GetFrameTime();
            ball.position.y += ball.speed.y * GetFrameTime();

            if ((ball.position.y - ball.radius) < 0)
            {
                ball.position.y = 6.0f;
                ball.speed.y *= -1;
            }

            if ((ball.position.y + ball.radius) > screen_height)
            {
                ball.position.y = 594.0f;
                ball.speed.y *= -1;
            }

            // Control left bar
            if (IsKeyDown(KEY_W))
            {
                leftBar.position.y -= bar_speed * GetFrameTime();
                if (leftBar.position.y < 50)
                {
                    leftBar.position.y = 50;
                }
            }

            if (IsKeyDown(KEY_S))
            {
                leftBar.position.y += bar_speed * GetFrameTime();
                if (leftBar.position.y > 550)
                {
                    leftBar.position.y = 550;
                }
            }

            // Control right bar
            if (IsKeyDown(KEY_UP))
            {
                rightBar.position.y -= bar_speed * GetFrameTime();
                if (rightBar.position.y < 50)
                {
                    rightBar.position.y = 50;
                }
            }

            if (IsKeyDown(KEY_DOWN))
            {
                rightBar.position.y += bar_speed * GetFrameTime();
                if (rightBar.position.y > 550)
                {
                    rightBar.position.y = 550;
                }
            }

            // Collision: ball with bars
            if (CheckCollisionCircleRec(Vector2{ball.position.x, ball.position.y}, ball.radius, leftBar.GetRect()))
            {
                PlaySound(hit);
                if (ball.speed.x < 0)
                {
                    ball.speed.x *= -1.08f;
                    ball.speed.y = (ball.position.y - leftBar.position.y) / 50 * ball.speed.x;
                }
            }

            if (CheckCollisionCircleRec(Vector2{ball.position.x, ball.position.y}, ball.radius, rightBar.GetRect()))
            {
                PlaySound(hit);
                if (ball.speed.x > 0)
                {
                    ball.speed.x *= -1.08f;
                    ball.speed.y = (ball.position.y - rightBar.position.y) / 50 * -ball.speed.x;
                }
            }

            // Set Winner
            if (ball.position.x < 20)
            {
                winner = "BLUE Wins!";
                if ((ball.position.x < 20) && score_update)
                {
                    blue_score += 1;
                    score_update = false;
                }
            }

            if (ball.position.x > 780)
            {
                winner = "RED Wins!";
                if ((ball.position.x > 780) && score_update)
                {
                    red_score += 1;
                    score_update = false;
                }
            }

            // Restart
            if (winner && IsKeyPressed(KEY_SPACE))
            {
                // reset ball position
                ball.position.x = GetScreenWidth() / 2;
                ball.position.y = GetScreenHeight() / 2;

                // reset bar position
                leftBar.position.y = GetScreenHeight() / 2;
                rightBar.position.y = GetScreenHeight() / 2;

                // reset bools and winner
                winner = nullptr;
                score_update = true;
                random_bool = (std::rand() % 2);

                // reset ball speed
                ball.speed.y = 300;
                if (random_bool)
                {
                    ball.speed.x = 200;
                }
                else
                {
                    ball.speed.x = -200;
                }
            }
        }

        // Draw to the screen
        BeginDrawing();

            ClearBackground(BLACK);

            switch (current_screen)
            {
            case TITLE:
            {

                DrawRectangle(0, 0, screen_width, screen_height, BLACK);
                DrawText("Pong!", 330, 80, 50, YELLOW);
                DrawText("by Silly Catto", 280, 180, 30, RAYWHITE);
                DrawText("Press ENTER to start", 270, 400, 20, BLUE);
            }
            break;
            case GAMEPLAY:
            {
                // draw ball
                ball.Draw();
                // Draw the bars
                leftBar.Draw(RED);
                rightBar.Draw(BLUE);

                // Declare winner and show score
                if (winner)
                {
                    DeclareWinner(winner, red_score, blue_score);
                }
            }
            break;
            default:
                break;
            }

        EndDrawing();
    }

    // unload sounds
    UnloadSound(hit);
    UnloadMusicStream(title);

    // close
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
