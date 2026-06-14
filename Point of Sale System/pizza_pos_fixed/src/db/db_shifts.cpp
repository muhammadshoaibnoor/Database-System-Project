#include <mysql.h>
#include <string>
#include <vector>
#include <cstring>
#include <cstdio>
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

// Computes hours worked from "HH:MM:SS" time strings.
// Returns 0 if the shift has no end time yet (still clocked in).
static double computeShiftHours(const std::string& startTime, const std::string& endTime) {
    if (startTime.empty() || endTime.empty()) return 0.0;

    int sh = 0, sm = 0, ss = 0, eh = 0, em = 0, es = 0;
    sscanf(startTime.c_str(), "%d:%d:%d", &sh, &sm, &ss);
    sscanf(endTime.c_str(), "%d:%d:%d", &eh, &em, &es);

    double startSec = sh * 3600.0 + sm * 60.0 + ss;
    double endSec = eh * 3600.0 + em * 60.0 + es;
    double diff = endSec - startSec;
    if (diff < 0) diff += 24 * 3600.0; // shift crossed midnight
    return diff / 3600.0;
}

bool clockIn(const std::string& username) {
    MYSQL* conn = getConn();
    if (!conn) return false;

    // Check not already clocked in
    if (isCurrentlyClockedIn(username)) return false;

    std::string sql =
        "INSERT INTO SHIFT (UserName, ShiftDate, StartTime, ShiftType) VALUES (" +
        escapeStr(username) + ", CURDATE(), CURTIME(), 'Regular')";

    return mysql_query(conn, sql.c_str()) == 0;
}

bool clockOut(const std::string& username) {
    MYSQL* conn = getConn();
    if (!conn) return false;

    // Find open shift for today
    std::string sql =
        "UPDATE SHIFT SET EndTime = CURTIME() "
        "WHERE UserName = " + escapeStr(username) +
        " AND ShiftDate = CURDATE() AND EndTime IS NULL "
        "ORDER BY ShiftId DESC LIMIT 1";

    if (mysql_query(conn, sql.c_str()) != 0) return false;
    return mysql_affected_rows(conn) > 0;
}

bool isCurrentlyClockedIn(const std::string& username) {
    MYSQL* conn = getConn();
    if (!conn) return false;

    std::string sql =
        "SELECT COUNT(*) FROM SHIFT "
        "WHERE UserName = " + escapeStr(username) +
        " AND ShiftDate = CURDATE() AND EndTime IS NULL";

    if (mysql_query(conn, sql.c_str()) != 0) return false;

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return false;

    MYSQL_ROW row = mysql_fetch_row(res);
    bool clockedIn = row && row[0] && std::stoi(row[0]) > 0;
    mysql_free_result(res);
    return clockedIn;
}

std::string getCurrentClockInTime(const std::string& username) {
    MYSQL* conn = getConn();
    if (!conn) return "";

    std::string sql =
        "SELECT StartTime FROM SHIFT "
        "WHERE UserName = " + escapeStr(username) +
        " AND ShiftDate = CURDATE() AND EndTime IS NULL "
        "ORDER BY ShiftId DESC LIMIT 1";

    if (mysql_query(conn, sql.c_str()) != 0) return "";

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return "";

    MYSQL_ROW row = mysql_fetch_row(res);
    std::string time = (row && row[0]) ? row[0] : "";
    mysql_free_result(res);
    return time;
}

std::vector<Shift> getShifts(const std::string& username, const std::string& dateFrom,
                             const std::string& dateTo) {
    std::vector<Shift> shifts;
    MYSQL* conn = getConn();
    if (!conn) return shifts;

    std::string sql =
        "SELECT s.ShiftId, s.UserName, CONCAT(e.FirstName, ' ', e.LastName) AS EmployeeName, "
        "e.Role, s.ShiftDate, s.StartTime, COALESCE(s.EndTime, '') AS EndTime, s.ShiftType "
        "FROM SHIFT s "
        "JOIN EMPLOYEE e ON s.UserName = e.UserName WHERE 1=1";

    if (!username.empty()) {
        sql += " AND s.UserName = " + escapeStr(username);
    }
    if (!dateFrom.empty()) {
        sql += " AND s.ShiftDate >= " + escapeStr(dateFrom);
    }
    if (!dateTo.empty()) {
        sql += " AND s.ShiftDate <= " + escapeStr(dateTo);
    }
    sql += " ORDER BY s.ShiftDate DESC, s.StartTime DESC LIMIT 200";

    if (mysql_query(conn, sql.c_str()) != 0) return shifts;

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return shifts;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        Shift s;
        s.shiftId      = row[0] ? std::stoi(row[0]) : 0;
        s.username     = row[1] ? row[1] : "";
        s.employeeName = row[2] ? row[2] : "";
        s.userRole     = row[3] ? row[3] : "";
        s.shiftDate    = row[4] ? row[4] : "";
        s.startTime    = row[5] ? row[5] : "";
        s.endTime      = row[6] ? row[6] : "";
        s.shiftType    = row[7] ? row[7] : "";
        s.hours        = computeShiftHours(s.startTime, s.endTime);
        shifts.push_back(s);
    }

    mysql_free_result(res);
    return shifts;
}

std::vector<Shift> getMyShifts(const std::string& username) {
    return getShifts(username, "", "");
}
} // namespace DB