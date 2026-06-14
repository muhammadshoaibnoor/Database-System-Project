#include "panel_shifts.h"
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

static std::vector<Shift> shifts;
static std::vector<Employee> employees;
static int staffFilterIdx = 0;
static char dateFromInp[20] = {0}, dateToInp[20] = {0};
static bool staffDropOpen = false;
static float scrollOffset = 0.0f;
static int activeField = 0; // 1=dateFrom, 2=dateTo

static void reload() {
    employees = getAllEmployees();
    std::string uname = (staffFilterIdx > 0 && staffFilterIdx - 1 < (int)employees.size()) ? employees[staffFilterIdx - 1].username : "";
    shifts = getShifts(uname, dateFromInp, dateToInp);
}

void InitShiftsPanel() {
    reload(); staffFilterIdx = 0; staffDropOpen = false;
    memset(dateFromInp, 0, sizeof(dateFromInp)); memset(dateToInp, 0, sizeof(dateToInp));
    scrollOffset = 0.0f; activeField = 0;
}

void DrawShiftsPanel() {
    float px = (float)CONTENT_X + 20, py = (float)CONTENT_Y + 20, pw = (float)CONTENT_W - 40, ph = (float)CONTENT_H - 40;
    DrawText("SHIFT RECORDS", (int)px, (int)py, FONT_SIZE_LARGE, COL_DARK);

    Vector2 mouse = GetMousePosition();

    // Staff filter dropdown
    std::vector<std::string> names = {"All Staff"};
    for (auto& e : employees) names.push_back(e.firstName + " " + e.lastName);
    if (DrawDropdown({px, py + 30, 220, 36}, "Staff:", names, staffFilterIdx, staffDropOpen)) {
        reload();
    }

    // Date From
    DrawText("From:", (int)px + 240, (int)py + 35, FONT_SIZE_SMALL, COL_DARK);
    Rectangle fromRect = {px + 290, py + 30, 110, 32};
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, fromRect)) activeField = 1;
    DrawInput(fromRect, "YYYY-MM-DD", dateFromInp, 19, activeField == 1, false);

    // Date To
    DrawText("To:", (int)px + 420, (int)py + 35, FONT_SIZE_SMALL, COL_DARK);
    Rectangle toRect = {px + 450, py + 30, 110, 32};
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, toRect)) activeField = 2;
    DrawInput(toRect, "YYYY-MM-DD", dateToInp, 19, activeField == 2, false);

    // Filter / Clear buttons
    if (DrawButton({px + 580, py + 30, 70, 32}, "Filter", COL_ACCENT, COL_WHITE) || IsKeyPressed(KEY_ENTER)) reload();
    if (DrawButton({px + 660, py + 30, 70, 32}, "Clear", COL_BORDER, COL_DARK)) {
        staffFilterIdx = 0;
        memset(dateFromInp, 0, sizeof(dateFromInp));
        memset(dateToInp, 0, sizeof(dateToInp));
        activeField = 0;
        reload();
    }

    float ty = py + 75;
    std::vector<std::string> hd = {"Staff Name", "Role", "Date", "Clock In", "Clock Out", "Duration"};
    std::vector<float> cw = {220, 140, 140, 140, 140, 140};
    DrawTableHeader(px, ty, pw, hd, cw);

    float rh = 36, ah = ph - 140, th = (float)shifts.size() * rh;
    DrawScrollbar({px, ty + 38, pw, ah}, th, scrollOffset);
    float ry = ty + 38 - scrollOffset;
    double totalHrs = 0;
    for (size_t i = 0; i < shifts.size(); i++) {
        float y = ry + (float)i * rh; if (y + rh < ty + 38 || y > ty + 38 + ah) continue;
        auto& s = shifts[i]; Color rc = (i % 2 == 0) ? COL_CARD : COL_BG;
        char hrs[16]; snprintf(hrs, sizeof(hrs), "%.1fh", s.hours); totalHrs += s.hours;
        DrawTableRow(px, y, pw, {s.employeeName, s.userRole, s.shiftDate, s.startTime, s.endTime, hrs}, cw, rc, false);
    }
    char totalStr[32]; snprintf(totalStr, sizeof(totalStr), "Total Hours: %.1f", totalHrs);
    DrawText(totalStr, (int)px, (int)(ty + 38 + ah + 5), FONT_SIZE_SMALL, COL_ACCENT);
}