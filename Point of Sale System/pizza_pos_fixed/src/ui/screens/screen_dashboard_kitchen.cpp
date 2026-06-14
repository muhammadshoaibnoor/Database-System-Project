#include "screen_dashboard_kitchen.h"
#include "raylib.h"
#include "../ui_helpers.h"
#include "../../core/session.h"
#include "../../db/db.h"
#include "../../core/models/models.h"
#include "../panels/panel_kitchen_queue.h"
#include "../panels/panel_clock.h"
#include <string>
#include <vector>

using namespace DB;

static KitchenPanel activePanel = KPANEL_QUEUE;
static float refreshTimer = 10.0f;
static const std::vector<std::string> sidebarItems = {"My Orders", "Clock In/Out"};

void InitKitchenDashboard() {
    activePanel = KPANEL_QUEUE;
    refreshTimer = 10.0f;
    InitKitchenQueuePanel();
    InitClockPanel();
}

void DrawKitchenDashboard() {
    DrawTopBar("Pizza POS", getBranchName(), getFullName());
    if (DrawLogoutButton((float)WINDOW_WIDTH - 105, 12)) {
        clearSession(); gScreen = SCREEN_LOGIN; return;
    }

    int clickedIdx = -1;
    DrawSidebar(sidebarItems, (int)activePanel, clickedIdx);
    if (clickedIdx >= 0) activePanel = (KitchenPanel)clickedIdx;

    DrawRectangleRec({(float)CONTENT_X, (float)CONTENT_Y, (float)CONTENT_W, (float)CONTENT_H}, COL_BG);

    // Auto-refresh queue every 10 seconds
    if (activePanel == KPANEL_QUEUE) {
        refreshTimer -= GetFrameTime();
        if (refreshTimer <= 0.0f) {
            RefreshKitchenQueue();
            refreshTimer = 10.0f;
        }
    }

    switch (activePanel) {
        case KPANEL_QUEUE: DrawKitchenQueuePanel(); break;
        case KPANEL_CLOCK: DrawClockPanel();        break;
    }
}