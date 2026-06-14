#include "panel_order_list.h"
#include "raylib.h"
#include "../ui_helpers.h"
#include "../../db/db.h"
#include "../../core/models/models.h"
#include "../../core/session.h"
#include "panel_order_form.h"
#include "../screens/screen_dashboard_receptionist.h"
#include <string>
#include <vector>
#include <cstring>
#include <cstdio>

using namespace DB;

static std::vector<Order> orders;
static int statusFilter = 0;
static float scrollOffset = 0.0f;
static bool showCancelDlg = false, showViewPopup = false;
static int cancelOrderId = -1, viewOrderId = -1;
static std::vector<OrderItem> viewItems;
static std::string statusMsg = ""; static float statusTimer = 0.0f;

static void reload() {
    const char* sf[] = {"", "Pending", "In Preparation", "Completed", "Cancelled"};
    orders = getOrdersByReceptionist(getUsername(), sf[statusFilter]);
}
static void setStatus(const char* m, bool ok) { statusMsg = m; statusTimer = ok ? 2.5f : 3.5f; }

void InitOrderListPanel() { reload(); statusFilter=0; scrollOffset=0.0f; showCancelDlg=showViewPopup=false; cancelOrderId=viewOrderId=-1; statusMsg=""; statusTimer=0.0f; }

void DrawOrderListPanel() {
    float px=(float)CONTENT_X+20, py=(float)CONTENT_Y+20, pw=(float)CONTENT_W-40, ph=(float)CONTENT_H-40;
    DrawText("MY ORDERS",(int)px,(int)py,FONT_SIZE_LARGE,COL_DARK);
    const char* flt[]={"All","Pending","In Prep","Completed","Cancelled"};
    for(int f=0;f<5;f++) if(DrawButton({px+f*105.0f,py+30,95,30},flt[f],statusFilter==f?COL_ACCENT:COL_CARD,statusFilter==f?COL_WHITE:COL_DARK)){statusFilter=f;reload();}
    if(DrawButton({px+pw-90,py+30,80,30},"Refresh",COL_ACCENT,COL_WHITE))reload();

    float cy=py+70,areaH=ph-80,totalH=orders.size()*140.0f;
    DrawScrollbar({px,cy,pw,areaH},totalH,scrollOffset);
    float ry=cy-scrollOffset;
    for(size_t i=0;i<orders.size();i++){
        float y=ry+i*140.0f; if(y+130<cy||y>cy+areaH)continue;
        auto&o=orders[i]; DrawCard({px,y,pw,125},"",(i%2==0)?COL_CARD:COL_BG);
        char hdr[128];snprintf(hdr,sizeof(hdr),"Order #%d | %s | %s",o.orderId,o.customerName.c_str(),o.orderDateTime.c_str());
        DrawText(hdr,(int)px+15,(int)y+8,FONT_SIZE_SMALL,COL_DARK);
        char tot[32];snprintf(tot,sizeof(tot),"PKR %.0f",o.totalAmount);
        DrawText(tot,(int)px+15,(int)y+30,FONT_SIZE_NORMAL,COL_ACCENT);
        DrawStatusBadge(px+pw-180,y+8,o.status);
        float by=y+125-40;

        // Normalize status for comparison (handle PENDING vs Pending)
        std::string st = o.status;
        if (st == "Pending" || st == "PENDING") {
            if(DrawButton({px+15,by,90,30},"Modify",COL_PENDING,COL_WHITE)){SetOrderFormForModification(o.orderId);gRecepActivePanel=RPANEL_NEW_ORDER;}
            if(DrawButton({px+115,by,80,30},"Cancel",COL_LOW_STOCK,COL_WHITE)){cancelOrderId=o.orderId;showCancelDlg=true;}
        } else if (st == "In Preparation" || st == "IN_PREPARATION") {
            DrawText("Locked",(int)px+15,(int)by+5,FONT_SIZE_SMALL,COL_BORDER);
        } else if (st == "Completed" || st == "COMPLETED") {
            if(DrawButton({px+15,by,120,30},"View Details",COL_BORDER,COL_DARK)){viewOrderId=o.orderId;viewItems=getOrderItems(o.orderId);showViewPopup=true;}
        } else {
            DrawText("Refunded",(int)px+15,(int)by+5,FONT_SIZE_SMALL,COL_LOW_STOCK);
        }
    }

    if(statusTimer>0)DrawNotification(statusMsg,statusMsg.find("Error")!=std::string::npos?COL_LOW_STOCK:COL_COMPLETED,statusTimer);
    if(showCancelDlg){bool sd=true;if(DrawConfirmDialog("Cancel","Cancel Order #"+std::to_string(cancelOrderId)+"?",sd)){setStatus(cancelOrder(cancelOrderId)?"Cancelled":"Error",true);reload();}showCancelDlg=sd;}
    if(showViewPopup){DrawPopupOverlay();float vw=500,vh=350,vx=(WINDOW_WIDTH-vw)/2,vy=(WINDOW_HEIGHT-vh)/2;
        DrawCard({vx,vy,vw,vh},"Order #"+std::to_string(viewOrderId),COL_CARD);
        float iy=vy+50;std::vector<std::string>hd={"Item","Qty","Price"};std::vector<float>cw={280,60,100};
        DrawTableHeader(vx+20,iy,vw-40,hd,cw);iy+=38;
        for(size_t k=0;k<viewItems.size();k++){auto&oi=viewItems[k];std::string nm=oi.itemName.empty()?oi.dealName:oi.itemName;
            char pr[32];snprintf(pr,sizeof(pr),"PKR %.0f",oi.unitPrice);
            DrawTableRow(vx+20,iy+k*32.0f,vw-40,{nm,std::to_string(oi.quantity),pr},cw,COL_CARD,false);}
        if(DrawButton({vx+vw-100,vy+vh-45,80,30},"Close",COL_BORDER,COL_DARK))showViewPopup=false;}
}