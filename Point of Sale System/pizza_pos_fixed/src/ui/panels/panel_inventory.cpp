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

static void reload() {
    if (selectedBranchIdx >= 0 && selectedBranchIdx < (int)branches.size())
        items = getInventory(branches[selectedBranchIdx].branchId);
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
    scrollOffset = 0.0f;
    statusMsg = ""; statusTimer = 0.0f;
}

void DrawInventoryPanel() {
    float px = (float)CONTENT_X + 20, py = (float)CONTENT_Y + 20, pw = (float)CONTENT_W - 40, ph = (float)CONTENT_H - 40;

    DrawText("INVENTORY", (int)px, (int)py, FONT_SIZE_LARGE, COL_DARK);
    std::vector<std::string> brNames; for (auto& b : branches) brNames.push_back(b.branchName);
    DrawDropdown({px + 200, py - 5, 200, 36}, "Branch:", brNames, selectedBranchIdx, branchDropdownOpen);
    if (DrawButton({px + pw - 160, py - 5, 150, 36}, "+ Add Item", COL_ACCENT, COL_WHITE)) {
        memset(newNameInput, 0, sizeof(newNameInput)); memset(newUnitInput, 0, sizeof(newUnitInput));
        memset(newQtyInput, 0, sizeof(newQtyInput)); memset(newReorderInput, 0, sizeof(newReorderInput));
        memset(newCostInput, 0, sizeof(newCostInput));
        showAddPopup = true; showUpdatePopup = false;
    }

    int lowCount = 0; for (auto& i : items) if (i.isLowStock) lowCount++;
    if (lowCount > 0) {
        char buf[64]; snprintf(buf, sizeof(buf), "%d item(s) low on stock!", lowCount);
        DrawRectangleRounded({px, py + 35, pw, 28}, 0.1f, 8, Fade(COL_LOW_STOCK, 0.15f));
        DrawText(buf, (int)px + 10, (int)py + 39, FONT_SIZE_SMALL, COL_LOW_STOCK);
    }

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
        char sq[32], rl[32], uc[32]; snprintf(sq, sizeof(sq), "%.1f", inv.quantity); snprintf(rl, sizeof(rl), "%.1f", inv.reorderLevel); snprintf(uc, sizeof(uc), "%.1f", inv.unitCost);
        DrawTableRow(px, y, pw, {std::to_string(inv.inventoryId), inv.inventoryName, sq, inv.unit, rl, uc, inv.lastRestocked, ""}, cw, rc, false);
        float ax = px; for (int c = 0; c < 7; c++) ax += cw[c];
        if (DrawButton({ax, y + 2, 80, 28}, "Update", COL_IN_PREP, COL_WHITE)) {
            selectedInvId = inv.inventoryId;
            memset(addQtyInput, 0, sizeof(addQtyInput)); snprintf(reorderInput, sizeof(reorderInput), "%.1f", inv.reorderLevel); snprintf(unitCostInput, sizeof(unitCostInput), "%.1f", inv.unitCost);
            showUpdatePopup = true; showAddPopup = false;
        }
    }

    if (statusTimer > 0) DrawNotification(statusMsg, statusMsg.find("Error") != std::string::npos ? COL_LOW_STOCK : COL_COMPLETED, statusTimer);

    // UPDATE POPUP
    if (showUpdatePopup) {
        DrawPopupOverlay();
        float pw2 = 400, ph2 = 280, px2 = (WINDOW_WIDTH - pw2) / 2, py2 = (WINDOW_HEIGHT - ph2) / 2;
        DrawCard({px2, py2, pw2, ph2}, "Update Stock", COL_CARD);
        float ix = px2 + 30, iy = py2 + 50;
        DrawText("Add Quantity:", (int)ix, (int)iy, FONT_SIZE_SMALL, COL_DARK); DrawInput({ix + 150, iy - 5, 180, 36}, "", addQtyInput, 31, true, false); iy += 55;
        DrawText("Reorder Level:", (int)ix, (int)iy, FONT_SIZE_SMALL, COL_DARK); DrawInput({ix + 150, iy - 5, 180, 36}, "", reorderInput, 31, true, false); iy += 55;
        DrawText("Unit Cost:", (int)ix, (int)iy, FONT_SIZE_SMALL, COL_DARK); DrawInput({ix + 150, iy - 5, 180, 36}, "", unitCostInput, 31, true, false);
        if (DrawButton({px2 + 30, py2 + ph2 - 55, 120, 36}, "Save", COL_ACCENT, COL_WHITE)) {
            double aq = atof(addQtyInput), rl = atof(reorderInput);
            if (aq != 0 || rl > 0) {
                bool ok = updateStock(selectedInvId, aq) && updateReorderLevel(selectedInvId, rl);
                setStatus(ok ? "Stock updated" : "Error updating", ok); reload(); showUpdatePopup = false;
            }
        }
        if (DrawButton({px2 + pw2 - 150, py2 + ph2 - 55, 120, 36}, "Cancel", COL_BORDER, COL_DARK)) showUpdatePopup = false;
        if (IsKeyPressed(KEY_ESCAPE)) showUpdatePopup = false;
    }

    // ADD POPUP
    if (showAddPopup) {
        DrawPopupOverlay();
        float pw2 = 450, ph2 = 360, px2 = (WINDOW_WIDTH - pw2) / 2, py2 = (WINDOW_HEIGHT - ph2) / 2;
        DrawCard({px2, py2, pw2, ph2}, "Add Inventory Item", COL_CARD);
        float ix = px2 + 30, iy = py2 + 50;
        DrawText("Item Name:", (int)ix, (int)iy, FONT_SIZE_SMALL, COL_DARK); DrawInput({ix + 140, iy - 5, 240, 36}, "", newNameInput, 255, true, false); iy += 50;
        DrawText("Unit:", (int)ix, (int)iy, FONT_SIZE_SMALL, COL_DARK); DrawInput({ix + 140, iy - 5, 240, 36}, "", newUnitInput, 31, true, false); iy += 50;
        DrawText("Quantity:", (int)ix, (int)iy, FONT_SIZE_SMALL, COL_DARK); DrawInput({ix + 140, iy - 5, 240, 36}, "", newQtyInput, 31, true, false); iy += 50;
        DrawText("Reorder Lvl:", (int)ix, (int)iy, FONT_SIZE_SMALL, COL_DARK); DrawInput({ix + 140, iy - 5, 240, 36}, "", newReorderInput, 31, true, false); iy += 50;
        DrawText("Unit Cost:", (int)ix, (int)iy, FONT_SIZE_SMALL, COL_DARK); DrawInput({ix + 140, iy - 5, 240, 36}, "", newCostInput, 31, true, false);
        if (DrawButton({px2 + 30, py2 + ph2 - 55, 120, 36}, "Save", COL_ACCENT, COL_WHITE)) {
            int bid = (selectedBranchIdx < (int)branches.size()) ? branches[selectedBranchIdx].branchId : 1;
            bool ok = addInventoryItem(bid, newNameInput, atof(newQtyInput), newUnitInput, atof(newReorderInput), atof(newCostInput));
            setStatus(ok ? "Item added" : "Error", ok); reload(); showAddPopup = false;
        }
        if (DrawButton({px2 + pw2 - 150, py2 + ph2 - 55, 120, 36}, "Cancel", COL_BORDER, COL_DARK)) showAddPopup = false;
        if (IsKeyPressed(KEY_ESCAPE)) showAddPopup = false;
    }
}