#include "panel_menu_manager.h"
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

static std::vector<MenuItem> menuItems;
static std::vector<Category> categories;
static std::vector<Price> priceHistory;

static bool showAddPopup = false;
static bool showEditPopup = false;
static bool showPriceHistoryPopup = false;
static int selectedItemId = -1;

static char nameInput[256] = {0};
static char descInput[256] = {0};
static char priceInput[32] = {0};
static char newPriceInput[32] = {0};
static int selectedCategoryIdx = 0;
static int selectedTypeIdx = 0;
static bool catDropdownOpen = false;
static bool typeDropdownOpen = false;

static std::string statusMessage = "";
static float statusTimer = 0.0f;
static float scrollOffset = 0.0f;
static int activeInputField = 0;

static std::vector<std::string> typeOptions = {"Pizza", "AddOn", "Coldrink"};

static void reloadData() {
    menuItems = getMenuItems(getBranchId());
    categories = getCategories();
}

static void clearInputs() {
    memset(nameInput, 0, sizeof(nameInput));
    memset(descInput, 0, sizeof(descInput));
    memset(priceInput, 0, sizeof(priceInput));
    memset(newPriceInput, 0, sizeof(newPriceInput));
    selectedCategoryIdx = 0;
    selectedTypeIdx = 0;
    catDropdownOpen = false;
    typeDropdownOpen = false;
    activeInputField = 0;
}

static void setStatus(const char* msg, bool ok) {
    statusMessage = msg;
    statusTimer = ok ? 2.5f : 3.0f;
}

void InitMenuManagerPanel() {
    reloadData();
    showAddPopup = showEditPopup = showPriceHistoryPopup = false;
    selectedItemId = -1;
    clearInputs();
    statusMessage = "";
    statusTimer = 0.0f;
    scrollOffset = 0.0f;
}

void DrawMenuManagerPanel() {
    float px = (float)CONTENT_X + 20, py = (float)CONTENT_Y + 20;
    float pw = (float)CONTENT_W - 40, ph = (float)CONTENT_H - 40;

    // Header
    DrawText("MENU ITEMS", (int)px, (int)py, FONT_SIZE_LARGE, COL_DARK);
    if (DrawButton({px + pw - 160, py - 5, 150, 36}, "+ Add New Item", COL_ACCENT, COL_WHITE)) {
        clearInputs();
        showAddPopup = true; showEditPopup = false; showPriceHistoryPopup = false;
    }

    // Table
    float tableY = py + 45;
    std::vector<std::string> headers = {"ID", "Name", "Category", "Type", "Price", "Actions"};
    std::vector<float> cols = {60, 280, 140, 120, 100, 300};
    DrawTableHeader(px, tableY, pw, headers, cols);

    float rowH = 36, areaH = ph - 100, totalH = (float)menuItems.size() * rowH;
    DrawScrollbar({px, tableY + 38, pw, areaH}, totalH, scrollOffset);

    float ry = tableY + 38 - scrollOffset;
    for (size_t i = 0; i < menuItems.size(); i++) {
        float y = ry + (float)i * rowH;
        if (y + rowH < tableY + 38 || y > tableY + 38 + areaH) continue;

        auto& mi = menuItems[i];
        Color rc = (i % 2 == 0) ? COL_CARD : COL_BG;
        if (!mi.isAvailable) rc = Fade(COL_BORDER, 0.2f);

        char pr[32]; snprintf(pr, sizeof(pr), "Rs. %.0f", mi.currentPrice);
        DrawTableRow(px, y, pw, {std::to_string(mi.menuItemId), mi.itemName, mi.categoryName, mi.menuItemType, pr, ""}, cols, rc, false);

        float ax = px; for (int c = 0; c < 5; c++) ax += cols[c];

        if (DrawButton({ax, y + 2, 60, 28}, "Edit", COL_IN_PREP, COL_WHITE)) {
            selectedItemId = mi.menuItemId;
            snprintf(nameInput, sizeof(nameInput), "%s", mi.itemName.c_str());
            snprintf(descInput, sizeof(descInput), "%s", mi.description.c_str());
            snprintf(priceInput, sizeof(priceInput), "%.0f", mi.currentPrice);
            selectedTypeIdx = 0;
            for (int t = 0; t < (int)typeOptions.size(); t++)
                if (typeOptions[t] == mi.menuItemType) selectedTypeIdx = t;
            selectedCategoryIdx = 0;
            for (int c = 0; c < (int)categories.size(); c++)
                if (categories[c].categoryId == mi.categoryId) selectedCategoryIdx = c;
            showEditPopup = true; showAddPopup = false; showPriceHistoryPopup = false;
            catDropdownOpen = typeDropdownOpen = false;
        }

        if (DrawButton({ax + 70, y + 2, 120, 28}, "Price History", COL_COMPLETED, COL_WHITE)) {
            selectedItemId = mi.menuItemId;
            priceHistory = getPriceHistory(selectedItemId);
            memset(newPriceInput, 0, sizeof(newPriceInput));
            showPriceHistoryPopup = true; showAddPopup = false; showEditPopup = false;
        }
    }

    // Status notification
    if (statusTimer > 0)
        DrawNotification(statusMessage, statusMessage.find("Error") != std::string::npos ? COL_LOW_STOCK : COL_COMPLETED, statusTimer);

    // ==================== ADD POPUP (600x420) ====================
    if (showAddPopup) {
        DrawPopupOverlay();
        float pw2 = 600, ph2 = 420, px2 = (WINDOW_WIDTH - pw2) / 2, py2 = (WINDOW_HEIGHT - ph2) / 2;
        DrawCard({px2, py2, pw2, ph2}, "Add New Menu Item", COL_CARD);
        float ix = px2 + 30, iy = py2 + 55;

        auto field = [&](const char* label) { DrawText(label, (int)ix, (int)iy, FONT_SIZE_SMALL, COL_DARK); };
        Rectangle inR = {ix + 150, iy - 5, pw2 - 210, 36};

        field("Item Name:"); DrawInput(inR, "", nameInput, 255, activeInputField == 1, false); iy += 55;
        field("Category:");
        std::vector<std::string> cn; for (auto& c : categories) cn.push_back(c.categoryName);
        DrawDropdown({ix + 150, iy - 5, pw2 - 210, 36}, "", cn, selectedCategoryIdx, catDropdownOpen); iy += 55;
        field("Type:"); DrawDropdown({ix + 150, iy - 5, pw2 - 210, 36}, "", typeOptions, selectedTypeIdx, typeDropdownOpen); iy += 55;
        field("Description:"); DrawInput({ix + 150, iy - 5, pw2 - 210, 36}, "", descInput, 255, activeInputField == 4, false); iy += 55;
        field("Price (PKR):"); DrawInput({ix + 150, iy - 5, pw2 - 210, 36}, "", priceInput, 31, activeInputField == 5, false);

        if (DrawButton({px2 + pw2 - 280, py2 + ph2 - 55, 120, 36}, "Save", COL_ACCENT, COL_WHITE)) {
            int cid = (selectedCategoryIdx < (int)categories.size()) ? categories[selectedCategoryIdx].categoryId : 1;
            double pr = atof(priceInput);
            if (strlen(nameInput) > 0 && pr > 0) {
                bool ok = addMenuItem(nameInput, cid, typeOptions[selectedTypeIdx], descInput, pr);
                setStatus(ok ? "Item added successfully" : "Error adding item", ok);
                reloadData();
                showAddPopup = false; clearInputs();
            }
        }
        if (DrawButton({px2 + pw2 - 150, py2 + ph2 - 55, 120, 36}, "Cancel", COL_BORDER, COL_DARK)) { showAddPopup = false; clearInputs(); }
        if (IsKeyPressed(KEY_ESCAPE)) { showAddPopup = false; clearInputs(); }
    }

    // ==================== EDIT POPUP (600x420) ====================
    if (showEditPopup) {
        DrawPopupOverlay();
        float pw2 = 600, ph2 = 420, px2 = (WINDOW_WIDTH - pw2) / 2, py2 = (WINDOW_HEIGHT - ph2) / 2;
        DrawCard({px2, py2, pw2, ph2}, "Edit Menu Item", COL_CARD);
        float ix = px2 + 30, iy = py2 + 55;

        auto field = [&](const char* label) { DrawText(label, (int)ix, (int)iy, FONT_SIZE_SMALL, COL_DARK); };
        field("Item Name:"); DrawInput({ix + 150, iy - 5, pw2 - 210, 36}, "", nameInput, 255, true, false); iy += 55;
        field("Category:");
        std::vector<std::string> cn; for (auto& c : categories) cn.push_back(c.categoryName);
        DrawDropdown({ix + 150, iy - 5, pw2 - 210, 36}, "", cn, selectedCategoryIdx, catDropdownOpen); iy += 55;
        field("Type:"); DrawDropdown({ix + 150, iy - 5, pw2 - 210, 36}, "", typeOptions, selectedTypeIdx, typeDropdownOpen); iy += 55;
        field("Description:"); DrawInput({ix + 150, iy - 5, pw2 - 210, 36}, "", descInput, 255, true, false); iy += 55;
        field("Price (PKR):"); DrawText(priceInput, (int)ix + 150, (int)iy, FONT_SIZE_SMALL, COL_BORDER);

        if (DrawButton({px2 + pw2 - 280, py2 + ph2 - 55, 120, 36}, "Update", COL_ACCENT, COL_WHITE)) {
            int cid = (selectedCategoryIdx < (int)categories.size()) ? categories[selectedCategoryIdx].categoryId : 1;
            bool ok = updateMenuItem(selectedItemId, nameInput, cid, typeOptions[selectedTypeIdx], descInput);
            setStatus(ok ? "Item updated successfully" : "Error updating item", ok);
            reloadData();
            showEditPopup = false; clearInputs();
        }
        if (DrawButton({px2 + pw2 - 150, py2 + ph2 - 55, 120, 36}, "Cancel", COL_BORDER, COL_DARK)) { showEditPopup = false; clearInputs(); }
        if (IsKeyPressed(KEY_ESCAPE)) { showEditPopup = false; clearInputs(); }
    }

    // ==================== PRICE HISTORY POPUP (600x460) ====================
    if (showPriceHistoryPopup) {
        DrawPopupOverlay();
        float pw2 = 600, ph2 = 460, px2 = (WINDOW_WIDTH - pw2) / 2, py2 = (WINDOW_HEIGHT - ph2) / 2;
        DrawCard({px2, py2, pw2, ph2}, "Price History", COL_CARD);

        float hx = px2 + 20, hy = py2 + 50;
        std::vector<std::string> hd = {"Effective Date", "End Date", "Amount", "Status"};
        std::vector<float> hw = {140, 140, 140, 100};
        DrawTableHeader(hx, hy, pw2 - 40, hd, hw);

        for (size_t i = 0; i < priceHistory.size() && i < 8; i++) {
            float ryy = hy + 38 + (float)i * 32;
            Color rc = priceHistory[i].isActive ? ColorAlphaBlend(COL_CARD, Fade(COL_ACCENT, 0.15f), WHITE) : COL_CARD;
            char amt[32]; snprintf(amt, sizeof(amt), "PKR %.0f", priceHistory[i].amount);
            DrawTableRow(hx, ryy, pw2 - 40, {
                priceHistory[i].effectiveDate,
                priceHistory[i].endDate.empty() ? "Current" : priceHistory[i].endDate,
                amt,
                priceHistory[i].isActive ? "Active" : "Inactive"
            }, hw, rc, false);
        }

        float sy = py2 + ph2 - 70;
        DrawText("Set New Price:", (int)px2 + 20, (int)sy, FONT_SIZE_SMALL, COL_DARK);
        DrawInput({px2 + 160, sy - 5, 150, 36}, "", newPriceInput, 31, true, false);

        if (DrawButton({px2 + 320, sy - 5, 140, 36}, "Update Price", COL_ACCENT, COL_WHITE)) {
            double np = atof(newPriceInput);
            if (np > 0 && selectedItemId > 0) {
                bool ok = setNewPrice(selectedItemId, np);
                setStatus(ok ? "Price updated" : "Error updating price", ok);
                priceHistory = getPriceHistory(selectedItemId);
                reloadData();
                memset(newPriceInput, 0, sizeof(newPriceInput));
            }
        }
        if (DrawButton({px2 + pw2 - 130, sy - 5, 100, 36}, "Close", COL_BORDER, COL_DARK)) showPriceHistoryPopup = false;
        if (IsKeyPressed(KEY_ESCAPE)) showPriceHistoryPopup = false;
    }
}