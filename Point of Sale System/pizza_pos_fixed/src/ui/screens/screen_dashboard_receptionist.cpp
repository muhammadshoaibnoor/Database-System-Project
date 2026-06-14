#include "screen_dashboard_receptionist.h"
#include "raylib.h"
#include "../ui_helpers.h"
#include "../../core/session.h"
#include "../../db/db.h"
#include "../../core/models/models.h"
#include "../panels/panel_order_form.h"
#include "../panels/panel_order_list.h"
#include "../panels/panel_clock.h"
#include <string>
#include <vector>

using namespace DB;

RecepPanel gRecepActivePanel = RPANEL_NEW_ORDER;
static const std::vector<std::string> sidebarItems = {"New Order", "My Orders", "Clock In/Out"};

void InitReceptionistDashboard() {
    gRecepActivePanel = RPANEL_NEW_ORDER;
    InitOrderFormPanel();
    InitOrderListPanel();
    InitClockPanel();
}

void DrawReceptionistDashboard() {
    DrawTopBar("Pizza POS", getBranchName(), getFullName());
    if (DrawLogoutButton((float)WINDOW_WIDTH - 105, 12)) {
        clearSession(); gScreen = SCREEN_LOGIN; return;
    }

    int clickedIdx = -1;
    DrawSidebar(sidebarItems, (int)gRecepActivePanel, clickedIdx);
    if (clickedIdx >= 0) gRecepActivePanel = (RecepPanel)clickedIdx;

    DrawRectangleRec({(float)CONTENT_X, (float)CONTENT_Y, (float)CONTENT_W, (float)CONTENT_H}, COL_BG);

    switch (gRecepActivePanel) {
        case RPANEL_NEW_ORDER: DrawOrderFormPanel(); break;
        case RPANEL_MY_ORDERS: DrawOrderListPanel(); break;
        case RPANEL_CLOCK:     DrawClockPanel();     break;
    }
}