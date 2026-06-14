#include "screen_dashboard_admin.h"
#include "raylib.h"
#include "../ui_helpers.h"
#include "../../core/session.h"
#include "../../db/db.h"
#include "../../core/models/models.h"

#include "../panels/panel_menu_manager.h"
#include "../panels/panel_inventory.h"
#include "../panels/panel_staff.h"
#include "../panels/panel_salary.h"
#include "../panels/panel_deals.h"
#include "../panels/panel_discounts.h"
#include "../panels/panel_branches.h"
#include "../panels/panel_shifts.h"
#include "../panels/panel_reports.h"

#include <string>
#include <ctime>
#include <vector>
#include <algorithm>
#include <cstdio>

using namespace DB;

static AdminPanel activePanel = PANEL_MENU;
static bool panelInited[10] = {false};

static const std::vector<std::string> sidebarItems = {
    "Overview", "Menu Items", "Inventory", "Staff", "Salary",
    "Deals", "Discounts", "Branches", "Shifts", "Reports"
};

static std::string getTodayDateStr() {
    time_t now = time(0); struct tm t; char buf[11];
    t = *localtime(&now); strftime(buf, sizeof(buf), "%Y-%m-%d", &t);
    return std::string(buf);
}

static void DrawDashboardOverview() {
    float x = (float)CONTENT_X + 20, y = (float)CONTENT_Y + 20, w = (float)CONTENT_W - 40;
    DrawText("Dashboard Overview", (int)x, (int)y, FONT_SIZE_LARGE, COL_DARK);

    auto orders = getAllOrders(getBranchId(), getTodayDateStr(), getTodayDateStr());
    int totalOrders = 0; double totalRevenue = 0.0, totalDiscounts = 0.0;
    for (auto& o : orders) {
        if (o.status != "Cancelled") {
            totalOrders++; totalRevenue += o.totalAmount; totalDiscounts += o.discountAmount;
        }
    }
    double netRevenue = totalRevenue - totalDiscounts;
    float cardW = (w - 40) / 3.0f, cardH = 100.0f, cardY = y + 40;

    auto drawCard = [](Rectangle r, const char* label, const char* value, Color valCol) {
        DrawCard(r, "", COL_CARD);
        DrawText(label, (int)r.x + 15, (int)r.y + 15, FONT_SIZE_SMALL, COL_BORDER);
        float tw = (float)MeasureText(value, FONT_SIZE_TITLE);
        DrawText(value, (int)(r.x + (r.width - tw) / 2), (int)(r.y + 40), FONT_SIZE_TITLE, valCol);
    };

    char b1[32], b2[32], b3[32], b4[32];
    snprintf(b1, sizeof(b1), "%d", totalOrders);
    snprintf(b2, sizeof(b2), "%.0f", totalRevenue);
    snprintf(b3, sizeof(b3), "%.0f", totalDiscounts);
    snprintf(b4, sizeof(b4), "PKR %.0f", netRevenue);

    drawCard({x, cardY, cardW, cardH}, "Total Orders Today", b1, COL_ACCENT);
    drawCard({x + cardW + 20, cardY, cardW, cardH}, "Revenue (PKR)", b2, COL_COMPLETED);
    drawCard({x + (cardW + 20) * 2, cardY, cardW, cardH}, "Discounts (PKR)", b3, COL_PENDING);

    float row2Y = cardY + cardH + 20, halfW = (w - 20) / 2.0f;
    drawCard({x, row2Y, halfW, cardH}, "Net Revenue Today", b4, COL_ACCENT);

    Rectangle r5 = {x + halfW + 20, row2Y, halfW, cardH + 120};
    DrawCard(r5, "Top Items Today", COL_CARD);

    std::vector<std::pair<std::string, int>> counts;
    for (auto& o : orders) {
        if (o.status == "Cancelled") continue;
        for (auto& oi : getOrderItems(o.orderId)) {
            std::string nm = oi.itemName.empty() ? oi.dealName : oi.itemName;
            if (nm.empty()) nm = "Item #" + std::to_string(oi.menuItemId);
            bool f = false;
            for (auto& p : counts) { if (p.first == nm) { p.second += oi.quantity; f = true; break; } }
            if (!f) counts.push_back({nm, oi.quantity});
        }
    }
    for (size_t i = 0; i < counts.size(); i++)
        for (size_t j = i + 1; j < counts.size(); j++)
            if (counts[j].second > counts[i].second) std::swap(counts[i], counts[j]);

    int maxQ = 1; for (auto& p : counts) if (p.second > maxQ) maxQ = p.second;
    float barMaxW = halfW - 40;
    for (int i = 0; i < (int)counts.size() && i < 5; i++) {
        float barW = (maxQ > 0) ? (barMaxW * counts[i].second / maxQ) : 10.0f;
        if (barW < 10.0f) barW = 10.0f;
        float by = row2Y + 45.0f + i * 28.0f;
        DrawText(counts[i].first.c_str(), (int)r5.x + 10, (int)by, FONT_SIZE_SMALL, COL_DARK);
        DrawRectangleRounded({r5.x + 130, by + 2, barW, 18}, 0.2f, 8, COL_ACCENT);
        char qs[16]; snprintf(qs, sizeof(qs), "%d", counts[i].second);
        DrawText(qs, (int)(r5.x + 135 + barW), (int)by, FONT_SIZE_SMALL, COL_DARK);
    }
}

void InitAdminDashboard() {
    activePanel = PANEL_MENU;
    for (int i = 0; i < 10; i++) panelInited[i] = false;
}

void DrawAdminDashboard() {
    DrawTopBar("Pizza POS", getBranchName(), getFullName());
    if (DrawLogoutButton((float)WINDOW_WIDTH - 105, 12)) { clearSession(); gScreen = SCREEN_LOGIN; return; }

    int clickedIdx = -1;
    DrawSidebar(sidebarItems, (int)activePanel, clickedIdx);
    if (clickedIdx >= 0) activePanel = (AdminPanel)clickedIdx;

    DrawRectangleRec({(float)CONTENT_X, (float)CONTENT_Y, (float)CONTENT_W, (float)CONTENT_H}, COL_BG);

    int idx = (int)activePanel;
    if (!panelInited[idx]) {
        switch (activePanel) {
            case PANEL_MENU: InitMenuManagerPanel(); break;
            case PANEL_INVENTORY: InitInventoryPanel(); break;
            case PANEL_STAFF: InitStaffPanel(); break;
            case PANEL_SALARY: InitSalaryPanel(); break;
            case PANEL_DEALS: InitDealsPanel(); break;
            case PANEL_DISCOUNTS: InitDiscountsPanel(); break;
            case PANEL_BRANCHES: InitBranchesPanel(); break;
            case PANEL_SHIFTS: InitShiftsPanel(); break;
            case PANEL_REPORTS: InitReportsPanel(); break;
            default: break;
        }
        panelInited[idx] = true;
    }

    switch (activePanel) {
        case 0: DrawDashboardOverview(); break;
        case PANEL_MENU: DrawMenuManagerPanel(); break;
        case PANEL_INVENTORY: DrawInventoryPanel(); break;
        case PANEL_STAFF: DrawStaffPanel(); break;
        case PANEL_SALARY: DrawSalaryPanel(); break;
        case PANEL_DEALS: DrawDealsPanel(); break;
        case PANEL_DISCOUNTS: DrawDiscountsPanel(); break;
        case PANEL_BRANCHES: DrawBranchesPanel(); break;
        case PANEL_SHIFTS: DrawShiftsPanel(); break;
        case PANEL_REPORTS: DrawReportsPanel(); break;
        default: break;
    }
}