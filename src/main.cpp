#include "Game.h"
#include "UDPReceiver.h"

constexpr float CURSOR_LERP = 0.5f;

int main() {
    Game game;
    game.Init();

    UDPReceiver udp(5005);

    
    Vector2 smoothedCursor = { GetScreenWidth()  / 2.0f,
                               GetScreenHeight() / 2.0f };

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        udp.Poll();

        InputState in{};
        in.udpAlive          = udp.IsAlive();
        in.rightClickPressed = IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);

        if (in.udpAlive) {
            in.handPresent      = udp.HandPresent();
           
            Vector2 target      = { udp.NormalizedX() * (float)GetScreenWidth(),
                                    udp.NormalizedY() * (float)GetScreenHeight() };
            if (in.handPresent) {
               
                smoothedCursor.x += (target.x - smoothedCursor.x) * CURSOR_LERP;
                smoothedCursor.y += (target.y - smoothedCursor.y) * CURSOR_LERP;
            } else {
                smoothedCursor = target;
            }
            in.cursor           = smoothedCursor;
            in.clickDown        = udp.ClickDown();
            in.clickPressed     = udp.ClickPressed();
            in.clickReleased    = udp.ClickReleased();
            in.startWavePressed = udp.StartWavePressed();
            in.upgradePressed   = udp.UpgradePressed();
            in.sellPressed      = udp.SellPressed();
            in.ultPressed       = udp.UltPressed();
        } else {
            
            in.handPresent      = true;            // kursor mouse selalu dianggap ada
            smoothedCursor      = GetMousePosition();
            in.cursor           = smoothedCursor;
            in.clickDown        = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
            in.clickPressed     = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
            in.clickReleased    = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);

            in.startWavePressed = false;
            in.upgradePressed   = false;
            in.sellPressed      = false;
            in.ultPressed       = false;
        }

        game.Update(dt, in);

        BeginDrawing();
        game.Draw();
        EndDrawing();
    }

    game.Shutdown();
    CloseWindow();
    return 0;
}
