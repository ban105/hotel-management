#include <stdio.h>
#include "menu.h"
#include "room.h"
#include "customer.h"
#include "booking.h"
#include "utils.h"

void showWelcome() {
    clearScreen();
    printf("\n");
    printLine(56);
    printf("          ***  HE THONG QUAN LY KHACH SAN  ***\n");
    printf("            ***      Hotel Manager       ***\n");
    printLine(56);
    printf("\n");
}

void mainMenu(Room rooms[], int *roomCount,
              Customer customers[], int *customerCount,
              Booking bookings[], int *bookingCount,
              UsedService usedServices[], int *usedCount,
              Service services[], int *serviceCount) {

    int choice;
    do {
        clearScreen();
        printHeader("MENU CHINH");
        printf("| 1. Quan ly Phong                                       |\n");
        printf("| 2. Quan ly Khach hang                                  |\n");
        printf("| 3. Dat phong / Check-in / Check-out                    |\n");
        printf("| 4. Quan ly Dich vu                                     |\n");
        printf("| 5. Quan ly Hoa don                                     |\n");
        printf("| 6. Khu vuc Quan tri vien (Admin)                       |\n");   /* <- THEM DONG NAY */
        printf("| 0. Thoat chuong trinh                                  |\n");
        printLine(58);

        choice = inputInt("  Chon chuc nang: ", 0, 6);   /* <- DOI 5 THANH 6 */

        switch (choice) {
            case 1:
                menuRoom(rooms, roomCount);
                break;
            case 2:
                menuCustomer(customers, customerCount);
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
            case 6:                                        /* <- THEM CASE NAY */
                menuAdmin(rooms,     roomCount,
                          customers, customerCount,
                          bookings,  bookingCount,
                          usedServices, usedCount,
                          services,  serviceCount);
                break;
            case 0:
                clearScreen();
                printf("\n  Tam biet! Hen gap lai.\n\n");
                break;
        }
    } while (choice != 0);
}