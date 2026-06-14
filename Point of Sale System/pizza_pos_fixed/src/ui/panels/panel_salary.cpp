#include "panel_salary.h"
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

static std::vector<Salary> records;
static std::vector<Employee> employees;
static int staffFilterIdx = 0;
static bool showAddPopup = false, staffDropOpen = false;
static char amountInput[32] = {0}, dateInput[32] = {0}, notesInput[256] = {0};
static float scrollOffset = 0.0f;
static std::string statusMsg = ""; static float statusTimer = 0.0f;

static void reload() {
    employees = getAllEmployees();
    std::vector<std::string> names; for (auto& e : employees) names.push_back(e.firstName + " " + e.lastName);
    if (staffFilterIdx == 0) records = getSalaryHistory();
    else if (staffFilterIdx - 1 < (int)employees.size()) records = getSalaryHistory(employees[staffFilterIdx - 1].username);
}
static void setStatus(const char* m, bool ok) { statusMsg = m; statusTimer = ok ? 2.5f : 3.0f; }

void InitSalaryPanel() { reload(); showAddPopup = false; staffDropOpen = false; memset(amountInput, 0, sizeof(amountInput)); memset(dateInput, 0, sizeof(dateInput)); memset(notesInput, 0, sizeof(notesInput)); scrollOffset = 0.0f; statusMsg = ""; statusTimer = 0.0f; }

void DrawSalaryPanel() {
    if (!hasRole(ROLE_ADMIN)) { DrawText("Access Denied - Admin Only", CONTENT_X + 50, CONTENT_Y + 50, FONT_SIZE_LARGE, COL_LOW_STOCK); return; }

    float px = (float)CONTENT_X + 20, py = (float)CONTENT_Y + 20, pw = (float)CONTENT_W - 40, ph = (float)CONTENT_H - 40;
    DrawText("SALARY MANAGEMENT (Admin Only)", (int)px, (int)py, FONT_SIZE_LARGE, COL_DARK);

    std::vector<std::string> enames = {"All Staff"}; for (auto& e : employees) enames.push_back(e.firstName + " " + e.lastName);
    DrawDropdown({px + 450, py - 5, 220, 36}, "Staff:", enames, staffFilterIdx, staffDropOpen);
    if (DrawButton({px + pw - 160, py - 5, 150, 36}, "+ Add Payment", COL_ACCENT, COL_WHITE)) {
        memset(amountInput, 0, sizeof(amountInput)); memset(dateInput, 0, sizeof(dateInput)); memset(notesInput, 0, sizeof(notesInput));
        showAddPopup = true;
    }

    float ty = py + 45;
    std::vector<std::string> hd = {"Date", "Staff Name", "Amount (PKR)", "Notes"};
    std::vector<float> cw = {140, 250, 180, 430};
    DrawTableHeader(px, ty, pw, hd, cw);

    float rh = 36, ah = ph - 100, th = (float)records.size() * rh;
    DrawScrollbar({px, ty + 38, pw, ah}, th, scrollOffset);
    float ry = ty + 38 - scrollOffset;
    for (size_t i = 0; i < records.size(); i++) {
        float y = ry + (float)i * rh; if (y + rh < ty + 38 || y > ty + 38 + ah) continue;
        auto& r = records[i]; Color rc = (i % 2 == 0) ? COL_CARD : COL_BG;
        char amt[32]; snprintf(amt, sizeof(amt), "PKR %.0f", r.amount);
        DrawTableRow(px, y, pw, {r.paymentDate, r.employeeName, amt, r.notes}, cw, rc, false);
    }

    if (statusTimer > 0) DrawNotification(statusMsg, statusMsg.find("Error") != std::string::npos ? COL_LOW_STOCK : COL_COMPLETED, statusTimer);

    if (showAddPopup) {
        DrawPopupOverlay();
        float pw2 = 450, ph2 = 300, px2 = (WINDOW_WIDTH - pw2) / 2, py2 = (WINDOW_HEIGHT - ph2) / 2;
        DrawCard({px2, py2, pw2, ph2}, "Add Salary Payment", COL_CARD);
        float ix = px2 + 30, iy = py2 + 55;
        DrawText("Staff:", (int)ix, (int)iy, FONT_SIZE_SMALL, COL_DARK);
        std::vector<std::string> sn; for (auto& e : employees) sn.push_back(e.firstName + " " + e.lastName);
        static int payStaffIdx = 0; static bool payDrop = false;
        DrawDropdown({ix + 150, iy - 5, 240, 36}, "", sn, payStaffIdx, payDrop); iy += 50;
        DrawText("Amount:", (int)ix, (int)iy, FONT_SIZE_SMALL, COL_DARK); DrawInput({ix + 150, iy - 5, 240, 36}, "", amountInput, 31, true, false); iy += 50;
        DrawText("Date:", (int)ix, (int)iy, FONT_SIZE_SMALL, COL_DARK); DrawInput({ix + 150, iy - 5, 240, 36}, "", dateInput, 31, true, false); iy += 50;
        DrawText("Notes:", (int)ix, (int)iy, FONT_SIZE_SMALL, COL_DARK); DrawInput({ix + 150, iy - 5, 240, 36}, "", notesInput, 255, true, false);
        if (DrawButton({px2 + 30, py2 + ph2 - 55, 120, 36}, "Save", COL_ACCENT, COL_WHITE)) {
            if (payStaffIdx < (int)employees.size()) {
                bool ok = addSalaryPayment(employees[payStaffIdx].username, atof(amountInput), dateInput, notesInput);
                setStatus(ok ? "Payment added" : "Error", ok); reload(); showAddPopup = false;
            }
        }
        if (DrawButton({px2 + pw2 - 150, py2 + ph2 - 55, 120, 36}, "Cancel", COL_BORDER, COL_DARK)) showAddPopup = false;
        if (IsKeyPressed(KEY_ESCAPE)) showAddPopup = false;
    }
}