#include <stdio.h>
#include "report.h"
#include "room.h"
#include "customer.h"
#include "booking.h"
#include "service.h"
#include "utils.h"

#define REPORT_WIDTH 56

static void printReportCentered(const char *text) {
    int len = 0;
    while (text[len] != '\0') len++;
    if (len > REPORT_WIDTH) len = REPORT_WIDTH;

    int left = (REPORT_WIDTH - len) / 2;
    int right = REPORT_WIDTH - len - left;

    printf("|%*s%.*s%*s|\n", left, "", len, text, right, "");
}

static void printReportSection(const char *title) {
    printf("| %-*.*s |\n", REPORT_WIDTH - 2, REPORT_WIDTH - 2, title);
}

static void printReportIntRow(const char *label, int value) {
    printf("| %-30s : %21d |\n", label, value);
}

static void printReportMoneyRow(const char *label, float value) {
    printf("| %-30s : %17.0f VND |\n", label, value);
}

static void printReportPercentRow(const char *label, float value) {
    printf("| %-30s : %19.2f %% |\n", label, value);
}

void showStatistics(Room rooms[], int roomCount,
                    Customer customers[], int customerCount,
                    Booking bookings[], int bookingCount,
                    UsedService usedServices[], int usedCount,
                    Service services[], int serviceCount) {
    int emptyRooms = 0;
    int occupiedRooms = 0;
    float roomRevenue = 0.0f;
    float serviceRevenue = 0.0f;

    for (int i = 0; i < roomCount; i++) {
        if (rooms[i].status == ROOM_EMPTY) {
            emptyRooms++;
        } else if (rooms[i].status == ROOM_OCCUPIED) {
            occupiedRooms++;
        }
    }

    /* Tinh doanh thu tu cac booking da hoan tat. */
    for (int i = 0; i < bookingCount; i++) {
        if (bookings[i].status != BOOKING_DONE) continue;

        int roomIndex = findRoomById(rooms, roomCount, bookings[i].roomId);
        int customerIndex = findCustomerById(customers, customerCount,
                                             bookings[i].customerId);
        if (roomIndex < 0 || customerIndex < 0) continue;

        int nights = calcNights(bookings[i].checkInDate,
                                bookings[i].checkOutDate);
        if (nights <= 0) nights = 1;

        roomRevenue += rooms[roomIndex].price * nights *
                       (1.0f - rankDiscount(customers[customerIndex].rank));

        serviceRevenue += calcServiceCost(usedServices, usedCount,
                                          services, serviceCount,
                                          bookings[i].bookingId);
    }

    float totalRevenue = roomRevenue + serviceRevenue;
    float occupancyRate = 0.0f;
    if (roomCount > 0) {
        occupancyRate = (float)occupiedRooms / roomCount * 100.0f;
    }

    clearScreen();
    printf("+========================================================+\n");
    printReportCentered("BAO CAO THONG KE TONG HOP");
    printf("+========================================================+\n");
    printReportSection("THONG KE PHONG");
    printf("+--------------------------------------------------------+\n");
    printReportIntRow("Tong so phong", roomCount);
    printReportIntRow("Phong trong", emptyRooms);
    printReportIntRow("Phong dang thue", occupiedRooms);
    printf("+--------------------------------------------------------+\n");
    printReportSection("THONG KE KHACH HANG / BOOKING");
    printf("+--------------------------------------------------------+\n");
    printReportIntRow("Tong so khach hang", customerCount);
    printReportIntRow("Tong so booking", bookingCount);
    printf("+--------------------------------------------------------+\n");
    printReportSection("THONG KE DOANH THU");
    printf("+--------------------------------------------------------+\n");
    printReportMoneyRow("Doanh thu phong", roomRevenue);
    printReportMoneyRow("Doanh thu dich vu", serviceRevenue);
    printReportMoneyRow("Tong doanh thu", totalRevenue);
    printf("+--------------------------------------------------------+\n");
    printReportPercentRow("Ty le lap day phong", occupancyRate);
    printf("+========================================================+\n");

    pauseScreen();
}
