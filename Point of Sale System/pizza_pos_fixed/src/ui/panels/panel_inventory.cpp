#include "panel_inventory.h"
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

static std::vector<Inventory> items;
static std::vector<Branch> branches;
static int selectedBranchIdx = 0;
static bool showUpdatePopup = false, showAddPopup = false;
static int selectedInvId = -1;
static char addQtyInput[32] = {0}, reorderInput[32] = {0}, unitCostInput[32] = {0};
static char newNameInput[256] = {0}, newUnitInput[32] = {0}, newQtyInput[32] = {0}, newReorderInput[32] = {0}, newCostInput[32] = {0};
static bool branchDropdownOpen = false;
static float scrollOffset = 0.0f;
static std::string statusMsg = "";
static float statusTimer = 0.0f;
static int activeField = 0; // 1=addQty, 2=reorder, 3=unitCost, for add: 1=name,2=unit,3=qty,4=reorder,5=cost

static void reload() {
    if (selectedBranchIdx >= 0 && selectedBranchIdx < (int)branches.size())
        items = getInventory(branches[selectedBranchIdx].branchId);
    else
        items.clear();
}
static void setStatus(const char* m, bool ok) { statusMsg = m; statusTimer = ok ? 2.5f : 3.0f; }

void InitInventoryPanel() {
    branches = getAllBranches();
    selectedBranchIdx = 0;
    reload();
    showUpdatePopup = showAddPopup = false;
    selectedInvId = -1;
    memset(addQtyInput, 0, sizeof(addQtyInput));
    memset(reorderInput, 0, sizeof(reorderInput));
    memset(unitCostInput, 0, sizeof(unitCostInput));
    memset(newNameInput, 0, sizeof(newNameInput));
    memset(newUnitInput, 0, sizeof(newUnitInput));
    memset(newQtyInput, 0, sizeof(newQtyInput));
    memset(newReorderInput, 0, sizeof(newReorderInput));
    memset(newCostInput, 0, sizeof(newCostInput));
    branchDropdownOpen = false;
    scrollOffset = 0.0f;
    statusMsg = ""; statusTimer = 0.0f;
    activeField = 0;
}

void DrawInventoryPanel() {
    float px = (float)CONTENT_X + 20, py = (float)CONTENT_Y + 20, pw = (float)CONTENT_W - 40, ph = (float)CONTENT_H - 40;

    DrawText("INVENTORY", (int)px, (int)py, FONT_SIZE_LARGE, COL_DARK);

    // Branch dropdown
    std::vector<std::string> brNames;
    for (auto& b : branches) brNames.push_back(b.branchName);
    if (DrawDropdown({px + 200, py - 5, 200, 36}, "Branch:", brNames, selectedBranchIdx, branchDropdownOpen)) {
        reload();
    }

    // + Add Item button
    if (DrawButton({px + pw - 160, py - 5, 150, 36}, "+ Add Item", COL_ACCENT, COL_WHITE)) {
        memset(newNameInput, 0, sizeof(newNameInput));
        memset(newUnitInput, 0, sizeof(newUnitInput));
        memset(newQtyInput, 0, sizeof(newQtyInput));
        memset(newReorderInput, 0, sizeof(newReorderInput));
        memset(newCostInput, 0, sizeof(newCostInput));
        activeField = 1;
        showAddPopup = true; showUpdatePopup = false;
    }

    // Low stock banner
    int lowCount = 0;
    for (auto& i : items) if (i.isLowStock) lowCount++;
    if (lowCount > 0) {
        char buf[64]; snprintf(buf, sizeof(buf), "%d item(s) low on stock!", lowCount);
        DrawRectangleRounded({px, py + 35, pw, 28}, 0.1f, 8, Fade(COL_LOW_STOCK, 0.15f));
        DrawText(buf, (int)px + 10, (int)py + 39, FONT_SIZE_SMALL, COL_LOW_STOCK);
    }

    // Table
    float tableY = py + 72;
    std::vector<std::string> hd = {"ID", "Item Name", "Stock", "Unit", "Reorder Lvl", "Unit Cost", "Last Restocked", "Actions"};
    std::vector<float> cw = {50, 220, 90, 70, 100, 100, 160, 120};
    DrawTableHeader(px, tableY, pw, hd, cw);

    float rowH = 36, areaH = ph - 130, totalH = (float)items.size() * rowH;
    DrawScrollbar({px, tableY + 38, pw, areaH}, totalH, scrollOffset);

    float ry = tableY + 38 - scrollOffset;
    for (size_t i = 0; i < items.size(); i++) {
        float y = ry + (float)i * rowH;
        if (y + rowH < tableY + 38 || y > tableY + 38 + areaH) continue;
        auto& inv = items[i];
        Color rc = inv.isLowStock ? ColorAlphaBlend(COL_CARD, Fade(COL_LOW_STOCK, 0.2f), WHITE) : ((i % 2 == 0) ? COL_CARD : COL_BG);
        char sq[32], rl[32], uc[32];
        snprintf(sq, sizeof(sq), "%.1f", inv.quantity);
        snprintf(rl, sizeof(rl), "%.1f", inv.reorderLevel);
        snprintf(uc, sizeof(uc), "%.1f", inv.unitCost);
        DrawTableRow(px, y, pw, {std::to_string(inv.inventoryId), inv.inventoryName, sq, inv.unit, rl, uc, inv.lastRestocked, ""}, cw, rc, false);
        float ax = px;
        for (int c = 0; c < 7; c++) ax += cw[c];
        if (DrawButton({ax, y + 2, 80, 28}, "Update", COL_IN_PREP, COL_WHITE)) {
            selectedInvId = inv.inventoryId;
            memset(addQtyInput, 0, sizeof(addQtyInput));
            snprintf(reorderInput, sizeof(reorderInput), "%.1f", inv.reorderLevel);
            snprintf(unitCostInput, sizeof(unitCostInput), "%.1f", inv.unitCost);
            activeField = 1;
            showUpdatePopup = true; showAddPopup = false;
        }
    }

    // Status notification
    if (statusTimer > 0)
        DrawNotification(statusMsg, statusMsg.find("Error") != std::string::npos ? COL_LOW_STOCK : COL_COMPLETED, statusTimer);

    // ==================== UPDATE POPUP ====================
    if (showUpdatePopup) {
        DrawPopupOverlay();
        float pw2 = 420, ph2 = 300, px2 = (WINDOW_WIDTH - pw2) / 2, py2 = (WINDOW_HEIGHT - ph2) / 2;
        DrawCard({px2, py2, pw2, ph2}, "Update Stock", COL_CARD);

        float ix = px2 + 30, iy = py2 + 55;
        Vector2 mouse = GetMousePosition();

        // Add Quantity
        DrawText("Add Quantity:", (int)ix, (int)iy, FONT_SIZE_SMALL, COL_DARK);
        Rectangle qtyRect = {ix + 150, iy - 5, 200, 36};
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, qtyRect)) activeField = 1;
        DrawInput(qtyRect, "", addQtyInput, 31, activeField == 1, false);
        iy += 55;

        // Reorder Level
        DrawText("Reorder Level:", (int)ix, (int)iy, FONT_SIZE_SMALL, COL_DARK);
        Rectangle reorderRect = {ix + 150, iy - 5, 200, 36};
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, reorderRect)) activeField = 2;
        DrawInput(reorderRect, "", reorderInput, 31, activeField == 2, false);
        iy += 55;

        // Unit Cost
        DrawText("Unit Cost:", (int)ix, (int)iy, FONT_SIZE_SMALL, COL_DARK);
        Rectangle costRect = {ix + 150, iy - 5, 200, 36};
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, costRect)) activeField = 3;
        DrawInput(costRect, "", unitCostInput, 31, activeField == 3, false);

        // Keyboard
        if (IsKeyPressed(KEY_TAB)) activeField = (activeField % 3) + 1;
        if (IsKeyPressed(KEY_ESCAPE)) { showUpdatePopup = false; activeField = 0; }

        // Buttons
        float btnY = py2 + ph2 - 55;
        if (DrawButton({px2 + 30, btnY, 120, 36}, "Save", COL_ACCENT, COL_WHITE) || IsKeyPressed(KEY_ENTER)) {
            double aq = atof(addQtyInput);
            double rl = atof(reorderInput);
            double uc = atof(unitCostInput);
            bool ok = true;
            if (aq != 0) ok = updateStock(selectedInvId, aq);
            if (ok && rl > 0) ok = updateReorderLevel(selectedInvId, rl);
            if (ok && uc > 0) ok = updateInventoryItem(selectedInvId, "", rl, uc);
            setStatus(ok ? "Stock updated" : "Error updating", ok);
            reload();
            showUpdatePopup = false; activeField = 0;
        }
        if (DrawButton({px2 + pw2 - 150, btnY, 120, 36}, "Cancel", COL_BORDER, COL_DARK)) {
            showUpdatePopup = false; activeField = 0;
        }
    }

    // ==================== ADD POPUP ====================
    if (showAddPopup) {
        DrawPopupOverlay();
        float pw2 = 480, ph2 = 400, px2 = (WINDOW_WIDTH - pw2) / 2, py2 = (WINDOW_HEIGHT - ph2) / 2;
        DrawCard({px2, py2, pw2, ph2}, "Add Inventory Item", COL_CARD);

        float ix = px2 + 30, iy = py2 + 55;
        Vector2 mouse = GetMousePosition();

        // Item Name
        DrawText("Item Name:", (int)ix, (int)iy, FONT_SIZE_SMALL, COL_DARK);
        Rectangle nameRect = {ix + 140, iy - 5, 260, 36};
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, nameRect)) activeField = 1;
        DrawInput(nameRect, "", newNameInput, 255, activeField == 1, false);
        iy += 50;

        // Unit
        DrawText("Unit:", (int)ix, (int)iy, FONT_SIZE_SMALL, COL_DARK);
        Rectangle unitRect = {ix + 140, iy - 5, 260, 36};
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, unitRect)) activeField = 2;
        DrawInput(unitRect, "", newUnitInput, 31, activeField == 2, false);
        iy += 50;

        // Quantity
        DrawText("Quantity:", (int)ix, (int)iy, FONT_SIZE_SMALL, COL_DARK);
        Rectangle newQtyRect = {ix + 140, iy - 5, 260, 36};
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, newQtyRect)) activeField = 3;
        DrawInput(newQtyRect, "", newQtyInput, 31, activeField == 3, false);
        iy += 50;

        // Reorder Level
        DrawText("Reorder Lvl:", (int)ix, (int)iy, FONT_SIZE_SMALL, COL_DARK);
        Rectangle newReorderRect = {ix + 140, iy - 5, 260, 36};
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, newReorderRect)) activeField = 4;
        DrawInput(newReorderRect, "", newReorderInput, 31, activeField == 4, false);
        iy += 50;

        // Unit Cost
        DrawText("Unit Cost:", (int)ix, (int)iy, FONT_SIZE_SMALL, COL_DARK);
        Rectangle newCostRect = {ix + 140, iy - 5, 260, 36};
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, newCostRect)) activeField = 5;
        DrawInput(newCostRect, "", newCostInput, 31, activeField == 5, false);

        // Keyboard
        if (IsKeyPressed(KEY_TAB)) activeField = (activeField % 5) + 1;
        if (IsKeyPressed(KEY_ESCAPE)) { showAddPopup = false; activeField = 0; }

        // Buttons
        float btnY = py2 + ph2 - 55;
        if (DrawButton({px2 + 30, btnY, 120, 36}, "Save", COL_ACCENT, COL_WHITE) || IsKeyPressed(KEY_ENTER)) {
            int bid = (selectedBranchIdx < (int)branches.size()) ? branches[selectedBranchIdx].branchId : 1;
            if (strlen(newNameInput) > 0) {
                bool ok = addInventoryItem(bid, newNameInput, atof(newQtyInput), newUnitInput, atof(newReorderInput), atof(newCostInput));
                setStatus(ok ? "Item added" : "Error", ok);
                reload();
                showAddPopup = false; activeField = 0;
            }
        }
        if (DrawButton({px2 + pw2 - 150, btnY, 120, 36}, "Cancel", COL_BORDER, COL_DARK)) {
            showAddPopup = false; activeField = 0;
        }
    }
}