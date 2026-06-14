#include "panel_order_form.h"
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

static std::vector<MenuItem> allItems, pizzas, addons, colddrinks;
static std::vector<Deal> deals;
static std::vector<Discount> activeDiscounts;
static std::vector<CartItem> cart;
static int selectedDiscountIdx = -1;
static int categoryTab = 0;
static bool isModifyMode = false;
static int modifyOrderId = -1;
static char customerName[256] = {0};
static bool discountDropOpen = false;
static float leftScroll = 0.0f, rightScroll = 0.0f;
static std::string statusMsg = ""; static float statusTimer = 0.0f;

static void reloadMenu() {
    int bid = getBranchId();
    allItems = getMenuItems(bid);
    pizzas.clear(); addons.clear(); colddrinks.clear();
    for (auto& mi : allItems) {
        if (mi.menuItemType == "Pizza") pizzas.push_back(mi);
        else if (mi.menuItemType == "AddOn") addons.push_back(mi);
        else colddrinks.push_back(mi);
    }
    deals = getAvailableDeals(bid);
    activeDiscounts = getActiveDiscounts();
}

static void setStatus(const char* m, bool ok) { statusMsg = m; statusTimer = ok ? 3.0f : 3.5f; }
static std::vector<MenuItem>& curList() { switch(categoryTab){case 1:return pizzas;case 2:return addons;case 3:return colddrinks;default:return allItems;} }

static void addItem(MenuItem& mi) {
    for (auto& c : cart) { if (!c.isDeal && c.menuItemId == mi.menuItemId) { c.quantity++; return; } }
    cart.push_back({mi.menuItemId, -1, 1, mi.itemName, "", mi.currentPrice, false, (mi.menuItemType == "AddOn")});
}
static void addDeal(Deal& d) {
    for (auto& c : cart) { if (c.isDeal && c.dealId == d.dealId) { c.quantity++; return; } }
    cart.push_back({-1, d.dealId, 1, "", d.dealName, d.totalAmount, true, false});
}
static double subtotal() { double s=0; for(auto&c:cart) s+=c.unitPrice*c.quantity; return s; }
static double discountAmt() {
    if(selectedDiscountIdx>=0&&selectedDiscountIdx<(int)activeDiscounts.size())
        return subtotal()*activeDiscounts[selectedDiscountIdx].discountValue/100.0;
    return 0;
}
static double total() { return subtotal()-discountAmt(); }

void InitOrderFormPanel() {
    reloadMenu(); cart.clear(); selectedDiscountIdx=-1; categoryTab=0;
    isModifyMode=false; modifyOrderId=-1; memset(customerName,0,sizeof(customerName));
    discountDropOpen=false; leftScroll=rightScroll=0.0f; statusMsg=""; statusTimer=0.0f;
}

void SetOrderFormForModification(int orderId) {
    InitOrderFormPanel(); isModifyMode=true; modifyOrderId=orderId;
    Order o = getOrderById(orderId);
    snprintf(customerName,sizeof(customerName),"%s",o.customerName.c_str());
    auto items = getOrderItems(orderId); cart.clear();
    for(auto& oi : items) cart.push_back({oi.menuItemId, oi.dealId, oi.quantity, oi.itemName, oi.dealName, oi.unitPrice, (oi.dealId>0), false});
    if(o.discountId>0) for(int i=0;i<(int)activeDiscounts.size();i++)
        if(activeDiscounts[i].discountId==o.discountId){selectedDiscountIdx=i;break;}
}

void DrawOrderFormPanel() {
    float lx=(float)CONTENT_X+10, ly=(float)CONTENT_Y+10, lw=640, lh=(float)CONTENT_H-20;
    float rx=lx+lw+10, ry=ly, rw=(float)CONTENT_W-lw-30, rh=lh;

    DrawText("Customer:",(int)lx,(int)ly,FONT_SIZE_SMALL,COL_DARK);
    DrawInput({lx+100,ly-3,250,32},"",customerName,255,true,false);
    float tabY=ly+40;
    const char* tabs[]={"All","Pizza","AddOn","Coldrink","Deals"};
    for(int t=0;t<5;t++)
        if(DrawButton({lx+t*90.0f,tabY,82,30},tabs[t],categoryTab==t?COL_ACCENT:COL_CARD,categoryTab==t?COL_WHITE:COL_DARK)) categoryTab=t;

    float gridY=tabY+38, gridH=lh-120;
    if(categoryTab==4){
        float th=deals.size()*80.0f; DrawScrollbar({lx,gridY,lw,gridH},th,leftScroll);
        float ryy=gridY-leftScroll;
        for(size_t i=0;i<deals.size();i++){
            float y=ryy+i*80.0f; if(y+80<gridY||y>gridY+gridH)continue;
            DrawCard({lx,y,lw-20,72},deals[i].dealName,(i%2==0)?COL_CARD:COL_BG);
            char p[32];snprintf(p,sizeof(p),"PKR %.0f",deals[i].totalAmount);
            DrawText(p,(int)lx+15,(int)y+35,FONT_SIZE_SMALL,COL_ACCENT);
            if(DrawButton({lx+lw-160,y+20,130,32},"Add to Order",COL_ACCENT,COL_WHITE)) addDeal(deals[i]);
        }
    }else{
        auto& lst=curList(); float cw=(lw-40)/2, ch=100;
        int rows=((int)lst.size()+1)/2; float th=rows*(ch+10);
        DrawScrollbar({lx,gridY,lw,gridH},th,leftScroll);
        float ryy=gridY-leftScroll;
        for(size_t i=0;i<lst.size();i++){
            int r=(int)i/2, c=(int)i%2; float y=ryy+r*(ch+10); if(y+ch<gridY||y>gridY+gridH)continue;
            float x=lx+c*(cw+10);
            DrawCard({x,y,cw,ch},lst[i].itemName,(i%2==0)?COL_CARD:COL_BG);
            char p[32];snprintf(p,sizeof(p),"PKR %.0f",lst[i].currentPrice);
            DrawText(p,(int)x+10,(int)y+30,FONT_SIZE_SMALL,COL_ACCENT);
            DrawText(lst[i].menuItemType.c_str(),(int)x+10,(int)y+52,FONT_SIZE_SMALL,COL_BORDER);
            if(DrawButton({x+cw-120,y+58,110,28},"Add",COL_ACCENT,COL_WHITE)) addItem(lst[i]);
        }
    }

    DrawCard({rx,ry,rw,rh},"ORDER SUMMARY",COL_CARD);
    float sy=ry+45, ia=rh-240, cth=cart.size()*40.0f;
    DrawScrollbar({rx+5,sy,rw-10,ia},cth,rightScroll);
    float cry=sy-rightScroll;
    for(size_t i=0;i<cart.size();i++){
        float y=cry+i*40.0f; if(y+40<sy||y>sy+ia)continue;
        std::string nm=cart[i].isDeal?cart[i].dealName:cart[i].itemName;
        char ln[128];snprintf(ln,sizeof(ln),"%s x%d  PKR %.0f",nm.c_str(),cart[i].quantity,cart[i].unitPrice*cart[i].quantity);
        DrawText(ln,(int)rx+10,(int)y,FONT_SIZE_SMALL,COL_DARK);
        if(DrawButton({rx+rw-100,y-2,22,22},"-",COL_BORDER,COL_DARK)){cart[i].quantity--;if(cart[i].quantity<=0)cart.erase(cart.begin()+i);}
        char qs[8];snprintf(qs,sizeof(qs),"%d",cart[i].quantity);DrawText(qs,(int)(rx+rw-72),(int)y,FONT_SIZE_SMALL,COL_DARK);
        if(DrawButton({rx+rw-52,y-2,22,22},"+",COL_ACCENT,COL_WHITE))cart[i].quantity++;
        if(DrawButton({rx+rw-25,y-2,22,22},"X",COL_LOW_STOCK,COL_WHITE))cart.erase(cart.begin()+i);
    }

    float ty=sy+ia+5;
    char sb[32],db[32],tb[32];snprintf(sb,sizeof(sb),"Subtotal: PKR %.0f",subtotal());
    snprintf(db,sizeof(db),"Discount: -PKR %.0f",discountAmt());snprintf(tb,sizeof(tb),"TOTAL: PKR %.0f",total());
    DrawText(sb,(int)rx+10,(int)ty,FONT_SIZE_SMALL,COL_DARK);ty+=22;
    std::vector<std::string> dn={"None"};for(auto&d:activeDiscounts)dn.push_back(d.discountName+" "+std::to_string((int)d.discountValue)+"%");
    int ddid=selectedDiscountIdx+1; DrawDropdown({rx+10,ty,rw-20,32},"Discount:",dn,ddid,discountDropOpen);
    selectedDiscountIdx=ddid-1;ty+=40;
    if(selectedDiscountIdx>=0){DrawText(db,(int)rx+10,(int)ty,FONT_SIZE_SMALL,COL_PENDING);}
    ty+=22;
    DrawText(tb,(int)rx+10,(int)ty,FONT_SIZE_NORMAL,COL_ACCENT);

    float by=ry+rh-50;
    if(DrawButton({rx+10,by,(rw-30)/2,36},"CANCEL",COL_LOW_STOCK,COL_WHITE)){
        cart.clear();selectedDiscountIdx=-1;isModifyMode=false;modifyOrderId=-1;
        memset(customerName,0,sizeof(customerName));setStatus("Order cleared",true);
    }
    if(DrawButton({rx+(rw-30)/2+20,by,(rw-30)/2,36},isModifyMode?"UPDATE":"CONFIRM",COL_COMPLETED,COL_WHITE)){
        if(cart.empty()){setStatus("Cart is empty!",false);}
        else{
            bool ok=true;
            for(auto&ci:cart)if(!ci.isDeal&&!checkStockSufficiency(ci.menuItemId,getBranchId(),ci.quantity))
                {char e[128];snprintf(e,sizeof(e),"Insufficient stock: %s",ci.itemName.c_str());setStatus(e,false);ok=false;break;}
            if(ok){
                std::string kitch=findLeastBusyKitchenStaff(getBranchId());
                if(kitch.empty())setStatus("No kitchen staff available!",false);
                else{
                    int did=(selectedDiscountIdx>=0)?activeDiscounts[selectedDiscountIdx].discountId:-1;
                    if(isModifyMode){bool r=modifyOrder(modifyOrderId,cart,total(),discountAmt());setStatus(r?"Order updated!":"Error",r);}
                    else{int oid=createOrder(getUsername(),kitch,did,discountAmt(),total(),total(),0,cart);
                        if(oid>0){char m[64];snprintf(m,sizeof(m),"Order #%d placed!",oid);setStatus(m,true);}else setStatus("Error",false);}
                    cart.clear();selectedDiscountIdx=-1;isModifyMode=false;modifyOrderId=-1;memset(customerName,0,sizeof(customerName));
                }
            }
        }
    }
    if(statusTimer>0)DrawNotification(statusMsg,statusMsg.find("Error")!=std::string::npos?COL_LOW_STOCK:COL_COMPLETED,statusTimer);
}