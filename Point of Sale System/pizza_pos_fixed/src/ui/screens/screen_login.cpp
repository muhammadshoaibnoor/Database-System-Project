#include "screen_login.h"
#include "raylib.h"
#include "../ui_helpers.h"
#include "../../core/session.h"
#include "../../db/db.h"
#include "../../core/models/models.h"

#include <cstring>
#include <string>

using namespace DB;

static char usernameInput[256] = {0};
static char passwordInput[256] = {0};
static bool usernameActive = false;
static bool passwordActive = false;
static bool showError = false;
static float errorTimer = 0.0f;
static std::string errorMsg = "";

void InitLoginScreen() {
    memset(usernameInput, 0, sizeof(usernameInput));
    memset(passwordInput, 0, sizeof(passwordInput));
    usernameActive = false;
    passwordActive = false;
    showError = false;
    errorTimer = 0.0f;
    errorMsg = "";
}

static UserRole roleStringToEnum(const std::string& role) {
    if (role == "Admin")           return ROLE_ADMIN;
    if (role == "Receptionist")    return ROLE_RECEPTIONIST;
    if (role == "KitchenStaff")    return ROLE_KITCHEN;
    if (role == "KitchenStaff")   return ROLE_KITCHEN;  // support both
    return ROLE_RECEPTIONIST;
}

static void attemptLogin() {
    std::string user(usernameInput);
    std::string pass(passwordInput);

    if (user.empty() || pass.empty()) {
        showError = true;
        errorTimer = 3.0f;
        errorMsg = "Please enter username and password";
        return;
    }

    Employee emp = loginUser(user, pass);

    if (emp.username.empty()) {
        showError = true;
        errorTimer = 3.0f;
        errorMsg = "Invalid credentials";
        memset(passwordInput, 0, sizeof(passwordInput));
        return;
    }

    UserRole role = roleStringToEnum(emp.role);
    setSession(emp.username, emp.firstName + " " + emp.lastName,
               role, emp.branchId, emp.branchName);

    switch (role) {
        case ROLE_ADMIN:         gScreen = SCREEN_ADMIN;         break;
        case ROLE_RECEPTIONIST:  gScreen = SCREEN_RECEPTIONIST;  break;
        case ROLE_KITCHEN:       gScreen = SCREEN_KITCHEN;       break;
        default:                 gScreen = SCREEN_LOGIN;          break;
    }
}

void DrawLoginScreen() {
    ClearBackground(Color{ 45, 25, 10, 255 });

    DrawCircle(100, 150, 80, Fade(Color{ 200, 80, 30, 255 }, 0.15f));
    DrawCircle(1180, 550, 120, Fade(Color{ 200, 80, 30, 255 }, 0.12f));
    DrawCircle(200, 600, 60, Fade(Color{ 220, 180, 50, 255 }, 0.10f));
    DrawCircle(1100, 120, 50, Fade(Color{ 220, 180, 50, 255 }, 0.10f));
    DrawCircle(600, 80, 40, Fade(Color{ 200, 80, 30, 255 }, 0.08f));
    DrawCircle(640, 650, 70, Fade(Color{ 220, 180, 50, 255 }, 0.08f));

    float cardW = 400, cardH = 380;
    float cardX = (WINDOW_WIDTH - cardW) / 2.0f;
    float cardY = (WINDOW_HEIGHT - cardH) / 2.0f;
    Rectangle cardRect = { cardX, cardY, cardW, cardH };

    DrawRectangleRounded({ cardX + 4, cardY + 4, cardW, cardH }, 0.08f, 16, Fade(BLACK, 0.2f));
    DrawRectangleRounded(cardRect, 0.08f, 16, Color{ 252, 248, 240, 255 });
    DrawRectangleRoundedLinesEx(cardRect, 0.08f, 16, 2, COL_ACCENT);

    const char* title = "Pizza Point of Sale";
    float titleW = (float)MeasureText(title, FONT_SIZE_TITLE);
    DrawText(title, (int)(cardX + (cardW - titleW) / 2.0f), (int)cardY + 20, FONT_SIZE_TITLE, COL_DARK);

    const char* subtitle = "Staff Login";
    float subW = (float)MeasureText(subtitle, FONT_SIZE_SMALL);
    DrawText(subtitle, (int)(cardX + (cardW - subW) / 2.0f), (int)cardY + 52, FONT_SIZE_SMALL, COL_BORDER);

    Rectangle userRect = { cardX + 40, cardY + 90, 320, 44 };
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mouse = GetMousePosition();
        usernameActive = CheckCollisionPointRec(mouse, userRect);
        if (usernameActive) passwordActive = false;
    }
    DrawInput(userRect, "Username", usernameInput, 255, usernameActive, false);

    Rectangle passRect = { cardX + 40, cardY + 170, 320, 44 };
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mouse = GetMousePosition();
        passwordActive = CheckCollisionPointRec(mouse, passRect);
        if (passwordActive) usernameActive = false;
    }
    DrawInput(passRect, "Password", passwordInput, 255, passwordActive, true);

    const char* forgetText = "Forget Password?";
    float forgetW = (float)MeasureText(forgetText, FONT_SIZE_SMALL);
    DrawText(forgetText, (int)(cardX + cardW - forgetW - 40), (int)cardY + 222, FONT_SIZE_SMALL, Fade(COL_BORDER, 0.7f));

    Rectangle loginBtn = { cardX + 40, cardY + 270, 320, 48 };
    if (DrawButton(loginBtn, "LOGIN", COL_ACCENT, COL_WHITE)) {
        attemptLogin();
    }

    if (showError) {
        errorTimer -= GetFrameTime();
        if (errorTimer <= 0.0f) {
            showError = false;
            errorMsg = "";
        } else {
            float errW = (float)MeasureText(errorMsg.c_str(), FONT_SIZE_SMALL);
            Color errColor = (errorTimer < 1.0f) ? Fade(COL_LOW_STOCK, errorTimer) : COL_LOW_STOCK;
            DrawText(errorMsg.c_str(), (int)(cardX + (cardW - errW) / 2.0f), (int)cardY + 335, FONT_SIZE_SMALL, errColor);
        }
    }

    if (IsKeyPressed(KEY_TAB)) {
        if (usernameActive) { usernameActive = false; passwordActive = true; }
        else if (passwordActive) { passwordActive = false; usernameActive = true; }
        else { usernameActive = true; }
    }
    if (IsKeyPressed(KEY_ENTER) && (usernameActive || passwordActive)) {
        attemptLogin();
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
        usernameActive = false;
        passwordActive = false;
    }
}