Place MySQL Connector/C header files here.

Required files (from MySQL Connector/C or MySQL Server installation):
  - mysql.h         (main header)
  - mysql_com.h
  - mysql_version.h
  - mysql_time.h
  - my_list.h
  - my_alloc.h
  - typelib.h
  - errmsg.h
  - mysqld_error.h

How to get them:
  Option A: Copy from your MySQL Server installation:
    C:\Program Files\MySQL\MySQL Server X.X\include\

  Option B: Download MySQL Connector/C (libmysqlclient) from:
    https://dev.mysql.com/downloads/connector/c/

  Option C: If using w64devkit/MSYS2/MinGW, install via pacman:
    pacman -S mingw-w64-x86_64-libmysqlclient
    Then copy headers from:
    C:\msys64\mingw64\include\mysql\
