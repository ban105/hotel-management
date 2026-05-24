#include <stdio.h>
#include <string.h>
#include "types.h"
#include "menu.h"
#include "room.h"
#include "customer.h"
#include "booking.h"
#include "service.h"
#include "bill.h"
#include "admin.h"      /* <- THEM DONG NAY */
#include "utils.h"

/**
 * Ham dong bo trang thai phong dua tren danh sach booking
 * - Khach "Dang o" (status == 1) -> Ep trang thai phong sang "Dang thue" (1)
 * - Khach "Da dat" (status == 0)  -> Giu nguyen phong la "Trong" (0)
 */
void syncRoomStatusWithBookings(Room rooms[], int roomCount,
                                Booking bookings[], int bookingCount) {
    /* Reset tat ca phong Dang thue (1) ve Trong (0) truoc */
    for (int j = 0; j < roomCount; j++) {
        if (rooms[j].status == ROOM_OCCUPIED)
            rooms[j].status = ROOM_EMPTY;
    }

    /* Quet tung booking va cap nhat trang thai phong */
    for (int i = 0; i < bookingCount; i++) {
        int rIdx = findRoomById(rooms, roomCount, bookings[i].roomId);
        if (rIdx < 0) continue;

        switch (bookings[i].status) {
            case BOOKING_PENDING:   /* Da dat (0) -> phong bi giu, khong cho dat tiep */
                rooms[rIdx].status = ROOM_OCCUPIED;
                break;

            case BOOKING_CHECKIN:   /* Dang o (1) -> phong dang thue */
                rooms[rIdx].status = ROOM_OCCUPIED;
                break;

            case BOOKING_DONE:      /* Da tra (2) -> phong ve Trong */
            case BOOKING_CANCEL:    /* Da huy (3) -> phong ve Trong */
                if (rooms[rIdx].status != ROOM_MAINTAIN)
                    rooms[rIdx].status = ROOM_EMPTY;
                break;
        }
    }

    /* Luu lai file rooms.txt sau khi dong bo */
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

    // 1. Tai toan bo du lieu tu file text len mang RAM
    roomCount     = loadRooms(rooms);
    customerCount = loadCustomers(customers);
    bookingCount  = loadBookings(bookings);
    serviceCount  = loadServices(services);
    usedCount     = loadUsedServices(usedServices);

    // ================================================================
    // 2. DONG BO HOA DU LIEU
    // ================================================================
    syncRoomStatusWithBookings(rooms, roomCount, bookings, bookingCount);
    // ================================================================

    // 3. Hien thi man hinh chao mung
    showWelcome();
    printf("  Da tai %d phong | %d khach hang | %d booking (Da dong bo).\n",
           roomCount, customerCount, bookingCount);
    printf("  Da tai %d dich vu | %d ban ghi dich vu da goi.\n",
           serviceCount, usedCount);
    pauseScreen();

    // 4. Vao vong lap Menu chinh
    mainMenu(rooms,        &roomCount,
             customers,    &customerCount,
             bookings,     &bookingCount,
             usedServices, &usedCount,
             services,     &serviceCount);  /* <- DOI serviceCount THANH &serviceCount */

    return 0;
}