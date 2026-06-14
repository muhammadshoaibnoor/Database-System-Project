#include <mysql.h>
#include <string>
#include <vector>
#include <cstring>
#include <iostream>
#include "../core/models/models.h"
#include "db.h"

extern MYSQL* getConn();

namespace DB {

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

static void populateDealRelations(Deal& d) {
    MYSQL* conn = getConn();
    if (!conn) return;

    d.menuItemIds.clear();
    d.branchIds.clear();
    d.branchNames.clear();

    std::string itemSql =
        "SELECT MenuItemId FROM DEAL_ITEM WHERE DealId = " + std::to_string(d.dealId);
    if (mysql_query(conn, itemSql.c_str()) == 0) {
        MYSQL_RES* res = mysql_store_result(conn);
        if (res) {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res))) {
                if (row[0]) d.menuItemIds.push_back(std::stoi(row[0]));
            }
            mysql_free_result(res);
        }
    }

    std::string branchSql =
        "SELECT bd.BranchId, b.BranchName FROM BRANCH_DEAL bd "
        "JOIN BRANCH b ON bd.BranchId = b.BranchId "
        "WHERE bd.DealId = " + std::to_string(d.dealId) + " AND bd.IsAvailable = 1 "
        "ORDER BY b.BranchName";
    if (mysql_query(conn, branchSql.c_str()) == 0) {
        MYSQL_RES* res = mysql_store_result(conn);
        if (res) {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res))) {
                if (row[0]) d.branchIds.push_back(std::stoi(row[0]));
                if (row[1]) d.branchNames.push_back(row[1]);
            }
            mysql_free_result(res);
        }
    }
}

std::vector<Deal> getDeals(int branchId) {
    std::vector<Deal> deals;
    MYSQL* conn = getConn();
    if (!conn) return deals;

    std::string sql =
        "SELECT d.DealId, d.DealName, d.TotalAmount, d.IsActive, "
        "COALESCE(bd.IsAvailable, 0) AS BranchAvailable "
        "FROM DEAL d "
        "LEFT JOIN BRANCH_DEAL bd ON d.DealId = bd.DealId AND bd.BranchId = " +
        std::to_string(branchId) + " "
        "WHERE d.IsActive = 1 "
        "ORDER BY d.DealName";

    if (mysql_query(conn, sql.c_str()) != 0) return deals;

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return deals;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        Deal d;
        d.dealId      = row[0] ? std::stoi(row[0]) : 0;
        d.dealName    = row[1] ? row[1] : "";
        d.totalAmount = row[2] ? std::stod(row[2]) : 0.0;
        d.isActive    = row[3] ? (std::stoi(row[3]) == 1) : false;
        int branchAvail = row[4] ? std::stoi(row[4]) : 0;
        if (branchAvail == 1) {
            populateDealRelations(d);
            deals.push_back(d);
        }
    }

    mysql_free_result(res);
    return deals;
}

std::vector<Deal> getAvailableDeals(int branchId) {
    std::vector<Deal> all = getDeals(branchId);
    std::vector<Deal> available;
    for (auto& deal : all) {
        if (isDealAvailableAtBranch(deal.dealId, branchId)) {
            available.push_back(deal);
        }
    }
    return available;
}

std::vector<DealItem> getDealItems(int dealId) {
    std::vector<DealItem> items;
    MYSQL* conn = getConn();
    if (!conn) return items;

    std::string sql =
        "SELECT di.DealItemId, di.DealId, di.MenuItemId, di.Quantity, mi.ItemName "
        "FROM DEAL_ITEM di "
        "JOIN menuitem mi ON di.MenuItemId = mi.MenuItemId "
        "WHERE di.DealId = " + std::to_string(dealId) +
        " ORDER BY mi.ItemName";

    if (mysql_query(conn, sql.c_str()) != 0) return items;

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return items;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        DealItem di;
        di.dealItemId = row[0] ? std::stoi(row[0]) : 0;
        di.dealId     = row[1] ? std::stoi(row[1]) : 0;
        di.menuItemId = row[2] ? std::stoi(row[2]) : 0;
        di.quantity   = row[3] ? std::stoi(row[3]) : 0;
        di.itemName   = row[4] ? row[4] : "";
        items.push_back(di);
    }

    mysql_free_result(res);
    return items;
}

bool isDealAvailableAtBranch(int dealId, int branchId) {
    MYSQL* conn = getConn();
    if (!conn) return false;

    std::string branchSql =
        "SELECT IsAvailable FROM BRANCH_DEAL "
        "WHERE DealId = " + std::to_string(dealId) +
        " AND BranchId = " + std::to_string(branchId);

    if (mysql_query(conn, branchSql.c_str()) != 0) return false;

    MYSQL_RES* branchRes = mysql_store_result(conn);
    if (!branchRes) return false;

    MYSQL_ROW branchRow = mysql_fetch_row(branchRes);
    if (!branchRow || (branchRow[0] && std::stoi(branchRow[0]) != 1)) {
        mysql_free_result(branchRes);
        return false;
    }
    mysql_free_result(branchRes);

    std::string sql =
        "SELECT mi.InventoryId, di.Quantity, i.Quantity AS StockQty "
        "FROM DEAL_ITEM di "
        "JOIN menu_ingredient mi ON di.MenuItemId = mi.MenuItemId "
        "JOIN INVENTORY i ON mi.InventoryId = i.InventoryId AND i.BranchId = " +
        std::to_string(branchId) + " "
        "WHERE di.DealId = " + std::to_string(dealId);

    if (mysql_query(conn, sql.c_str()) != 0) return false;

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return false;

    bool available = true;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        double required = row[1] ? std::stod(row[1]) : 0.0;
        double stock    = row[2] ? std::stod(row[2]) : 0.0;
        if (stock < required) {
            available = false;
            break;
        }
    }

    mysql_free_result(res);
    return available;
}

bool createDeal(const std::string& dealName, double totalAmount,
                const std::vector<std::pair<int, int>>& items,
                const std::vector<int>& branchIds) {
    MYSQL* conn = getConn();
    if (!conn) return false;

    mysql_query(conn, "START TRANSACTION");

    std::string dealSql =
        "INSERT INTO DEAL (DealName, TotalAmount, IsActive) VALUES (" +
        escapeStr(dealName) + ", " + std::to_string(totalAmount) + ", 1)";

    if (mysql_query(conn, dealSql.c_str()) != 0) {
        mysql_query(conn, "ROLLBACK");
        return false;
    }

    int dealId = static_cast<int>(mysql_insert_id(conn));

    for (const auto& item : items) {
        std::string itemSql =
            "INSERT INTO DEAL_ITEM (DealId, MenuItemId, Quantity) VALUES (" +
            std::to_string(dealId) + ", " +
            std::to_string(item.first) + ", " +
            std::to_string(item.second) + ")";
        if (mysql_query(conn, itemSql.c_str()) != 0) {
            mysql_query(conn, "ROLLBACK");
            return false;
        }
    }

    for (int brId : branchIds) {
        std::string branchSql =
            "INSERT INTO BRANCH_DEAL (BranchId, DealId, IsAvailable) VALUES (" +
            std::to_string(brId) + ", " + std::to_string(dealId) + ", 1)";
        if (mysql_query(conn, branchSql.c_str()) != 0) {
            mysql_query(conn, "ROLLBACK");
            return false;
        }
    }

    mysql_query(conn, "COMMIT");
    return true;
}

bool updateDeal(int dealId, const std::string& dealName, double totalAmount,
                const std::vector<std::pair<int, int>>& items,
                const std::vector<int>& branchIds) {
    MYSQL* conn = getConn();
    if (!conn) return false;

    mysql_query(conn, "START TRANSACTION");

    std::string updateSql =
        "UPDATE DEAL SET DealName = " + escapeStr(dealName) +
        ", TotalAmount = " + std::to_string(totalAmount) +
        " WHERE DealId = " + std::to_string(dealId);

    if (mysql_query(conn, updateSql.c_str()) != 0) {
        mysql_query(conn, "ROLLBACK");
        return false;
    }

    mysql_query(conn, ("DELETE FROM DEAL_ITEM WHERE DealId = " + std::to_string(dealId)).c_str());

    for (const auto& item : items) {
        std::string itemSql =
            "INSERT INTO DEAL_ITEM (DealId, MenuItemId, Quantity) VALUES (" +
            std::to_string(dealId) + ", " +
            std::to_string(item.first) + ", " +
            std::to_string(item.second) + ")";
        if (mysql_query(conn, itemSql.c_str()) != 0) {
            mysql_query(conn, "ROLLBACK");
            return false;
        }
    }

    mysql_query(conn, ("DELETE FROM BRANCH_DEAL WHERE DealId = " + std::to_string(dealId)).c_str());

    for (int brId : branchIds) {
        std::string branchSql =
            "INSERT INTO BRANCH_DEAL (BranchId, DealId, IsAvailable) VALUES (" +
            std::to_string(brId) + ", " + std::to_string(dealId) + ", 1)";
        if (mysql_query(conn, branchSql.c_str()) != 0) {
            mysql_query(conn, "ROLLBACK");
            return false;
        }
    }

    mysql_query(conn, "COMMIT");
    return true;
}

bool setBranchDealAvailability(int branchId, int dealId, bool available) {
    MYSQL* conn = getConn();
    if (!conn) return false;

    std::string sql =
        "INSERT INTO BRANCH_DEAL (BranchId, DealId, IsAvailable) VALUES (" +
        std::to_string(branchId) + ", " +
        std::to_string(dealId) + ", " +
        std::to_string(available ? 1 : 0) + ") "
        "ON DUPLICATE KEY UPDATE IsAvailable = " + std::to_string(available ? 1 : 0);

    return mysql_query(conn, sql.c_str()) == 0;
}

Deal getDealById(int dealId) {
    Deal d;
    d.dealId = 0;
    MYSQL* conn = getConn();
    if (!conn) return d;

    std::string sql =
        "SELECT DealId, DealName, TotalAmount, IsActive "
        "FROM DEAL WHERE DealId = " + std::to_string(dealId);

    if (mysql_query(conn, sql.c_str()) != 0) return d;

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return d;

    MYSQL_ROW row = mysql_fetch_row(res);
    if (row) {
        d.dealId      = row[0] ? std::stoi(row[0]) : 0;
        d.dealName    = row[1] ? row[1] : "";
        d.totalAmount = row[2] ? std::stod(row[2]) : 0.0;
        d.isActive    = row[3] ? (std::stoi(row[3]) == 1) : false;
        populateDealRelations(d);
    }

    mysql_free_result(res);
    return d;
}

bool toggleDealActive(int dealId, bool isActive) {
    MYSQL* conn = getConn();
    if (!conn) return false;

    std::string sql =
        "UPDATE DEAL SET IsActive = " + std::to_string(isActive ? 1 : 0) +
        " WHERE DealId = " + std::to_string(dealId);

    return mysql_query(conn, sql.c_str()) == 0;
}

std::vector<Deal> getAllDeals() {
    std::vector<Deal> deals;
    MYSQL* conn = getConn();
    if (!conn) return deals;

    std::string sql =
        "SELECT DealId, DealName, TotalAmount, IsActive "
        "FROM DEAL WHERE IsActive = 1 ORDER BY DealName";

    if (mysql_query(conn, sql.c_str()) != 0) return deals;

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return deals;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        Deal d;
        d.dealId      = row[0] ? std::stoi(row[0]) : 0;
        d.dealName    = row[1] ? row[1] : "";
        d.totalAmount = row[2] ? std::stod(row[2]) : 0.0;
        d.isActive    = row[3] ? (std::stoi(row[3]) == 1) : false;
        populateDealRelations(d);
        deals.push_back(d);
    }

    mysql_free_result(res);
    return deals;
}

} // namespace DB