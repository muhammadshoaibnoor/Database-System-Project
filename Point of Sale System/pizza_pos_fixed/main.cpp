#include "raylib.h"
#include "src/core/globals.h"
#include "src/core/session.h"
#include "src/db/db.h"
#include "src/ui/ui_helpers.h"
#include "src/ui/screens/screen_login.h"
#include "src/ui/screens/screen_dashboard_admin.h"
#include "src/ui/screens/screen_dashboard_receptionist.h"
#include "src/ui/screens/screen_dashboard_kitchen.h"

ScreenID gScreen = SCREEN_LOGIN;

int main() {
    // Initialize window
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Pizza Point of Sale");
    SetTargetFPS(60);

    // Initialize database
    if (!DB::initDB()) {
        // Show error and exit
        float timer = 3.0f;
        while (!WindowShouldClose() && timer > 0.0f) {
            timer -= GetFrameTime();
            BeginDrawing();
            ClearBackground(COL_BG);
            DrawText("Database connection failed!", 200, 300, 30, COL_LOW_STOCK);
            DrawText("Please check MySQL is running.", 200, 340, 20, COL_DARK);
            EndDrawing();
        }
        CloseWindow();
        return 1;
    }

    // Lazy init flags
    bool adminInited = false;
    bool receptionistInited = false;
    bool kitchenInited = false;

    // Start at login screen
    InitLoginScreen();

    // Main loop
    while (!WindowShouldClose()) {
        // Track previous screen to detect transitions
        static ScreenID prevScreen = SCREEN_LOGIN;
        ScreenID currentScreen = gScreen;

        // Reset init flags when returning to login
        if (currentScreen == SCREEN_LOGIN && prevScreen != SCREEN_LOGIN) {
            adminInited = false;
            receptionistInited = false;
            kitchenInited = false;
            InitLoginScreen();
        }

        BeginDrawing();
        ClearBackground(COL_BG);

        switch (gScreen) {
            case SCREEN_LOGIN:
                DrawLoginScreen();
                break;

            case SCREEN_ADMIN:
                if (!adminInited) {
                    InitAdminDashboard();
                    adminInited = true;
                }
                DrawAdminDashboard();
                break;

            case SCREEN_RECEPTIONIST:
                if (!receptionistInited) {
                    InitReceptionistDashboard();
                    receptionistInited = true;
                }
                DrawReceptionistDashboard();
                break;

            case SCREEN_KITCHEN:
                if (!kitchenInited) {
                    InitKitchenDashboard();
                    kitchenInited = true;
                }
                DrawKitchenDashboard();
                break;
        }

        EndDrawing();
        prevScreen = currentScreen;
    }

    // Clean shutdown
    DB::closeDB();
    CloseWindow();
    return 0;
}