#ifndef DB_H
#define DB_H

#include <string>
#include <vector>
#include "../core/models/models.h"

// ==================== DATABASE FUNCTION DECLARATIONS ====================
// NEVER include mysql.h in this file — only declarations
// All MySQL types are hidden behind these functions

namespace DB {

// Connection
bool initDB();
void closeDB();
bool isDBConnected();
bool safeQuery(const std::string& sql);

// Auth
Employee loginUser(const std::string& username, const std::string& password);
bool changePassword(const std::string& username, const std::string& newPassword);
Employee getEmployeeByUsername(const std::string& username);

// Menu
std::vector<MenuItem> getMenuItems(int branchId);
std::vector<MenuItem> getAvailableMenuItems(int branchId);
std::vector<MenuItem> getMenuItemsByType(int branchId, const std::string& menuItemType);
MenuItem getMenuItemById(int menuItemId, int branchId);
bool addMenuItem(const std::string& itemName, int categoryId, const std::string& menuItemType, const std::string& description, double initialPrice);
bool updateMenuItem(int menuItemId, const std::string& itemName, int categoryId, const std::string& menuItemType, const std::string& description);
bool setNewPrice(int menuItemId, double newAmount);
std::vector<Price> getPriceHistory(int menuItemId);
std::vector<Category> getCategories();
bool addCategory(const std::string& categoryName);
bool setBranchMenuItemAvailability(int branchId, int menuItemId, bool available);
double getItemCurrentPrice(int menuItemId);

// Inventory
std::vector<Inventory> getInventory(int branchId);
std::vector<Inventory> getLowStockItems(int branchId);
std::vector<LowStockAlert> getLowStockAlerts(int branchId);
bool addInventoryItem(int branchId, const std::string& inventoryName, double quantity, const std::string& unit, double reorderLevel, double unitCost);
bool updateStock(int inventoryId, double addQuantity);
bool updateReorderLevel(int inventoryId, double newLevel);
bool updateInventoryItem(int inventoryId, const std::string& name, double reorderLevel, double unitCost);
bool checkStockSufficiency(int menuItemId, int branchId, double requiredQty);
bool deductStock(int inventoryId, double quantity);
bool restoreStock(int inventoryId, double quantity);
Inventory getInventoryById(int inventoryId);
std::vector<Inventory> getIngredientsByMenuItem(int menuItemId, int branchId);

// Staff
std::vector<Employee> getAllEmployees(int branchId = -1);
std::vector<Employee> getEmployeesByBranch(int branchId);
std::vector<Employee> getKitchenStaffByBranch(int branchId);
bool addEmployee(const std::string& username, const std::string& managedBy, int branchId, const std::string& firstName, const std::string& lastName, const std::string& email, const std::string& phone, const std::string& password, const std::string& role);
bool updateEmployee(const std::string& username, const std::string& firstName, const std::string& lastName, const std::string& email, const std::string& phone, const std::string& role, int branchId);
bool deactivateEmployee(const std::string& username);

// Shifts
bool clockIn(const std::string& username);
bool clockOut(const std::string& username);
bool isCurrentlyClockedIn(const std::string& username);
std::string getCurrentClockInTime(const std::string& username);
std::vector<Shift> getShifts(const std::string& username = "", const std::string& dateFrom = "", const std::string& dateTo = "");
std::vector<Shift> getMyShifts(const std::string& username);

// Salary
std::vector<Salary> getSalaryHistory(const std::string& username = "");
bool addSalaryPayment(const std::string& username, double amount, const std::string& paymentDate, const std::string& notes);
std::vector<Salary> getAllSalaries(int branchId = -1);
double getTotalSalaryPaid(const std::string& username);

// Deals
std::vector<Deal> getDeals(int branchId);
std::vector<Deal> getAvailableDeals(int branchId);
std::vector<DealItem> getDealItems(int dealId);
bool isDealAvailableAtBranch(int dealId, int branchId);
bool createDeal(const std::string& dealName, double totalAmount, const std::vector<std::pair<int, int>>& items, const std::vector<int>& branchIds);
bool updateDeal(int dealId, const std::string& dealName, double totalAmount, const std::vector<std::pair<int, int>>& items, const std::vector<int>& branchIds);
bool setBranchDealAvailability(int branchId, int dealId, bool available);
Deal getDealById(int dealId);
bool toggleDealActive(int dealId, bool isActive);
std::vector<Deal> getAllDeals();

// Discounts
std::vector<Discount> getActiveDiscounts();
std::vector<Discount> getAllDiscounts();
bool addDiscount(const std::string& discountName, double discountValue, const std::string& startDate, const std::string& endDate);
bool updateDiscount(int discountId, const std::string& discountName, double discountValue, const std::string& startDate, const std::string& endDate);
bool isDiscountValid(int discountId);
Discount getDiscountById(int discountId);

// Branches
std::vector<Branch> getAllBranches();
Branch getBranchById(int branchId);
bool addBranch(const std::string& branchName, const std::string& address, const std::string& phone, const std::string& email, const std::string& openingTime, const std::string& closingTime);
bool updateBranch(int branchId, const std::string& branchName, const std::string& address, const std::string& phone, const std::string& email);
bool deactivateBranch(int branchId);

// Orders
std::string findLeastBusyKitchenStaff(int branchId);
int createOrder(const std::string& takenBy, const std::string& preparedBy, int discountId, double discountAmount, double totalAmount, double cashTendered, double changeDue, const std::vector<CartItem>& items);
std::vector<Order> getOrdersByReceptionist(const std::string& username, const std::string& statusFilter = "");
std::vector<Order> getOrdersByKitchenStaff(const std::string& username, const std::string& statusFilter = "");
std::vector<OrderItem> getOrderItems(int orderId);
Order getOrderById(int orderId);
bool updateOrderStatus(int orderId, const std::string& newStatus);
bool cancelOrder(int orderId);
bool modifyOrder(int orderId, const std::vector<CartItem>& newItems, double newTotal, double newDiscountAmount);
std::vector<Order> getAllOrders(int branchId, const std::string& dateFrom = "", const std::string& dateTo = "");

// Reports
// (Report structs defined locally in db_reports.cpp, but functions are callable)

} // namespace DB

#endif // DB_H
