#include <iostream>
#include <string>

#include "raylib.h"

class Ball
{
private:
    // random bool for ball direction (mod by 2 gives 0 or 1 which acts as true or false for ball direction)
    bool ball_direction{};

public:
    Vector2 position;
    Vector2 speed;
    const float radius = 6.0f;

    void Init()
    {
        ball_direction = (std::rand() % 2);
        position.x = GetScreenWidth() / 2;
        position.y = GetScreenHeight() / 2;
        speed.y = 300;

        // randomize ball direction at start (towards blue or towards red)
        if (ball_direction)
        {
            speed.x = 200;
        }
        else
        {
            speed.x = -200;
        }
    }

    void Draw() { DrawCircle(position.x, position.y, radius, WHITE); }

    void Update()
    {
        position.x += speed.x * GetFrameTime();
        position.y += speed.y * GetFrameTime();
    }

    void Bounce() { speed.y *= -1; }
};

class Bar
{
private:
    const int bar_speed = 330;

public:
    Vector2 position;

    Rectangle GetSize() { return Rectangle{position.x - 10 / 2, position.y - 100 / 2, 10, 100}; }

    void Draw(Color color) { DrawRectangleRec(GetSize(), color); }

    void Init(int pos_x, int pos_y)
    {
        this->position.x = pos_x;
        this->position.y = pos_y;
    }

    void MoveUp()
    {
        position.y -= bar_speed * GetFrameTime();
        if (position.y < 50)
        {
            position.y = 50;
        }
    }

    void MoveDown()
    {
        position.y += bar_speed * GetFrameTime();
        if (position.y > 550)
        {
            position.y = 550;
        }
    }

    void Reset() { position.y = GetScreenHeight() / 2; }
};

enum GameScreen
{
    TITLE = 0,
    GAMEPLAY,
    PAUSED
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

int main()
{
    const int screen_width = 800;
    const int screen_height = 600;
    const char *winner = nullptr;
    int red_score = 0;
    int blue_score = 0;
    bool score_update = true;
    std::srand(time(0));

    // init window
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(screen_width, screen_height, "Pong");
    SetTargetFPS(60);

    // init game state
    GameScreen current_screen = TITLE;

    // init sound
    InitAudioDevice();
    Sound hit = LoadSound("./assets/sound/hit.mp3");
    Music title = LoadMusicStream("./assets/sound/title.mp3");
    SetMasterVolume(0.9f);

    // init ball
    Ball ball;
    ball.Init();

    // init left player
    Bar leftBar;
    leftBar.Init(40, GetScreenHeight() / 2);

    // init right player
    Bar rightBar;
    rightBar.Init(GetScreenWidth() - 40, GetScreenHeight() / 2);

    // ----------game loop----------
    while (!WindowShouldClose())
    {
        // Update music buffer with new stream data
        UpdateMusicStream(title);

        switch (current_screen)
        {
        // ----------title state, press ENTER to start----------
        case TITLE:
        {
            // play title screen music
            PlayMusicStream(title);
            if (IsKeyPressed(KEY_ENTER))
            {
                current_screen = GAMEPLAY;
            }
        }
        break;

        // ----------gameplay logic----------
        case GAMEPLAY:
        {
            // stop title screen music
            StopMusicStream(title);

            // update ball position
            ball.Update();

            // ----------check collision with the top----------
            if ((ball.position.y - ball.radius) < 0)
            {
                ball.Bounce();
            }

            // ----------check collision with the bottom----------
            if ((ball.position.y + ball.radius) > screen_height)
            {
                ball.Bounce();
            }

            // ----------control left bar----------
            if (IsKeyDown(KEY_W))
            {
                leftBar.MoveUp();
            }

            if (IsKeyDown(KEY_S))
            {
                leftBar.MoveDown();
            }

            // ----------control right bar----------
            if (IsKeyDown(KEY_UP))
            {
                rightBar.MoveUp();
            }

            if (IsKeyDown(KEY_DOWN))
            {
                rightBar.MoveDown();
            }

            // ----------collision: ball with bars----------
            if (CheckCollisionCircleRec(Vector2{ball.position.x, ball.position.y}, ball.radius, leftBar.GetSize()))
            {
                PlaySound(hit);
                if (ball.speed.x < 0)
                {
                    ball.speed.x *= -1.08f;
                    ball.speed.y = (ball.position.y - leftBar.position.y) / 50 * ball.speed.x;
                }
            }

            if (CheckCollisionCircleRec(Vector2{ball.position.x, ball.position.y}, ball.radius, rightBar.GetSize()))
            {
                PlaySound(hit);
                if (ball.speed.x > 0)
                {
                    ball.speed.x *= -1.08f;
                    ball.speed.y = (ball.position.y - rightBar.position.y) / 50 * -ball.speed.x;
                }
            }

            // ----------set winner and update score----------
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

            // ----------press SPACE to pause----------
            if (IsKeyPressed(KEY_SPACE) && (winner == nullptr))
            {
                current_screen = PAUSED;
            }

            // ----------reset----------
            if (winner && IsKeyPressed(KEY_SPACE))
            {
                // reset ball position and speed
                ball.Init();

                // reset bar position
                leftBar.Reset();
                rightBar.Reset();

                // reset bools and winner
                winner = nullptr;
                score_update = true;
            }
        }
        break;

        // ----------pause state, press SPACE to continue----------
        case PAUSED:
        {
            if (IsKeyPressed(KEY_SPACE))
            {
                current_screen = GAMEPLAY;
            }
        }
        break;
        default:
            break;
        }

        // ----------drawing begins----------
        BeginDrawing();

        ClearBackground(BLACK);

        //----------title screen drawing----------
        if (current_screen == TITLE)
        {
            DrawRectangle(0, 0, screen_width, screen_height, BLACK);
            DrawText("Pong!", 330, 80, 50, YELLOW);
            DrawText("by Silly Catto", 280, 180, 30, RAYWHITE);
            DrawText("Press ENTER to start", 270, 400, 20, BLUE);
        }

        // ----------gameplay screen drawing----------
        if ((current_screen == GAMEPLAY) || (current_screen == PAUSED))
        {
            // draw a line at the middle
            DrawLine((screen_width / 2), 0, (screen_width / 2), screen_height, GRAY);
            // draw ball
            ball.Draw();
            // Draw the bars
            leftBar.Draw(RED);
            rightBar.Draw(BLUE);

            DrawFPS(10, 10);

            // ----------paused screen drawing----------
            if ((current_screen == PAUSED) && (current_screen != GAMEPLAY))
            {
                DrawText("PAUSED", 317, 200, 40, GREEN);
            }

            // ----------declare winner and show score----------
            if (winner)
            {
                std::string red_score_string = std::to_string(red_score);
                std::string blue_score_string = std::to_string(blue_score);

                DrawLine((screen_width / 2), 0, (screen_width / 2), screen_height, BLACK);

                DrawText(winner, 305, GetScreenHeight() / 2 - 20, 30, GetColor(winner));
                DrawText(red_score_string.c_str(), 320, 200, 50, RED);
                DrawText(" - ", 350, 200, 50, WHITE);
                DrawText(blue_score_string.c_str(), 420, 200, 50, BLUE);

                DrawText("Press SPACE to restart", 250, 400, 20, WHITE);
            }
        }

        EndDrawing();
    }

    // ----------unload and close----------
    UnloadSound(hit);
    UnloadMusicStream(title);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
