# Pizza Point of Sale (POS) System

A full-featured, multi-branch pizza restaurant Point of Sale system built in C++17, using Raylib for the graphical interface and MySQL (C API) for persistent data storage. The system supports three distinct user roles — Admin, Receptionist, and Kitchen Staff — each with a dedicated dashboard. It handles the complete order lifecycle from placement through kitchen preparation and completion, with integrated inventory tracking, staff management, salary recording, deals, discounts, and sales reporting.

---

## System Requirements

### Hardware
- Display resolution: 1280 × 720 or higher
- RAM: 512 MB minimum (1 GB recommended)
- Disk: 100 MB free space

### Software
| Component | Version | Notes |
|---|---|---|
| Windows | 10 or 11 | 64-bit required |
| MinGW w64devkit | Latest | Must be installed at `C:\raylib\w64devkit\` |
| Raylib | 5.x | Headers and libs must be in w64devkit include/lib paths |
| MySQL Server | 8.x or 9.x | Community Edition is sufficient |
| MySQL C Connector | Matching server version | `libmysql.dll` and `libmysql.lib` required |

### Libraries
- `raylib` — graphics, input, and windowing
- `libmysql` — MySQL C API client
- `opengl32`, `gdi32`, `winmm`, `ws2_32` — Windows system libraries (linked automatically via Makefile)

---

## Installation Instructions

### Step 1 — Install Prerequisites

1. Install **MySQL Server** from https://dev.mysql.com/downloads/mysql/ and note your root password during setup.
2. Install **MinGW w64devkit** and extract it to `C:\raylib\w64devkit\`.
3. Install **Raylib 5.x** such that headers are available at `C:\raylib\w64devkit\include\raylib.h` and the static library at `C:\raylib\w64devkit\lib\libraylib.a`.

### Step 2 — Clone or Extract the Project

Place the project folder (`pizza_pos_fixed`) anywhere on your machine, for example:

```
C:\Projects\pizza_pos_fixed\
```

### Step 3 — Copy MySQL Libraries

Open PowerShell or Command Prompt and copy the MySQL client library files into the project root:

```powershell
copy "C:\Program Files\MySQL\MySQL Server 9.7\lib\libmysql.lib" "C:\Projects\pizza_pos_fixed\"
copy "C:\Program Files\MySQL\MySQL Server 9.7\lib\libmysql.dll" "C:\Projects\pizza_pos_fixed\"
```

Adjust the MySQL Server version number in the path to match your installation.

### Step 4 — Copy MySQL Header Files

Copy the MySQL include headers into the project's `include\` directory:

```powershell
xcopy "C:\Program Files\MySQL\MySQL Server 9.7\include\*" "C:\Projects\pizza_pos_fixed\include\" /E /Y
```

### Step 5 — Create the Database

Open MySQL Workbench or the MySQL command-line client and create the database and all required tables. Run the following SQL:

```sql
CREATE DATABASE Point_of_Sale_system;
USE Point_of_Sale_system;

CREATE TABLE BRANCH (
    BranchId     INT AUTO_INCREMENT PRIMARY KEY,
    BranchName   VARCHAR(100) NOT NULL,
    Address      VARCHAR(255),
    PhoneNumber  VARCHAR(20),
    Email        VARCHAR(100),
    OpeningTime  TIME,
    ClosingTime  TIME,
    IsActive     TINYINT(1) DEFAULT 1
);

CREATE TABLE EMPLOYEE (
    UserName         VARCHAR(50) PRIMARY KEY,
    ManagedBy        VARCHAR(50),
    BranchId         INT,
    FirstName        VARCHAR(100) NOT NULL,
    LastName         VARCHAR(100) NOT NULL,
    Email            VARCHAR(100),
    PhoneNumber      VARCHAR(20),
    Password         VARCHAR(255) NOT NULL,
    Role             ENUM('Admin','Receptionist','Kitchen Staff') NOT NULL,
    IsActive         TINYINT(1) DEFAULT 1,
    HireDate         DATE,
    ResignationDate  DATE,
    FOREIGN KEY (BranchId) REFERENCES BRANCH(BranchId)
);

CREATE TABLE SHIFT (
    ShiftId    INT AUTO_INCREMENT PRIMARY KEY,
    UserName   VARCHAR(50),
    ShiftDate  DATE,
    StartTime  TIME,
    EndTime    TIME,
    ShiftType  VARCHAR(50) DEFAULT 'Regular',
    FOREIGN KEY (UserName) REFERENCES EMPLOYEE(UserName)
);

CREATE TABLE SALARY (
    SalaryId     INT AUTO_INCREMENT PRIMARY KEY,
    UserName     VARCHAR(50),
    Amount       DECIMAL(10,2),
    PaymentDate  DATE,
    Notes        TEXT,
    FOREIGN KEY (UserName) REFERENCES EMPLOYEE(UserName)
);

CREATE TABLE CATEGORY (
    CategoryId    INT AUTO_INCREMENT PRIMARY KEY,
    CategoryName  VARCHAR(100) NOT NULL
);

CREATE TABLE MENUITEM (
    MenuItemId    INT AUTO_INCREMENT PRIMARY KEY,
    CategoryId    INT,
    ItemName      VARCHAR(100) NOT NULL,
    Description   TEXT,
    MenuItemType  VARCHAR(50),
    FOREIGN KEY (CategoryId) REFERENCES CATEGORY(CategoryId)
);

CREATE TABLE PRICE (
    PriceId        INT AUTO_INCREMENT PRIMARY KEY,
    MenuItemId     INT,
    Amount         DECIMAL(10,2),
    EffectiveDate  DATE,
    EndDate        DATE,
    IsActive       TINYINT(1) DEFAULT 1,
    FOREIGN KEY (MenuItemId) REFERENCES MENUITEM(MenuItemId)
);

CREATE TABLE INVENTORY (
    InventoryId    INT AUTO_INCREMENT PRIMARY KEY,
    BranchId       INT,
    InventoryName  VARCHAR(100) NOT NULL,
    Quantity       DECIMAL(10,2),
    Unit           VARCHAR(20),
    ReorderLevel   DECIMAL(10,2),
    UnitCost       DECIMAL(10,2),
    LastRestocked  DATETIME,
    FOREIGN KEY (BranchId) REFERENCES BRANCH(BranchId)
);

CREATE TABLE MENU_INGREDIENT (
    MenuIngredientId  INT AUTO_INCREMENT PRIMARY KEY,
    MenuItemId        INT,
    InventoryId       INT,
    FOREIGN KEY (MenuItemId) REFERENCES MENUITEM(MenuItemId),
    FOREIGN KEY (InventoryId) REFERENCES INVENTORY(InventoryId)
);

CREATE TABLE BRANCH_MENU_ITEM (
    BranchMenuItemId  INT AUTO_INCREMENT PRIMARY KEY,
    BranchId          INT,
    MenuItemId        INT,
    IsAvailable       TINYINT(1) DEFAULT 1,
    UNIQUE KEY unique_branch_item (BranchId, MenuItemId),
    FOREIGN KEY (BranchId) REFERENCES BRANCH(BranchId),
    FOREIGN KEY (MenuItemId) REFERENCES MENUITEM(MenuItemId)
);

CREATE TABLE DISCOUNT (
    DiscountId     INT AUTO_INCREMENT PRIMARY KEY,
    DiscountName   VARCHAR(100),
    DiscountValue  DECIMAL(10,2),
    StartDate      DATE,
    EndDate        DATE,
    IsActive       TINYINT(1) DEFAULT 1
);

CREATE TABLE DEAL (
    DealId       INT AUTO_INCREMENT PRIMARY KEY,
    DealName     VARCHAR(100),
    TotalAmount  DECIMAL(10,2),
    IsActive     TINYINT(1) DEFAULT 1
);

CREATE TABLE DEAL_ITEM (
    DealItemId  INT AUTO_INCREMENT PRIMARY KEY,
    DealId      INT,
    MenuItemId  INT,
    Quantity    INT DEFAULT 1,
    FOREIGN KEY (DealId) REFERENCES DEAL(DealId),
    FOREIGN KEY (MenuItemId) REFERENCES MENUITEM(MenuItemId)
);

CREATE TABLE BRANCH_DEAL (
    BranchDealId  INT AUTO_INCREMENT PRIMARY KEY,
    BranchId      INT,
    DealId        INT,
    IsAvailable   TINYINT(1) DEFAULT 1,
    UNIQUE KEY unique_branch_deal (BranchId, DealId),
    FOREIGN KEY (BranchId) REFERENCES BRANCH(BranchId),
    FOREIGN KEY (DealId) REFERENCES DEAL(DealId)
);

CREATE TABLE `ORDER` (
    OrderId         INT AUTO_INCREMENT PRIMARY KEY,
    DiscountId      INT,
    TakenBy         VARCHAR(50),
    PreparedBy      VARCHAR(50),
    CustomerName    VARCHAR(100),
    OrderDateTime   DATETIME,
    Status          ENUM('Pending','In Preparation','Completed','Cancelled') DEFAULT 'Pending',
    PaymentStatus   ENUM('Paid','Unpaid','Refunded') DEFAULT 'Paid',
    DiscountAmount  DECIMAL(10,2) DEFAULT 0,
    CashTendered    DECIMAL(10,2) DEFAULT 0,
    ChangeDue       DECIMAL(10,2) DEFAULT 0,
    TotalAmount     DECIMAL(10,2) DEFAULT 0,
    FOREIGN KEY (TakenBy) REFERENCES EMPLOYEE(UserName),
    FOREIGN KEY (PreparedBy) REFERENCES EMPLOYEE(UserName),
    FOREIGN KEY (DiscountId) REFERENCES DISCOUNT(DiscountId)
);

CREATE TABLE ORDER_ITEM (
    OrderItemId  INT AUTO_INCREMENT PRIMARY KEY,
    OrderId      INT,
    MenuItemId   INT,
    DealId       INT,
    Quantity     INT DEFAULT 1,
    UnitPrice    DECIMAL(10,2),
    FOREIGN KEY (OrderId) REFERENCES `ORDER`(OrderId),
    FOREIGN KEY (MenuItemId) REFERENCES MENUITEM(MenuItemId),
    FOREIGN KEY (DealId) REFERENCES DEAL(DealId)
);
```

### Step 6 — Insert a Seed Admin User

Insert at least one branch and one Admin account so you can log in for the first time:

```sql
USE Point_of_Sale_system;

INSERT INTO BRANCH (BranchName, Address, PhoneNumber, Email, OpeningTime, ClosingTime, IsActive)
VALUES ('Main Branch', '123 Pizza Street', '0300-0000000', 'main@pizza.com', '09:00:00', '23:00:00', 1);

INSERT INTO EMPLOYEE (UserName, ManagedBy, BranchId, FirstName, LastName, Email, PhoneNumber, Password, Role, IsActive, HireDate)
VALUES ('admin', NULL, 1, 'System', 'Admin', 'admin@pizza.com', '0000000000', 'admin123', 'Admin', 1, CURDATE());
```

### Step 7 — Configure the Database Connection

Open `src/db/db.cpp` and confirm the connection parameters match your MySQL installation:

```cpp
MYSQL* result = mysql_real_connect(
    conn,
    "localhost",           // host
    "root",                // MySQL username
    "your_password",       // MySQL root password
    "Point_of_Sale_system",// database name
    3306,                  // port
    nullptr,
    0
);
```

Replace `"your_password"` with the password you set during MySQL Server installation.

### Step 8 — Build the Project

Open a terminal using the w64devkit shell (`C:\raylib\w64devkit\w64devkit.exe`) and navigate to the project folder:

```bash
cd /c/Projects/pizza_pos_fixed
make
```

This compiles all source files and produces `game.exe` in the project root.

### Step 9 — Run the Application

```bash
make run
```

Or double-click `game.exe` from Windows Explorer. Ensure `libmysql.dll` is in the same folder as `game.exe` and that MySQL Server is running before launching.

---

## Usage Instructions

### Logging In

Launch the application. On the login screen, enter your username and password and press **LOGIN** or hit **Enter**. You can use **Tab** to switch between the username and password fields.

The system redirects you to the appropriate dashboard based on your role:
- **Admin** → Full management dashboard
- **Receptionist** → Order taking dashboard
- **Kitchen Staff** → Order preparation dashboard

### Admin Dashboard

The Admin dashboard has a sidebar with the following sections:

**Overview** — Shows today's total orders, total revenue, discounts applied, net revenue, and a bar chart of the top 5 selling items.

**Menu Items** — Add new menu items with category, type, description, and initial price. Update existing items or set a new price (price history is preserved). Toggle per-branch availability.

**Inventory** — View all inventory items for the branch. Add new items with quantity, unit, reorder level, and unit cost. Restock items by adding quantity. Items below the reorder level are highlighted in red as low-stock alerts.

**Staff** — Add new employees (username, name, contact, role, branch, password). Edit existing employees. Deactivate employees (soft delete with resignation date).

**Salary** — Record salary payments for any employee with date and notes. View full payment history per employee.

**Deals** — Create bundled deals composed of multiple menu items at a fixed total price. Assign deals to one or more branches. Toggle deal availability.

**Discounts** — Create percentage or flat discounts with a validity date range. Discounts are automatically considered expired after their end date.

**Branches** — Add and manage restaurant branches with address, contact, and operating hours.

**Shifts** — View shift records across all staff with optional date range filtering.

**Reports** — View sales summary (total orders, revenue, discounts, top items), inventory status, and staff shift/hours report. Filters available by branch and date range.

### Receptionist Dashboard

**New Order** — Browse the menu by category, add items or deals to the cart, apply an active discount, enter cash tendered to calculate change, and submit the order. The system automatically assigns the least busy kitchen staff member and deducts inventory.

**My Orders** — View all orders taken by the logged-in receptionist. Filter by status (Pending, In Preparation, Completed, Cancelled). Cancel a pending order (restores inventory). Modify a pending order.

**Clock In/Out** — Clock in at the start of a shift and clock out at the end. The current clock-in time is displayed when clocked in.

### Kitchen Staff Dashboard

**My Orders** — Displays all orders assigned to the logged-in kitchen staff. Orders show customer name, items, quantities, total, and current status. Update an order status from Pending → In Preparation → Completed.

**Clock In/Out** — Same clock in/out functionality as the Receptionist.

### Logging Out

Click the **LOGOUT** button at the top right of any dashboard. This clears the session and returns to the login screen.

---

## Code Structure

```
pizza_pos_fixed/
│
├── main.cpp                          Entry point. Initializes window and DB, runs screen loop.
│
├── Makefile                          Build configuration for MinGW/w64devkit.
│
├── libmysql.dll                      MySQL C client runtime (must be in same folder as executable).
├── libmysql.lib                      MySQL C client import library for linking.
│
├── include/                          MySQL C API header files.
│   ├── mysql.h
│   ├── mysql_com.h
│   └── ...
│
└── src/
    ├── core/
    │   ├── globals.h                 All enums (UserRole, ScreenID, AdminPanel, etc.),
    │   │                             color macros, and layout constants.
    │   ├── session.h                 SessionData struct declaration and accessor function headers.
    │   ├── session.cpp               Global gSession instance. setSession(), clearSession(),
    │   │                             isLoggedIn(), hasRole(), getUsername(), etc.
    │   └── models/
    │       └── models.h              All database model structs: Branch, Employee, Salary,
    │                                 Shift, MenuItem, Category, Price, Inventory, Order,
    │                                 OrderItem, Deal, DealItem, Discount, CartItem, LowStockAlert.
    │
    ├── db/
    │   ├── db.h                      Declares all DB namespace functions (the public API).
    │   ├── db.cpp                    MySQL connection: initDB(), closeDB(), isDBConnected(),
    │   │                             safeQuery(). Owns the static MYSQL* conn pointer.
    │   ├── db_auth.cpp               loginUser(), changePassword().
    │   ├── db_menu.cpp               getMenuItems(), addMenuItem(), updateMenuItem(),
    │   │                             setNewPrice(), getPriceHistory(), getCategories(),
    │   │                             addCategory(), setBranchMenuItemAvailability().
    │   ├── db_inventory.cpp          getInventory(), getLowStockAlerts(), addInventoryItem(),
    │   │                             updateStock(), deductStock(), restoreStock(),
    │   │                             getIngredientsByMenuItem().
    │   ├── db_staff.cpp              getAllEmployees(), addEmployee(), updateEmployee(),
    │   │                             deactivateEmployee(), getEmployeeByUsername(),
    │   │                             getKitchenStaffByBranch().
    │   ├── db_shifts.cpp             clockIn(), clockOut(), isCurrentlyClockedIn(),
    │   │                             getCurrentClockInTime(), getShifts(), getMyShifts().
    │   ├── db_salary.cpp             getSalaryHistory(), addSalaryPayment(), getAllSalaries(),
    │   │                             getTotalSalaryPaid().
    │   ├── db_orders.cpp             createOrder() (with transaction + inventory deduction),
    │   │                             cancelOrder() (with inventory restore), modifyOrder(),
    │   │                             getOrdersByReceptionist(), getOrdersByKitchenStaff(),
    │   │                             updateOrderStatus(), getAllOrders(), getOrderItems().
    │   ├── db_deals.cpp              getDeals(), getAvailableDeals(), getDealItems(),
    │   │                             createDeal(), updateDeal(), toggleDealActive(),
    │   │                             setBranchDealAvailability(), isDealAvailableAtBranch().
    │   ├── db_discounts.cpp          getActiveDiscounts(), getAllDiscounts(), addDiscount(),
    │   │                             updateDiscount(), isDiscountValid(), getDiscountById().
    │   ├── db_branches.cpp           getAllBranches(), getBranchById(), addBranch(),
    │   │                             updateBranch(), deactivateBranch().
    │   └── db_reports.cpp            getSalesReport(), getInventoryReport(), getStaffReport().
    │                                 Report structs (SalesReport, InventoryReportItem,
    │                                 StaffReportItem) are defined locally in this file.
    │
    └── ui/
        ├── ui_helpers.h              Declares all shared UI components.
        ├── ui_helpers.cpp            Implements buttons, text inputs, search bar, dropdown,
        │                             checkbox, sidebar, top bar, table header/rows,
        │                             scrollbar, notifications, and confirm dialog.
        │
        ├── screens/
        │   ├── screen_login.h/.cpp               Login screen with username/password fields,
        │   │                                     error messages, and role-based redirection.
        │   ├── screen_dashboard_admin.h/.cpp      Admin dashboard. Manages sidebar navigation
        │   │                                     across 10 panels. Draws the Overview panel
        │   │                                     inline; delegates all other panels.
        │   ├── screen_dashboard_receptionist.h/.cpp  Receptionist dashboard with 3 panels:
        │   │                                         New Order, My Orders, Clock In/Out.
        │   └── screen_dashboard_kitchen.h/.cpp    Kitchen dashboard with auto-refresh every
        │                                          10 seconds on the order queue.
        │
        └── panels/
            ├── panel_menu_manager.h/.cpp     Menu item and category CRUD with price history view.
            ├── panel_inventory.h/.cpp        Inventory management with low-stock highlighting.
            ├── panel_staff.h/.cpp            Employee add/edit/deactivate with role and branch.
            ├── panel_salary.h/.cpp           Salary payment recording and history per employee.
            ├── panel_shifts.h/.cpp           Shift history viewer with date range filter.
            ├── panel_deals.h/.cpp            Deal creation and branch assignment management.
            ├── panel_discounts.h/.cpp        Discount campaign management with date ranges.
            ├── panel_branches.h/.cpp         Branch add/edit/deactivate panel.
            ├── panel_reports.h/.cpp          Sales, inventory, and staff report rendering.
            ├── panel_order_form.h/.cpp       Full cart-based order form with deal/discount support.
            ├── panel_order_list.h/.cpp       Order list with cancel and modify actions.
            ├── panel_kitchen_queue.h/.cpp    Kitchen order queue with status update controls.
            └── panel_clock.h/.cpp            Clock in/out panel for Receptionist and Kitchen roles.
```

### Key Design Decisions

The database layer (`src/db/`) is fully decoupled from the UI layer (`src/ui/`). UI files never include `mysql.h` directly; they only call functions declared in `db.h`. The single `MYSQL*` connection pointer is owned by `db.cpp` and shared with other `db_*.cpp` files via the `getConn()` function declared `extern`.

All order creation, cancellation, and modification operations are wrapped in MySQL transactions (`START TRANSACTION` / `COMMIT` / `ROLLBACK`) to ensure inventory counts remain consistent even on partial failures.

The session module (`session.cpp`) holds a single global `gSession` struct that is populated on login and cleared on logout. All UI panels read the current user's username, role, and branch ID from this struct via the accessor functions in `session.h`.

---

## Troubleshooting

**"Database connection failed" on startup** — Verify MySQL Server is running (check Windows Services), confirm the password in `db.cpp` matches your MySQL root password, and ensure the database `Point_of_Sale_system` exists.

**Linker errors about `libmysql`** — Confirm `libmysql.lib` is in the project root and `libmysql.dll` is present alongside the compiled `game.exe`.

**`raylib.h` not found** — Confirm Raylib headers are installed at `C:\raylib\w64devkit\include\raylib.h`. If your path differs, update the `INCLUDES` variable in the Makefile.

**`game.exe` crashes immediately** — Ensure `libmysql.dll` is in the same directory as `game.exe`. Windows must be able to find it at runtime.
