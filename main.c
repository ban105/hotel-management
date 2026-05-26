#include <stdio.h>
#include <string.h>
#include "types.h"
#include "menu.h"
#include "room.h"
#include "customer.h"
#include "booking.h"
#include "service.h"
#include "bill.h"
#include "admin.h"      
#include "utils.h"
#include "employee.h"


void syncRoomStatusWithBookings(Room rooms[], int roomCount,
                                Booking bookings[], int bookingCount) {
    
    for (int j = 0; j < roomCount; j++) {
        if (rooms[j].status == ROOM_OCCUPIED)
            rooms[j].status = ROOM_EMPTY;
    }

    
    for (int i = 0; i < bookingCount; i++) {
        int rIdx = findRoomById(rooms, roomCount, bookings[i].roomId);
        if (rIdx < 0) continue;

        switch (bookings[i].status) {
            case BOOKING_CHECKIN:   
                rooms[rIdx].status = ROOM_OCCUPIED;
                break;

            case BOOKING_DONE:      
            case BOOKING_CANCEL:    
                if (rooms[rIdx].status != ROOM_MAINTAIN)
                    rooms[rIdx].status = ROOM_EMPTY;
                break;
        }
    }

    
    saveRooms(rooms, roomCount);
}

int main() {
    Room     rooms[MAX_ROOMS];
    int      roomCount     = 0;
    Customer customers[MAX_CUSTOMERS];
    int      customerCount = 0;
    Booking  bookings[MAX_BOOKINGS];
    int      bookingCount  = 0;

    Service     services[MAX_SERVICES];
    int         serviceCount = 0;
    UsedService usedServices[MAX_USED_SERVICES];
    int         usedCount    = 0;
    Employee employees[MAX_EMPLOYEES];
    int         employeeCount = 0;


    // 1. Tai toan bo du lieu tu file text len mang RAM
    roomCount     = loadRooms(rooms);
    customerCount = loadCustomers(customers);
    bookingCount  = loadBookings(bookings);
    serviceCount  = loadServices(services);
    usedCount     = loadUsedServices(usedServices);
    employeeCount = loadEmployees(employees);

    // ================================================================
    // 2. DONG BO HOA DU LIEU
    // ================================================================
    syncRoomStatusWithBookings(rooms, roomCount, bookings, bookingCount);
    // ================================================================

    // 3. Hien thi man hinh chao mung
    showWelcome();
    char summaryLine[80];

    printf("  +------------------------------------------+\n");
    printf("  | TOM TAT DU LIEU                          |\n");
    printf("  +------------------------------------------+\n");
    snprintf(summaryLine, sizeof(summaryLine),
             "%-7s: %-3d | %-6s: %-3d | %-7s: %-3d",
             "Phong", roomCount, "Khach", customerCount, "Booking", bookingCount);
    printf("  | %-40.40s |\n", summaryLine);
    snprintf(summaryLine, sizeof(summaryLine),
             "%-7s: %-3d | %-6s: %-3d",
             "Dich vu", serviceCount, "Da goi", usedCount);
    printf("  | %-40.40s |\n", summaryLine);
    printf("  +------------------------------------------+\n");
    pauseScreen();

    User currentUser;
    if (!systemLogin(&currentUser)) {
        printf("\n  Da huy dang nhap. Thoat chuong trinh.\n");
        return 0;
    }

    // 4. Vao vong lap Menu chinh
    mainMenu(rooms,        &roomCount,
             customers,    &customerCount,
             bookings,     &bookingCount,
             usedServices, &usedCount,
             services,     &serviceCount,
             employees,    &employeeCount,
             currentUser.role);

    return 0;
}
