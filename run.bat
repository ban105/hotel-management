@echo off
setlocal

gcc -Wall -Wextra -std=c11 -Iinclude main.c src\room.c src\customer.c src\booking.c src\service.c src\bill.c src\admin.c src\employee.c src\menu.c src\utils.c -o hotel.exe
if errorlevel 1 (
    echo.
    echo Build failed. Kiem tra loi bien dich o tren.
    pause
    exit /b 1
)

hotel.exe
