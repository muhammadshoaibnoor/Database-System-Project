#include "panel_kitchen_queue.h"
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

struct OrderCard {
    Order order;
    std::vector<OrderItem> items;
};

static std::vector<OrderCard> orderCards;
static int activeCount = 0;
static float scrollOffset = 0.0f;
static std::string statusMsg = "";
static float statusTimer = 0.0f;

void RefreshKitchenQueue() {
    orderCards.clear();
    activeCount = 0;
    auto orders = getOrdersByKitchenStaff(getUsername(), "");
    for (auto& o : orders) {
        // Skip cancelled orders
        if (o.status == "Cancelled") continue;
        OrderCard oc;
        oc.order = o;
        oc.items = getOrderItems(o.orderId);
        orderCards.push_back(oc);
        // Count active (Pending or In Preparation)
        if (o.status == "Pending" || o.status == "In Preparation") activeCount++;
    }
}

static void setStatus(const char* m, bool ok) { statusMsg = m; statusTimer = ok ? 2.5f : 3.5f; }

void InitKitchenQueuePanel() {
    RefreshKitchenQueue();
    scrollOffset = 0.0f;
    statusMsg = ""; statusTimer = 0.0f;
}

void DrawKitchenQueuePanel() {
    float px = (float)CONTENT_X + 20, py = (float)CONTENT_Y + 20, pw = (float)CONTENT_W - 40, ph = (float)CONTENT_H - 40;

    const char* title = "KITCHEN DASHBOARD";
    float tw = (float)MeasureText(title, FONT_SIZE_TITLE);
    DrawText(title, (int)(CONTENT_X + (CONTENT_W - tw) / 2), (int)py, FONT_SIZE_TITLE, COL_ACCENT);

    char badge[32]; snprintf(badge, sizeof(badge), "Active Orders: %d", activeCount);
    float bw = (float)MeasureText(badge, FONT_SIZE_SMALL);
    DrawRectangleRounded({px + pw - bw - 30, py + 5, bw + 20, 28}, 0.2f, 8, activeCount > 0 ? COL_LOW_STOCK : COL_COMPLETED);
    DrawText(badge, (int)(px + pw - bw - 20), (int)py + 9, FONT_SIZE_SMALL, COL_WHITE);

    if (DrawButton({px + pw - 100, py + 40, 90, 28}, "Refresh", COL_ACCENT, COL_WHITE)) RefreshKitchenQueue();

    float cy = py + 80;
    float areaH = ph - 90;
    float cardH = 135;
    float totalH = orderCards.size() * (cardH + 10);
    DrawScrollbar({px, cy, pw, areaH}, totalH, scrollOffset);

    float ry = cy - scrollOffset;
    for (size_t i = 0; i < orderCards.size(); i++) {
        float y = ry + i * (cardH + 10);
        if (y + cardH < cy || y > cy + areaH) continue;

        auto& oc = orderCards[i];
        auto& o = oc.order;

        // Normalize status for display
        std::string st = o.status;

        Color cardBg = COL_CARD;
        if (st == "Pending") cardBg = ColorAlphaBlend(COL_CARD, Fade(COL_PENDING, 0.08f), WHITE);
        else if (st == "In Preparation") cardBg = ColorAlphaBlend(COL_CARD, Fade(COL_IN_PREP, 0.08f), WHITE);

        DrawCard({px, y, pw, cardH}, "", cardBg);

        char hdr[128]; snprintf(hdr, sizeof(hdr), "Order #%d", o.orderId);
        DrawText(hdr, (int)px + 15, (int)y + 8, FONT_SIZE_NORMAL, COL_DARK);
        char info[128]; snprintf(info, sizeof(info), "Taken by: %s | %s", o.takenBy.c_str(), o.orderDateTime.c_str());
        DrawText(info, (int)px + 15, (int)y + 30, FONT_SIZE_SMALL, COL_BORDER);

        DrawStatusBadge(px + pw - 160, y + 8, st);

        std::string itemsStr = "Items: ";
        for (size_t j = 0; j < oc.items.size(); j++) {
            std::string nm = oc.items[j].itemName.empty() ? oc.items[j].dealName : oc.items[j].itemName;
            if (!nm.empty()) {
                itemsStr += nm + " x" + std::to_string(oc.items[j].quantity);
                if (j < oc.items.size() - 1) itemsStr += ", ";
            }
        }
        if (itemsStr.length() > 100) itemsStr = itemsStr.substr(0, 97) + "...";
        DrawText(itemsStr.c_str(), (int)px + 15, (int)y + 55, FONT_SIZE_SMALL, COL_DARK);

        float btnY = y + cardH - 38;

        if (st == "Pending") {
            if (DrawButton({px + 15, btnY, 150, 32}, "Accept Order", COL_PENDING, COL_WHITE)) {
                bool ok = updateOrderStatus(o.orderId, "In Preparation");
                setStatus(ok ? "Order accepted!" : "Error", ok);
                RefreshKitchenQueue();
            }
        } else if (st == "In Preparation") {
            if (DrawButton({px + 15, btnY, 180, 32}, "Mark as Complete", COL_COMPLETED, COL_WHITE)) {
                bool ok = updateOrderStatus(o.orderId, "Completed");
                setStatus(ok ? "Order completed!" : "Error", ok);
                RefreshKitchenQueue();
            }
        } else if (st == "Completed") {
            DrawText("Completed", (int)px + 15, (int)btnY + 5, FONT_SIZE_SMALL, COL_COMPLETED);
        }
    }

    if (orderCards.empty()) {
        DrawText("No orders assigned yet.", (int)px + 20, (int)cy + 20, FONT_SIZE_NORMAL, COL_BORDER);
    }

    if (statusTimer > 0)
        DrawNotification(statusMsg, statusMsg.find("Error") != std::string::npos ? COL_LOW_STOCK : COL_COMPLETED, statusTimer);
}