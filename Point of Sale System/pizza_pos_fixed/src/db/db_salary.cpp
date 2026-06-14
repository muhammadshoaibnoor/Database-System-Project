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

std::vector<Salary> getSalaryHistory(const std::string& username) {
    std::vector<Salary> salaries;
    MYSQL* conn = getConn();
    if (!conn) return salaries;

    std::string sql =
        "SELECT s.SalaryId, s.UserName, CONCAT(e.FirstName, ' ', e.LastName) AS EmployeeName, "
        "s.Amount, s.PaymentDate, COALESCE(s.Notes, '') AS Notes "
        "FROM SALARY s "
        "JOIN EMPLOYEE e ON s.UserName = e.UserName WHERE 1=1";

    if (!username.empty()) {
        sql += " AND s.UserName = " + escapeStr(username);
    }
    sql += " ORDER BY s.PaymentDate DESC LIMIT 200";

    if (mysql_query(conn, sql.c_str()) != 0) return salaries;

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return salaries;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        Salary s;
        s.salaryId     = row[0] ? std::stoi(row[0]) : 0;
        s.username     = row[1] ? row[1] : "";
        s.employeeName = row[2] ? row[2] : "";
        s.amount       = row[3] ? std::stod(row[3]) : 0.0;
        s.paymentDate  = row[4] ? row[4] : "";
        s.notes        = row[5] ? row[5] : "";
        salaries.push_back(s);
    }

    mysql_free_result(res);
    return salaries;
}

bool addSalaryPayment(const std::string& username, double amount,
                      const std::string& paymentDate, const std::string& notes) {
    MYSQL* conn = getConn();
    if (!conn) return false;

    std::string sql =
        "INSERT INTO SALARY (UserName, Amount, PaymentDate, Notes) VALUES (" +
        escapeStr(username) + ", " +
        std::to_string(amount) + ", " +
        escapeStr(paymentDate) + ", " +
        escapeStr(notes) + ")";

    return mysql_query(conn, sql.c_str()) == 0;
}

std::vector<Salary> getAllSalaries(int branchId) {
    std::vector<Salary> salaries;
    MYSQL* conn = getConn();
    if (!conn) return salaries;

    std::string sql =
        "SELECT s.SalaryId, s.UserName, CONCAT(e.FirstName, ' ', e.LastName) AS EmployeeName, "
        "s.Amount, s.PaymentDate, COALESCE(s.Notes, '') AS Notes "
        "FROM SALARY s "
        "JOIN EMPLOYEE e ON s.UserName = e.UserName WHERE 1=1";

    if (branchId > 0) {
        sql += " AND e.BranchId = " + std::to_string(branchId);
    }
    sql += " ORDER BY s.PaymentDate DESC LIMIT 500";

    if (mysql_query(conn, sql.c_str()) != 0) return salaries;

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return salaries;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        Salary s;
        s.salaryId     = row[0] ? std::stoi(row[0]) : 0;
        s.username     = row[1] ? row[1] : "";
        s.employeeName = row[2] ? row[2] : "";
        s.amount       = row[3] ? std::stod(row[3]) : 0.0;
        s.paymentDate  = row[4] ? row[4] : "";
        s.notes        = row[5] ? row[5] : "";
        salaries.push_back(s);
    }

    mysql_free_result(res);
    return salaries;
}

double getTotalSalaryPaid(const std::string& username) {
    MYSQL* conn = getConn();
    if (!conn) return 0.0;

    std::string sql =
        "SELECT COALESCE(SUM(Amount), 0) FROM SALARY WHERE UserName = " + escapeStr(username);

    if (mysql_query(conn, sql.c_str()) != 0) return 0.0;

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return 0.0;

    MYSQL_ROW row = mysql_fetch_row(res);
    double total = (row && row[0]) ? std::stod(row[0]) : 0.0;
    mysql_free_result(res);
    return total;
}

} // namespace DB