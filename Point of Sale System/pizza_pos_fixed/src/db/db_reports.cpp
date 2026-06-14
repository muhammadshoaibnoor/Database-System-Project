#include <mysql.h>
#include <string>
#include <vector>
#include <map>
#include <cstring>
#include "../core/models/models.h"

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

// ==================== LOCAL REPORT STRUCTS ====================

struct SalesReport {
    int totalOrders;
    double totalRevenue;
    double totalDiscounts;
    double netRevenue;
    std::vector<std::pair<std::string, int>> topItems;
};

struct InventoryReportItem {
    std::string itemName;
    std::string unit;
    double quantity;
    double reorderLevel;
    bool isLow;
};

struct StaffReportItem {
    std::string username;
    std::string fullName;
    std::string role;
    int totalShifts;
    double totalHours;
};

// ==================== REPORT FUNCTIONS ====================

SalesReport getSalesReport(int branchId, const std::string& dateFrom, const std::string& dateTo) {
    SalesReport report;
    report.totalOrders = 0;
    report.totalRevenue = 0.0;
    report.totalDiscounts = 0.0;
    report.netRevenue = 0.0;

    MYSQL* conn = getConn();
    if (!conn) return report;

    // Build WHERE clause for branch/date filtering
    std::string whereClause = "WHERE 1=1";
    if (branchId > 0) {
        whereClause += " AND e.BranchId = " + std::to_string(branchId);
    }
    if (!dateFrom.empty()) {
        whereClause += " AND DATE(o.OrderDateTime) >= " + escapeStr(dateFrom);
    }
    if (!dateTo.empty()) {
        whereClause += " AND DATE(o.OrderDateTime) <= " + escapeStr(dateTo);
    }

    // Totals query
    std::string totalSql =
        "SELECT COUNT(o.OrderId), "
        "COALESCE(SUM(o.TotalAmount), 0), "
        "COALESCE(SUM(o.DiscountAmount), 0) "
        "FROM `ORDER` o "
        "JOIN EMPLOYEE e ON o.TakenBy = e.UserName " +
        whereClause + " AND o.Status != 'Cancelled'";

    if (mysql_query(conn, totalSql.c_str()) == 0) {
        MYSQL_RES* res = mysql_store_result(conn);
        if (res) {
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row) {
                report.totalOrders    = row[0] ? std::stoi(row[0]) : 0;
                report.totalRevenue   = row[1] ? std::stod(row[1]) : 0.0;
                report.totalDiscounts = row[2] ? std::stod(row[2]) : 0.0;
                report.netRevenue     = report.totalRevenue - report.totalDiscounts;
            }
            mysql_free_result(res);
        }
    }

    // Top 5 items query
    std::string topSql =
        "SELECT COALESCE(mi.ItemName, d.DealName) AS ItemName, "
        "SUM(oi.Quantity) AS TotalQty "
        "FROM ORDER_ITEM oi "
        "JOIN `ORDER` o ON oi.OrderId = o.OrderId "
        "JOIN EMPLOYEE e ON o.TakenBy = e.UserName "
        "LEFT JOIN MENUITEM mi ON oi.MenuItemId = mi.MenuItemId "
        "LEFT JOIN DEAL d ON oi.DealId = d.DealId " +
        whereClause + " AND o.Status != 'Cancelled' "
        "GROUP BY ItemName "
        "ORDER BY TotalQty DESC LIMIT 5";

    if (mysql_query(conn, topSql.c_str()) == 0) {
        MYSQL_RES* res = mysql_store_result(conn);
        if (res) {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res))) {
                std::string name = row[0] ? row[0] : "Unknown";
                int qty = row[1] ? std::stoi(row[1]) : 0;
                report.topItems.push_back({name, qty});
            }
            mysql_free_result(res);
        }
    }

    return report;
}

std::vector<InventoryReportItem> getInventoryReport(int branchId) {
    std::vector<InventoryReportItem> items;
    MYSQL* conn = getConn();
    if (!conn) return items;

    std::string sql =
        "SELECT InventoryName, Unit, Quantity, ReorderLevel "
        "FROM INVENTORY WHERE BranchId = " + std::to_string(branchId) +
        " ORDER BY InventoryName";

    if (mysql_query(conn, sql.c_str()) != 0) return items;

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return items;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        InventoryReportItem item;
        item.itemName     = row[0] ? row[0] : "";
        item.unit         = row[1] ? row[1] : "";
        item.quantity     = row[2] ? std::stod(row[2]) : 0.0;
        item.reorderLevel = row[3] ? std::stod(row[3]) : 0.0;
        item.isLow        = (item.quantity <= item.reorderLevel);
        items.push_back(item);
    }

    mysql_free_result(res);
    return items;
}

std::vector<StaffReportItem> getStaffReport(int branchId) {
    std::vector<StaffReportItem> staff;
    MYSQL* conn = getConn();
    if (!conn) return staff;

    std::string sql =
        "SELECT e.UserName, CONCAT(e.FirstName, ' ', e.LastName) AS FullName, e.Role, "
        "COUNT(s.ShiftId) AS TotalShifts, "
        "COALESCE(SUM(TIMESTAMPDIFF(MINUTE, s.StartTime, COALESCE(s.EndTime, s.StartTime))) / 60.0, 0) AS TotalHours "
        "FROM EMPLOYEE e "
        "LEFT JOIN SHIFT s ON e.UserName = s.UserName "
        "WHERE e.IsActive = 1";

    if (branchId > 0) {
        sql += " AND e.BranchId = " + std::to_string(branchId);
    }
    sql += " GROUP BY e.UserName, FullName, e.Role ORDER BY e.Role, FullName";

    if (mysql_query(conn, sql.c_str()) != 0) return staff;

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return staff;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        StaffReportItem s;
        s.username    = row[0] ? row[0] : "";
        s.fullName    = row[1] ? row[1] : "";
        s.role        = row[2] ? row[2] : "";
        s.totalShifts = row[3] ? std::stoi(row[3]) : 0;
        s.totalHours  = row[4] ? std::stod(row[4]) : 0.0;
        staff.push_back(s);
    }

    mysql_free_result(res);
    return staff;
}

} // namespace DB