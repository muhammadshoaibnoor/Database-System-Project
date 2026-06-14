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

// ==================== INVENTORY FUNCTIONS ====================

std::vector<Inventory> getInventory(int branchId) {
    std::vector<Inventory> items;
    MYSQL* conn = getConn();
    if (!conn) return items;

    std::string sql =
        "SELECT InventoryId, BranchId, InventoryName, Quantity, Unit, "
        "ReorderLevel, UnitCost, "
        "COALESCE(DATE_FORMAT(LastRestocked, '%Y-%m-%d %H:%i'), '') AS LastRestocked "
        "FROM INVENTORY WHERE BranchId = " + std::to_string(branchId) +
        " ORDER BY InventoryName";

    if (mysql_query(conn, sql.c_str()) != 0) return items;

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return items;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        Inventory inv;
        inv.inventoryId   = row[0] ? std::stoi(row[0]) : 0;
        inv.branchId      = row[1] ? std::stoi(row[1]) : 0;
        inv.inventoryName = row[2] ? row[2] : "";
        inv.quantity      = row[3] ? std::stod(row[3]) : 0.0;
        inv.unit          = row[4] ? row[4] : "";
        inv.reorderLevel  = row[5] ? std::stod(row[5]) : 0.0;
        inv.unitCost      = row[6] ? std::stod(row[6]) : 0.0;
        inv.lastRestocked = row[7] ? row[7] : "";
        inv.isLowStock    = (inv.quantity <= inv.reorderLevel);
        items.push_back(inv);
    }

    mysql_free_result(res);
    return items;
}

std::vector<Inventory> getLowStockItems(int branchId) {
    std::vector<Inventory> all = getInventory(branchId);
    std::vector<Inventory> lowStock;
    for (const auto& item : all) {
        if (item.isLowStock) {
            lowStock.push_back(item);
        }
    }
    return lowStock;
}

std::vector<LowStockAlert> getLowStockAlerts(int branchId) {
    std::vector<LowStockAlert> alerts;
    MYSQL* conn = getConn();
    if (!conn) return alerts;

    std::string sql =
        "SELECT InventoryId, BranchId, InventoryName, Quantity, ReorderLevel, Unit "
        "FROM INVENTORY WHERE BranchId = " + std::to_string(branchId) +
        " AND Quantity <= ReorderLevel ORDER BY InventoryName";

    if (mysql_query(conn, sql.c_str()) != 0) return alerts;

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return alerts;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        LowStockAlert alert;
        alert.inventoryId   = row[0] ? std::stoi(row[0]) : 0;
        alert.branchId      = row[1] ? std::stoi(row[1]) : 0;
        alert.inventoryName = row[2] ? row[2] : "";
        alert.quantity      = row[3] ? std::stod(row[3]) : 0.0;
        alert.reorderLevel  = row[4] ? std::stod(row[4]) : 0.0;
        alert.unit          = row[5] ? row[5] : "";
        alerts.push_back(alert);
    }

    mysql_free_result(res);
    return alerts;
}

bool addInventoryItem(int branchId, const std::string& inventoryName, double quantity,
                      const std::string& unit, double reorderLevel, double unitCost) {
    MYSQL* conn = getConn();
    if (!conn) return false;

    std::string sql =
        "INSERT INTO INVENTORY (BranchId, InventoryName, Quantity, Unit, ReorderLevel, UnitCost, LastRestocked) VALUES (" +
        std::to_string(branchId) + ", " +
        escapeStr(inventoryName) + ", " +
        std::to_string(quantity) + ", " +
        escapeStr(unit) + ", " +
        std::to_string(reorderLevel) + ", " +
        std::to_string(unitCost) + ", NOW())";

    return mysql_query(conn, sql.c_str()) == 0;
}

bool updateStock(int inventoryId, double addQuantity) {
    MYSQL* conn = getConn();
    if (!conn) return false;

    std::string sql =
        "UPDATE INVENTORY SET Quantity = Quantity + " + std::to_string(addQuantity) +
        ", LastRestocked = NOW() WHERE InventoryId = " + std::to_string(inventoryId);

    if (mysql_query(conn, sql.c_str()) != 0) return false;

    // Check if any row was affected
    return mysql_affected_rows(conn) > 0;
}

bool updateReorderLevel(int inventoryId, double newLevel) {
    MYSQL* conn = getConn();
    if (!conn) return false;

    std::string sql =
        "UPDATE INVENTORY SET ReorderLevel = " + std::to_string(newLevel) +
        " WHERE InventoryId = " + std::to_string(inventoryId);

    return mysql_query(conn, sql.c_str()) == 0;
}

bool updateInventoryItem(int inventoryId, const std::string& name,
                         double reorderLevel, double unitCost) {
    MYSQL* conn = getConn();
    if (!conn) return false;

    std::string sql =
        "UPDATE INVENTORY SET InventoryName = " + escapeStr(name) +
        ", ReorderLevel = " + std::to_string(reorderLevel) +
        ", UnitCost = " + std::to_string(unitCost) +
        " WHERE InventoryId = " + std::to_string(inventoryId);

    return mysql_query(conn, sql.c_str()) == 0;
}

bool checkStockSufficiency(int menuItemId, int branchId, double requiredQty) {
    MYSQL* conn = getConn();
    if (!conn) return false;

    // Check all ingredients for this menu item have sufficient stock
    std::string sql =
        "SELECT mi.InventoryId, i.Quantity "
        "FROM MENU_INGREDIENT mi "
        "JOIN INVENTORY i ON mi.InventoryId = i.InventoryId "
        "WHERE mi.MenuItemId = " + std::to_string(menuItemId) +
        " AND i.BranchId = " + std::to_string(branchId);

    if (mysql_query(conn, sql.c_str()) != 0) return false;

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return false;

    bool sufficient = true;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        double available = row[1] ? std::stod(row[1]) : 0.0;
        if (available < requiredQty) {
            sufficient = false;
            break;
        }
    }

    mysql_free_result(res);
    return sufficient;
}

bool deductStock(int inventoryId, double quantity) {
    MYSQL* conn = getConn();
    if (!conn) return false;

    std::string sql =
        "UPDATE INVENTORY SET Quantity = Quantity - " + std::to_string(quantity) +
        " WHERE InventoryId = " + std::to_string(inventoryId) +
        " AND Quantity >= " + std::to_string(quantity);

    if (mysql_query(conn, sql.c_str()) != 0) return false;

    // Return false if stock was insufficient (0 rows affected)
    return mysql_affected_rows(conn) > 0;
}

bool restoreStock(int inventoryId, double quantity) {
    MYSQL* conn = getConn();
    if (!conn) return false;

    std::string sql =
        "UPDATE INVENTORY SET Quantity = Quantity + " + std::to_string(quantity) +
        " WHERE InventoryId = " + std::to_string(inventoryId);

    return mysql_query(conn, sql.c_str()) == 0;
}

Inventory getInventoryById(int inventoryId) {
    Inventory inv;
    inv.inventoryId = -1; // Sentinel for "not found"
    MYSQL* conn = getConn();
    if (!conn) return inv;

    std::string sql =
        "SELECT InventoryId, BranchId, InventoryName, Quantity, Unit, "
        "ReorderLevel, UnitCost, "
        "COALESCE(DATE_FORMAT(LastRestocked, '%Y-%m-%d %H:%i'), '') AS LastRestocked "
        "FROM INVENTORY WHERE InventoryId = " + std::to_string(inventoryId);

    if (mysql_query(conn, sql.c_str()) != 0) return inv;

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return inv;

    MYSQL_ROW row = mysql_fetch_row(res);
    if (row) {
        inv.inventoryId   = row[0] ? std::stoi(row[0]) : 0;
        inv.branchId      = row[1] ? std::stoi(row[1]) : 0;
        inv.inventoryName = row[2] ? row[2] : "";
        inv.quantity      = row[3] ? std::stod(row[3]) : 0.0;
        inv.unit          = row[4] ? row[4] : "";
        inv.reorderLevel  = row[5] ? std::stod(row[5]) : 0.0;
        inv.unitCost      = row[6] ? std::stod(row[6]) : 0.0;
        inv.lastRestocked = row[7] ? row[7] : "";
        inv.isLowStock    = (inv.quantity <= inv.reorderLevel);
    }

    mysql_free_result(res);
    return inv;
}

std::vector<Inventory> getIngredientsByMenuItem(int menuItemId, int branchId) {
    std::vector<Inventory> ingredients;
    MYSQL* conn = getConn();
    if (!conn) return ingredients;

    std::string sql =
        "SELECT i.InventoryId, i.BranchId, i.InventoryName, i.Quantity, i.Unit, "
        "i.ReorderLevel, i.UnitCost, "
        "COALESCE(DATE_FORMAT(i.LastRestocked, '%Y-%m-%d %H:%i'), '') AS LastRestocked "
        "FROM MENU_INGREDIENT mi "
        "JOIN INVENTORY i ON mi.InventoryId = i.InventoryId "
        "WHERE mi.MenuItemId = " + std::to_string(menuItemId) +
        " AND i.BranchId = " + std::to_string(branchId) +
        " ORDER BY i.InventoryName";

    if (mysql_query(conn, sql.c_str()) != 0) return ingredients;

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return ingredients;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        Inventory inv;
        inv.inventoryId   = row[0] ? std::stoi(row[0]) : 0;
        inv.branchId      = row[1] ? std::stoi(row[1]) : 0;
        inv.inventoryName = row[2] ? row[2] : "";
        inv.quantity      = row[3] ? std::stod(row[3]) : 0.0;
        inv.unit          = row[4] ? row[4] : "";
        inv.reorderLevel  = row[5] ? std::stod(row[5]) : 0.0;
        inv.unitCost      = row[6] ? std::stod(row[6]) : 0.0;
        inv.lastRestocked = row[7] ? row[7] : "";
        inv.isLowStock    = (inv.quantity <= inv.reorderLevel);
        ingredients.push_back(inv);
    }

    mysql_free_result(res);
    return ingredients;
}

} // namespace DB