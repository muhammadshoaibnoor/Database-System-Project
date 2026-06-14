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

static void reload() { discounts = getAllDiscounts(); }
static void setStatus(const char* m, bool ok) { statusMsg = m; statusTimer = ok ? 2.5f : 3.0f; }

void InitDiscountsPanel() { reload(); showAddPopup = false; memset(nameInp, 0, sizeof(nameInp)); memset(pctInp, 0, sizeof(pctInp)); memset(startInp, 0, sizeof(startInp)); memset(endInp, 0, sizeof(endInp)); scrollOffset = 0.0f; statusMsg = ""; statusTimer = 0.0f; }

void DrawDiscountsPanel() {
    float px = (float)CONTENT_X + 20, py = (float)CONTENT_Y + 20, pw = (float)CONTENT_W - 40, ph = (float)CONTENT_H - 40;
    DrawText("DISCOUNTS", (int)px, (int)py, FONT_SIZE_LARGE, COL_DARK);
    if (DrawButton({px + pw - 160, py - 5, 150, 36}, "+ Add Discount", COL_ACCENT, COL_WHITE)) {
        memset(nameInp, 0, sizeof(nameInp)); memset(pctInp, 0, sizeof(pctInp)); memset(startInp, 0, sizeof(startInp)); memset(endInp, 0, sizeof(endInp));
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

    if (showAddPopup) {
        DrawPopupOverlay();
        float pw2 = 450, ph2 = 320, px2 = (WINDOW_WIDTH - pw2) / 2, py2 = (WINDOW_HEIGHT - ph2) / 2;
        DrawCard({px2, py2, pw2, ph2}, "Add Discount", COL_CARD);
        float ix = px2 + 30, iy = py2 + 55;
        DrawText("Name:", (int)ix, (int)iy, FONT_SIZE_SMALL, COL_DARK); DrawInput({ix + 150, iy - 5, 240, 36}, "", nameInp, 99, true, false); iy += 50;
        DrawText("Percentage:", (int)ix, (int)iy, FONT_SIZE_SMALL, COL_DARK); DrawInput({ix + 150, iy - 5, 240, 36}, "", pctInp, 31, true, false); iy += 50;
        DrawText("Start Date:", (int)ix, (int)iy, FONT_SIZE_SMALL, COL_DARK); DrawInput({ix + 150, iy - 5, 240, 36}, "YYYY-MM-DD", startInp, 31, true, false); iy += 50;
        DrawText("End Date:", (int)ix, (int)iy, FONT_SIZE_SMALL, COL_DARK); DrawInput({ix + 150, iy - 5, 240, 36}, "YYYY-MM-DD", endInp, 31, true, false);
        if (DrawButton({px2 + 30, py2 + ph2 - 55, 120, 36}, "Save", COL_ACCENT, COL_WHITE)) {
            bool ok = addDiscount(nameInp, atof(pctInp), startInp, endInp);
            setStatus(ok ? "Discount added" : "Error", ok); reload(); showAddPopup = false;
        }
        if (DrawButton({px2 + pw2 - 150, py2 + ph2 - 55, 120, 36}, "Cancel", COL_BORDER, COL_DARK)) showAddPopup = false;
        if (IsKeyPressed(KEY_ESCAPE)) showAddPopup = false;
    }
}