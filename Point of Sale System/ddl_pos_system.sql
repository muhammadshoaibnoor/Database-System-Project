CREATE DATABASE IF NOT EXISTS Point_of_Sale_System;
USE Point_of_Sale_System;


CREATE TABLE BRANCH (
    BranchId      INT            NOT NULL AUTO_INCREMENT,
    BranchName    VARCHAR(100)   NOT NULL,
    Address       VARCHAR(255)   NOT NULL,
    PhoneNumber   VARCHAR(20)    NOT NULL,
    Email         VARCHAR(100)   NOT NULL UNIQUE,
    OpeningTime   TIME           NOT NULL,
    ClosingTime   TIME           NOT NULL,
    IsActive      BOOLEAN        NOT NULL DEFAULT TRUE,
    CONSTRAINT PK_Branch PRIMARY KEY (BranchId)
);


CREATE TABLE EMPLOYEE (
    UserName         VARCHAR(50)   NOT NULL,
    ManagedBy        VARCHAR(50)   NULL,
    BranchId         INT           NOT NULL,
    FirstName        VARCHAR(50)   NOT NULL,
    LastName         VARCHAR(50)   NOT NULL,
    Email            VARCHAR(100)  NOT NULL UNIQUE,
    PhoneNumber      VARCHAR(20)   NOT NULL,
    Password         VARCHAR(255)  NOT NULL,
    Role             VARCHAR(50)   NOT NULL,
    IsActive         BOOLEAN       NOT NULL DEFAULT TRUE,
    HireDate         DATE          NOT NULL,
    ResignationDate  DATE          NULL,
    CONSTRAINT PK_Employee    PRIMARY KEY (UserName),
    CONSTRAINT FK_Emp_Manager FOREIGN KEY (ManagedBy)
        REFERENCES EMPLOYEE(UserName)
        ON DELETE SET NULL ON UPDATE CASCADE,
    CONSTRAINT FK_Emp_Branch  FOREIGN KEY (BranchId)
        REFERENCES BRANCH(BranchId)
        ON DELETE RESTRICT ON UPDATE CASCADE
);


CREATE TABLE SALARY (
    SalaryId     INT             NOT NULL AUTO_INCREMENT,
    UserName     VARCHAR(50)     NOT NULL,
    Amount       DECIMAL(10,2)   NOT NULL,
    PaymentDate  DATE            NOT NULL,
    Notes        TEXT            NULL,
    CONSTRAINT PK_Salary  PRIMARY KEY (SalaryId),
    CONSTRAINT FK_Sal_Emp FOREIGN KEY (UserName)
        REFERENCES EMPLOYEE(UserName)
        ON DELETE RESTRICT ON UPDATE CASCADE
);

CREATE TABLE SHIFT (
    ShiftId    INT          NOT NULL AUTO_INCREMENT,
    UserName   VARCHAR(50)  NOT NULL,
    ShiftDate  DATE         NOT NULL,
    StartTime  TIME         NOT NULL,
    EndTime    TIME         NOT NULL,
    ShiftType  VARCHAR(30)  NOT NULL,
    CONSTRAINT PK_Shift    PRIMARY KEY (ShiftId),
    CONSTRAINT FK_Shft_Emp FOREIGN KEY (UserName)
        REFERENCES EMPLOYEE(UserName)
        ON DELETE RESTRICT ON UPDATE CASCADE
);


CREATE TABLE CATEGORY (
    CategoryId    INT           NOT NULL AUTO_INCREMENT,
    CategoryName  VARCHAR(100)  NOT NULL UNIQUE,
    CONSTRAINT PK_Category PRIMARY KEY (CategoryId)
);


CREATE TABLE MENUITEM (
    MenuItemId    INT           NOT NULL AUTO_INCREMENT,
    CategoryId    INT           NOT NULL,
    ItemName      VARCHAR(100)  NOT NULL,
    Description   TEXT          NULL,
    MenuItemType  VARCHAR(50)   NOT NULL,
    CONSTRAINT PK_MenuItem    PRIMARY KEY (MenuItemId),
    CONSTRAINT FK_MI_Category FOREIGN KEY (CategoryId)
        REFERENCES CATEGORY(CategoryId)
        ON DELETE RESTRICT ON UPDATE CASCADE
);


CREATE TABLE PRICE (
    PriceId        INT            NOT NULL AUTO_INCREMENT,
    MenuItemId     INT            NOT NULL,
    Amount         DECIMAL(10,2)  NOT NULL,
    EffectiveDate  DATE           NOT NULL,
    EndDate        DATE           NULL,
    IsActive       BOOLEAN        NOT NULL DEFAULT TRUE,
    CONSTRAINT PK_Price    PRIMARY KEY (PriceId),
    CONSTRAINT FK_Price_MI FOREIGN KEY (MenuItemId)
        REFERENCES MENUITEM(MenuItemId)
        ON DELETE CASCADE ON UPDATE CASCADE
);


CREATE TABLE INVENTORY (
    InventoryId    INT            NOT NULL AUTO_INCREMENT,
    BranchId       INT            NOT NULL,
    InventoryName  VARCHAR(100)   NOT NULL,
    Quantity       DECIMAL(10,2)  NOT NULL,
    Unit           VARCHAR(20)    NOT NULL,
    ReorderLevel   DECIMAL(10,2)  NOT NULL,
    UnitCost       DECIMAL(10,2)  NOT NULL,
    LastRestocked  DATETIME       NULL,
    CONSTRAINT PK_Inventory  PRIMARY KEY (InventoryId),
    CONSTRAINT FK_Inv_Branch FOREIGN KEY (BranchId)
        REFERENCES BRANCH(BranchId)
        ON DELETE RESTRICT ON UPDATE CASCADE
);

CREATE TABLE MENU_INGREDIENT (
    MenuIngredientId  INT  NOT NULL AUTO_INCREMENT,
    MenuItemId        INT  NOT NULL,
    InventoryId       INT  NOT NULL,
    CONSTRAINT PK_MenuIngredient PRIMARY KEY (MenuIngredientId),
    CONSTRAINT UQ_MI_Inv         UNIQUE (MenuItemId, InventoryId),
    CONSTRAINT FK_MIngr_MI       FOREIGN KEY (MenuItemId)
        REFERENCES MENUITEM(MenuItemId)
        ON DELETE CASCADE ON UPDATE CASCADE,
    CONSTRAINT FK_MIngr_Inv      FOREIGN KEY (InventoryId)
        REFERENCES INVENTORY(InventoryId)
        ON DELETE RESTRICT ON UPDATE CASCADE
);


CREATE TABLE BRANCH_MENU_ITEM (
    BranchMenuItemId  INT      NOT NULL AUTO_INCREMENT,
    BranchId          INT      NOT NULL,
    MenuItemId        INT      NOT NULL,
    IsAvailable       BOOLEAN  NOT NULL DEFAULT TRUE,
    CONSTRAINT PK_BranchMenuItem PRIMARY KEY (BranchMenuItemId),
    CONSTRAINT UQ_Br_MI          UNIQUE (BranchId, MenuItemId),
    CONSTRAINT FK_BMI_Branch     FOREIGN KEY (BranchId)
        REFERENCES BRANCH(BranchId)
        ON DELETE RESTRICT ON UPDATE CASCADE,
    CONSTRAINT FK_BMI_MI         FOREIGN KEY (MenuItemId)
        REFERENCES MENUITEM(MenuItemId)
        ON DELETE RESTRICT ON UPDATE CASCADE
);

CREATE TABLE DISCOUNT (
    DiscountId     INT            NOT NULL AUTO_INCREMENT,
    DiscountName   VARCHAR(100)   NOT NULL,
    DiscountValue  DECIMAL(5,2)   NOT NULL,
    StartDate      DATE           NOT NULL,
    EndDate        DATE           NULL,
    IsActive       BOOLEAN        NOT NULL DEFAULT TRUE,
    CONSTRAINT PK_Discount PRIMARY KEY (DiscountId)
);


CREATE TABLE `ORDER` (
    OrderId        INT            NOT NULL AUTO_INCREMENT,
    TakenBy        VARCHAR(50)    NOT NULL,
    PreparedBy     VARCHAR(50)    NOT NULL,
    DiscountId     INT            NULL,
    DiscountAmount DECIMAL(10,2)  NOT NULL DEFAULT 0.00,
    OrderDateTime  DATETIME       NOT NULL DEFAULT CURRENT_TIMESTAMP,
    Status         VARCHAR(30)    NOT NULL,
    PaymentStatus  VARCHAR(30)    NOT NULL,
    CashTendered   DECIMAL(10,2)  NULL,
    ChangeDue      DECIMAL(10,2)  NULL,
    TotalAmount    DECIMAL(10,2)  NOT NULL,
    CONSTRAINT PK_Order          PRIMARY KEY (OrderId),
    CONSTRAINT FK_Ord_TakenBy    FOREIGN KEY (TakenBy)
        REFERENCES EMPLOYEE(UserName)
        ON DELETE RESTRICT ON UPDATE CASCADE,
    CONSTRAINT FK_Ord_PreparedBy FOREIGN KEY (PreparedBy)
        REFERENCES EMPLOYEE(UserName)
        ON DELETE RESTRICT ON UPDATE CASCADE,
    CONSTRAINT FK_Ord_Discount   FOREIGN KEY (DiscountId)
        REFERENCES DISCOUNT(DiscountId)
        ON DELETE SET NULL ON UPDATE CASCADE
);


CREATE TABLE DEAL (
    DealId       INT            NOT NULL AUTO_INCREMENT,
    DealName     VARCHAR(100)   NOT NULL,
    TotalAmount  DECIMAL(10,2)  NOT NULL,
    IsActive     BOOLEAN        NOT NULL DEFAULT TRUE,
    CONSTRAINT PK_Deal PRIMARY KEY (DealId)
);


CREATE TABLE ORDER_ITEM (
    OrderItemId  INT            NOT NULL AUTO_INCREMENT,
    OrderId      INT            NOT NULL,
    MenuItemId   INT            NOT NULL,
    DealId       INT            NULL,
    Quantity     INT            NOT NULL DEFAULT 1,
    UnitPrice    DECIMAL(10,2)  NOT NULL,
    CONSTRAINT PK_OrderItem   PRIMARY KEY (OrderItemId),
    CONSTRAINT FK_OI_Order    FOREIGN KEY (OrderId)
        REFERENCES `ORDER`(OrderId)
        ON DELETE CASCADE ON UPDATE CASCADE,
    CONSTRAINT FK_OI_MenuItem FOREIGN KEY (MenuItemId)
        REFERENCES MENUITEM(MenuItemId)
        ON DELETE RESTRICT ON UPDATE CASCADE,
    CONSTRAINT FK_OI_Deal     FOREIGN KEY (DealId)
        REFERENCES DEAL(DealId)
        ON DELETE SET NULL ON UPDATE CASCADE
);

CREATE TABLE DEAL_ITEM (
    DealItemId  INT  NOT NULL AUTO_INCREMENT,
    DealId      INT  NOT NULL,
    MenuItemId  INT  NOT NULL,
    Quantity    INT  NOT NULL DEFAULT 1,
    CONSTRAINT PK_DealItem PRIMARY KEY (DealItemId),
    CONSTRAINT UQ_DI_combo UNIQUE (DealId, MenuItemId),
    CONSTRAINT FK_DI_Deal  FOREIGN KEY (DealId)
        REFERENCES DEAL(DealId)
        ON DELETE CASCADE ON UPDATE CASCADE,
    CONSTRAINT FK_DI_MI    FOREIGN KEY (MenuItemId)
        REFERENCES MENUITEM(MenuItemId)
        ON DELETE RESTRICT ON UPDATE CASCADE
);


CREATE TABLE BRANCH_DEAL (
    BranchDealId  INT      NOT NULL AUTO_INCREMENT,
    BranchId      INT      NOT NULL,
    DealId        INT      NOT NULL,
    IsAvailable   BOOLEAN  NOT NULL DEFAULT TRUE,
    CONSTRAINT PK_BranchDeal PRIMARY KEY (BranchDealId),
    CONSTRAINT UQ_BD_combo   UNIQUE (BranchId, DealId),
    CONSTRAINT FK_BD_Branch  FOREIGN KEY (BranchId)
        REFERENCES BRANCH(BranchId)
        ON DELETE RESTRICT ON UPDATE CASCADE,
    CONSTRAINT FK_BD_Deal    FOREIGN KEY (DealId)
        REFERENCES DEAL(DealId)
        ON DELETE CASCADE ON UPDATE CASCADE
);
