#include <mysql.h>
#include <string>
#include <vector>
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

std::vector<Branch> getAllBranches() {
    std::vector<Branch> branches;
    MYSQL* conn = getConn();
    if (!conn) return branches;

    std::string sql =
        "SELECT BranchId, BranchName, Address, PhoneNumber, Email, "
        "OpeningTime, ClosingTime, IsActive "
        "FROM BRANCH WHERE IsActive = 1 ORDER BY BranchName";

    if (mysql_query(conn, sql.c_str()) != 0) return branches;

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return branches;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        Branch b;
        b.branchId    = row[0] ? std::stoi(row[0]) : 0;
        b.branchName  = row[1] ? row[1] : "";
        b.address     = row[2] ? row[2] : "";
        b.phone       = row[3] ? row[3] : "";
        b.email       = row[4] ? row[4] : "";
        b.openingTime = row[5] ? row[5] : "";
        b.closingTime = row[6] ? row[6] : "";
        b.isActive    = row[7] ? (std::stoi(row[7]) == 1) : false;
        branches.push_back(b);
    }

    mysql_free_result(res);
    return branches;
}

Branch getBranchById(int branchId) {
    Branch b;
    b.branchId = -1;
    MYSQL* conn = getConn();
    if (!conn) return b;

    std::string sql =
        "SELECT BranchId, BranchName, Address, PhoneNumber, Email, "
        "OpeningTime, ClosingTime, IsActive "
        "FROM BRANCH WHERE BranchId = " + std::to_string(branchId);

    if (mysql_query(conn, sql.c_str()) != 0) return b;

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return b;

    MYSQL_ROW row = mysql_fetch_row(res);
    if (row) {
        b.branchId    = row[0] ? std::stoi(row[0]) : 0;
        b.branchName  = row[1] ? row[1] : "";
        b.address     = row[2] ? row[2] : "";
        b.phone       = row[3] ? row[3] : "";
        b.email       = row[4] ? row[4] : "";
        b.openingTime = row[5] ? row[5] : "";
        b.closingTime = row[6] ? row[6] : "";
        b.isActive    = row[7] ? (std::stoi(row[7]) == 1) : false;
    }

    mysql_free_result(res);
    return b;
}

bool addBranch(const std::string& branchName, const std::string& address,
               const std::string& phone, const std::string& email,
               const std::string& openingTime, const std::string& closingTime) {
    MYSQL* conn = getConn();
    if (!conn) return false;

    std::string sql =
        "INSERT INTO BRANCH (BranchName, Address, PhoneNumber, Email, OpeningTime, ClosingTime, IsActive) VALUES (" +
        escapeStr(branchName) + ", " +
        escapeStr(address) + ", " +
        escapeStr(phone) + ", " +
        escapeStr(email) + ", " +
        escapeStr(openingTime) + ", " +
        escapeStr(closingTime) + ", 1)";

    return mysql_query(conn, sql.c_str()) == 0;
}

bool updateBranch(int branchId, const std::string& branchName, const std::string& address,
                  const std::string& phone, const std::string& email) {
    MYSQL* conn = getConn();
    if (!conn) return false;

    std::string sql =
        "UPDATE BRANCH SET "
        "BranchName = " + escapeStr(branchName) + ", "
        "Address = " + escapeStr(address) + ", "
        "PhoneNumber = " + escapeStr(phone) + ", "
        "Email = " + escapeStr(email) + " "
        "WHERE BranchId = " + std::to_string(branchId);

    return mysql_query(conn, sql.c_str()) == 0;
}

bool deactivateBranch(int branchId) {
    MYSQL* conn = getConn();
    if (!conn) return false;

    std::string sql =
        "UPDATE BRANCH SET IsActive = 0 WHERE BranchId = " + std::to_string(branchId);

    return mysql_query(conn, sql.c_str()) == 0;
}

} // namespace DB