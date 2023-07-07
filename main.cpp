#include <iostream>
#include <string.h>

#include "raylib.h"

class Ball
{
public:
    Vector2 position;
    Vector2 velocity;
    const float radius = 6.50f;

    Vector2 RandomizeDirection()
    {
        velocity.x = 250;
        velocity.y = GetRandomValue(50, 400);

        // get a random value 0,1,2 or 3 for 4 quadrants
        int quadrant = (GetRandomValue(4, 7) % 4);

        // set ball direction according to the quadrants
        if (quadrant == 0)
        {
            velocity.y *= -1.0f;
        }
        else if (quadrant == 1)
        {
            velocity.x *= -1.0f;
            velocity.y *= -1.0f;
        }
        else if (quadrant == 2)
        {
            velocity.x *= -1.0f;
        }

        return velocity;
    }

    void Init()
    {
        position.x = GetScreenWidth() / 2;
        position.y = GetScreenHeight() / 2;

        // randomize ball direction at start (towards blue or towards red)
        velocity = RandomizeDirection();
    }

    void Draw() { DrawCircle(position.x, position.y, radius, WHITE); }

    void Update()
    {
        position.x += velocity.x * GetFrameTime();
        position.y += velocity.y * GetFrameTime();
    }

    void Bounce() { velocity.y *= -1.0f; }
};

class Bar
{
private:
    const int bar_speed = 350;

public:
    Vector2 position;

    Rectangle GetSize() { return Rectangle{position.x - 13 / 2, position.y - 100 / 2, 13, 100}; }

    void Draw(Color color) { DrawRectangleRounded(GetSize(), 0.8, 0, color); }

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
    TIMER,
    GAMEPLAY,
    PAUSED
};

Color GetColor(const char *winner_call)
{
    if (TextIsEqual(winner_call, "RED Wins!"))
    {
        return RED;
    }
    if (TextIsEqual(winner_call, "BLUE Wins!"))
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
    int frame_count = 0;
    int seconds_to_start = 1;

    SetRandomSeed(time(0));

    // init window
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(screen_width, screen_height, "Pong");
    SetTargetFPS(60);

    // init game state
    GameScreen current_screen = TITLE;

    // init sound
    InitAudioDevice();
    Sound hit = LoadSound("./assets/sound/hit.mp3");
    SetMasterVolume(1.0f);

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

        switch (current_screen)
        {
        // ----------title state, press ENTER to start----------
        case TITLE:
        {
            if (IsKeyPressed(KEY_ENTER))
            {
                current_screen = TIMER;
            }
        }
        break;

        case TIMER:
        {
            frame_count++;

            // count to 3 seconds before starting gameplay
            if (((frame_count / 60) % 2) == 1)
            {
                seconds_to_start++;
                frame_count = 0;
                if (seconds_to_start == 4)
                {
                    seconds_to_start = 1;
                    frame_count = 0;
                    current_screen = GAMEPLAY;
                }
            }
        }
        break;

        // ----------gameplay logic----------
        case GAMEPLAY:
        {
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
                if (ball.velocity.x < 0)
                {
                    ball.velocity.x *= -1.08f;
                    ball.velocity.y = (ball.position.y - leftBar.position.y) / 50 * ball.velocity.x;
                }
            }

            if (CheckCollisionCircleRec(Vector2{ball.position.x, ball.position.y}, ball.radius, rightBar.GetSize()))
            {
                PlaySound(hit);
                if (ball.velocity.x > 0)
                {
                    ball.velocity.x *= -1.08f;
                    ball.velocity.y = (ball.position.y - rightBar.position.y) / 50 * -ball.velocity.x;
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
                // reset ball position and velocity
                ball.Init();

                // reset bar position
                leftBar.Reset();
                rightBar.Reset();

                // reset winner, score update and screen state
                winner = nullptr;
                score_update = true;
                current_screen = TIMER;
            }
        }
        break;

        // ----------pause state, press SPACE to continue----------
        case PAUSED:
        {
            frame_count++;
            if (IsKeyPressed(KEY_SPACE))
            {
                current_screen = GAMEPLAY;
                frame_count = 0;
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
            DrawRectangle((screen_width / 2) - 260, (screen_height / 2) - 210, 520, 420, GRAY);
            DrawRectangle((screen_width / 2) - 250, (screen_height / 2) - 200, 500, 400, LIGHTGRAY);
            DrawText("Pong!", 322, 150, 60, VIOLET);
            DrawText("by Silly Catto", 297, 250, 30, MAROON);
            DrawText("Press ENTER to start", 283, 430, 20, DARKGRAY);
        }

        // ----------gameplay screen drawing----------
        if ((current_screen == GAMEPLAY) || (current_screen == PAUSED) || (current_screen == TIMER))
        {
            // show red score
            DrawText(TextFormat("%i", red_score),
                     (((screen_width / 2) - MeasureText(TextFormat("%i", red_score), 40)) - 25),
                     15,
                     40,
                     GRAY);
            // show blue score
            DrawText(TextFormat("%i", blue_score),
                     ((screen_width / 2) + 25),
                     15,
                     40,
                     GRAY);
            // draw a line at the middle
            DrawLine((screen_width / 2), 0, (screen_width / 2), screen_height, GRAY);
            // Draw the bars
            leftBar.Draw(RED);
            rightBar.Draw(BLUE);
            // draw ball
            ball.Draw();

            // ----------timer screen drawing----------
            if ((current_screen == TIMER))
            {
                DrawText(TextFormat("%i", seconds_to_start),
                         (screen_width / 2) - (MeasureText(TextFormat("%i", seconds_to_start), 70) / 2),
                         200,
                         70,
                         YELLOW);
            }

            // ----------paused screen drawing----------
            if ((current_screen == PAUSED) && (((frame_count / 30) % 2) == 1))
            {
                DrawText("PAUSED", 317, 200, 40, GREEN);
            }

            // ----------declare winner and show score----------
            if (winner)
            {
                DrawRectangle((screen_width / 2) - 210, (screen_height / 2) - 160, 420, 320, GRAY);
                DrawRectangle((screen_width / 2) - 200, (screen_height / 2) - 150, 400, 300, LIGHTGRAY);
                // declare winner name (red/blue)
                DrawText(winner,
                         ((screen_width / 2) - (MeasureText(winner, 40) / 2)),
                         GetScreenHeight() / 2 - 100,
                         40,
                         GetColor(winner));

                // show prompt to continue
                DrawText("Press SPACE to continue", 272, 350, 20, DARKGRAY);
            }
        }
        EndDrawing();
    }

    // ----------unload and close----------
    UnloadSound(hit);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
