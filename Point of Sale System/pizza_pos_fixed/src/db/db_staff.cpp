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

std::vector<Employee> getAllEmployees(int branchId) {
    std::vector<Employee> employees;
    MYSQL* conn = getConn();
    if (!conn) return employees;

    std::string sql =
        "SELECT e.UserName, e.ManagedBy, e.BranchId, e.FirstName, e.LastName, "
        "e.Email, e.PhoneNumber, e.Role, e.IsActive, e.HireDate, "
        "COALESCE(e.ResignationDate, ''), COALESCE(b.BranchName, '') "
        "FROM EMPLOYEE e "
        "LEFT JOIN BRANCH b ON e.BranchId = b.BranchId "
        "WHERE e.IsActive = 1";

    if (branchId > 0) {
        sql += " AND e.BranchId = " + std::to_string(branchId);
    }
    sql += " ORDER BY e.FirstName, e.LastName";

    if (mysql_query(conn, sql.c_str()) != 0) return employees;

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return employees;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        Employee e;
        e.username     = row[0]  ? row[0]  : "";
        e.managedBy    = row[1]  ? row[1]  : "";
        e.branchId     = row[2]  ? std::stoi(row[2]) : -1;
        e.firstName    = row[3]  ? row[3]  : "";
        e.lastName     = row[4]  ? row[4]  : "";
        e.email        = row[5]  ? row[5]  : "";
        e.phone        = row[6]  ? row[6]  : "";
        e.role         = row[7]  ? row[7]  : "";
        e.isActive     = row[8]  ? (std::stoi(row[8]) == 1) : false;
        e.hireDate     = row[9]  ? row[9]  : "";
        e.resignationDate = row[10] ? row[10] : "";
        e.branchName   = row[11] ? row[11] : "";
        employees.push_back(e);
    }

    mysql_free_result(res);
    return employees;
}

std::vector<Employee> getEmployeesByBranch(int branchId) {
    return getAllEmployees(branchId);
}

std::vector<Employee> getKitchenStaffByBranch(int branchId) {
    std::vector<Employee> all = getAllEmployees(branchId);
    std::vector<Employee> kitchen;
    for (const auto& e : all) {
        if (e.role == "KitchenStaff" && e.isActive) {
            kitchen.push_back(e);
        }
    }
    return kitchen;
}

bool addEmployee(const std::string& username, const std::string& managedBy,
                 int branchId, const std::string& firstName, const std::string& lastName,
                 const std::string& email, const std::string& phone,
                 const std::string& password, const std::string& role) {
    MYSQL* conn = getConn();
    if (!conn) return false;

    std::string emailVal = email.empty() ? "NULL" : escapeStr(email);
    std::string phoneVal = phone.empty() ? "NULL" : escapeStr(phone);
    std::string managedVal = managedBy.empty() ? "NULL" : escapeStr(managedBy);

    std::string sql =
        "INSERT INTO EMPLOYEE (UserName, ManagedBy, BranchId, FirstName, LastName, "
        "Email, PhoneNumber, Password, Role, IsActive, HireDate) VALUES (" +
        escapeStr(username) + ", " +
        managedVal + ", " +
        std::to_string(branchId) + ", " +
        escapeStr(firstName) + ", " +
        escapeStr(lastName) + ", " +
        emailVal + ", " +
        phoneVal + ", " +
        escapeStr(password) + ", " +
        escapeStr(role) + ", 1, CURDATE())";

    return mysql_query(conn, sql.c_str()) == 0;
}

bool updateEmployee(const std::string& username, const std::string& firstName,
                    const std::string& lastName, const std::string& email,
                    const std::string& phone, const std::string& role, int branchId) {
    MYSQL* conn = getConn();
    if (!conn) return false;

    std::string emailVal = email.empty() ? "NULL" : escapeStr(email);
    std::string phoneVal = phone.empty() ? "NULL" : escapeStr(phone);

    std::string sql =
        "UPDATE EMPLOYEE SET "
        "FirstName = " + escapeStr(firstName) + ", "
        "LastName = " + escapeStr(lastName) + ", "
        "Email = " + emailVal + ", "
        "PhoneNumber = " + phoneVal + ", "
        "Role = " + escapeStr(role) + ", "
        "BranchId = " + std::to_string(branchId) + " "
        "WHERE UserName = " + escapeStr(username);

    return mysql_query(conn, sql.c_str()) == 0;
}

bool deactivateEmployee(const std::string& username) {
    MYSQL* conn = getConn();
    if (!conn) return false;

    std::string sql =
        "UPDATE EMPLOYEE SET IsActive = 0, ResignationDate = CURDATE() "
        "WHERE UserName = " + escapeStr(username);

    return mysql_query(conn, sql.c_str()) == 0;
}

Employee getEmployeeByUsername(const std::string& username) {
    Employee emp;
    emp.username = "";
    MYSQL* conn = getConn();
    if (!conn) return emp;

    std::string sql =
        "SELECT e.UserName, e.ManagedBy, e.BranchId, e.FirstName, e.LastName, "
        "e.Email, e.PhoneNumber, e.Role, e.IsActive, e.HireDate, "
        "COALESCE(e.ResignationDate, ''), COALESCE(b.BranchName, '') "
        "FROM EMPLOYEE e "
        "LEFT JOIN BRANCH b ON e.BranchId = b.BranchId "
        "WHERE e.UserName = " + escapeStr(username);

    if (mysql_query(conn, sql.c_str()) != 0) return emp;

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return emp;

    MYSQL_ROW row = mysql_fetch_row(res);
    if (row) {
        emp.username     = row[0]  ? row[0]  : "";
        emp.managedBy    = row[1]  ? row[1]  : "";
        emp.branchId     = row[2]  ? std::stoi(row[2]) : -1;
        emp.firstName    = row[3]  ? row[3]  : "";
        emp.lastName     = row[4]  ? row[4]  : "";
        emp.email        = row[5]  ? row[5]  : "";
        emp.phone        = row[6]  ? row[6]  : "";
        emp.role         = row[7]  ? row[7]  : "";
        emp.isActive     = row[8]  ? (std::stoi(row[8]) == 1) : false;
        emp.hireDate     = row[9]  ? row[9]  : "";
        emp.resignationDate = row[10] ? row[10] : "";
        emp.branchName   = row[11] ? row[11] : "";
    }

    mysql_free_result(res);
    return emp;
}

} // namespace DB