#include <mysql.h>
#include <string>
#include <cstring>
#include "../core/models/models.h"

// Shared connection from db.cpp — NOT defined here, only declared
extern MYSQL* getConn();

namespace DB {

// ==================== HELPER: Escape String ====================

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

// ==================== AUTHENTICATION FUNCTIONS ====================

Employee loginUser(const std::string& username, const std::string& password) {
    Employee emp;
    emp.username = ""; // Sentinel for "not found / failed"

    MYSQL* conn = getConn();
    if (!conn) return emp;

    std::string sql = "SELECT e.UserName, e.FirstName, e.LastName, e.Role, "
                      "e.BranchId, b.BranchName "
                      "FROM EMPLOYEE e "
                      "JOIN BRANCH b ON e.BranchId = b.BranchId "
                      "WHERE e.UserName = " + escapeStr(username) +
                      " AND e.Password = " + escapeStr(password) +
                      " AND e.IsActive = 1";

    if (mysql_query(conn, sql.c_str()) != 0) {
        return emp;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return emp;

    MYSQL_ROW row = mysql_fetch_row(res);
    if (row) {
        emp.username    = row[0] ? row[0] : "";
        emp.firstName   = row[1] ? row[1] : "";
        emp.lastName    = row[2] ? row[2] : "";
        emp.role        = row[3] ? row[3] : "";
        emp.branchId    = row[4] ? std::stoi(row[4]) : -1;
        emp.branchName  = row[5] ? row[5] : "";
        emp.isActive    = true;
    }

    mysql_free_result(res);
    return emp;
}

bool changePassword(const std::string& username, const std::string& newPassword) {
    MYSQL* conn = getConn();
    if (!conn) return false;

    std::string sql = "UPDATE EMPLOYEE SET Password = " + escapeStr(newPassword) +
                      " WHERE UserName = " + escapeStr(username);

    return mysql_query(conn, sql.c_str()) == 0;
}

// getEmployeeByUsername is implemented in db_staff.cpp
} // namespace DB