#include "panel_branches.h"
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

static std::vector<Branch> branches;
static bool showAddPopup = false, showEditPopup = false;
static int selectedBranchId = -1;
static char nameInp[100] = {0}, addrInp[100] = {0}, phoneInp[32] = {0}, emailInp[64] = {0}, openInp[16] = {0}, closeInp[16] = {0};
static std::string statusMsg = ""; static float statusTimer = 0.0f;
static int activeField = 0;

static void reload() { branches = getAllBranches(); }
static void setStatus(const char* m, bool ok) { statusMsg = m; statusTimer = ok ? 2.5f : 3.0f; }

void InitBranchesPanel() {
    reload(); showAddPopup = showEditPopup = false; selectedBranchId = -1;
    memset(nameInp, 0, sizeof(nameInp)); memset(addrInp, 0, sizeof(addrInp));
    memset(phoneInp, 0, sizeof(phoneInp)); memset(emailInp, 0, sizeof(emailInp));
    memset(openInp, 0, sizeof(openInp)); memset(closeInp, 0, sizeof(closeInp));
    statusMsg = ""; statusTimer = 0.0f; activeField = 0;
}

void DrawBranchesPanel() {
    float px = (float)CONTENT_X + 20, py = (float)CONTENT_Y + 20, pw = (float)CONTENT_W - 40;
    DrawText("BRANCH MANAGEMENT", (int)px, (int)py, FONT_SIZE_LARGE, COL_DARK);
    if (DrawButton({px + pw - 160, py - 5, 150, 36}, "+ Add Branch", COL_ACCENT, COL_WHITE)) {
        memset(nameInp, 0, sizeof(nameInp)); memset(addrInp, 0, sizeof(addrInp));
        memset(phoneInp, 0, sizeof(phoneInp)); memset(emailInp, 0, sizeof(emailInp));
        memset(openInp, 0, sizeof(openInp)); memset(closeInp, 0, sizeof(closeInp));
        activeField = 1;
        showAddPopup = true; showEditPopup = false;
    }

    float ty = py + 45;
    std::vector<std::string> hd = {"ID", "Name", "Address", "Phone", "Email", "Hours", "Actions"};
    std::vector<float> cw = {50, 200, 250, 140, 200, 120, 80};
    DrawTableHeader(px, ty, pw, hd, cw);

    for (size_t i = 0; i < branches.size(); i++) {
        float y = ty + 38 + i * 36.0f; auto& b = branches[i]; Color rc = (i % 2 == 0) ? COL_CARD : COL_BG;
        std::string hrs = b.openingTime + "-" + b.closingTime;
        DrawTableRow(px, y, pw, {std::to_string(b.branchId), b.branchName, b.address, b.phone, b.email, hrs, ""}, cw, rc, false);
        float ax = px; for (int c = 0; c < 6; c++) ax += cw[c];
        if (DrawButton({ax, y + 2, 55, 28}, "Edit", COL_IN_PREP, COL_WHITE)) {
            selectedBranchId = b.branchId;
            snprintf(nameInp, sizeof(nameInp), "%s", b.branchName.c_str());
            snprintf(addrInp, sizeof(addrInp), "%s", b.address.c_str());
            snprintf(phoneInp, sizeof(phoneInp), "%s", b.phone.c_str());
            snprintf(emailInp, sizeof(emailInp), "%s", b.email.c_str());
            snprintf(openInp, sizeof(openInp), "%s", b.openingTime.c_str());
            snprintf(closeInp, sizeof(closeInp), "%s", b.closingTime.c_str());
            activeField = 1;
            showEditPopup = true; showAddPopup = false;
        }
    }

    if (statusTimer > 0) DrawNotification(statusMsg, statusMsg.find("Error") != std::string::npos ? COL_LOW_STOCK : COL_COMPLETED, statusTimer);

    // ==================== ADD / EDIT POPUP ====================
    if (showAddPopup || showEditPopup) {
        DrawPopupOverlay();
        bool isEdit = showEditPopup;
        float pw2 = 480, ph2 = 420, px2 = (WINDOW_WIDTH - pw2) / 2, py2 = (WINDOW_HEIGHT - ph2) / 2;
        DrawCard({px2, py2, pw2, ph2}, isEdit ? "Edit Branch" : "Add Branch", COL_CARD);

        float ix = px2 + 30, iy = py2 + 55;
        Vector2 mouse = GetMousePosition();
        const char* lbs[] = {"Name:", "Address:", "Phone:", "Email:", "Opens:", "Closes:"};
        char* bufs[] = {nameInp, addrInp, phoneInp, emailInp, openInp, closeInp};
        int maxLens[] = {99, 99, 31, 63, 15, 15};

        for (int f = 0; f < 6; f++) {
            DrawText(lbs[f], (int)ix, (int)iy, FONT_SIZE_SMALL, COL_DARK);
            Rectangle fieldRect = {ix + 120, iy - 5, pw2 - 180, 36};
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, fieldRect))
                activeField = f + 1;
            DrawInput(fieldRect, "", bufs[f], maxLens[f], activeField == f + 1, false);
            iy += 45;
        }

        // Keyboard
        if (IsKeyPressed(KEY_TAB)) activeField = (activeField % 6) + 1;
        if (IsKeyPressed(KEY_ESCAPE)) { showAddPopup = false; showEditPopup = false; activeField = 0; }

        // Buttons
        float btnY = py2 + ph2 - 55;
        if (DrawButton({px2 + 30, btnY, 120, 36}, "Save", COL_ACCENT, COL_WHITE) || IsKeyPressed(KEY_ENTER)) {
            if (strlen(nameInp) > 0) {
                bool ok;
                if (isEdit)
                    ok = updateBranch(selectedBranchId, nameInp, addrInp, phoneInp, emailInp);
                else
                    ok = addBranch(nameInp, addrInp, phoneInp, emailInp, openInp, closeInp);
                setStatus(ok ? "Saved" : "Error", ok);
                reload();
                showAddPopup = false; showEditPopup = false; activeField = 0;
            }
        }
        if (DrawButton({px2 + pw2 - 150, btnY, 120, 36}, "Cancel", COL_BORDER, COL_DARK)) {
            showAddPopup = false; showEditPopup = false; activeField = 0;
        }
    }
}