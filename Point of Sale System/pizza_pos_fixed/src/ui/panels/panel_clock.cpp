#include "panel_clock.h"
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

static bool clockedIn = false;
static std::string clockInTime = "";
static std::vector<Shift> shifts;
static std::string statusMsg = "";
static float statusTimer = 0.0f;
static float scrollOffset = 0.0f;

static void reload() {
    clockedIn = isCurrentlyClockedIn(getUsername());
    clockInTime = clockedIn ? getCurrentClockInTime(getUsername()) : "";
    shifts = getMyShifts(getUsername());
}
static void setStatus(const char* m, bool ok) { statusMsg = m; statusTimer = ok ? 2.5f : 3.5f; }

void InitClockPanel() {
    reload();
    scrollOffset = 0.0f;
    statusMsg = ""; statusTimer = 0.0f;
}

void DrawClockPanel() {
    float px = (float)CONTENT_X + 20, py = (float)CONTENT_Y + 20;
    float cw = (float)CONTENT_W - 40;

    DrawText("MY SHIFTS", (int)px, (int)py, FONT_SIZE_LARGE, COL_DARK);

    // Status card
    float cardX = (float)CONTENT_X + (CONTENT_W - 500) / 2.0f;
    float cardY = py + 45;
    float cardW = 500, cardH = 130;
    DrawCard({cardX, cardY, cardW, cardH}, "", COL_CARD);

    if (clockedIn) {
        DrawText("Currently Clocked In", (int)(cardX + (cardW - MeasureText("Currently Clocked In", FONT_SIZE_LARGE)) / 2),
                 (int)cardY + 15, FONT_SIZE_LARGE, COL_COMPLETED);
        std::string sinceText = "Since: " + clockInTime;
        float sw = (float)MeasureText(sinceText.c_str(), FONT_SIZE_NORMAL);
        DrawText(sinceText.c_str(), (int)(cardX + (cardW - sw) / 2), (int)cardY + 50, FONT_SIZE_NORMAL, COL_DARK);

        if (DrawButton({cardX + cardW / 2 - 70, cardY + 80, 140, 36}, "CLOCK OUT", COL_LOW_STOCK, COL_WHITE)) {
            bool ok = clockOut(getUsername());
            setStatus(ok ? "Clocked out successfully" : "Error clocking out", ok);
            reload();
        }
    } else {
        DrawText("Not Clocked In", (int)(cardX + (cardW - MeasureText("Not Clocked In", FONT_SIZE_LARGE)) / 2),
                 (int)cardY + 20, FONT_SIZE_LARGE, COL_LOW_STOCK);

        if (DrawButton({cardX + cardW / 2 - 70, cardY + 65, 140, 36}, "CLOCK IN", COL_COMPLETED, COL_WHITE)) {
            bool ok = clockIn(getUsername());
            setStatus(ok ? "Clocked in successfully" : "Error clocking in", ok);
            reload();
        }
    }

    // Shift history table
    float tableY = cardY + cardH + 25;
    float tableW = cw;
    DrawText("Shift History", (int)px, (int)tableY - 25, FONT_SIZE_NORMAL, COL_DARK);

    std::vector<std::string> hd = {"Date", "Clock In", "Clock Out", "Duration"};
    std::vector<float> cws = {160, 200, 200, 400};
    DrawTableHeader(px, tableY, tableW, hd, cws);

    float areaH = (float)CONTENT_H - (tableY - CONTENT_Y) - 60;
    float rowH = 36, totalH = shifts.size() * rowH;
    DrawScrollbar({px, tableY + 38, tableW, areaH}, totalH, scrollOffset);

    float ry = tableY + 38 - scrollOffset;
    for (size_t i = 0; i < shifts.size(); i++) {
        float y = ry + i * rowH;
        if (y + rowH < tableY + 38 || y > tableY + 38 + areaH) continue;
        auto& s = shifts[i];
        Color rc = (i % 2 == 0) ? COL_CARD : COL_BG;
        char dur[16]; snprintf(dur, sizeof(dur), "%.1f hrs", s.hours);
        DrawTableRow(px, y, tableW, {s.shiftDate, s.startTime, s.endTime.empty() ? "Active" : s.endTime, dur}, cws, rc, false);
    }

    if (statusTimer > 0)
        DrawNotification(statusMsg, statusMsg.find("Error") != std::string::npos ? COL_LOW_STOCK : COL_COMPLETED, statusTimer);
}