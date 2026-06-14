# Pizza Point of Sale (POS) System

A full-featured multi-branch pizza shop Point of Sale system built with C++17, Raylib (GUI), and MySQL (C API) on Windows MinGW.

## Features

- Role-based dashboards: Admin, Receptionist, Kitchen Staff
- Order management with real-time stock validation
- Per-branch inventory with low-stock alerts
- Menu management with price history
- Deals and date-ranged discounts
- Staff management, attendance, and salary
- Sales, inventory, and staff reports with charts

---

## Prerequisites

| Software | Location |
|---|---|
| MinGW w64devkit | `C:\raylib\w64devkit\` |
| Raylib 5.x | `C:\raylib\w64devkit\include` and `lib` |
| MySQL Server 9.x | `C:\Program Files\MySQL\MySQL Server 9.7\` |

---

## Setup

### 1. Copy MySQL libraries
```powershell
copy "C:\Program Files\MySQL\MySQL Server 9.7\lib\libmysql.lib" .
copy "C:\Program Files\MySQL\MySQL Server 9.7\lib\libmysql.dll" .