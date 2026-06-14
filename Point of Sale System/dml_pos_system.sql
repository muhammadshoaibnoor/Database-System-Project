USE Point_of_Sale_System;

INSERT INTO BRANCH (BranchName, Address, PhoneNumber, Email, OpeningTime, ClosingTime, IsActive) VALUES
('Gulberg Branch',       '12-A Main Boulevard, Gulberg III, Lahore',           '042-35761234', 'gulberg@pizzapoint.pk',       '10:00:00', '23:00:00', TRUE),
('DHA Branch',           '45 Y Commercial Zone, Phase 3, DHA, Lahore',         '042-35892345', 'dha@pizzapoint.pk',            '10:00:00', '23:30:00', TRUE),
('Johar Town Branch',    'Plot 78, H-Block Commercial, Johar Town, Lahore',    '042-35123456', 'johartown@pizzapoint.pk',     '10:00:00', '23:00:00', TRUE),
('Model Town Branch',    '9 Link Road, Model Town, Lahore',                    '042-35234567', 'modeltown@pizzapoint.pk',     '11:00:00', '22:30:00', TRUE),
('Bahria Town Branch',   'Sector C Commercial, Bahria Town, Lahore',           '042-35345678', 'bahriatown@pizzapoint.pk',    '10:30:00', '23:00:00', TRUE),
('Wapda Town Branch',    'Block K Commercial, Wapda Town, Lahore',             '042-35456789', 'wapdalahore@pizzapoint.pk',   '11:00:00', '22:00:00', TRUE),
('Faisalabad Branch',    '22 Susan Road, Madina Town, Faisalabad',             '041-38567890', 'faisalabad@pizzapoint.pk',    '10:00:00', '23:00:00', TRUE),
('Rawalpindi Branch',    '5 Murree Road, Saddar, Rawalpindi',                  '051-35678901', 'rawalpindi@pizzapoint.pk',    '10:00:00', '22:30:00', TRUE),
('Islamabad Branch',     'G-9 Markaz, Islamabad',                             '051-28789012', 'islamabad@pizzapoint.pk',     '10:00:00', '23:00:00', TRUE),
('Karachi Branch',       'Block 4, Clifton, Karachi',                         '021-35890123', 'karachi@pizzapoint.pk',       '10:00:00', '23:30:00', TRUE),
('Multan Branch',        '12 Abdali Road, Multan Cantt, Multan',              '061-34901234', 'multan@pizzapoint.pk',        '10:30:00', '22:30:00', TRUE),
('Gujranwala Branch',    'G.T. Road, Trust Plaza, Gujranwala',                '055-33012345', 'gujranwala@pizzapoint.pk',    '11:00:00', '22:00:00', TRUE),
('Sialkot Branch',       'Paris Road, Cantt, Sialkot',                        '052-34123456', 'sialkot@pizzapoint.pk',       '10:00:00', '22:30:00', TRUE),
('Peshawar Branch',      'Saddar Bazar, Peshawar Cantt, Peshawar',            '091-35234567', 'peshawar@pizzapoint.pk',      '10:00:00', '22:00:00', TRUE),
('Quetta Branch',        'Jinnah Road, Quetta',                               '081-32345678', 'quetta@pizzapoint.pk',        '11:00:00', '22:00:00', TRUE),
('Hyderabad Branch',     'Auto Bhan Road, Hyderabad',                         '022-32456789', 'hyderabad@pizzapoint.pk',     '10:30:00', '22:30:00', FALSE);

INSERT INTO EMPLOYEE (UserName, ManagedBy, BranchId, FirstName, LastName, Email, PhoneNumber, Password, Role, IsActive, HireDate, ResignationDate) VALUES
('ahmed.khan',    NULL,            1, 'Ahmed',    'Khan',      'ahmed.khan@pizzapoint.pk',    '03001234567', 'Admin@123',    'Admin',         TRUE,  '2021-01-15', NULL),
('sara.malik',    'ahmed.khan',    1, 'Sara',     'Malik',     'sara.malik@pizzapoint.pk',    '03011234567', 'Recep@123',    'Receptionist',  TRUE,  '2021-03-01', NULL),
('bilal.ahmed',   'ahmed.khan',    1, 'Bilal',    'Ahmed',     'bilal.ahmed@pizzapoint.pk',   '03021234567', 'Kitchen@123',  'Kitchen Staff', TRUE,  '2021-03-15', NULL),
('nadia.ali',     'ahmed.khan',    1, 'Nadia',    'Ali',       'nadia.ali@pizzapoint.pk',     '03031234567', 'Kitchen@123',  'Kitchen Staff', TRUE,  '2022-01-10', NULL),
('usman.raza',    NULL,            2, 'Usman',    'Raza',      'usman.raza@pizzapoint.pk',    '03041234567', 'Admin@456',    'Admin',         TRUE,  '2021-02-01', NULL),
('hina.shah',     'usman.raza',   2, 'Hina',     'Shah',      'hina.shah@pizzapoint.pk',     '03051234567', 'Recep@456',    'Receptionist',  TRUE,  '2021-04-01', NULL),
('kamran.butt',   'usman.raza',   2, 'Kamran',   'Butt',      'kamran.butt@pizzapoint.pk',   '03061234567', 'Kitchen@456',  'Kitchen Staff', TRUE,  '2021-05-01', NULL),
('zara.qureshi',  'usman.raza',   2, 'Zara',     'Qureshi',   'zara.qureshi@pizzapoint.pk',  '03071234567', 'Kitchen@789',  'Kitchen Staff', TRUE,  '2022-06-01', NULL),
('omar.sheikh',   NULL,            3, 'Omar',     'Sheikh',    'omar.sheikh@pizzapoint.pk',   '03081234567', 'Admin@789',    'Admin',         TRUE,  '2021-06-01', NULL),
('farah.nawaz',   'omar.sheikh',  3, 'Farah',    'Nawaz',     'farah.nawaz@pizzapoint.pk',   '03091234567', 'Recep@789',    'Receptionist',  TRUE,  '2022-02-01', NULL),
('ali.hassan',    'omar.sheikh',  3, 'Ali',      'Hassan',    'ali.hassan@pizzapoint.pk',    '03101234567', 'Kitchen@321',  'Kitchen Staff', TRUE,  '2022-03-01', NULL),
('sana.iqbal',    'omar.sheikh',  3, 'Sana',     'Iqbal',     'sana.iqbal@pizzapoint.pk',    '03111234567', 'Kitchen@654',  'Kitchen Staff', TRUE,  '2022-04-01', NULL),
('tariq.mehmood', NULL,            4, 'Tariq',    'Mehmood',   'tariq.mehmood@pizzapoint.pk', '03121234567', 'Admin@321',    'Admin',         TRUE,  '2021-07-01', NULL),
('rida.farooq',   'tariq.mehmood',4, 'Rida',     'Farooq',    'rida.farooq@pizzapoint.pk',   '03131234567', 'Recep@321',    'Receptionist',  TRUE,  '2022-05-01', NULL),
('asad.mirza',    'tariq.mehmood',4, 'Asad',     'Mirza',     'asad.mirza@pizzapoint.pk',    '03141234567', 'Kitchen@987',  'Kitchen Staff', TRUE,  '2022-07-01', NULL),
('mehwish.javed', 'tariq.mehmood',4, 'Mehwish',  'Javed',     'mehwish.javed@pizzapoint.pk', '03151234567', 'Kitchen@147',  'Kitchen Staff', FALSE, '2022-08-01', '2024-01-15');

INSERT INTO SALARY (UserName, Amount, PaymentDate, Notes) VALUES
('ahmed.khan',    95000.00, '2024-11-30', 'November salary'),
('ahmed.khan',    95000.00, '2024-12-31', 'December salary'),
('sara.malik',    45000.00, '2024-11-30', 'November salary'),
('sara.malik',    45000.00, '2024-12-31', 'December salary'),
('bilal.ahmed',   40000.00, '2024-11-30', 'November salary'),
('bilal.ahmed',   40000.00, '2024-12-31', 'December salary'),
('nadia.ali',     38000.00, '2024-11-30', 'November salary'),
('nadia.ali',     38000.00, '2024-12-31', 'December salary'),
('usman.raza',    92000.00, '2024-11-30', 'November salary'),
('usman.raza',    92000.00, '2024-12-31', 'December salary'),
('hina.shah',     44000.00, '2024-11-30', 'November salary'),
('kamran.butt',   39000.00, '2024-11-30', 'November salary'),
('kamran.butt',   40500.00, '2024-12-31', 'December salary with increment'),
('zara.qureshi',  37000.00, '2024-11-30', 'November salary'),
('omar.sheikh',   90000.00, '2024-12-31', 'December salary'),
('farah.nawaz',   43000.00, '2024-12-31', 'December salary'),
('ali.hassan',    38500.00, '2024-12-31', 'December salary'),
('sana.iqbal',    37500.00, '2024-12-31', 'December salary'),
('tariq.mehmood', 88000.00, '2024-12-31', 'December salary'),
('rida.farooq',   42000.00, '2024-12-31', 'December salary');

INSERT INTO SHIFT (UserName, ShiftDate, StartTime, EndTime, ShiftType) VALUES
('sara.malik',    '2025-01-06', '10:00:00', '18:00:00', 'Morning'),
('sara.malik',    '2025-01-07', '10:00:00', '18:00:00', 'Morning'),
('sara.malik',    '2025-01-08', '15:00:00', '23:00:00', 'Evening'),
('bilal.ahmed',   '2025-01-06', '10:00:00', '18:00:00', 'Morning'),
('bilal.ahmed',   '2025-01-07', '15:00:00', '23:00:00', 'Evening'),
('bilal.ahmed',   '2025-01-08', '10:00:00', '18:00:00', 'Morning'),
('nadia.ali',     '2025-01-06', '15:00:00', '23:00:00', 'Evening'),
('nadia.ali',     '2025-01-07', '10:00:00', '18:00:00', 'Morning'),
('nadia.ali',     '2025-01-08', '15:00:00', '23:00:00', 'Evening'),
('hina.shah',     '2025-01-06', '10:00:00', '18:00:00', 'Morning'),
('hina.shah',     '2025-01-07', '10:00:00', '18:00:00', 'Morning'),
('kamran.butt',   '2025-01-06', '10:00:00', '18:00:00', 'Morning'),
('kamran.butt',   '2025-01-07', '15:00:00', '23:00:00', 'Evening'),
('zara.qureshi',  '2025-01-06', '15:00:00', '23:00:00', 'Evening'),
('zara.qureshi',  '2025-01-08', '10:00:00', '18:00:00', 'Morning'),
('farah.nawaz',   '2025-01-07', '10:00:00', '18:00:00', 'Morning'),
('ali.hassan',    '2025-01-07', '15:00:00', '23:00:00', 'Evening'),
('sana.iqbal',    '2025-01-08', '10:00:00', '18:00:00', 'Morning'),
('rida.farooq',   '2025-01-06', '10:00:00', '18:00:00', 'Morning'),
('asad.mirza',    '2025-01-06', '15:00:00', '23:00:00', 'Evening');

INSERT INTO CATEGORY (CategoryName) VALUES
('Classic Pizzas'),
('Gourmet Pizzas'),
('Thin Crust Pizzas'),
('Deep Dish Pizzas'),
('Calzones'),
('Pasta'),
('Burgers'),
('Sandwiches'),
('Sides'),
('Dips & Sauces'),
('Cold Drinks'),
('Hot Drinks'),
('Desserts'),
('Kids Meals'),
('Seasonal Specials');

INSERT INTO MENUITEM (CategoryId, ItemName, Description, MenuItemType) VALUES
(1, 'Margherita',           'Classic tomato base with mozzarella and fresh basil',             'Pizza'),
(1, 'Pepperoni Classic',    'Tomato base loaded with pepperoni slices',                        'Pizza'),
(2, 'BBQ Chicken Gourmet',  'Smoky BBQ sauce, grilled chicken, caramelised onions',            'Pizza'),
(2, 'Veggie Supreme',       'Bell peppers, mushrooms, olives, onions on tomato base',          'Pizza'),
(3, 'Thin Crust Mexicana',  'Jalapeños, chicken, corn, salsa on a crispy thin base',          'Pizza'),
(3, 'Thin Crust Tikka',     'Marinated chicken tikka with mint chutney drizzle',              'Pizza'),
(4, 'Deep Dish Meat Feast', 'Beef mince, sausage, pepperoni in a thick deep dish',            'Pizza'),
(5, 'Chicken Calzone',      'Folded dough stuffed with chicken, cheese and mushrooms',        'Calzone'),
(6, 'Penne Arrabbiata',     'Penne in spicy tomato and garlic sauce',                         'Pasta'),
(6, 'Fettuccine Alfredo',   'Fettuccine in creamy parmesan sauce',                            'Pasta'),
(7, 'Zinger Burger',        'Crispy fried chicken fillet with coleslaw and mayo',             'Burger'),
(8, 'Club Sandwich',        'Triple decker with chicken, egg, lettuce and tomato',            'Sandwich'),
(9, 'Garlic Bread',         'Toasted bread with garlic butter and herbs',                     'Side'),
(9, 'Coleslaw',             'Creamy homemade coleslaw',                                       'Side'),
(10,'Garlic Dip',           'Creamy roasted garlic dipping sauce',                            'Dip'),
(11,'Pepsi 500ml',          'Chilled Pepsi can',                                              'Drink'),
(11,'7Up 500ml',            'Chilled 7Up can',                                                'Drink'),
(12,'Cappuccino',           'Espresso with steamed milk foam',                                'Hot Drink'),
(13,'Chocolate Lava Cake',  'Warm chocolate cake with molten centre',                        'Dessert'),
(14,'Kids Pizza Meal',      'Small margherita pizza with a juice box and fries',              'Meal');

INSERT INTO PRICE (MenuItemId, Amount, EffectiveDate, EndDate, IsActive) VALUES
(1,  750.00,  '2023-01-01', '2023-12-31', FALSE),
(1,  850.00,  '2024-01-01', NULL,         TRUE),
(2,  950.00,  '2024-01-01', NULL,         TRUE),
(3,  1150.00, '2024-01-01', NULL,         TRUE),
(4,  950.00,  '2024-01-01', NULL,         TRUE),
(5,  900.00,  '2024-01-01', NULL,         TRUE),
(6,  950.00,  '2024-01-01', NULL,         TRUE),
(7,  1350.00, '2024-01-01', NULL,         TRUE),
(8,  1100.00, '2024-01-01', NULL,         TRUE),
(9,  650.00,  '2024-01-01', NULL,         TRUE),
(10, 700.00,  '2024-01-01', NULL,         TRUE),
(11, 550.00,  '2024-01-01', NULL,         TRUE),
(12, 650.00,  '2024-01-01', NULL,         TRUE),
(13, 200.00,  '2024-01-01', NULL,         TRUE),
(14, 150.00,  '2024-01-01', NULL,         TRUE),
(15, 100.00,  '2024-01-01', NULL,         TRUE),
(16, 120.00,  '2024-01-01', NULL,         TRUE),
(17, 120.00,  '2024-01-01', NULL,         TRUE),
(18, 350.00,  '2024-01-01', NULL,         TRUE),
(19, 450.00,  '2024-01-01', NULL,         TRUE),
(20, 800.00,  '2024-01-01', NULL,         TRUE);

INSERT INTO INVENTORY (BranchId, InventoryName, Quantity, Unit, ReorderLevel, UnitCost, LastRestocked) VALUES
(1, 'Mozzarella Cheese',    25.00,  'kg',     5.00,  1200.00, '2025-01-05 09:00:00'),
(1, 'Pizza Dough',          80.00,  'units',  20.00,  85.00,  '2025-01-05 09:00:00'),
(1, 'Tomato Sauce',         30.00,  'litres', 8.00,   180.00, '2025-01-05 09:00:00'),
(1, 'Pepperoni',            10.00,  'kg',     2.00,  1800.00, '2025-01-05 09:00:00'),
(1, 'Chicken Breast',       20.00,  'kg',     5.00,   700.00, '2025-01-04 09:00:00'),
(1, 'Bell Peppers',          8.00,  'kg',     2.00,   200.00, '2025-01-04 09:00:00'),
(1, 'Mushrooms',             6.00,  'kg',     1.50,   350.00, '2025-01-04 09:00:00'),
(1, 'Olive Oil',            10.00,  'litres', 2.00,   600.00, '2025-01-03 09:00:00'),
(1, 'Penne Pasta',          15.00,  'kg',     3.00,   250.00, '2025-01-03 09:00:00'),
(1, 'Cream',                 8.00,  'litres', 2.00,   350.00, '2025-01-03 09:00:00'),
(1, 'Garlic',                5.00,  'kg',     1.00,   180.00, '2025-01-02 09:00:00'),
(1, 'Chicken Mince',        12.00,  'kg',     3.00,   650.00, '2025-01-02 09:00:00'),
(1, 'Burger Buns',          60.00,  'units',  15.00,   35.00, '2025-01-02 09:00:00'),
(1, 'Lettuce',               4.00,  'kg',     1.00,   150.00, '2025-01-01 09:00:00'),
(1, 'Cheddar Cheese',        8.00,  'kg',     2.00,  1100.00, '2025-01-01 09:00:00'),
(2, 'Mozzarella Cheese',    20.00,  'kg',     5.00,  1200.00, '2025-01-05 09:00:00'),
(2, 'Pizza Dough',          70.00,  'units',  20.00,   85.00, '2025-01-05 09:00:00'),
(2, 'Tomato Sauce',         25.00,  'litres', 8.00,   180.00, '2025-01-05 09:00:00'),
(2, 'Pepperoni',             8.00,  'kg',     2.00,  1800.00, '2025-01-04 09:00:00'),
(2, 'Chicken Breast',       18.00,  'kg',     5.00,   700.00, '2025-01-04 09:00:00');

INSERT INTO MENU_INGREDIENT (MenuItemId, InventoryId) VALUES
(1,  1),
(1,  2),
(1,  3),
(2,  1),
(2,  2),
(2,  3),
(2,  4),
(3,  1),
(3,  2),
(3,  3),
(3,  5),
(4,  1),
(4,  2),
(4,  3),
(4,  6),
(4,  7),
(5,  2),
(5,  3),
(5,  5),
(9,  9),
(9,  3),
(9,  11),
(10, 9),
(10, 10),
(11, 5),
(11, 13),
(11, 15),
(12, 5),
(12, 14),
(13, 2),
(13, 11),
(13, 8),
(6,  2),
(6,  3),
(6,  5),
(7,  2),
(7,  3),
(7,  12),
(7,  4),
(8,  2),
(8,  5),
(8,  1);

INSERT INTO BRANCH_MENU_ITEM (BranchId, MenuItemId, IsAvailable) VALUES
(1,  1,  TRUE),
(1,  2,  TRUE),
(1,  3,  TRUE),
(1,  4,  TRUE),
(1,  5,  TRUE),
(1,  6,  TRUE),
(1,  7,  TRUE),
(1,  8,  TRUE),
(1,  9,  TRUE),
(1,  10, TRUE),
(1,  11, TRUE),
(1,  12, TRUE),
(1,  13, TRUE),
(1,  16, TRUE),
(1,  17, TRUE),
(1,  19, TRUE),
(2,  1,  TRUE),
(2,  2,  TRUE),
(2,  3,  TRUE),
(2,  4,  TRUE),
(2,  5,  TRUE),
(2,  7,  TRUE),
(2,  11, TRUE),
(2,  13, TRUE),
(2,  16, TRUE),
(2,  17, TRUE),
(3,  1,  TRUE),
(3,  2,  TRUE),
(3,  3,  TRUE),
(3,  6,  TRUE),
(3,  8,  TRUE),
(3,  9,  TRUE),
(3,  11, TRUE),
(3,  13, TRUE),
(3,  16, TRUE),
(3,  19, TRUE);

INSERT INTO DISCOUNT (DiscountName, DiscountValue, StartDate, EndDate, IsActive) VALUES
('New Year Offer',        15.00, '2025-01-01', '2025-01-07', TRUE),
('Student Discount',      10.00, '2025-01-01', '2025-06-30', TRUE),
('Happy Hour',            20.00, '2025-01-01', '2025-12-31', TRUE),
('Weekend Special',       12.00, '2025-01-01', '2025-12-31', TRUE),
('Loyalty Member',         8.00, '2025-01-01', '2025-12-31', TRUE),
('Ramadan Special',       25.00, '2025-03-01', '2025-03-31', FALSE),
('Eid Mubarak Deal',      20.00, '2025-04-01', '2025-04-05', FALSE),
('Summer Sizzle',         15.00, '2025-06-01', '2025-08-31', FALSE),
('Back to School',        10.00, '2024-09-01', '2024-09-30', FALSE),
('Anniversary Special',   30.00, '2024-10-01', '2024-10-15', FALSE),
('Flash Sale',            35.00, '2024-11-25', '2024-11-25', FALSE),
('Christmas Offer',       20.00, '2024-12-24', '2024-12-26', FALSE),
('Independence Day',      14.00, '2025-08-14', '2025-08-14', FALSE),
('Defense Day',           10.00, '2025-09-06', '2025-09-06', FALSE),
('Grand Opening DHA',     50.00, '2021-04-01', '2021-04-07', FALSE),
('Winter Warmer',          5.00, '2025-01-01', '2025-02-28', TRUE);

INSERT INTO DEAL (DealName, TotalAmount, IsActive) VALUES
('Family Feast',          2999.00, TRUE),
('Couple Combo',          1699.00, TRUE),
('Student Saver',          999.00, TRUE),
('Friday Night Special',  3499.00, TRUE),
('Pizza Party Pack',      4999.00, TRUE),
('Lunch Express',          799.00, TRUE),
('Kids Fun Meal',          699.00, TRUE),
('Meat Lovers Bundle',    3299.00, TRUE),
('Veggie Delight Deal',   2499.00, TRUE),
('Weekend Brunch Box',    1899.00, TRUE),
('Office Order Pack',     5999.00, TRUE),
('Midnight Munchies',     1299.00, TRUE),
('Tikka Lover',           1599.00, TRUE),
('Pasta Night',           1399.00, TRUE),
('Drinks Bundle',          499.00, TRUE),
('Ramadan Iftar Pack',    3999.00, FALSE);

INSERT INTO DEAL_ITEM (DealId, MenuItemId, Quantity) VALUES
(1,  1,  1),
(1,  2,  1),
(1,  13, 2),
(1,  16, 4),
(2,  3,  1),
(2,  13, 1),
(2,  16, 2),
(3,  1,  1),
(3,  13, 1),
(3,  16, 1),
(4,  7,  1),
(4,  2,  1),
(4,  3,  1),
(4,  13, 2),
(4,  16, 4),
(5,  1,  2),
(5,  2,  1),
(5,  3,  1),
(5,  13, 4),
(5,  16, 6),
(6,  9,  1),
(6,  13, 1),
(6,  16, 1),
(7,  20, 1),
(7,  16, 1),
(8,  2,  1),
(8,  7,  1),
(8,  11, 1),
(8,  16, 2),
(9,  4,  1),
(9,  5,  1),
(9,  14, 1),
(9,  16, 2),
(10, 1,  1),
(10, 13, 2),
(10, 18, 2),
(11, 2,  2),
(11, 3,  2),
(11, 7,  1),
(11, 13, 4),
(11, 16, 6),
(12, 5,  1),
(12, 13, 1),
(12, 16, 2),
(13, 6,  1),
(13, 13, 1),
(13, 16, 2),
(14, 9,  1),
(14, 10, 1),
(14, 13, 1),
(15, 16, 3),
(15, 17, 2);

INSERT INTO BRANCH_DEAL (BranchId, DealId, IsAvailable) VALUES
(1,  1,  TRUE),
(1,  2,  TRUE),
(1,  3,  TRUE),
(1,  4,  TRUE),
(1,  5,  TRUE),
(1,  6,  TRUE),
(1,  7,  TRUE),
(1,  8,  TRUE),
(1,  15, TRUE),
(2,  1,  TRUE),
(2,  2,  TRUE),
(2,  3,  TRUE),
(2,  4,  TRUE),
(2,  8,  TRUE),
(2,  12, TRUE),
(2,  15, TRUE),
(3,  1,  TRUE),
(3,  2,  TRUE),
(3,  3,  TRUE),
(3,  6,  TRUE),
(3,  9,  TRUE),
(3,  13, TRUE),
(3,  14, TRUE),
(3,  15, TRUE),
(4,  1,  TRUE),
(4,  2,  TRUE),
(4,  5,  TRUE),
(4,  10, TRUE),
(4,  15, TRUE),
(5,  1,  TRUE),
(5,  4,  TRUE),
(5,  11, TRUE),
(5,  15, TRUE),
(6,  1,  FALSE),
(6,  2,  FALSE);

INSERT INTO `ORDER` (TakenBy, PreparedBy, DiscountId, DiscountAmount, OrderDateTime, Status, PaymentStatus, CashTendered, ChangeDue, TotalAmount) VALUES
('sara.malik', 'bilal.ahmed', 1,    127.50, '2025-01-06 11:15:00', 'Completed',      'Paid',   1000.00,  127.50,  850.00),
('sara.malik', 'bilal.ahmed', NULL,   0.00, '2025-01-06 12:30:00', 'Completed',      'Paid',   2000.00,   50.00, 1950.00),
('sara.malik', 'nadia.ali',   2,    170.00, '2025-01-06 13:45:00', 'Completed',      'Paid',   2000.00,  320.00, 1530.00),
('sara.malik', 'bilal.ahmed', NULL,   0.00, '2025-01-06 15:00:00', 'Completed',      'Paid',   3000.00,  150.00, 2850.00),
('sara.malik', 'nadia.ali',   3,    230.00, '2025-01-06 16:20:00', 'Completed',      'Paid',   1500.00,  350.00, 920.00),
('sara.malik', 'bilal.ahmed', NULL,   0.00, '2025-01-06 17:45:00', 'Completed',      'Paid',   2000.00,  250.00, 1700.00),
('sara.malik', 'nadia.ali',   4,    114.00, '2025-01-06 19:00:00', 'Completed',      'Paid',   1200.00,   86.00,  950.00),
('sara.malik', 'bilal.ahmed', NULL,   0.00, '2025-01-06 20:15:00', 'In Preparation', 'Paid',   1500.00,   50.00, 1150.00),
('sara.malik', 'nadia.ali',   NULL,   0.00, '2025-01-06 21:30:00', 'Pending',        'Paid',   2000.00,  350.00, 1700.00),
('sara.malik', 'bilal.ahmed', 1,    202.50, '2025-01-07 11:00:00', 'Completed',      'Paid',   2000.00,  147.50, 1350.00),
('sara.malik', 'nadia.ali',   NULL,   0.00, '2025-01-07 12:30:00', 'Completed',      'Paid',   1000.00,   50.00,  950.00),
('sara.malik', 'bilal.ahmed', 2,     95.00, '2025-01-07 14:00:00', 'Completed',      'Paid',   1500.00,  455.00,  855.00),
('sara.malik', 'nadia.ali',   NULL,   0.00, '2025-01-07 15:30:00', 'Cancelled',      'Refunded', NULL,   NULL,  1150.00),
('sara.malik', 'bilal.ahmed', 3,    270.00, '2025-01-07 17:00:00', 'Completed',      'Paid',   1500.00,  270.00, 1080.00),
('sara.malik', 'nadia.ali',   NULL,   0.00, '2025-01-07 19:00:00', 'Completed',      'Paid',   3000.00,  850.00, 1700.00),
('sara.malik', 'bilal.ahmed', 4,    102.00, '2025-01-08 11:00:00', 'Completed',      'Paid',   1000.00,  148.00,  850.00),
('sara.malik', 'nadia.ali',   NULL,   0.00, '2025-01-08 12:45:00', 'In Preparation', 'Paid',   2000.00,   50.00, 1150.00),
('hina.shah',  'kamran.butt', NULL,   0.00, '2025-01-06 11:30:00', 'Completed',      'Paid',   2000.00,  200.00, 1700.00),
('hina.shah',  'zara.qureshi',1,    202.50, '2025-01-06 13:00:00', 'Completed',      'Paid',   2000.00,  352.50, 1350.00),
('hina.shah',  'kamran.butt', NULL,   0.00, '2025-01-06 15:30:00', 'Completed',      'Paid',   1000.00,   50.00,  950.00);

INSERT INTO ORDER_ITEM (OrderId, MenuItemId, DealId, Quantity, UnitPrice) VALUES
(1,  1,  NULL, 1,  850.00),
(2,  2,  NULL, 1,  950.00),
(2,  13, NULL, 2,  200.00),
(3,  3,  NULL, 1, 1150.00),
(3,  13, NULL, 1,  200.00),
(3,  16, NULL, 2,  120.00),
(4,  7,  NULL, 1, 1350.00),
(4,  14, NULL, 1,  150.00),
(4,  16, NULL, 3,  120.00),
(5,  1,  NULL, 1,  850.00),
(5,  13, NULL, 1,  200.00),
(6,  3,  NULL, 1, 1150.00),
(6,  16, NULL, 2,  120.00),
(7,  2,  NULL, 1,  950.00),
(8,  3,  NULL, 1, 1150.00),
(9,  1,  NULL, 1,  850.00),
(9,  2,  NULL, 1,  950.00),
(10, 4,  NULL, 1,  950.00),
(10, 13, NULL, 2,  200.00),
(11, 2,  NULL, 1,  950.00),
(12, 1,  NULL, 1,  850.00),
(13, 3,  NULL, 1, 1150.00),
(14, 6,  NULL, 1,  950.00),
(14, 13, NULL, 1,  200.00),
(15, 3,  NULL, 1, 1150.00),
(15, 16, NULL, 2,  120.00),
(16, 1,  NULL, 1,  850.00),
(17, 3,  NULL, 1, 1150.00),
(18, 3,  NULL, 1, 1150.00),
(18, 16, NULL, 2,  120.00),
(19, 4,  NULL, 1,  950.00),
(19, 13, NULL, 2,  200.00),
(20, 2,  NULL, 1,  950.00);

UPDATE BRANCH
SET PhoneNumber = '042-35761999',
    ClosingTime = '23:30:00'
WHERE BranchId = 1;

UPDATE BRANCH
SET IsActive = TRUE
WHERE BranchId = 16;

UPDATE EMPLOYEE
SET PhoneNumber  = '03001239999',
    Email        = 'ahmed.khan.new@pizzapoint.pk'
WHERE UserName = 'ahmed.khan';

UPDATE EMPLOYEE
SET BranchId = 2
WHERE UserName = 'farah.nawaz';

UPDATE EMPLOYEE
SET IsActive        = FALSE,
    ResignationDate = '2025-01-08'
WHERE UserName = 'mehwish.javed';

UPDATE PRICE
SET IsActive = FALSE,
    EndDate  = '2025-01-07'
WHERE PriceId = 2;

INSERT INTO PRICE (MenuItemId, Amount, EffectiveDate, EndDate, IsActive)
VALUES (1, 900.00, '2025-01-08', NULL, TRUE);

UPDATE INVENTORY
SET Quantity      = 30.00,
    LastRestocked = NOW()
WHERE InventoryId = 1;

UPDATE INVENTORY
SET ReorderLevel = 3.00
WHERE InventoryId = 4;

UPDATE DISCOUNT
SET EndDate  = '2025-01-10',
    IsActive = TRUE
WHERE DiscountId = 1;

UPDATE DEAL
SET TotalAmount = 3199.00
WHERE DealId = 8;

UPDATE `ORDER`
SET Status = 'Completed'
WHERE OrderId = 8;

UPDATE `ORDER`
SET Status        = 'Cancelled',
    PaymentStatus = 'Refunded'
WHERE OrderId = 9;

UPDATE BRANCH_MENU_ITEM
SET IsAvailable = FALSE
WHERE BranchId = 1 AND MenuItemId = 8;

UPDATE BRANCH_DEAL
SET IsAvailable = FALSE
WHERE BranchId = 1 AND DealId = 5;

DELETE FROM SHIFT
WHERE UserName = 'mehwish.javed';

DELETE FROM BRANCH_DEAL
WHERE BranchId = 6;

DELETE FROM BRANCH_MENU_ITEM
WHERE BranchId = 16 AND IsAvailable = FALSE;