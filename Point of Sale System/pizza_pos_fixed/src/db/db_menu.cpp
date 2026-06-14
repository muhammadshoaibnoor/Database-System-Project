#include <mysql.h>
#include <string>
#include <vector>
#include <cstring>
#include <iostream>
#include "../core/models/models.h"

extern MYSQL* getConn();


namespace DB {

// ==================== HELPER ====================

static std::string escapeStr(const std::string& input) {
    if (input.empty()) return "''";
    MYSQL* conn = getConn();
    if (!conn) return "''";
    char* escaped = new char[input.size() * 2 + 3];
    mysql_real_escape_string(conn, escaped, input.c_str(), static_cast<unsigned long>(input.size()));
    std::string result = "'" + std::string(escaped) + "'";
    delete[] escaped;
    return result;
}

// ==================== MENU FUNCTIONS ====================

std::vector<MenuItem> getMenuItems(int branchId) {
    std::vector<MenuItem> items;
    MYSQL* conn = getConn();
    if (!conn) return items;

    std::string sql =
        "SELECT mi.MenuItemId, mi.CategoryId, c.CategoryName, mi.ItemName, "
        "mi.Description, mi.MenuItemType, "
        "COALESCE(p.Amount, 0) AS CurrentPrice, "
        "COALESCE(bmi.IsAvailable, 1) AS IsAvailable "
        "FROM MENUITEM mi "
        "JOIN CATEGORY c ON mi.CategoryId = c.CategoryId "
        "LEFT JOIN PRICE p ON mi.MenuItemId = p.MenuItemId AND p.IsActive = 1 "
        "LEFT JOIN BRANCH_MENU_ITEM bmi ON mi.MenuItemId = bmi.MenuItemId AND bmi.BranchId = " +
        std::to_string(branchId) + " "
        "ORDER BY mi.MenuItemType, mi.ItemName";

    if (mysql_query(conn, sql.c_str()) != 0) return items;

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return items;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        MenuItem item;
        item.menuItemId   = row[0] ? std::stoi(row[0]) : 0;
        item.categoryId   = row[1] ? std::stoi(row[1]) : 0;
        item.categoryName = row[2] ? row[2] : "";
        item.itemName     = row[3] ? row[3] : "";
        item.description  = row[4] ? row[4] : "";
        item.menuItemType = row[5] ? row[5] : "";
        item.currentPrice = row[6] ? std::stod(row[6]) : 0.0;
        item.isAvailable  = row[7] ? (std::stoi(row[7]) == 1) : true;
        items.push_back(item);
    }

    mysql_free_result(res);
    return items;
}

std::vector<MenuItem> getAvailableMenuItems(int branchId) {
    std::vector<MenuItem> all = getMenuItems(branchId);
    std::vector<MenuItem> available;
    for (const auto& item : all) {
        if (item.isAvailable) {
            available.push_back(item);
        }
    }
    return available;
}

std::vector<MenuItem> getMenuItemsByType(int branchId, const std::string& menuItemType) {
    std::vector<MenuItem> all = getMenuItems(branchId);
    std::vector<MenuItem> filtered;
    for (const auto& item : all) {
        if (item.menuItemType == menuItemType) {
            filtered.push_back(item);
        }
    }
    return filtered;
}

bool addMenuItem(const std::string& itemName, int categoryId,
                 const std::string& menuItemType, const std::string& description,
                 double initialPrice) {
    MYSQL* conn = getConn();
    if (!conn) return false;

    std::string sql =
        "INSERT INTO MENUITEM (CategoryId, ItemName, Description, MenuItemType) VALUES (" +
        std::to_string(categoryId) + ", " +
        escapeStr(itemName) + ", " +
        escapeStr(description) + ", " +
        escapeStr(menuItemType) + ")";

    if (mysql_query(conn, sql.c_str()) != 0) return false;

    int newId = static_cast<int>(mysql_insert_id(conn));

    // Insert initial price
    std::string priceSql =
        "INSERT INTO PRICE (MenuItemId, Amount, EffectiveDate, IsActive) VALUES (" +
        std::to_string(newId) + ", " +
        std::to_string(initialPrice) + ", CURDATE(), 1)";

    return mysql_query(conn, priceSql.c_str()) == 0;
}

bool updateMenuItem(int menuItemId, const std::string& itemName, int categoryId,
                    const std::string& menuItemType, const std::string& description) {
    MYSQL* conn = getConn();
    if (!conn) return false;

    std::string sql =
        "UPDATE MENUITEM SET "
        "ItemName = " + escapeStr(itemName) + ", "
        "CategoryId = " + std::to_string(categoryId) + ", "
        "MenuItemType = " + escapeStr(menuItemType) + ", "
        "Description = " + escapeStr(description) + " "
        "WHERE MenuItemId = " + std::to_string(menuItemId);

    return mysql_query(conn, sql.c_str()) == 0;
}

bool setNewPrice(int menuItemId, double newAmount) {
    MYSQL* conn = getConn();
    if (!conn) return false;

    // Use transaction
    mysql_query(conn, "START TRANSACTION");

    // Deactivate old price
    std::string deactivateSql =
        "UPDATE PRICE SET EndDate = CURDATE(), IsActive = 0 "
        "WHERE MenuItemId = " + std::to_string(menuItemId) +
        " AND IsActive = 1";

    if (mysql_query(conn, deactivateSql.c_str()) != 0) {
        mysql_query(conn, "ROLLBACK");
        return false;
    }

    // Insert new active price
    std::string insertSql =
        "INSERT INTO PRICE (MenuItemId, Amount, EffectiveDate, IsActive) VALUES (" +
        std::to_string(menuItemId) + ", " +
        std::to_string(newAmount) + ", CURDATE(), 1)";

    if (mysql_query(conn, insertSql.c_str()) != 0) {
        mysql_query(conn, "ROLLBACK");
        return false;
    }

    mysql_query(conn, "COMMIT");
    return true;
}

std::vector<Price> getPriceHistory(int menuItemId) {
    std::vector<Price> prices;
    MYSQL* conn = getConn();
    if (!conn) return prices;

    std::string sql =
        "SELECT PriceId, MenuItemId, Amount, EffectiveDate, "
        "COALESCE(EndDate, '') AS EndDate, IsActive "
        "FROM PRICE WHERE MenuItemId = " + std::to_string(menuItemId) +
        " ORDER BY EffectiveDate DESC";

    if (mysql_query(conn, sql.c_str()) != 0) return prices;

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return prices;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        Price p;
        p.priceId       = row[0] ? std::stoi(row[0]) : 0;
        p.menuItemId    = row[1] ? std::stoi(row[1]) : 0;
        p.amount        = row[2] ? std::stod(row[2]) : 0.0;
        p.effectiveDate = row[3] ? row[3] : "";
        p.endDate       = row[4] ? row[4] : "";
        p.isActive      = row[5] ? (std::stoi(row[5]) == 1) : false;
        prices.push_back(p);
    }

    mysql_free_result(res);
    return prices;
}

std::vector<Category> getCategories() {
    std::vector<Category> categories;
    MYSQL* conn = getConn();
    if (!conn) return categories;

    std::string sql = "SELECT CategoryId, CategoryName FROM CATEGORY ORDER BY CategoryName";

    if (mysql_query(conn, sql.c_str()) != 0) return categories;

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return categories;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        Category c;
        c.categoryId   = row[0] ? std::stoi(row[0]) : 0;
        c.categoryName = row[1] ? row[1] : "";
        categories.push_back(c);
    }

    mysql_free_result(res);
    return categories;
}

bool addCategory(const std::string& categoryName) {
    MYSQL* conn = getConn();
    if (!conn) return false;

    std::string sql =
        "INSERT INTO CATEGORY (CategoryName) VALUES (" + escapeStr(categoryName) + ")";

    return mysql_query(conn, sql.c_str()) == 0;
}

bool setBranchMenuItemAvailability(int branchId, int menuItemId, bool available) {
    MYSQL* conn = getConn();
    if (!conn) return false;

    std::string sql =
        "INSERT INTO BRANCH_MENU_ITEM (BranchId, MenuItemId, IsAvailable) VALUES (" +
        std::to_string(branchId) + ", " +
        std::to_string(menuItemId) + ", " +
        std::to_string(available ? 1 : 0) + ") "
        "ON DUPLICATE KEY UPDATE IsAvailable = " + std::to_string(available ? 1 : 0);

    return mysql_query(conn, sql.c_str()) == 0;
}

double getItemCurrentPrice(int menuItemId) {
    MYSQL* conn = getConn();
    if (!conn) return 0.0;

    std::string sql =
        "SELECT Amount FROM PRICE "
        "WHERE MenuItemId = " + std::to_string(menuItemId) +
        " AND IsActive = 1 LIMIT 1";

    if (mysql_query(conn, sql.c_str()) != 0) return 0.0;

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return 0.0;

    MYSQL_ROW row = mysql_fetch_row(res);
    double price = (row && row[0]) ? std::stod(row[0]) : 0.0;
    mysql_free_result(res);
    return price;
}

MenuItem getMenuItemById(int menuItemId, int branchId) {
    MenuItem item;
    item.menuItemId = 0;
    MYSQL* conn = getConn();
    if (!conn) return item;

    std::string sql =
        "SELECT mi.MenuItemId, mi.CategoryId, c.CategoryName, mi.ItemName, "
        "mi.Description, mi.MenuItemType, "
        "COALESCE(p.Amount, 0) AS CurrentPrice, "
        "COALESCE(bmi.IsAvailable, 1) AS IsAvailable "
        "FROM MENUITEM mi "
        "JOIN CATEGORY c ON mi.CategoryId = c.CategoryId "
        "LEFT JOIN PRICE p ON mi.MenuItemId = p.MenuItemId AND p.IsActive = 1 "
        "LEFT JOIN BRANCH_MENU_ITEM bmi ON mi.MenuItemId = bmi.MenuItemId AND bmi.BranchId = " +
        std::to_string(branchId) + " "
        "WHERE mi.MenuItemId = " + std::to_string(menuItemId);

    if (mysql_query(conn, sql.c_str()) != 0) return item;

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return item;

    MYSQL_ROW row = mysql_fetch_row(res);
    if (row) {
        item.menuItemId   = row[0] ? std::stoi(row[0]) : 0;
        item.categoryId   = row[1] ? std::stoi(row[1]) : 0;
        item.categoryName = row[2] ? row[2] : "";
        item.itemName     = row[3] ? row[3] : "";
        item.description  = row[4] ? row[4] : "";
        item.menuItemType = row[5] ? row[5] : "";
        item.currentPrice = row[6] ? std::stod(row[6]) : 0.0;
        item.isAvailable  = row[7] ? (std::stoi(row[7]) == 1) : true;
    }

    mysql_free_result(res);
    return item;
}

} // namespace DB