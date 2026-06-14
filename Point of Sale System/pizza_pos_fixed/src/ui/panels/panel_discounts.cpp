#include "panel_discounts.h"
#include "raylib.h"
#include "../ui_helpers.h"
#include "../../db/db.h"
#include "../../core/models/models.h"
#include "../../core/session.h"
#include <string>
#include <vector>
#include <cstring>
#include <cstdio>
using namespace DB;

static std::vector<Discount> discounts;
static bool showAddPopup = false;
static char nameInp[100] = {0}, pctInp[32] = {0}, startInp[32] = {0}, endInp[32] = {0};
static float scrollOffset = 0.0f;
static std::string statusMsg = ""; static float statusTimer = 0.0f;
static int activeField = 0; // 1=name, 2=pct, 3=start, 4=end

static void reload() { discounts = getAllDiscounts(); }
static void setStatus(const char* m, bool ok) { statusMsg = m; statusTimer = ok ? 2.5f : 3.0f; }

void InitDiscountsPanel() {
    reload(); showAddPopup = false;
    memset(nameInp, 0, sizeof(nameInp)); memset(pctInp, 0, sizeof(pctInp));
    memset(startInp, 0, sizeof(startInp)); memset(endInp, 0, sizeof(endInp));
    scrollOffset = 0.0f; statusMsg = ""; statusTimer = 0.0f; activeField = 0;
}

void DrawDiscountsPanel() {
    float px = (float)CONTENT_X + 20, py = (float)CONTENT_Y + 20, pw = (float)CONTENT_W - 40, ph = (float)CONTENT_H - 40;
    DrawText("DISCOUNTS", (int)px, (int)py, FONT_SIZE_LARGE, COL_DARK);
    if (DrawButton({px + pw - 160, py - 5, 150, 36}, "+ Add Discount", COL_ACCENT, COL_WHITE)) {
        memset(nameInp, 0, sizeof(nameInp)); memset(pctInp, 0, sizeof(pctInp));
        memset(startInp, 0, sizeof(startInp)); memset(endInp, 0, sizeof(endInp));
        activeField = 1;
        showAddPopup = true;
    }

    float ty = py + 45;
    std::vector<std::string> hd = {"ID", "Name", "Value %", "Start Date", "End Date", "Status"};
    std::vector<float> cw = {60, 320, 100, 150, 150, 140};
    DrawTableHeader(px, ty, pw, hd, cw);

    float rh = 36, ah = ph - 100, th = (float)discounts.size() * rh;
    DrawScrollbar({px, ty + 38, pw, ah}, th, scrollOffset);
    float ry = ty + 38 - scrollOffset;
    for (size_t i = 0; i < discounts.size(); i++) {
        float y = ry + (float)i * rh; if (y + rh < ty + 38 || y > ty + 38 + ah) continue;
        auto& d = discounts[i]; Color rc = (i % 2 == 0) ? COL_CARD : COL_BG;
        char pct[32]; snprintf(pct, sizeof(pct), "%.1f%%", d.discountValue);
        DrawTableRow(px, y, pw, {std::to_string(d.discountId), d.discountName, pct, d.startDate, d.endDate, ""}, cw, rc, false);
        float ax = px; for (int c = 0; c < 5; c++) ax += cw[c];
        DrawStatusBadge(ax, y + 4, d.isActive ? "Active" : "Expired");
    }

    if (statusTimer > 0) DrawNotification(statusMsg, statusMsg.find("Error") != std::string::npos ? COL_LOW_STOCK : COL_COMPLETED, statusTimer);

    // ==================== ADD POPUP ====================
    if (showAddPopup) {
        DrawPopupOverlay();
        float pw2 = 480, ph2 = 340, px2 = (WINDOW_WIDTH - pw2) / 2, py2 = (WINDOW_HEIGHT - ph2) / 2;
        DrawCard({px2, py2, pw2, ph2}, "Add Discount", COL_CARD);

        float ix = px2 + 30, iy = py2 + 55;
        Vector2 mouse = GetMousePosition();

        // Name
        DrawText("Name:", (int)ix, (int)iy, FONT_SIZE_SMALL, COL_DARK);
        Rectangle nameRect = {ix + 150, iy - 5, 260, 36};
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, nameRect)) activeField = 1;
        DrawInput(nameRect, "", nameInp, 99, activeField == 1, false);
        iy += 50;

        // Percentage
        DrawText("Percentage:", (int)ix, (int)iy, FONT_SIZE_SMALL, COL_DARK);
        Rectangle pctRect = {ix + 150, iy - 5, 260, 36};
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, pctRect)) activeField = 2;
        DrawInput(pctRect, "", pctInp, 31, activeField == 2, false);
        iy += 50;

        // Start Date
        DrawText("Start Date:", (int)ix, (int)iy, FONT_SIZE_SMALL, COL_DARK);
        Rectangle startRect = {ix + 150, iy - 5, 260, 36};
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, startRect)) activeField = 3;
        DrawInput(startRect, "YYYY-MM-DD", startInp, 31, activeField == 3, false);
        iy += 50;

        // End Date
        DrawText("End Date:", (int)ix, (int)iy, FONT_SIZE_SMALL, COL_DARK);
        Rectangle endRect = {ix + 150, iy - 5, 260, 36};
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, endRect)) activeField = 4;
        DrawInput(endRect, "YYYY-MM-DD", endInp, 31, activeField == 4, false);

        // Keyboard
        if (IsKeyPressed(KEY_TAB)) activeField = (activeField % 4) + 1;
        if (IsKeyPressed(KEY_ESCAPE)) { showAddPopup = false; activeField = 0; }

        // Buttons
        float btnY = py2 + ph2 - 55;
        if (DrawButton({px2 + 30, btnY, 120, 36}, "Save", COL_ACCENT, COL_WHITE) || IsKeyPressed(KEY_ENTER)) {
            if (strlen(nameInp) > 0 && strlen(pctInp) > 0) {
                bool ok = addDiscount(nameInp, atof(pctInp), startInp, endInp);
                setStatus(ok ? "Discount added" : "Error", ok);
                reload();
                showAddPopup = false; activeField = 0;
            }
        }
        if (DrawButton({px2 + pw2 - 150, btnY, 120, 36}, "Cancel", COL_BORDER, COL_DARK)) {
            showAddPopup = false; activeField = 0;
        }
    }
}