#ifndef MODELS_H
#define MODELS_H

#include <string>
#include <vector>
#include "../globals.h"
// ==================== DATABASE MODEL STRUCTS ====================

struct Branch {
    int branchId;
    std::string branchName;
    std::string address;
    std::string phone;
    std::string email;
    std::string openingTime;
    std::string closingTime;
    bool isActive;
};

struct Employee {
    std::string username;
    std::string managedBy;
    std::string firstName;
    std::string lastName;
    std::string email;
    std::string phone;
    std::string role;
    int branchId;
    std::string branchName;
    bool isActive;
    std::string hireDate;
    std::string resignationDate;
};

struct Salary {
    int salaryId;
    std::string username;
    std::string employeeName;
    double amount;
    std::string paymentDate;
    std::string notes;
};

struct Shift {
    int shiftId;
    std::string username;
    std::string employeeName;
    std::string userRole;
    std::string shiftDate;
    std::string startTime;
    std::string endTime;
    std::string shiftType;
    double hours;
};

struct Category {
    int categoryId;
    std::string categoryName;
};

struct MenuItem {
    int menuItemId;
    int categoryId;
    std::string categoryName;
    std::string itemName;
    std::string description;
    std::string menuItemType;
    double currentPrice;
    bool isAvailable;
};

struct Price {
    int priceId;
    int menuItemId;
    double amount;
    std::string effectiveDate;
    std::string endDate;
    bool isActive;
};

struct Inventory {
    int inventoryId;
    int branchId;
    std::string inventoryName;
    double quantity;
    std::string unit;
    double reorderLevel;
    double unitCost;
    std::string lastRestocked;
    bool isLowStock;
};

struct MenuIngredient {
    int menuIngredientId;
    int menuItemId;
    int inventoryId;
    std::string inventoryName;
};

struct BranchMenuItem {
    int branchMenuItemId;
    int branchId;
    int menuItemId;
    bool isAvailable;
};

struct Discount {
    int discountId;
    std::string discountName;
    double discountValue;
    std::string startDate;
    std::string endDate;
    bool isActive;
};

struct Order {
    int orderId;
    int discountId;
    std::string takenBy;
    std::string preparedBy;
    std::string customerName;
    std::string orderDateTime;
    std::string status;
    std::string paymentStatus;
    double discountAmount;
    double cashTendered;
    double changeDue;
    double totalAmount;
};

struct OrderItem {
    int orderItemId;
    int orderId;
    int menuItemId;
    int dealId;
    int quantity;
    std::string itemName;
    std::string dealName;
    double unitPrice;
};

struct Deal {
    int dealId;
    std::string dealName;
    double totalAmount;
    bool isActive;
    std::vector<int> menuItemIds;
    std::vector<int> branchIds;
    std::vector<std::string> branchNames;
};

struct DealItem {
    int dealItemId;
    int dealId;
    int menuItemId;
    int quantity;
    std::string itemName;
};

struct BranchDeal {
    int branchDealId;
    int branchId;
    int dealId;
    bool isAvailable;
};

struct CartItem {
    int menuItemId;
    int dealId;
    int quantity;
    std::string itemName;
    std::string dealName;
    double unitPrice;
    bool isAddon;
    bool isDeal;
};

struct LowStockAlert {
    int inventoryId;
    int branchId;
    std::string inventoryName;
    double quantity;
    double reorderLevel;
    std::string unit;
};

#endif // MODELS_H