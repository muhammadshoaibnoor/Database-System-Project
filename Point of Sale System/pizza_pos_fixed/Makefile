CXX      = g++
CXXFLAGS = -std=c++17 -Wall -O2
INCLUDES = -IC:/raylib/w64devkit/include -Isrc -I"include"
LIBS     = -LC:/raylib/w64devkit/lib -L"." -lraylib -lmysql -lopengl32 -lgdi32 -lwinmm -lws2_32
TARGET   = game

SRCS = main.cpp \
       src/core/session.cpp \
       src/db/db.cpp \
       src/db/db_auth.cpp \
       src/db/db_orders.cpp \
       src/db/db_menu.cpp \
       src/db/db_inventory.cpp \
       src/db/db_deals.cpp \
       src/db/db_staff.cpp \
       src/db/db_shifts.cpp \
       src/db/db_salary.cpp \
       src/db/db_discounts.cpp \
       src/db/db_branches.cpp \
       src/db/db_reports.cpp \
       src/ui/ui_helpers.cpp \
       src/ui/screens/screen_login.cpp \
       src/ui/screens/screen_dashboard_admin.cpp \
       src/ui/screens/screen_dashboard_receptionist.cpp \
       src/ui/screens/screen_dashboard_kitchen.cpp \
       src/ui/panels/panel_menu_manager.cpp \
       src/ui/panels/panel_inventory.cpp \
       src/ui/panels/panel_staff.cpp \
       src/ui/panels/panel_salary.cpp \
       src/ui/panels/panel_deals.cpp \
       src/ui/panels/panel_discounts.cpp \
       src/ui/panels/panel_branches.cpp \
       src/ui/panels/panel_shifts.cpp \
       src/ui/panels/panel_reports.cpp \
       src/ui/panels/panel_order_form.cpp \
       src/ui/panels/panel_order_list.cpp \
       src/ui/panels/panel_kitchen_queue.cpp \
       src/ui/panels/panel_clock.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(INCLUDES) $(LIBS)

clean:
	rm -f game.exe

run:
	.\game.exe

.PHONY: all clean run