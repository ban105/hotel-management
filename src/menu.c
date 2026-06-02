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
    printBoxLine();
    printBoxCentered("HE THONG QUAN LY KHACH SAN");
    printBoxCentered("HOTEL MANAGER");
    printBoxLine();
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
        printBoxLine();
        printBoxCentered("MENU CHINH");
        printBoxLine();
        printBoxText("1. Quan ly Phong");
        printBoxText("2. Quan ly Khach hang");
        printBoxText("3. Check-in / Check-out");
        printBoxText("4. Them dich vu");
        printBoxText("5. Xem/In hoa don");
        printBoxText("0. Thoat chuong trinh");
        printBoxLine();

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
