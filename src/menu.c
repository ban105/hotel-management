#include <stdio.h>
#include "menu.h"
#include "room.h"
#include "customer.h"
#include "booking.h"
#include "bill.h"
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
              Service services[], int *serviceCount) {

    int choice;
    do {
        clearScreen();
        printHeader("MENU CHINH");
        printLine(58);
        printf("| 1. Quan ly Phong                                       |\n");
        printf("| 2. Quan ly Khach hang                                  |\n");
        printf("| 3. Check-in / Check-out                                |\n");
        printf("| 4. Them dich vu                                        |\n");
        printf("| 5. Xem/In hoa don                                      |\n");
        printf("| 0. Thoat chuong trinh                                  |\n");
        printLine(58);

        choice = inputInt("  Chon chuc nang: ", 0, 5);

        switch (choice) {
            case 1:
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
                viewBillByBooking(bookings, *bookingCount,
                                  rooms, *roomCount,
                                  customers, *customerCount,
                                  usedServices, *usedCount,
                                  services, *serviceCount);
                break;
            case 0:
                clearScreen();
                printf("\n  Tam biet! Hen gap lai.\n\n");
                break;
        }
    } while (choice != 0);
}
