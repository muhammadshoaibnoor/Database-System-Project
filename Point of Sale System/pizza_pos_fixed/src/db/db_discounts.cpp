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

std::vector<Discount> getActiveDiscounts() {
    std::vector<Discount> discounts;
    MYSQL* conn = getConn();
    if (!conn) return discounts;

    std::string sql =
        "SELECT DiscountId, DiscountName, DiscountValue, StartDate, EndDate, IsActive "
        "FROM DISCOUNT "
        "WHERE IsActive = 1 AND CURDATE() BETWEEN StartDate AND EndDate "
        "ORDER BY DiscountName";

    if (mysql_query(conn, sql.c_str()) != 0) return discounts;

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return discounts;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        Discount d;
        d.discountId    = row[0] ? std::stoi(row[0]) : 0;
        d.discountName  = row[1] ? row[1] : "";
        d.discountValue = row[2] ? std::stod(row[2]) : 0.0;
        d.startDate     = row[3] ? row[3] : "";
        d.endDate       = row[4] ? row[4] : "";
        d.isActive      = row[5] ? (std::stoi(row[5]) == 1) : false;
        discounts.push_back(d);
    }

    mysql_free_result(res);
    return discounts;
}

std::vector<Discount> getAllDiscounts() {
    std::vector<Discount> discounts;
    MYSQL* conn = getConn();
    if (!conn) return discounts;

    std::string sql =
        "SELECT DiscountId, DiscountName, DiscountValue, StartDate, EndDate, IsActive "
        "FROM DISCOUNT ORDER BY StartDate DESC";

    if (mysql_query(conn, sql.c_str()) != 0) return discounts;

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return discounts;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        Discount d;
        d.discountId    = row[0] ? std::stoi(row[0]) : 0;
        d.discountName  = row[1] ? row[1] : "";
        d.discountValue = row[2] ? std::stod(row[2]) : 0.0;
        d.startDate     = row[3] ? row[3] : "";
        d.endDate       = row[4] ? row[4] : "";
        d.isActive      = row[5] ? (std::stoi(row[5]) == 1) : false;
        discounts.push_back(d);
    }

    mysql_free_result(res);
    return discounts;
}

bool addDiscount(const std::string& discountName, double discountValue,
                 const std::string& startDate, const std::string& endDate) {
    MYSQL* conn = getConn();
    if (!conn) return false;

    std::string sql =
        "INSERT INTO DISCOUNT (DiscountName, DiscountValue, StartDate, EndDate, IsActive) VALUES (" +
        escapeStr(discountName) + ", " +
        std::to_string(discountValue) + ", " +
        escapeStr(startDate) + ", " +
        escapeStr(endDate) + ", 1)";

    return mysql_query(conn, sql.c_str()) == 0;
}

bool updateDiscount(int discountId, const std::string& discountName, double discountValue,
                    const std::string& startDate, const std::string& endDate) {
    MYSQL* conn = getConn();
    if (!conn) return false;

    std::string sql =
        "UPDATE DISCOUNT SET "
        "DiscountName = " + escapeStr(discountName) + ", "
        "DiscountValue = " + std::to_string(discountValue) + ", "
        "StartDate = " + escapeStr(startDate) + ", "
        "EndDate = " + escapeStr(endDate) + " "
        "WHERE DiscountId = " + std::to_string(discountId);

    return mysql_query(conn, sql.c_str()) == 0;
}

bool isDiscountValid(int discountId) {
    MYSQL* conn = getConn();
    if (!conn) return false;

    std::string sql =
        "SELECT COUNT(*) FROM DISCOUNT "
        "WHERE DiscountId = " + std::to_string(discountId) +
        " AND IsActive = 1 AND CURDATE() BETWEEN StartDate AND EndDate";

    if (mysql_query(conn, sql.c_str()) != 0) return false;

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return false;

    MYSQL_ROW row = mysql_fetch_row(res);
    bool valid = row && row[0] && std::stoi(row[0]) > 0;
    mysql_free_result(res);
    return valid;
}

Discount getDiscountById(int discountId) {
    Discount d;
    d.discountId = 0;
    MYSQL* conn = getConn();
    if (!conn) return d;

    std::string sql =
        "SELECT DiscountId, DiscountName, DiscountValue, StartDate, EndDate, IsActive "
        "FROM DISCOUNT WHERE DiscountId = " + std::to_string(discountId);

    if (mysql_query(conn, sql.c_str()) != 0) return d;

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return d;

    MYSQL_ROW row = mysql_fetch_row(res);
    if (row) {
        d.discountId    = row[0] ? std::stoi(row[0]) : 0;
        d.discountName  = row[1] ? row[1] : "";
        d.discountValue = row[2] ? std::stod(row[2]) : 0.0;
        d.startDate     = row[3] ? row[3] : "";
        d.endDate       = row[4] ? row[4] : "";
        d.isActive      = row[5] ? (std::stoi(row[5]) == 1) : false;
    }

    mysql_free_result(res);
    return d;
}

} // namespace DB