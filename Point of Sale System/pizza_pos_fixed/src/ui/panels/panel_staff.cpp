#include "panel_staff.h"
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

static std::vector<Employee> employees;
static std::vector<Branch> branches;
static bool showAddPopup = false, showEditPopup = false, showDeactivateDialog = false;
static char inputs[7][100] = {{0}};
static int roleIdx = 0, branchIdx = 0;
static bool roleDropOpen = false, branchDropOpen = false;
static std::string editUsername = "", deactUser = "";
static std::string statusMsg = ""; static float statusTimer = 0.0f; static float scrollOffset = 0.0f;
static std::vector<std::string> roles = {"Admin", "Receptionist", "Kitchen Staff"};

static void reload() { employees = getAllEmployees(); branches = getAllBranches(); }
static void clearInp() { for (int i = 0; i < 7; i++) memset(inputs[i], 0, 100); roleIdx = 0; branchIdx = 0; roleDropOpen = branchDropOpen = false; }
static void setStatus(const char* m, bool ok) { statusMsg = m; statusTimer = ok ? 2.5f : 3.0f; }

void InitStaffPanel() { reload(); showAddPopup = showEditPopup = showDeactivateDialog = false; clearInp(); editUsername = deactUser = ""; scrollOffset = 0.0f; statusMsg = ""; statusTimer = 0.0f; }

void DrawStaffPanel() {
    float px = (float)CONTENT_X + 20, py = (float)CONTENT_Y + 20, pw = (float)CONTENT_W - 40, ph = (float)CONTENT_H - 40;
    DrawText("STAFF MANAGEMENT", (int)px, (int)py, FONT_SIZE_LARGE, COL_DARK);
    if (DrawButton({px + pw - 160, py - 5, 150, 36}, "+ Add New Staff", COL_ACCENT, COL_WHITE)) { clearInp(); showAddPopup = true; showEditPopup = false; showDeactivateDialog = false; }

    float ty = py + 45;
    std::vector<std::string> hd = {"Username", "Full Name", "Role", "Branch", "Hired", "Actions"};
    std::vector<float> cw = {140, 220, 140, 160, 120, 200};
    DrawTableHeader(px, ty, pw, hd, cw);

    float rh = 36, ah = ph - 100, th = (float)employees.size() * rh;
    DrawScrollbar({px, ty + 38, pw, ah}, th, scrollOffset);
    float ry = ty + 38 - scrollOffset;
    for (size_t i = 0; i < employees.size(); i++) {
        float y = ry + (float)i * rh;
        if (y + rh < ty + 38 || y > ty + 38 + ah) continue;
        auto& e = employees[i];
        Color rc = (i % 2 == 0) ? COL_CARD : COL_BG;
        DrawTableRow(px, y, pw, {e.username, e.firstName + " " + e.lastName, e.role, e.branchName, e.hireDate, ""}, cw, rc, false);
        float ax = px; for (int c = 0; c < 5; c++) ax += cw[c];
        if (DrawButton({ax, y + 2, 55, 28}, "Edit", COL_IN_PREP, COL_WHITE)) {
            editUsername = e.username; snprintf(inputs[0], 100, "%s", e.firstName.c_str()); snprintf(inputs[1], 100, "%s", e.lastName.c_str());
            snprintf(inputs[2], 100, "%s", e.email.c_str()); snprintf(inputs[3], 100, "%s", e.phone.c_str());
            roleIdx = 0; for (int r = 0; r < (int)roles.size(); r++) if (roles[r] == e.role) roleIdx = r;
            branchIdx = 0; for (int b = 0; b < (int)branches.size(); b++) if (branches[b].branchId == e.branchId) branchIdx = b;
            showEditPopup = true; showAddPopup = false; showDeactivateDialog = false;
        }
        if (DrawButton({ax + 62, y + 2, 85, 28}, "Deactivate", COL_LOW_STOCK, COL_WHITE)) { deactUser = e.username; showDeactivateDialog = true; showAddPopup = false; showEditPopup = false; }
    }

    if (statusTimer > 0) DrawNotification(statusMsg, statusMsg.find("Error") != std::string::npos ? COL_LOW_STOCK : COL_COMPLETED, statusTimer);

    if (showDeactivateDialog) { bool sd = true; if (DrawConfirmDialog("Deactivate Staff", "Deactivate " + deactUser + "?", sd)) { bool ok = deactivateEmployee(deactUser); setStatus(ok ? "Staff deactivated" : "Error", ok); reload(); } showDeactivateDialog = sd; }

    // ADD POPUP
    if (showAddPopup) {
        DrawPopupOverlay();
        float pw2 = 500, ph2 = 480, px2 = (WINDOW_WIDTH - pw2) / 2, py2 = (WINDOW_HEIGHT - ph2) / 2;
        DrawCard({px2, py2, pw2, ph2}, "Add New Staff", COL_CARD);
        float ix = px2 + 30, iy = py2 + 55;
        const char* labels[] = {"Username:", "First Name:", "Last Name:", "Email:", "Phone:", "Password:"};
        for (int f = 0; f < 6; f++) { DrawText(labels[f], (int)ix, (int)iy, FONT_SIZE_SMALL, COL_DARK); DrawInput({ix + 150, iy - 5, pw2 - 210, 36}, "", inputs[f], f == 5 ? 99 : 99, true, f == 5); iy += 48; }
        DrawText("Role:", (int)ix, (int)iy, FONT_SIZE_SMALL, COL_DARK); DrawDropdown({ix + 150, iy - 5, pw2 - 210, 36}, "", roles, roleIdx, roleDropOpen); iy += 48;
        DrawText("Branch:", (int)ix, (int)iy, FONT_SIZE_SMALL, COL_DARK); std::vector<std::string> bn; for (auto& b : branches) bn.push_back(b.branchName);
        DrawDropdown({ix + 150, iy - 5, pw2 - 210, 36}, "", bn, branchIdx, branchDropOpen);
        if (DrawButton({px2 + 30, py2 + ph2 - 55, 120, 36}, "Save", COL_ACCENT, COL_WHITE)) {
            int bid = (branchIdx < (int)branches.size()) ? branches[branchIdx].branchId : 1;
            bool ok = addEmployee(inputs[0], "", bid, inputs[1], inputs[2], inputs[3], inputs[4], inputs[5], roles[roleIdx]);
            setStatus(ok ? "Staff added" : "Error", ok); reload(); showAddPopup = false; clearInp();
        }
        if (DrawButton({px2 + pw2 - 150, py2 + ph2 - 55, 120, 36}, "Cancel", COL_BORDER, COL_DARK)) { showAddPopup = false; clearInp(); }
        if (IsKeyPressed(KEY_ESCAPE)) { showAddPopup = false; clearInp(); }
    }

    // EDIT POPUP
    if (showEditPopup) {
        DrawPopupOverlay();
        float pw2 = 500, ph2 = 400, px2 = (WINDOW_WIDTH - pw2) / 2, py2 = (WINDOW_HEIGHT - ph2) / 2;
        DrawCard({px2, py2, pw2, ph2}, "Edit Staff: " + editUsername, COL_CARD);
        float ix = px2 + 30, iy = py2 + 55;
        const char* labels[] = {"First Name:", "Last Name:", "Email:", "Phone:"};
        for (int f = 0; f < 4; f++) { DrawText(labels[f], (int)ix, (int)iy, FONT_SIZE_SMALL, COL_DARK); DrawInput({ix + 150, iy - 5, pw2 - 210, 36}, "", inputs[f], 99, true, false); iy += 48; }
        DrawText("Role:", (int)ix, (int)iy, FONT_SIZE_SMALL, COL_DARK); DrawDropdown({ix + 150, iy - 5, pw2 - 210, 36}, "", roles, roleIdx, roleDropOpen); iy += 48;
        DrawText("Branch:", (int)ix, (int)iy, FONT_SIZE_SMALL, COL_DARK); std::vector<std::string> bn; for (auto& b : branches) bn.push_back(b.branchName);
        DrawDropdown({ix + 150, iy - 5, pw2 - 210, 36}, "", bn, branchIdx, branchDropOpen);
        if (DrawButton({px2 + 30, py2 + ph2 - 55, 120, 36}, "Update", COL_ACCENT, COL_WHITE)) {
            int bid = (branchIdx < (int)branches.size()) ? branches[branchIdx].branchId : 1;
            bool ok = updateEmployee(editUsername, inputs[0], inputs[1], inputs[2], inputs[3], roles[roleIdx], bid);
            setStatus(ok ? "Staff updated" : "Error", ok); reload(); showEditPopup = false;
        }
        if (DrawButton({px2 + pw2 - 150, py2 + ph2 - 55, 120, 36}, "Cancel", COL_BORDER, COL_DARK)) showEditPopup = false;
        if (IsKeyPressed(KEY_ESCAPE)) showEditPopup = false;
    }
}