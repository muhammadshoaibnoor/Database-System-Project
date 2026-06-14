#include "panel_deals.h"
#include "raylib.h"
#include "../ui_helpers.h"
#include "../../db/db.h"
#include "../../core/models/models.h"
#include "../../core/session.h"
#include <string>
#include <vector>
#include <cstring>
#include <cstdio>
#include <deque>
using namespace DB;

static std::vector<Deal> deals;
static std::vector<MenuItem> menuItems;
static std::vector<Branch> branches;
static bool showViewPopup = false, showCreatePopup = false, showEditPopup = false;
static int selectedDealId = -1;
static std::deque<bool> itemChecked, branchChecked;
static char dealNameInput[100] = {0}, priceInput[32] = {0};
static float scrollOffset = 0.0f, popupScroll = 0.0f;
static std::string statusMsg = ""; static float statusTimer = 0.0f;

static void reload() { deals = getAllDeals(); menuItems = getMenuItems(getBranchId()); branches = getAllBranches(); }
static void setStatus(const char* m, bool ok) { statusMsg = m; statusTimer = ok ? 2.5f : 3.0f; }

void InitDealsPanel() {
    reload(); showViewPopup = showCreatePopup = showEditPopup = false; selectedDealId = -1;
    itemChecked.clear(); branchChecked.clear();
    memset(dealNameInput, 0, sizeof(dealNameInput)); memset(priceInput, 0, sizeof(priceInput));
    scrollOffset = popupScroll = 0.0f; statusMsg = ""; statusTimer = 0.0f;
}

void DrawDealsPanel() {
    float px = (float)CONTENT_X + 20, py = (float)CONTENT_Y + 20, pw = (float)CONTENT_W - 40, ph = (float)CONTENT_H - 40;
    DrawText("DEALS MANAGEMENT", (int)px, (int)py, FONT_SIZE_LARGE, COL_DARK);
    if (DrawButton({px + pw - 160, py - 5, 150, 36}, "+ Create Deal", COL_ACCENT, COL_WHITE)) {
        memset(dealNameInput, 0, sizeof(dealNameInput)); memset(priceInput, 0, sizeof(priceInput));
        itemChecked.assign(menuItems.size(), false); branchChecked.assign(branches.size(), false);
        showCreatePopup = true; showViewPopup = showEditPopup = false; popupScroll = 0.0f;
    }

    float ty = py + 45;
    std::vector<std::string> hd = {"ID", "Name", "Price", "Branches", "Actions"};
    std::vector<float> cw = {60, 280, 120, 300, 240};
    DrawTableHeader(px, ty, pw, hd, cw);

    float rh = 36, ah = ph - 100, th = (float)deals.size() * rh;
    DrawScrollbar({px, ty + 38, pw, ah}, th, scrollOffset);
    float ry = ty + 38 - scrollOffset;
    for (size_t i = 0; i < deals.size(); i++) {
        float y = ry + (float)i * rh; if (y + rh < ty + 38 || y > ty + 38 + ah) continue;
        auto& d = deals[i]; Color rc = (i % 2 == 0) ? COL_CARD : COL_BG;
        std::string brStr; for (auto& b : d.branchNames) brStr += b + " ";
        char pr[32]; snprintf(pr, sizeof(pr), "PKR %.0f", d.totalAmount);
        DrawTableRow(px, y, pw, {std::to_string(d.dealId), d.dealName, pr, brStr, ""}, cw, rc, false);
        float ax = px; for (int c = 0; c < 4; c++) ax += cw[c];
        if (DrawButton({ax, y + 2, 55, 28}, "View", COL_IN_PREP, COL_WHITE)) { selectedDealId = d.dealId; showViewPopup = true; showCreatePopup = showEditPopup = false; }
        if (DrawButton({ax + 62, y + 2, 55, 28}, "Edit", COL_COMPLETED, COL_WHITE)) {
            selectedDealId = d.dealId; snprintf(dealNameInput, sizeof(dealNameInput), "%s", d.dealName.c_str()); snprintf(priceInput, sizeof(priceInput), "%.0f", d.totalAmount);
            itemChecked.assign(menuItems.size(), false); branchChecked.assign(branches.size(), false);
            for (size_t mi = 0; mi < menuItems.size(); mi++) for (auto& di : d.menuItemIds) if (menuItems[mi].menuItemId == di) itemChecked[mi] = true;
            for (size_t bi = 0; bi < branches.size(); bi++) for (auto& db : d.branchIds) if (branches[bi].branchId == db) branchChecked[bi] = true;
            showEditPopup = true; showViewPopup = showCreatePopup = false; popupScroll = 0.0f;
        }
    }

    if (statusTimer > 0) DrawNotification(statusMsg, statusMsg.find("Error") != std::string::npos ? COL_LOW_STOCK : COL_COMPLETED, statusTimer);

    // VIEW POPUP
    if (showViewPopup) {
        Deal dd = getDealById(selectedDealId); auto di = getDealItems(selectedDealId);
        DrawPopupOverlay();
        float pw2 = 450, ph2 = 350, px2 = (WINDOW_WIDTH - pw2) / 2, py2 = (WINDOW_HEIGHT - ph2) / 2;
        DrawCard({px2, py2, pw2, ph2}, dd.dealName, COL_CARD);
        float iy = py2 + 50;
        DrawText("Items:", (int)px2 + 20, (int)iy, FONT_SIZE_SMALL, COL_DARK); iy += 25;
        for (auto& i : di) { DrawText((i.itemName + " x" + std::to_string(i.quantity)).c_str(), (int)px2 + 30, (int)iy, FONT_SIZE_SMALL, COL_DARK); iy += 22; }
        iy += 10; DrawText("Branches:", (int)px2 + 20, (int)iy, FONT_SIZE_SMALL, COL_DARK); iy += 25;
        for (int bi = 0; bi < (int)branches.size(); bi++) {
            bool avail = false; for (auto& b : dd.branchIds) if (branches[bi].branchId == b) avail = true;
            DrawText((std::string(avail ? "[x] " : "[ ] ") + branches[bi].branchName).c_str(), (int)px2 + 30, (int)iy, FONT_SIZE_SMALL, avail ? COL_COMPLETED : COL_BORDER); iy += 22;
        }
        if (DrawButton({px2 + pw2 - 100, py2 + ph2 - 45, 80, 30}, "Close", COL_BORDER, COL_DARK)) showViewPopup = false;
    }

    // CREATE/EDIT POPUP (shared layout)
    auto drawDealForm = [&](bool isEdit) {
        float pw2 = 550, ph2 = 500, px2 = (WINDOW_WIDTH - pw2) / 2, py2 = (WINDOW_HEIGHT - ph2) / 2;
        DrawCard({px2, py2, pw2, ph2}, isEdit ? "Edit Deal" : "Create Deal", COL_CARD);
        float ix = px2 + 20, iy = py2 + 45;
        DrawText("Deal Name:", (int)ix, (int)iy, FONT_SIZE_SMALL, COL_DARK); DrawInput({ix + 120, iy - 3, pw2 - 170, 32}, "", dealNameInput, 99, true, false); iy += 40;
        DrawText("Price:", (int)ix, (int)iy, FONT_SIZE_SMALL, COL_DARK); DrawInput({ix + 120, iy - 3, pw2 - 170, 32}, "", priceInput, 31, true, false); iy += 40;
        DrawText("Items:", (int)ix, (int)iy, FONT_SIZE_SMALL, COL_DARK); iy += 22;
        float isy = iy; float itemAreaH = 180;
        Rectangle itemClip = {ix, isy, pw2 - 40, itemAreaH};
        float itemTotalH = menuItems.size() * 24.0f;
        DrawScrollbar(itemClip, itemTotalH, popupScroll);
        float iry = isy - popupScroll;
        for (size_t mi = 0; mi < menuItems.size(); mi++) {
            float my = iry + mi * 24.0f; if (my + 24 < isy || my > isy + itemAreaH) continue;
            DrawCheckbox(ix, my, menuItems[mi].itemName, itemChecked[mi]);
        }
        iy = isy + itemAreaH + 10;
        DrawText("Branches:", (int)ix, (int)iy, FONT_SIZE_SMALL, COL_DARK); iy += 22;
        for (size_t bi = 0; bi < branches.size(); bi++) { DrawCheckbox(ix, iy, branches[bi].branchName, branchChecked[bi]); iy += 22; }
        iy = py2 + ph2 - 50;
        if (DrawButton({px2 + 20, iy, 120, 36}, "Save", COL_ACCENT, COL_WHITE)) {
            std::vector<std::pair<int,int>> selItems; for (size_t mi = 0; mi < menuItems.size(); mi++) if (itemChecked[mi]) selItems.push_back({menuItems[mi].menuItemId, 1});
            std::vector<int> selBranches; for (size_t bi = 0; bi < branches.size(); bi++) if (branchChecked[bi]) selBranches.push_back(branches[bi].branchId);
            bool ok = isEdit ? updateDeal(selectedDealId, dealNameInput, atof(priceInput), selItems, selBranches) : createDeal(dealNameInput, atof(priceInput), selItems, selBranches);
            setStatus(ok ? "Deal saved" : "Error", ok); reload();
            if (isEdit) showEditPopup = false; else showCreatePopup = false;
        }
        if (DrawButton({px2 + pw2 - 140, iy, 120, 36}, "Cancel", COL_BORDER, COL_DARK)) { if (isEdit) showEditPopup = false; else showCreatePopup = false; }
        if (IsKeyPressed(KEY_ESCAPE)) { if (isEdit) showEditPopup = false; else showCreatePopup = false; }
    };

    if (showCreatePopup) { DrawPopupOverlay(); drawDealForm(false); }
    if (showEditPopup) { DrawPopupOverlay(); drawDealForm(true); }
}