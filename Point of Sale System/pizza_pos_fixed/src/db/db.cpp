#include "db.h"
#include <mysql.h>
#include <iostream>

// ==================== PRIVATE GLOBALS ====================

static MYSQL* conn = nullptr;

// Internal getter — other db_*.cpp files declare: extern MYSQL* getConn();
MYSQL* getConn() {
    return conn;
}

// ==================== PUBLIC FUNCTIONS ====================

namespace DB {

bool initDB() {
    if (conn) {
        std::cerr << "[DB] Already connected." << std::endl;
        return true;
    }

    conn = mysql_init(nullptr);
    if (!conn) {
        std::cerr << "[DB] mysql_init() failed." << std::endl;
        return false;
    }

    MYSQL* result = mysql_real_connect(
        conn,
        "localhost",                // host
        "root",                     // user
        "noorzaman",                     // password
        "Point_of_Sale_system",     // database name
        3306,                       // port
        nullptr,                    // unix socket
        0                           // client flags
    );

    if (!result) {
        std::cerr << "[DB] Connection failed: " << mysql_error(conn) << std::endl;
        mysql_close(conn);
        conn = nullptr;
        return false;
    }

    std::cout << "[DB] Connected to MySQL successfully." << std::endl;
    return true;
}

void closeDB() {
    if (conn) {
        mysql_close(conn);
        conn = nullptr;
        std::cout << "[DB] Connection closed." << std::endl;
    }
}

bool isDBConnected() {
    if (!conn) return false;
    return mysql_ping(conn) == 0;
}

bool safeQuery(const std::string& sql) {
    if (!conn) {
        std::cerr << "[DB] safeQuery: Not connected." << std::endl;
        return false;
    }

    if (mysql_query(conn, sql.c_str()) != 0) {
        std::cerr << "[DB] Query failed: " << mysql_error(conn) << std::endl;
        std::cerr << "[DB] SQL: " << sql << std::endl;
        return false;
    }

    return true;
}

} // namespace DB
