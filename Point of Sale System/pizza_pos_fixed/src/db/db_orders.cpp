#include <mysql.h>
#include <string>
#include <vector>
#include <cstring>
#include <sstream>
#include <iostream>
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

std::string findLeastBusyKitchenStaff(int branchId) {
    MYSQL* conn = getConn();
    if (!conn) return "";

    std::string sql =
        "SELECT e.UserName, COUNT(o.OrderId) AS activeCount "
        "FROM EMPLOYEE e "
        "LEFT JOIN `ORDER` o ON e.UserName = o.PreparedBy "
        "AND o.Status IN ('Pending', 'In Preparation') "
        "WHERE e.Role = 'KitchenStaff' "
        "AND e.BranchId = " + std::to_string(branchId) + " "
        "AND e.IsActive = 1 "
        "GROUP BY e.UserName "
        "ORDER BY activeCount ASC, e.UserName ASC "
        "LIMIT 1";

    if (mysql_query(conn, sql.c_str()) != 0) return "";

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return "";

    MYSQL_ROW row = mysql_fetch_row(res);
    std::string result = (row && row[0]) ? row[0] : "";
    mysql_free_result(res);
    return result;
}

int createOrder(const std::string& takenBy, const std::string& preparedBy,
                int discountId, double discountAmount, double totalAmount,
                double cashTendered, double changeDue,
                const std::vector<CartItem>& items) {
    MYSQL* conn = getConn();
    if (!conn) return -1;

    mysql_query(conn, "START TRANSACTION");

    std::stringstream orderSql;
    orderSql << "INSERT INTO `ORDER` (TakenBy, PreparedBy, DiscountId, DiscountAmount, "
             << "OrderDateTime, Status, PaymentStatus, CashTendered, ChangeDue, TotalAmount) VALUES ("
             << escapeStr(takenBy) << ", "
             << escapeStr(preparedBy) << ", "
             << (discountId > 0 ? std::to_string(discountId) : "NULL") << ", "
             << discountAmount << ", "
             << "NOW(), 'Pending', 'Paid', "
             << cashTendered << ", "
             << changeDue << ", "
             << totalAmount << ")";

    if (mysql_query(conn, orderSql.str().c_str()) != 0) {
        mysql_query(conn, "ROLLBACK");
        return -1;
    }

    int orderId = static_cast<int>(mysql_insert_id(conn));

    for (const auto& item : items) {
        std::stringstream itemSql;
        itemSql << "INSERT INTO ORDER_ITEM (OrderId, MenuItemId, DealId, Quantity, UnitPrice) VALUES ("
                << orderId << ", "
                << (item.isDeal ? "NULL" : std::to_string(item.menuItemId)) << ", "
                << (item.isDeal ? std::to_string(item.dealId) : "NULL") << ", "
                << item.quantity << ", "
                << item.unitPrice << ")";

        if (mysql_query(conn, itemSql.str().c_str()) != 0) {
            mysql_query(conn, "ROLLBACK");
            return -1;
        }

        if (item.isDeal) {
            std::string dealSql =
                "SELECT di.MenuItemId, di.Quantity, mi.InventoryId "
                "FROM DEAL_ITEM di "
                "JOIN menu_ingredient mi ON di.MenuItemId = mi.MenuItemId "
                "WHERE di.DealId = " + std::to_string(item.dealId);

            if (mysql_query(conn, dealSql.c_str()) == 0) {
                MYSQL_RES* dealRes = mysql_store_result(conn);
                if (dealRes) {
                    MYSQL_ROW drow;
                    while ((drow = mysql_fetch_row(dealRes))) {
                        int invId = drow[2] ? std::stoi(drow[2]) : -1;
                        double qtyNeeded = (drow[1] ? std::stod(drow[1]) : 1.0) * item.quantity;
                        if (invId > 0) {
                            mysql_query(conn, ("UPDATE INVENTORY SET Quantity = Quantity - " + std::to_string(qtyNeeded) + " WHERE InventoryId = " + std::to_string(invId)).c_str());
                        }
                    }
                    mysql_free_result(dealRes);
                }
            }
        } else {
            std::string ingSql =
                "SELECT InventoryId FROM menu_ingredient WHERE MenuItemId = " + std::to_string(item.menuItemId);
            if (mysql_query(conn, ingSql.c_str()) == 0) {
                MYSQL_RES* ingRes = mysql_store_result(conn);
                if (ingRes) {
                    MYSQL_ROW irow;
                    while ((irow = mysql_fetch_row(ingRes))) {
                        int invId = irow[0] ? std::stoi(irow[0]) : -1;
                        if (invId > 0) {
                            mysql_query(conn, ("UPDATE INVENTORY SET Quantity = Quantity - " + std::to_string(item.quantity) + " WHERE InventoryId = " + std::to_string(invId)).c_str());
                        }
                    }
                    mysql_free_result(ingRes);
                }
            }
        }
    }

    mysql_query(conn, "COMMIT");
    return orderId;
}

std::vector<Order> getOrdersByReceptionist(const std::string& username, const std::string& statusFilter) {
    std::vector<Order> orders;
    MYSQL* conn = getConn();
    if (!conn) return orders;

    std::string sql =
        "SELECT OrderId, DiscountId, TakenBy, PreparedBy, OrderDateTime, Status, "
        "PaymentStatus, DiscountAmount, CashTendered, ChangeDue, TotalAmount "
        "FROM `ORDER` WHERE TakenBy = " + escapeStr(username);

    if (!statusFilter.empty()) {
        sql += " AND Status = " + escapeStr(statusFilter);
    }
    sql += " ORDER BY OrderId DESC LIMIT 100";

    if (mysql_query(conn, sql.c_str()) != 0) return orders;

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return orders;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        Order o;
        o.orderId        = row[0]  ? std::stoi(row[0])  : 0;
        o.discountId     = row[1]  ? std::stoi(row[1])  : 0;
        o.takenBy        = row[2]  ? row[2]  : "";
        o.preparedBy     = row[3]  ? row[3]  : "";
        o.orderDateTime  = row[4]  ? row[4]  : "";
        o.status         = row[5]  ? row[5]  : "";
        o.paymentStatus  = row[6]  ? row[6]  : "";
        o.discountAmount = row[7]  ? std::stod(row[7]) : 0.0;
        o.cashTendered   = row[8]  ? std::stod(row[8]) : 0.0;
        o.changeDue      = row[9]  ? std::stod(row[9]) : 0.0;
        o.totalAmount    = row[10] ? std::stod(row[10]) : 0.0;
        orders.push_back(o);
    }

    mysql_free_result(res);
    return orders;
}

std::vector<Order> getOrdersByKitchenStaff(const std::string& username, const std::string& statusFilter) {
    std::vector<Order> orders;
    MYSQL* conn = getConn();
    if (!conn) return orders;

    std::string sql =
        "SELECT OrderId, DiscountId, TakenBy, PreparedBy, OrderDateTime, Status, "
        "PaymentStatus, DiscountAmount, CashTendered, ChangeDue, TotalAmount "
        "FROM `ORDER` WHERE PreparedBy = " + escapeStr(username);

    if (!statusFilter.empty()) {
        sql += " AND Status = " + escapeStr(statusFilter);
    }
    sql += " ORDER BY OrderId DESC LIMIT 100";

    if (mysql_query(conn, sql.c_str()) != 0) return orders;

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return orders;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        Order o;
        o.orderId        = row[0]  ? std::stoi(row[0])  : 0;
        o.discountId     = row[1]  ? std::stoi(row[1])  : 0;
        o.takenBy        = row[2]  ? row[2]  : "";
        o.preparedBy     = row[3]  ? row[3]  : "";
        o.orderDateTime  = row[4]  ? row[4]  : "";
        o.status         = row[5]  ? row[5]  : "";
        o.paymentStatus  = row[6]  ? row[6]  : "";
        o.discountAmount = row[7]  ? std::stod(row[7]) : 0.0;
        o.cashTendered   = row[8]  ? std::stod(row[8]) : 0.0;
        o.changeDue      = row[9]  ? std::stod(row[9]) : 0.0;
        o.totalAmount    = row[10] ? std::stod(row[10]) : 0.0;
        orders.push_back(o);
    }

    mysql_free_result(res);
    return orders;
}

std::vector<OrderItem> getOrderItems(int orderId) {
    std::vector<OrderItem> items;
    MYSQL* conn = getConn();
    if (!conn) return items;

    std::string sql =
        "SELECT oi.OrderItemId, oi.OrderId, oi.MenuItemId, oi.DealId, oi.Quantity, "
        "oi.UnitPrice, mi.ItemName, d.DealName "
        "FROM ORDER_ITEM oi "
        "LEFT JOIN menuitem mi ON oi.MenuItemId = mi.MenuItemId "
        "LEFT JOIN DEAL d ON oi.DealId = d.DealId "
        "WHERE oi.OrderId = " + std::to_string(orderId);

    if (mysql_query(conn, sql.c_str()) != 0) return items;

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return items;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        OrderItem oi;
        oi.orderItemId = row[0] ? std::stoi(row[0]) : 0;
        oi.orderId     = row[1] ? std::stoi(row[1]) : 0;
        oi.menuItemId  = row[2] ? std::stoi(row[2]) : 0;
        oi.dealId      = row[3] ? std::stoi(row[3]) : 0;
        oi.quantity    = row[4] ? std::stoi(row[4]) : 0;
        oi.unitPrice   = row[5] ? std::stod(row[5]) : 0.0;
        oi.itemName    = row[6] ? row[6] : "";
        oi.dealName    = row[7] ? row[7] : "";
        items.push_back(oi);
    }

    mysql_free_result(res);
    return items;
}

Order getOrderById(int orderId) {
    Order o;
    o.orderId = 0;
    MYSQL* conn = getConn();
    if (!conn) return o;

    std::string sql =
        "SELECT OrderId, DiscountId, TakenBy, PreparedBy, OrderDateTime, Status, "
        "PaymentStatus, DiscountAmount, CashTendered, ChangeDue, TotalAmount "
        "FROM `ORDER` WHERE OrderId = " + std::to_string(orderId);

    if (mysql_query(conn, sql.c_str()) != 0) return o;

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return o;

    MYSQL_ROW row = mysql_fetch_row(res);
    if (row) {
        o.orderId        = row[0]  ? std::stoi(row[0])  : 0;
        o.discountId     = row[1]  ? std::stoi(row[1])  : 0;
        o.takenBy        = row[2]  ? row[2]  : "";
        o.preparedBy     = row[3]  ? row[3]  : "";
        o.orderDateTime  = row[4]  ? row[4]  : "";
        o.status         = row[5]  ? row[5]  : "";
        o.paymentStatus  = row[6]  ? row[6]  : "";
        o.discountAmount = row[7]  ? std::stod(row[7]) : 0.0;
        o.cashTendered   = row[8]  ? std::stod(row[8]) : 0.0;
        o.changeDue      = row[9]  ? std::stod(row[9]) : 0.0;
        o.totalAmount    = row[10] ? std::stod(row[10]) : 0.0;
    }

    mysql_free_result(res);
    return o;
}

bool updateOrderStatus(int orderId, const std::string& newStatus) {
    MYSQL* conn = getConn();
    if (!conn) return false;

    std::string sql =
        "UPDATE `ORDER` SET Status = " + escapeStr(newStatus) +
        " WHERE OrderId = " + std::to_string(orderId);

    return mysql_query(conn, sql.c_str()) == 0;
}

bool cancelOrder(int orderId) {
    MYSQL* conn = getConn();
    if (!conn) return false;

    Order o = getOrderById(orderId);
    if (o.status != "Pending" && o.status != "PENDING") return false;

    mysql_query(conn, "START TRANSACTION");

    std::vector<OrderItem> items = getOrderItems(orderId);
    for (const auto& item : items) {
        if (item.dealId > 0) {
            std::string dealSql =
                "SELECT di.MenuItemId, di.Quantity, mi.InventoryId "
                "FROM DEAL_ITEM di "
                "JOIN menu_ingredient mi ON di.MenuItemId = mi.MenuItemId "
                "WHERE di.DealId = " + std::to_string(item.dealId);
            if (mysql_query(conn, dealSql.c_str()) == 0) {
                MYSQL_RES* dealRes = mysql_store_result(conn);
                if (dealRes) {
                    MYSQL_ROW drow;
                    while ((drow = mysql_fetch_row(dealRes))) {
                        int invId = drow[2] ? std::stoi(drow[2]) : -1;
                        double qty = (drow[1] ? std::stod(drow[1]) : 1.0) * item.quantity;
                        if (invId > 0) {
                            mysql_query(conn, ("UPDATE INVENTORY SET Quantity = Quantity + " + std::to_string(qty) + " WHERE InventoryId = " + std::to_string(invId)).c_str());
                        }
                    }
                    mysql_free_result(dealRes);
                }
            }
        } else if (item.menuItemId > 0) {
            std::string ingSql = "SELECT InventoryId FROM menu_ingredient WHERE MenuItemId = " + std::to_string(item.menuItemId);
            if (mysql_query(conn, ingSql.c_str()) == 0) {
                MYSQL_RES* ingRes = mysql_store_result(conn);
                if (ingRes) {
                    MYSQL_ROW irow;
                    while ((irow = mysql_fetch_row(ingRes))) {
                        int invId = irow[0] ? std::stoi(irow[0]) : -1;
                        if (invId > 0) {
                            mysql_query(conn, ("UPDATE INVENTORY SET Quantity = Quantity + " + std::to_string(item.quantity) + " WHERE InventoryId = " + std::to_string(invId)).c_str());
                        }
                    }
                    mysql_free_result(ingRes);
                }
            }
        }
    }

    std::string updateSql =
        "UPDATE `ORDER` SET Status = 'Cancelled', PaymentStatus = 'Refunded' "
        "WHERE OrderId = " + std::to_string(orderId);

    if (mysql_query(conn, updateSql.c_str()) != 0) {
        mysql_query(conn, "ROLLBACK");
        return false;
    }

    mysql_query(conn, "COMMIT");
    return true;
}

bool modifyOrder(int orderId, const std::vector<CartItem>& newItems,
                 double newTotal, double newDiscountAmount) {
    MYSQL* conn = getConn();
    if (!conn) return false;

    Order o = getOrderById(orderId);
    if (o.status != "Pending" && o.status != "PENDING") return false;

    mysql_query(conn, "START TRANSACTION");

    std::vector<OrderItem> oldItems = getOrderItems(orderId);
    for (const auto& item : oldItems) {
        if (item.dealId > 0) {
            std::string dealSql =
                "SELECT di.MenuItemId, di.Quantity, mi.InventoryId "
                "FROM DEAL_ITEM di JOIN menu_ingredient mi ON di.MenuItemId = mi.MenuItemId "
                "WHERE di.DealId = " + std::to_string(item.dealId);
            if (mysql_query(conn, dealSql.c_str()) == 0) {
                MYSQL_RES* dr = mysql_store_result(conn);
                if (dr) {
                    MYSQL_ROW drow;
                    while ((drow = mysql_fetch_row(dr))) {
                        int invId = drow[2] ? std::stoi(drow[2]) : -1;
                        double qty = (drow[1] ? std::stod(drow[1]) : 1.0) * item.quantity;
                        if (invId > 0) {
                            mysql_query(conn, ("UPDATE INVENTORY SET Quantity = Quantity + " + std::to_string(qty) + " WHERE InventoryId = " + std::to_string(invId)).c_str());
                        }
                    }
                    mysql_free_result(dr);
                }
            }
        } else if (item.menuItemId > 0) {
            std::string ingSql = "SELECT InventoryId FROM menu_ingredient WHERE MenuItemId = " + std::to_string(item.menuItemId);
            if (mysql_query(conn, ingSql.c_str()) == 0) {
                MYSQL_RES* ir = mysql_store_result(conn);
                if (ir) {
                    MYSQL_ROW irow;
                    while ((irow = mysql_fetch_row(ir))) {
                        int invId = irow[0] ? std::stoi(irow[0]) : -1;
                        if (invId > 0) {
                            mysql_query(conn, ("UPDATE INVENTORY SET Quantity = Quantity + " + std::to_string(item.quantity) + " WHERE InventoryId = " + std::to_string(invId)).c_str());
                        }
                    }
                    mysql_free_result(ir);
                }
            }
        }
    }

    mysql_query(conn, ("DELETE FROM ORDER_ITEM WHERE OrderId = " + std::to_string(orderId)).c_str());

    std::stringstream updateSql;
    updateSql << "UPDATE `ORDER` SET DiscountAmount = " << newDiscountAmount
              << ", TotalAmount = " << newTotal
              << " WHERE OrderId = " << orderId;
    if (mysql_query(conn, updateSql.str().c_str()) != 0) {
        mysql_query(conn, "ROLLBACK");
        return false;
    }

    for (const auto& item : newItems) {
        std::stringstream itemSql;
        itemSql << "INSERT INTO ORDER_ITEM (OrderId, MenuItemId, DealId, Quantity, UnitPrice) VALUES ("
                << orderId << ", "
                << (item.isDeal ? "NULL" : std::to_string(item.menuItemId)) << ", "
                << (item.isDeal ? std::to_string(item.dealId) : "NULL") << ", "
                << item.quantity << ", "
                << item.unitPrice << ")";
        if (mysql_query(conn, itemSql.str().c_str()) != 0) {
            mysql_query(conn, "ROLLBACK");
            return false;
        }

        if (item.isDeal) {
            std::string dealSql =
                "SELECT di.MenuItemId, di.Quantity, mi.InventoryId "
                "FROM DEAL_ITEM di JOIN menu_ingredient mi ON di.MenuItemId = mi.MenuItemId "
                "WHERE di.DealId = " + std::to_string(item.dealId);
            if (mysql_query(conn, dealSql.c_str()) == 0) {
                MYSQL_RES* dr = mysql_store_result(conn);
                if (dr) {
                    MYSQL_ROW drow;
                    while ((drow = mysql_fetch_row(dr))) {
                        int invId = drow[2] ? std::stoi(drow[2]) : -1;
                        double qty = (drow[1] ? std::stod(drow[1]) : 1.0) * item.quantity;
                        if (invId > 0) {
                            mysql_query(conn, ("UPDATE INVENTORY SET Quantity = Quantity - " + std::to_string(qty) + " WHERE InventoryId = " + std::to_string(invId)).c_str());
                        }
                    }
                    mysql_free_result(dr);
                }
            }
        } else {
            std::string ingSql = "SELECT InventoryId FROM menu_ingredient WHERE MenuItemId = " + std::to_string(item.menuItemId);
            if (mysql_query(conn, ingSql.c_str()) == 0) {
                MYSQL_RES* ir = mysql_store_result(conn);
                if (ir) {
                    MYSQL_ROW irow;
                    while ((irow = mysql_fetch_row(ir))) {
                        int invId = irow[0] ? std::stoi(irow[0]) : -1;
                        if (invId > 0) {
                            mysql_query(conn, ("UPDATE INVENTORY SET Quantity = Quantity - " + std::to_string(item.quantity) + " WHERE InventoryId = " + std::to_string(invId)).c_str());
                        }
                    }
                    mysql_free_result(ir);
                }
            }
        }
    }

    mysql_query(conn, "COMMIT");
    return true;
}

std::vector<Order> getAllOrders(int branchId, const std::string& dateFrom, const std::string& dateTo) {
    std::vector<Order> orders;
    MYSQL* conn = getConn();
    if (!conn) return orders;

    std::string sql =
        "SELECT o.OrderId, o.DiscountId, o.TakenBy, o.PreparedBy, o.OrderDateTime, o.Status, "
        "o.PaymentStatus, o.DiscountAmount, o.CashTendered, o.ChangeDue, o.TotalAmount "
        "FROM `ORDER` o "
        "JOIN EMPLOYEE e ON o.TakenBy = e.UserName "
        "WHERE 1=1";

    if (branchId > 0) {
        sql += " AND e.BranchId = " + std::to_string(branchId);
    }
    if (!dateFrom.empty()) {
        sql += " AND DATE(o.OrderDateTime) >= " + escapeStr(dateFrom);
    }
    if (!dateTo.empty()) {
        sql += " AND DATE(o.OrderDateTime) <= " + escapeStr(dateTo);
    }
    sql += " ORDER BY o.OrderId DESC LIMIT 500";

    if (mysql_query(conn, sql.c_str()) != 0) return orders;

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return orders;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        Order o;
        o.orderId        = row[0]  ? std::stoi(row[0])  : 0;
        o.discountId     = row[1]  ? std::stoi(row[1])  : 0;
        o.takenBy        = row[2]  ? row[2]  : "";
        o.preparedBy     = row[3]  ? row[3]  : "";
        o.orderDateTime  = row[4]  ? row[4]  : "";
        o.status         = row[5]  ? row[5]  : "";
        o.paymentStatus  = row[6]  ? row[6]  : "";
        o.discountAmount = row[7]  ? std::stod(row[7]) : 0.0;
        o.cashTendered   = row[8]  ? std::stod(row[8]) : 0.0;
        o.changeDue      = row[9]  ? std::stod(row[9]) : 0.0;
        o.totalAmount    = row[10] ? std::stod(row[10]) : 0.0;
        orders.push_back(o);
    }

    mysql_free_result(res);
    return orders;
}

} // namespace DB