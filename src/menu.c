#include <stdio.h>
#include "menu.h"
#include "room.h"
#include "customer.h"
#include "booking.h"
#include "admin.h"
#include "utils.h"

void showWelcome() {
    clearScreen();
    printf("\n");
    printf("  +------------------------------------------+\n");
    printf("  |        HE THONG QUAN LY KHACH SAN        |\n");
    printf("  |               HOTEL MANAGER              |\n");
    printf("  +------------------------------------------+\n");
    printf("\n");
}

void mainMenu(Room rooms[], int *roomCount,
              Customer customers[], int *customerCount,
              Booking bookings[], int *bookingCount,
              UsedService usedServices[], int *usedCount,
              Service services[], int *serviceCount,
              Employee employees[], int *employeeCount,
              int currentRole) {

    int choice;
    do {
        clearScreen();
        printHeader("MENU CHINH");
        printf("| Vai tro: %-46s|\n",
               currentRole == ROLE_ADMIN ? "Admin" : "Le tan");
        printLine(58);
        if (currentRole == ROLE_ADMIN)
            printf("| 1. Quan ly Phong                                       |\n");
        else
            printf("| 1. Quan ly Phong                     (Admin)           |\n");
        printf("| 2. Quan ly Khach hang                                  |\n");
        printf("| 3. Dat phong / Check-in / Check-out                    |\n");
        printf("| 4. Quan ly Dich vu                                     |\n");
        printf("| 5. Quan ly Hoa don                                     |\n");
        if (currentRole == ROLE_ADMIN)
            printf("| 6. Khu vuc Quan tri vien (Admin)                       |\n");
        printf("| 0. Thoat chuong trinh                                  |\n");
        printLine(58);

        choice = inputInt("  Chon chuc nang: ", 0, 6);   

        switch (choice) {
            case 1:
                if (currentRole != ROLE_ADMIN) {
                    printf("  [!] Chuc nang nay chi danh cho Admin.\n");
                    pauseScreen();
                    break;
                }
                menuRoom(rooms, roomCount);
                break;
            case 2:
                menuCustomer(customers, customerCount,
                             bookings, *bookingCount);
                break;
            case 3:
                menuBooking(bookings, bookingCount,
                            rooms, roomCount,
                            customers, *customerCount);
                break;
            case 4:
                menuService(usedServices, usedCount,
                            bookings, *bookingCount,
                            rooms, *roomCount,
                            customers, *customerCount,
                            services, *serviceCount);
                break;
            case 5:
                menuBill(bookings, *bookingCount,
                         rooms, *roomCount,
                         customers, *customerCount,
                         usedServices, *usedCount,
                         services, *serviceCount);
                break;
            case 6:
                if (currentRole != ROLE_ADMIN) {
                    printf("  [!] Chuc nang nay chi danh cho Admin.\n");
                    pauseScreen();
                    break;
                }
                menuAdmin(rooms,     roomCount,
                          customers, customerCount,
                          bookings,  bookingCount,
                          usedServices, usedCount,
                          services,  serviceCount,
                          employees, employeeCount);
                break;
            case 0:
                clearScreen();
                printf("\n  Tam biet! Hen gap lai.\n\n");
                break;
        }
    } while (choice != 0);
}
