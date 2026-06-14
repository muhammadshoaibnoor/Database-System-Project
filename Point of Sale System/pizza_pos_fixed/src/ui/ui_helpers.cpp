#include "ui_helpers.h"
#include <cstring>
#include <cctype>

// ==================== COLOR CONSTANTS ====================
// Using simple struct initialization compatible with Raylib's Color

#define COL_PENDING    Color{ 255, 200,  50, 255 }
#define COL_IN_PREP    Color{  50, 120, 255, 255 }
#define COL_COMPLETED  Color{  50, 180,  50, 255 }
#define COL_LOW_STOCK  Color{ 220,  50,  50, 255 }
#define COL_BG         Color{ 250, 245, 235, 255 }
#define COL_SIDEBAR    Color{  44,  24,  16, 255 }
#define COL_ACCENT     Color{ 192,  99,  43, 255 }
#define COL_WHITE      Color{ 255, 255, 255, 255 }
#define COL_DARK       Color{  30,  30,  30, 255 }
#define COL_CARD       Color{ 255, 252, 245, 255 }
#define COL_BORDER     Color{ 200, 180, 160, 255 }

// ==================== INTERNAL CONSTANTS ====================

#define FONT_SIZE_SMALL   16
#define FONT_SIZE_NORMAL  18
#define FONT_SIZE_LARGE   22
#define FONT_SIZE_TITLE   26
#define ROUNDNESS         0.15f
#define SEGMENTS          16

// ==================== BUTTONS ====================

bool DrawButton(Rectangle rect, const std::string& label, Color bgColor, Color textColor) {
    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, rect);
    bool clicked = hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    Color drawColor = hovered ? ColorAlphaBlend(bgColor, Fade(BLACK, 0.15f), WHITE) : bgColor;
    DrawRectangleRounded(rect, ROUNDNESS, SEGMENTS, drawColor);

    int fontSize = (rect.height > 40) ? FONT_SIZE_NORMAL : FONT_SIZE_SMALL;
    float textW = (float)MeasureText(label.c_str(), fontSize);
    float textX = rect.x + (rect.width - textW) / 2.0f;
    float textY = rect.y + (rect.height - (float)fontSize) / 2.0f;
    DrawText(label.c_str(), (int)textX, (int)textY, fontSize, textColor);

    return clicked;
}

bool DrawButtonHover(Rectangle rect, const std::string& label, Color bgColor, Color hoverColor, Color textColor) {
    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, rect);
    bool clicked = hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    Color drawColor = hovered ? hoverColor : bgColor;
    DrawRectangleRounded(rect, ROUNDNESS, SEGMENTS, drawColor);

    int fontSize = (rect.height > 40) ? FONT_SIZE_NORMAL : FONT_SIZE_SMALL;
    float textW = (float)MeasureText(label.c_str(), fontSize);
    float textX = rect.x + (rect.width - textW) / 2.0f;
    float textY = rect.y + (rect.height - (float)fontSize) / 2.0f;
    DrawText(label.c_str(), (int)textX, (int)textY, fontSize, textColor);

    return clicked;
}

// ==================== INPUTS ====================

void DrawInput(Rectangle rect, const std::string& label, char* buffer, int maxLen, bool isActive, bool isMasked) {
    if (!label.empty()) {
        DrawText(label.c_str(), (int)rect.x, (int)rect.y - 20, FONT_SIZE_SMALL, COL_DARK);
    }

    Color bgColor = isActive ? COL_CARD : Fade(COL_BORDER, 0.3f);
    Color borderColor = isActive ? COL_ACCENT : COL_BORDER;
    DrawRectangleRounded(rect, 0.08f, SEGMENTS, bgColor);
    DrawRectangleRoundedLinesEx(rect, 0.08f, SEGMENTS, 2, borderColor);

    if (isActive) {
        int key = GetCharPressed();
        while (key > 0) {
            if (key >= 32 && key <= 126) {
                int len = (int)strlen(buffer);
                if (len < maxLen - 1) {
                    buffer[len] = (char)key;
                    buffer[len + 1] = '\0';
                }
            }
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE)) {
            int len = (int)strlen(buffer);
            if (len > 0) buffer[len - 1] = '\0';
        }
    }

    std::string displayText;
    if (isMasked && strlen(buffer) > 0) {
        displayText = std::string(strlen(buffer), '*');
    } else {
        displayText = buffer;
    }

    float textY = rect.y + (rect.height - (float)FONT_SIZE_NORMAL) / 2.0f;
    DrawText(displayText.c_str(), (int)rect.x + 10, (int)textY, FONT_SIZE_NORMAL, COL_DARK);

    if (isActive && ((int)(GetTime() * 2) % 2 == 0)) {
        float cursorX = rect.x + 12.0f + (float)MeasureText(displayText.c_str(), FONT_SIZE_NORMAL);
        DrawLine((int)cursorX, (int)rect.y + 8, (int)cursorX, (int)(rect.y + rect.height - 8), COL_DARK);
    }
}

void DrawSearchBar(Rectangle rect, char* buffer, int maxLen, bool& isActive) {
    Vector2 mouse = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        isActive = CheckCollisionPointRec(mouse, rect);
    }

    DrawRectangleRounded(rect, 0.3f, SEGMENTS, isActive ? COL_CARD : Fade(COL_BORDER, 0.3f));
    DrawRectangleRoundedLinesEx(rect, 0.3f, SEGMENTS, 2, isActive ? COL_ACCENT : COL_BORDER);

    float iconX = rect.x + 12.0f;
    float iconY = rect.y + rect.height / 2.0f;
    DrawCircle((int)iconX, (int)iconY, 6, COL_BORDER);
    DrawLine((int)(iconX + 5), (int)(iconY + 5), (int)(iconX + 10), (int)(iconY + 10), COL_BORDER);

    float textX = rect.x + 28.0f;
    float textY = rect.y + (rect.height - (float)FONT_SIZE_SMALL) / 2.0f;
    if (strlen(buffer) == 0 && !isActive) {
        DrawText("Search...", (int)textX, (int)textY, FONT_SIZE_SMALL, COL_BORDER);
    } else {
        DrawText(buffer, (int)textX, (int)textY, FONT_SIZE_SMALL, COL_DARK);
    }

    if (isActive) {
        int key = GetCharPressed();
        while (key > 0) {
            if (key >= 32 && key <= 126 && (int)strlen(buffer) < maxLen - 1) {
                int len = (int)strlen(buffer);
                buffer[len] = (char)key;
                buffer[len + 1] = '\0';
            }
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && strlen(buffer) > 0) {
            buffer[strlen(buffer) - 1] = '\0';
        }
    }
}

bool DrawCheckbox(float x, float y, const std::string& label, bool& checked) {
    Rectangle box = { x, y, 20, 20 };
    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, box);

    if (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        checked = !checked;
    }

    Color boxColor = checked ? COL_ACCENT : COL_BORDER;
    DrawRectangleRounded(box, 0.2f, SEGMENTS, boxColor);
    DrawRectangleRoundedLinesEx(box, 0.2f, SEGMENTS, 1, COL_DARK);

    if (checked) {
        DrawText("v", (int)x + 4, (int)y - 1, FONT_SIZE_SMALL, COL_WHITE);
    }

    DrawText(label.c_str(), (int)x + 28, (int)y, FONT_SIZE_SMALL, COL_DARK);
    return hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

// ==================== DROPDOWN ====================

bool DrawDropdown(Rectangle rect, const std::string& label, const std::vector<std::string>& options, int& selectedIndex, bool& isOpen) {
    bool changed = false;
    Vector2 mouse = GetMousePosition();

    if (!label.empty()) {
        DrawText(label.c_str(), (int)rect.x, (int)rect.y - 20, FONT_SIZE_SMALL, COL_DARK);
    }

    Color btnColor = isOpen ? COL_ACCENT : COL_CARD;
    DrawRectangleRounded(rect, 0.08f, SEGMENTS, btnColor);
    DrawRectangleRoundedLinesEx(rect, 0.08f, SEGMENTS, 2, COL_BORDER);

    std::string displayText = (selectedIndex >= 0 && selectedIndex < (int)options.size()) ? options[selectedIndex] : "Select...";
    float textY = rect.y + (rect.height - (float)FONT_SIZE_NORMAL) / 2.0f;
    DrawText(displayText.c_str(), (int)rect.x + 10, (int)textY, FONT_SIZE_NORMAL, COL_DARK);
    DrawText(isOpen ? "v" : "^", (int)(rect.x + rect.width - 30), (int)textY, FONT_SIZE_SMALL, COL_DARK);

    if (CheckCollisionPointRec(mouse, rect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        isOpen = !isOpen;
    }

    if (isOpen) {
        float optH = rect.height;
        for (int i = 0; i < (int)options.size(); i++) {
            Rectangle optRect = { rect.x, rect.y + rect.height + i * optH, rect.width, optH };
            Color optColor = CheckCollisionPointRec(mouse, optRect) ? COL_ACCENT : COL_CARD;
            DrawRectangleRounded(optRect, 0.08f, SEGMENTS, optColor);
            DrawRectangleRoundedLinesEx(optRect, 0.08f, SEGMENTS, 1, COL_BORDER);
            DrawText(options[i].c_str(), (int)optRect.x + 10, (int)(optRect.y + (optH - (float)FONT_SIZE_SMALL) / 2.0f), FONT_SIZE_SMALL, COL_DARK);

            if (CheckCollisionPointRec(mouse, optRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                selectedIndex = i;
                isOpen = false;
                changed = true;
            }
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !CheckCollisionPointRec(mouse, rect)) {
            bool clickedOption = false;
            for (int i = 0; i < (int)options.size(); i++) {
                Rectangle optRect = { rect.x, rect.y + rect.height + i * optH, rect.width, optH };
                if (CheckCollisionPointRec(mouse, optRect)) { clickedOption = true; break; }
            }
            if (!clickedOption) isOpen = false;
        }
    }

    return changed;
}

// ==================== LAYOUT COMPONENTS ====================

void DrawCard(Rectangle rect, const std::string& title, Color bgColor) {
    DrawRectangleRounded(rect, 0.05f, SEGMENTS, bgColor);
    DrawRectangleRoundedLinesEx(rect, 0.05f, SEGMENTS, 1, COL_BORDER);
    if (!title.empty()) {
        DrawText(title.c_str(), (int)rect.x + 15, (int)rect.y + 10, FONT_SIZE_LARGE, COL_DARK);
    }
}

void DrawStatusBadge(float x, float y, const std::string& status) {
    Color badgeColor;
    if (status == "Pending") badgeColor = COL_PENDING;
    else if (status == "In Preparation") badgeColor = COL_IN_PREP;
    else if (status == "Completed") badgeColor = COL_COMPLETED;
    else if (status == "Cancelled") badgeColor = COL_LOW_STOCK;
    else badgeColor = COL_BORDER;

    float textW = (float)MeasureText(status.c_str(), FONT_SIZE_SMALL);
    Rectangle badgeRect = { x, y, textW + 16, 26 };
    DrawRectangleRounded(badgeRect, 0.3f, SEGMENTS, badgeColor);
    DrawText(status.c_str(), (int)x + 8, (int)y + 4, FONT_SIZE_SMALL, COL_WHITE);
}

void DrawTopBar(const std::string& appTitle, const std::string& branchName, const std::string& userName) {
    Rectangle bar = { 0, 0, (float)WINDOW_WIDTH, (float)TOPBAR_H };
    DrawRectangleRec(bar, COL_SIDEBAR);

    DrawText(appTitle.c_str(), 20, 12, FONT_SIZE_LARGE, COL_WHITE);

    std::string branchText = "Branch: " + branchName;
    float branchW = (float)MeasureText(branchText.c_str(), FONT_SIZE_NORMAL);
    DrawText(branchText.c_str(), (WINDOW_WIDTH - (int)branchW) / 2, 16, FONT_SIZE_NORMAL, COL_WHITE);

    std::string userText = userName;
    float userW = (float)MeasureText(userText.c_str(), FONT_SIZE_NORMAL);
    DrawText(userText.c_str(), WINDOW_WIDTH - (int)userW - 100, 16, FONT_SIZE_NORMAL, COL_ACCENT);
}

bool DrawLogoutButton(float x, float y) {
    Rectangle btn = { x, y, 90, 32 };
    return DrawButton(btn, "LOGOUT", COL_LOW_STOCK, COL_WHITE);
}

void DrawSidebar(const std::vector<std::string>& items, int activeIndex, int& clickedIndex) {
    Rectangle sidebarRect = { 0, (float)TOPBAR_H, (float)SIDEBAR_W, (float)CONTENT_H };
    DrawRectangleRec(sidebarRect, COL_SIDEBAR);

    clickedIndex = -1;
    Vector2 mouse = GetMousePosition();

    for (int i = 0; i < (int)items.size(); i++) {
        Rectangle itemRect = { 0, TOPBAR_H + i * 50.0f, (float)SIDEBAR_W, 50 };
        bool isActive = (i == activeIndex);
        bool hovered = CheckCollisionPointRec(mouse, itemRect);

        Color bgColor = isActive ? COL_ACCENT : (hovered ? ColorAlphaBlend(COL_SIDEBAR, Fade(COL_ACCENT, 0.3f), WHITE) : COL_SIDEBAR);
        DrawRectangleRec(itemRect, bgColor);

        if (isActive) {
            DrawRectangle((int)itemRect.x, (int)itemRect.y, 4, 50, COL_ACCENT);
        }

        DrawText(items[i].c_str(), 20, (int)(itemRect.y + 14), FONT_SIZE_NORMAL, isActive ? COL_WHITE : Fade(COL_WHITE, 0.8f));
        DrawLine(10, (int)(itemRect.y + itemRect.height), SIDEBAR_W - 10, (int)(itemRect.y + itemRect.height), Fade(COL_WHITE, 0.1f));

        if (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            clickedIndex = i;
        }
    }
}

void DrawPopupOverlay() {
    DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, Fade(BLACK, 0.5f));
}

// ==================== TABLE ====================

void DrawTableHeader(float x, float y, float w, const std::vector<std::string>& headers, const std::vector<float>& colWidths) {
    float cx = x;
    for (int i = 0; i < (int)headers.size() && i < (int)colWidths.size(); i++) {
        Rectangle headerRect = { cx, y, colWidths[i], 36 };
        DrawRectangleRounded(headerRect, 0.1f, SEGMENTS, COL_ACCENT);
        DrawText(headers[i].c_str(), (int)cx + 8, (int)y + 8, FONT_SIZE_SMALL, COL_WHITE);
        cx += colWidths[i];
    }
    DrawLine((int)x, (int)(y + 36), (int)(x + w), (int)(y + 36), COL_BORDER);
}

void DrawTableRow(float x, float y, float w, const std::vector<std::string>& cells, const std::vector<float>& colWidths, Color rowColor, bool isSelected) {
    if (isSelected) {
        rowColor = ColorAlphaBlend(rowColor, Fade(COL_ACCENT, 0.2f), WHITE);
    }
    float cx = x;
    for (int i = 0; i < (int)cells.size() && i < (int)colWidths.size(); i++) {
        Rectangle cellRect = { cx, y, colWidths[i], 32 };
        DrawRectangleRec(cellRect, rowColor);
        DrawText(cells[i].c_str(), (int)cx + 8, (int)y + 6, FONT_SIZE_SMALL, COL_DARK);
        cx += colWidths[i];
    }
    DrawLine((int)x, (int)(y + 32), (int)(x + w), (int)(y + 32), Fade(COL_BORDER, 0.5f));
}

void DrawScrollbar(Rectangle contentRect, float contentHeight, float& scrollOffset) {
    float visibleHeight = contentRect.height;
    if (contentHeight <= visibleHeight) return;

    float thumbHeight = (visibleHeight / contentHeight) * visibleHeight;
    if (thumbHeight < 30.0f) thumbHeight = 30.0f;

    float maxOffset = contentHeight - visibleHeight;
    float thumbY = contentRect.y + (scrollOffset / maxOffset) * (visibleHeight - thumbHeight);

    Rectangle trackRect = { contentRect.x + contentRect.width - 12.0f, contentRect.y, 12.0f, visibleHeight };
    DrawRectangleRec(trackRect, Fade(COL_BORDER, 0.3f));

    Rectangle thumbRect = { contentRect.x + contentRect.width - 10.0f, thumbY, 8.0f, thumbHeight };
    DrawRectangleRounded(thumbRect, 0.3f, SEGMENTS, COL_ACCENT);

    float wheel = GetMouseWheelMove();
    scrollOffset -= wheel * 30.0f;
    if (scrollOffset < 0.0f) scrollOffset = 0.0f;
    if (scrollOffset > maxOffset) scrollOffset = maxOffset;

    Vector2 mouse = GetMousePosition();
    static bool dragging = false;
    static float dragStartY = 0.0f;
    static float dragStartOffset = 0.0f;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, thumbRect)) {
        dragging = true;
        dragStartY = mouse.y;
        dragStartOffset = scrollOffset;
    }
    if (dragging) {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            float deltaY = mouse.y - dragStartY;
            scrollOffset = dragStartOffset + (deltaY / (visibleHeight - thumbHeight)) * maxOffset;
            if (scrollOffset < 0.0f) scrollOffset = 0.0f;
            if (scrollOffset > maxOffset) scrollOffset = maxOffset;
        } else {
            dragging = false;
        }
    }
}

// ==================== DIALOGS & NOTIFICATIONS ====================

void DrawNotification(const std::string& message, Color color, float& timer) {
    if (timer <= 0.0f) return;

    timer -= GetFrameTime();
    float alpha = (timer < 1.0f) ? timer : 1.0f;

    float textW = (float)MeasureText(message.c_str(), FONT_SIZE_NORMAL);
    float notifW = textW + 40.0f;
    float notifH = 40.0f;
    float notifX = (float)WINDOW_WIDTH - notifW - 20.0f;
    float notifY = (float)TOPBAR_H + 10.0f;

    DrawRectangleRounded({ notifX, notifY, notifW, notifH }, 0.2f, SEGMENTS, Fade(color, alpha));
    DrawText(message.c_str(), (int)notifX + 20, (int)(notifY + 10), FONT_SIZE_NORMAL, Fade(COL_WHITE, alpha));
}

bool DrawConfirmDialog(const std::string& title, const std::string& message, bool& showDialog) {
    if (!showDialog) return false;

    DrawPopupOverlay();

    float dialogW = 400.0f;
    float dialogH = 200.0f;
    float dialogX = ((float)WINDOW_WIDTH - dialogW) / 2.0f;
    float dialogY = ((float)WINDOW_HEIGHT - dialogH) / 2.0f;

    Rectangle dialogRect = { dialogX, dialogY, dialogW, dialogH };
    DrawRectangleRounded(dialogRect, 0.1f, SEGMENTS, COL_CARD);
    DrawRectangleRoundedLinesEx(dialogRect, 0.1f, SEGMENTS, 2, COL_ACCENT);

    DrawText(title.c_str(), (int)dialogX + 20, (int)dialogY + 15, FONT_SIZE_LARGE, COL_DARK);
    DrawText(message.c_str(), (int)dialogX + 20, (int)dialogY + 60, FONT_SIZE_NORMAL, COL_DARK);

    Rectangle yesBtn = { dialogX + 60.0f, dialogY + 140.0f, 120.0f, 36.0f };
    bool yes = DrawButton(yesBtn, "YES", COL_ACCENT, COL_WHITE);

    Rectangle noBtn = { dialogX + 220.0f, dialogY + 140.0f, 120.0f, 36.0f };
    bool no = DrawButton(noBtn, "NO", COL_BORDER, COL_DARK);

    if (yes || no) {
        showDialog = false;
        return yes;
    }
    return false;
}