#include "panel_reports.h"
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

static int activeTab = 0;
static std::vector<Branch> branches;
static int branchIdx = 0;
static char dateFrom[20] = {0}, dateTo[20] = {0};
static bool branchDropOpen = false;
static bool generated = false;
static int activeField = 0; // 1=dateFrom, 2=dateTo

struct SalesRep { int totalOrders; double totalRevenue, totalDiscounts, netRevenue; std::vector<std::pair<std::string,int>> topItems; };
struct InvRepItem { std::string itemName, unit; double quantity, reorderLevel; bool isLow; };
struct StaffRepItem { std::string username, fullName, role; int totalShifts; double totalHours; };

static SalesRep salesData;
static std::vector<InvRepItem> invData;
static std::vector<StaffRepItem> staffData;

void InitReportsPanel() {
    branches = getAllBranches(); branchIdx = 0; activeTab = 0;
    memset(dateFrom, 0, sizeof(dateFrom)); memset(dateTo, 0, sizeof(dateTo));
    generated = false; branchDropOpen = false; activeField = 0;
}

void DrawReportsPanel() {
    float px = (float)CONTENT_X + 20, py = (float)CONTENT_Y + 20, pw = (float)CONTENT_W - 40;
    DrawText("REPORTS", (int)px, (int)py, FONT_SIZE_LARGE, COL_DARK);

    Vector2 mouse = GetMousePosition();

    // Branch dropdown
    std::vector<std::string> brNames = {"All Branches"};
    for (auto& b : branches) brNames.push_back(b.branchName);
    DrawDropdown({px, py + 35, 220, 36}, "Branch:", brNames, branchIdx, branchDropOpen);

    // Date From
    DrawText("From:", (int)px + 240, (int)py + 40, FONT_SIZE_SMALL, COL_DARK);
    Rectangle fromRect = {px + 290, py + 35, 110, 32};
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, fromRect)) activeField = 1;
    DrawInput(fromRect, "YYYY-MM-DD", dateFrom, 19, activeField == 1, false);

    // Date To
    DrawText("To:", (int)px + 420, (int)py + 40, FONT_SIZE_SMALL, COL_DARK);
    Rectangle toRect = {px + 450, py + 35, 110, 32};
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, toRect)) activeField = 2;
    DrawInput(toRect, "YYYY-MM-DD", dateTo, 19, activeField == 2, false);

    // Tab between date fields
    if (IsKeyPressed(KEY_TAB)) activeField = (activeField % 2) + 1;

    // Generate button
    if (DrawButton({px + 580, py + 35, 140, 32}, "Generate Report", COL_ACCENT, COL_WHITE) || IsKeyPressed(KEY_ENTER)) {
        int bid = (branchIdx > 0 && branchIdx - 1 < (int)branches.size()) ? branches[branchIdx - 1].branchId : -1;

        // Fetch data
        auto orders = getAllOrders(bid, dateFrom, dateTo);
        salesData.totalOrders = 0; salesData.totalRevenue = 0; salesData.totalDiscounts = 0;
        salesData.topItems.clear();
        std::vector<std::pair<std::string,int>> counts;
        for (auto& o : orders) {
            if (o.status == "Cancelled") continue;
            salesData.totalOrders++; salesData.totalRevenue += o.totalAmount; salesData.totalDiscounts += o.discountAmount;
            for (auto& oi : getOrderItems(o.orderId)) {
                std::string nm = oi.itemName.empty() ? oi.dealName : oi.itemName;
                if (nm.empty()) nm = "Item #" + std::to_string(oi.menuItemId);
                bool f = false; for (auto& p : counts) { if (p.first == nm) { p.second += oi.quantity; f = true; break; } }
                if (!f) counts.push_back({nm, oi.quantity});
            }
        }
        salesData.netRevenue = salesData.totalRevenue - salesData.totalDiscounts;
        for (size_t i = 0; i < counts.size(); i++)
            for (size_t j = i + 1; j < counts.size(); j++)
                if (counts[j].second > counts[i].second) std::swap(counts[i], counts[j]);
        salesData.topItems = counts;

        invData.clear();
        auto inv = getInventory(bid > 0 ? bid : getBranchId());
        for (auto& i : inv) invData.push_back({i.inventoryName, i.unit, i.quantity, i.reorderLevel, i.isLowStock});

        staffData.clear();
        auto emps = getAllEmployees(bid);
        for (auto& e : emps) {
            auto sh = getMyShifts(e.username); double hrs = 0; for (auto& s : sh) hrs += s.hours;
            staffData.push_back({e.username, e.firstName + " " + e.lastName, e.role, (int)sh.size(), hrs});
        }
        generated = true;
    }

    // Tabs
    float ty = py + 80;
    const char* tabs[] = {"Sales", "Inventory", "Staff"};
    for (int t = 0; t < 3; t++) {
        Rectangle tr = {px + t * 110.0f, ty, 100, 32};
        if (DrawButton(tr, tabs[t], activeTab == t ? COL_ACCENT : COL_CARD, activeTab == t ? COL_WHITE : COL_DARK)) activeTab = t;
    }

    if (!generated) {
        DrawText("Select branch, date range, and click Generate Report", (int)px, (int)(ty + 50), FONT_SIZE_NORMAL, COL_BORDER);
        return;
    }

    float cy = ty + 45;
    if (activeTab == 0) {
        float cw2 = (pw - 40) / 4.0f;
        auto sc = [&](Rectangle r, const char* l, const char* v, Color c) {
            DrawCard(r, "", COL_CARD);
            DrawText(l, (int)r.x + 10, (int)r.y + 10, FONT_SIZE_SMALL, COL_BORDER);
            float tw = (float)MeasureText(v, FONT_SIZE_TITLE);
            DrawText(v, (int)(r.x + (r.width - tw) / 2), (int)(r.y + 30), FONT_SIZE_TITLE, c);
        };
        char b1[32], b2[32], b3[32], b4[32];
        snprintf(b1, sizeof(b1), "%d", salesData.totalOrders);
        snprintf(b2, sizeof(b2), "%.0f", salesData.totalRevenue);
        snprintf(b3, sizeof(b3), "%.0f", salesData.totalDiscounts);
        snprintf(b4, sizeof(b4), "%.0f", salesData.netRevenue);
        sc({px, cy, cw2, 80}, "Total Orders", b1, COL_ACCENT);
        sc({px + cw2 + 10, cy, cw2, 80}, "Revenue (PKR)", b2, COL_COMPLETED);
        sc({px + (cw2 + 10) * 2, cy, cw2, 80}, "Discounts (PKR)", b3, COL_PENDING);
        sc({px + (cw2 + 10) * 3, cy, cw2, 80}, "Net Revenue (PKR)", b4, COL_ACCENT);
        cy += 95;
        DrawText("Top Items:", (int)px, (int)cy, FONT_SIZE_NORMAL, COL_DARK); cy += 25;
        int maxQ = 1; for (auto& ti : salesData.topItems) if (ti.second > maxQ) maxQ = ti.second;
        for (size_t i = 0; i < salesData.topItems.size() && i < 8; i++) {
            float bw = (maxQ > 0) ? ((pw - 200) * salesData.topItems[i].second / maxQ) : 10;
            if (bw < 10) bw = 10;
            DrawText((std::to_string(i + 1) + ". " + salesData.topItems[i].first).c_str(), (int)px, (int)cy, FONT_SIZE_SMALL, COL_DARK);
            DrawRectangleRounded({px + 200, cy + 2, bw, 18}, 0.2f, 8, COL_ACCENT);
            char qs[16]; snprintf(qs, sizeof(qs), "%d", salesData.topItems[i].second);
            DrawText(qs, (int)(px + 205 + bw), (int)cy, FONT_SIZE_SMALL, COL_DARK);
            cy += 24;
        }
    } else if (activeTab == 1) {
        std::vector<std::string> hd = {"Item", "Unit", "Quantity", "Reorder Lvl", "Status"};
        std::vector<float> cw = {250, 100, 120, 120, 120};
        DrawTableHeader(px, cy, pw, hd, cw);
        for (size_t i = 0; i < invData.size(); i++) {
            float y = cy + 38 + i * 32.0f; auto& it = invData[i];
            Color rc = it.isLow ? ColorAlphaBlend(COL_CARD, Fade(COL_LOW_STOCK, 0.2f), WHITE) : ((i % 2 == 0) ? COL_CARD : COL_BG);
            char qs[32], rs[32]; snprintf(qs, sizeof(qs), "%.1f", it.quantity); snprintf(rs, sizeof(rs), "%.1f", it.reorderLevel);
            DrawTableRow(px, y, pw, {it.itemName, it.unit, qs, rs, it.isLow ? "LOW" : "OK"}, cw, rc, false);
        }
    } else {
        std::vector<std::string> hd = {"Staff", "Role", "Shifts", "Total Hours"};
        std::vector<float> cw = {250, 180, 120, 150};
        DrawTableHeader(px, cy, pw, hd, cw);
        for (size_t i = 0; i < staffData.size(); i++) {
            float y = cy + 38 + i * 32.0f; auto& s = staffData[i]; Color rc = (i % 2 == 0) ? COL_CARD : COL_BG;
            char shs[16], hrs[16]; snprintf(shs, sizeof(shs), "%d", s.totalShifts); snprintf(hrs, sizeof(hrs), "%.1fh", s.totalHours);
            DrawTableRow(px, y, pw, {s.fullName, s.role, shs, hrs}, cw, rc, false);
        }
    }
}