#ifndef GLOBALS_H
#define GLOBALS_H

#include <string>
#include <vector>

// ==================== ENUMS ====================

enum UserRole {
    ROLE_ADMIN,
    ROLE_RECEPTIONIST,
    ROLE_KITCHEN
};

// Undefine Windows macros that clash with our enum names
#ifdef STATUS_PENDING
#undef STATUS_PENDING
#endif
#ifdef STATUS_IN_PREPARATION
#undef STATUS_IN_PREPARATION
#endif
#ifdef STATUS_COMPLETED
#undef STATUS_COMPLETED
#endif
#ifdef STATUS_CANCELLED
#undef STATUS_CANCELLED
#endif

enum OrderStatus {
    STATUS_PENDING,
    STATUS_IN_PREPARATION,
    STATUS_COMPLETED,
    STATUS_CANCELLED
};

enum ScreenID {
    SCREEN_LOGIN,
    SCREEN_ADMIN,
    SCREEN_RECEPTIONIST,
    SCREEN_KITCHEN
};

enum AdminPanel {
    PANEL_OVERVIEW,
    PANEL_MENU,
    PANEL_INVENTORY,
    PANEL_STAFF,
    PANEL_SALARY,
    PANEL_DEALS,
    PANEL_DISCOUNTS,
    PANEL_BRANCHES,
    PANEL_SHIFTS,
    PANEL_REPORTS
};

enum RecepPanel {
    RPANEL_NEW_ORDER,
    RPANEL_MY_ORDERS,
    RPANEL_CLOCK
};

enum KitchenPanel {
    KPANEL_QUEUE,
    KPANEL_CLOCK
};

// ==================== COLOR CONSTANTS ====================
// Use inline functions returning Color-compatible arrays
// Raylib's Color is { unsigned char r, g, b, a }
// These compile without including raylib.h

inline unsigned char COL_PENDING_R()  { return 255; } inline unsigned char COL_PENDING_G()  { return 200; } inline unsigned char COL_PENDING_B()  { return  50; }
inline unsigned char COL_IN_PREP_R()  { return  50; } inline unsigned char COL_IN_PREP_G()  { return 120; } inline unsigned char COL_IN_PREP_B()  { return 255; }
inline unsigned char COL_COMPLETED_R(){ return  50; } inline unsigned char COL_COMPLETED_G(){ return 180; } inline unsigned char COL_COMPLETED_B(){ return  50; }
inline unsigned char COL_LOW_STOCK_R(){ return 220; } inline unsigned char COL_LOW_STOCK_G(){ return  50; } inline unsigned char COL_LOW_STOCK_B(){ return  50; }
inline unsigned char COL_BG_R()       { return 250; } inline unsigned char COL_BG_G()       { return 245; } inline unsigned char COL_BG_B()       { return 235; }
inline unsigned char COL_SIDEBAR_R()  { return  44; } inline unsigned char COL_SIDEBAR_G()  { return  24; } inline unsigned char COL_SIDEBAR_B()  { return  16; }
inline unsigned char COL_ACCENT_R()   { return 192; } inline unsigned char COL_ACCENT_G()   { return  99; } inline unsigned char COL_ACCENT_B()   { return  43; }
inline unsigned char COL_WHITE_R()    { return 255; } inline unsigned char COL_WHITE_G()    { return 255; } inline unsigned char COL_WHITE_B()    { return 255; }
inline unsigned char COL_DARK_R()     { return  30; } inline unsigned char COL_DARK_G()     { return  30; } inline unsigned char COL_DARK_B()     { return  30; }
inline unsigned char COL_CARD_R()     { return 255; } inline unsigned char COL_CARD_G()     { return 252; } inline unsigned char COL_CARD_B()     { return 245; }
inline unsigned char COL_BORDER_R()   { return 200; } inline unsigned char COL_BORDER_G()   { return 180; } inline unsigned char COL_BORDER_B()   { return 160; }

// Helper macro to construct a Raylib Color from the above functions
// Usage: COL(COL_PENDING) expands to (Color){ COL_PENDING_R(), COL_PENDING_G(), COL_PENDING_B(), 255 }
#define COL(name) CLITERAL(Color){ name##R(), name##G(), name##B(), 255 }

// ==================== LAYOUT CONSTANTS ====================

const int WINDOW_WIDTH   = 1280;
const int WINDOW_HEIGHT  = 720;
const int SIDEBAR_W      = 220;
const int TOPBAR_H       = 55;
const int CONTENT_X      = SIDEBAR_W;
const int CONTENT_Y      = TOPBAR_H;
const int CONTENT_W      = WINDOW_WIDTH - SIDEBAR_W;
const int CONTENT_H      = WINDOW_HEIGHT - TOPBAR_H;

// ==================== STRING LIMITS ====================

const int MAX_INPUT = 256;
const int MAX_NAME  = 100;

#endif // GLOBALS_H