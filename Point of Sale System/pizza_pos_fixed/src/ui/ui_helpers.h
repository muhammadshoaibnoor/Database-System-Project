#ifndef UI_HELPERS_H
#define UI_HELPERS_H

#include "raylib.h"
#include "../core/globals.h"
#include <string>
#include <vector>

// ==================== COLOR MACROS ====================
// Defined as macros so they work anywhere raylib.h is included
// Each expands to a Color literal: Color{ r, g, b, a }

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

// ==================== FONT SIZE CONSTANTS ====================

#define FONT_SIZE_SMALL   16
#define FONT_SIZE_NORMAL  18
#define FONT_SIZE_LARGE   22
#define FONT_SIZE_TITLE   26

// ==================== BUTTONS ====================

// Draw a rounded button. Returns true if clicked.
bool DrawButton(Rectangle rect, const std::string& label, Color bgColor, Color textColor);

// Draw a button with hover color change. Returns true if clicked.
bool DrawButtonHover(Rectangle rect, const std::string& label, Color bgColor, Color hoverColor, Color textColor);

// ==================== INPUTS ====================

// Draw a text input field with label. Handles keyboard input when isActive is true.
// buffer is modified in-place. isMasked shows '*' for password fields.
void DrawInput(Rectangle rect, const std::string& label, char* buffer, int maxLen, bool isActive, bool isMasked = false);

// Draw a search bar with magnifying glass icon. Toggles isActive on click.
void DrawSearchBar(Rectangle rect, char* buffer, int maxLen, bool& isActive);

// Draw a toggle checkbox. Returns true if toggled this frame.
bool DrawCheckbox(float x, float y, const std::string& label, bool& checked);

// ==================== DROPDOWN ====================

// Draw a dropdown selector. Returns true if selection changed.
// selectedIndex is modified when user picks an option.
// isOpen tracks whether the dropdown list is expanded.
bool DrawDropdown(Rectangle rect, const std::string& label, const std::vector<std::string>& options, int& selectedIndex, bool& isOpen);

// ==================== LAYOUT COMPONENTS ====================

// Draw a rounded card container with optional title.
void DrawCard(Rectangle rect, const std::string& title, Color bgColor);

// Draw a colored status badge (Pending=yellow, In Preparation=blue, Completed=green, Cancelled=red).
void DrawStatusBadge(float x, float y, const std::string& status);

// Draw the full-width top bar with app title, branch name, and user name.
void DrawTopBar(const std::string& appTitle, const std::string& branchName, const std::string& userName);

// Draw a red logout button. Returns true if clicked.
bool DrawLogoutButton(float x, float y);

// Draw the sidebar navigation. clickedIndex is set to the item clicked, or -1 if none.
void DrawSidebar(const std::vector<std::string>& items, int activeIndex, int& clickedIndex);

// Draw a semi-transparent overlay behind popups.
void DrawPopupOverlay();

// ==================== TABLE ====================

// Draw a table header row with accent background.
void DrawTableHeader(float x, float y, float w, const std::vector<std::string>& headers, const std::vector<float>& colWidths);

// Draw a single table data row. isSelected highlights the row.
void DrawTableRow(float x, float y, float w, const std::vector<std::string>& cells, const std::vector<float>& colWidths, Color rowColor, bool isSelected);

// Draw a vertical scrollbar. scrollOffset is modified by mouse wheel and drag.
void DrawScrollbar(Rectangle contentRect, float contentHeight, float& scrollOffset);

// ==================== DIALOGS & NOTIFICATIONS ====================

// Draw a toast notification at top-right. timer decreases automatically.
// Notification disappears when timer reaches 0.
void DrawNotification(const std::string& message, Color color, float& timer);

// Draw a yes/no confirmation dialog with overlay. Returns true if YES clicked.
// showDialog is set to false when either button is clicked.
bool DrawConfirmDialog(const std::string& title, const std::string& message, bool& showDialog);

#endif // UI_HELPERS_H