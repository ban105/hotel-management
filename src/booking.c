#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "booking.h"
#include "customer.h"
#include "bill.h"
#include "utils.h"

#define BOOKING_FILE "data/bookings.txt"

/* ============================
   TIEN ICH
   ============================ */

/* Lay ngay gio hien tai */
void getCurrentDateTime(char *dateStr, char *timeStr) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    if (dateStr) sprintf(dateStr, "%02d/%02d/%04d",
                         tm->tm_mday, tm->tm_mon+1, tm->tm_year+1900);
    if (timeStr) sprintf(timeStr, "%02d:%02d",
                         tm->tm_hour, tm->tm_min);
}

/* Chuyen ngay DD/MM/YYYY sang so ngay dai dien dua tren epoch time */
static int dateToInt(const char *date) {
    int d, m, y;
    if (sscanf(date, "%d/%d/%d", &d, &m, &y) != 3) {
        return 0;
    }
    
    struct tm t = {0};
    t.tm_mday = d;
    t.tm_mon = m - 1;       // Trong C, thang chay tu 0 den 11
    t.tm_year = y - 1900;   // Nam tinh tu moc 1900
    t.tm_isdst = -1;

    time_t seconds = mktime(&t);
    if (seconds == -1) {
        return 0;
    }
    return (int)(seconds / 86400);
}

/* Ngay hom nay theo DD/MM/YYYY */
static void getToday(char *dateStr) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    sprintf(dateStr, "%02d/%02d/%04d",
            tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900);
}

/* Tinh so dem giua 2 ngay */
int calcNights(const char *checkIn, const char *checkOut) {
    int diff = dateToInt(checkOut) - dateToInt(checkIn);
    return diff > 0 ? diff : 1;
}

/* Tao ma booking tu dong BK001... */
void generateBookingId(Booking bookings[], int count, char *id) {
    int maxNum = 0;
    for (int i = 0; i < count; i++) {
        int num = 0;
        sscanf(bookings[i].bookingId, "BK%d", &num);
        if (num > maxNum) maxNum = num;
    }
    sprintf(id, "BK%03d", maxNum + 1);
}

/* Tao ma hoa don tu ma booking */
void generateBillId(char *billId, const char *bookingId) {
    int num = 0;
    sscanf(bookingId, "BK%d", &num);
    sprintf(billId, "HD%03d", num);
}

/* Chuoi trang thai booking */
static const char* bookingStatusStr(int status) {
    switch (status) {
        case 1: return "Dang o";  
        case 2: return "Da tra";  
        case 3: return "Da huy";  
        default: return "Khong ro"; 
    }
}

/* =========================================================================
   HE THONG IN BANG VA BANNER
   ========================================================================= */
static void printBookingBorder() {
    printf("  +-------+-------+-------+------------+------------+----------+\n");
}

static void printBookingBanner(const char *title) {
    printf("  +------------------------------------------------------------+\n");
    if (title) {
        int len = strlen(title);
        int total = 60;
        int pad = (total - len) / 2;
        printf("  |");
        for (int i = 0; i < pad; i++) printf(" ");
        printf("%s", title);
        for (int i = 0; i < total - pad - len; i++) printf(" ");
        printf("|\n");
    }
    printf("  +------------------------------------------------------------+\n");
}

static void printBookingTableHeader(const char *title) {
    if (title) {
        printBookingBanner(title);
    }
    printBookingBorder();
    printf("  | %-5s | %-5s | %-5s | %-10s | %-10s | %-8s |\n",
           "Ma BK", "Ma KH", "Phong", "Check-in", "Check-out", "T.Thai");
    printBookingBorder();
}

static void printBookingRow(Booking *b) {
    printf("  | %-5.5s | %-5.5s | %-5.5s | %-10.10s | %-10.10s | %-8.8s |\n",
           b->bookingId, b->customerId, b->roomId,
           b->checkInDate, b->checkOutDate,
           bookingStatusStr(b->status));
}

/* ============================
   IN PHIEU CHECK-IN
   ============================ */
void printCheckInReceipt(Booking *b, Room *r, Customer *c) {
    int nights    = calcNights(b->checkInDate, b->checkOutDate);
    float roomCost = r->price * nights;
    float discount = rankDiscount(c->rank);
    float discAmt  = roomCost * discount;
    float estimate = roomCost - discAmt;

    char billId[10];
    generateBillId(billId, b->bookingId);

    printf("  ---------------------------------------------------\n");
    printf("  |%-49s|\n", "               CHECK-IN THANH CONG");
    printf("  ---------------------------------------------------\n");
    printf("  | %-18s | %26.26s |\n", "Booking", b->bookingId);
    printf("  | %-18s | %26.26s |\n", "Bill", billId);
    printf("  | %-18s | %26.26s |\n", "Khach", c->name);
    printf("  | %-18s | %26.26s |\n", "Phong", r->id);
    printf("  | %-18s | %26.26s |\n", "Ngay check-in", b->checkInDate);
    printf("  | %-18s | %26.26s |\n", "Checkout du kien", b->checkOutDate);
    printf("  | %-18s | %22.0f VND |\n", "Tien phong", roomCost);
    printf("  | %-18s | %22.0f VND |\n", "Giam gia", discAmt);
    printf("  | %-18s | %22.0f VND |\n", "Tong", estimate);
    printf("  ---------------------------------------------------\n");
    printf("\n");
}

/* ============================
   TIM BOOKING
   ============================ */
int findBookingById(Booking bookings[], int count, const char *id) {
    for (int i = 0; i < count; i++)
        if (strcmp(bookings[i].bookingId, id) == 0) return i;
    return -1;
}

/* ============================
   CHECK-IN
   ============================ */
void checkIn(Booking bookings[], int *bookingCount,
             Room rooms[], int roomCount,
             Customer customers[], int customerCount) {
    clearScreen();
    printBookingBanner("CHECK-IN");

    if (*bookingCount >= MAX_BOOKINGS) {
        printf("  [!] Da dat toi da so booking.\n");
        pauseScreen();
        return;
    }

    Booking b;
    generateBookingId(bookings, *bookingCount, b.bookingId);

    do {
        printf("  Ma khach hang (VD: KH001): ");
        safeInput(b.customerId, sizeof(b.customerId));
        trimStr(b.customerId);
        if (findCustomerById(customers, customerCount, b.customerId) == -1) {
            printf("  [!] Khong tim thay khach hang '%s'.\n", b.customerId);
            b.customerId[0] = '\0';
        }
    } while (!isNotEmpty(b.customerId));

    getToday(b.checkInDate);
    printf("  Ngay check-in: %s (tu dong)\n", b.checkInDate);

    do {
        printf("  Ngay check-out (DD/MM/YYYY): ");
        safeInput(b.checkOutDate, sizeof(b.checkOutDate));
        trimStr(b.checkOutDate);
        if (!isValidDate(b.checkOutDate)) {
            printf("  [!] Sai dinh dang ngay thang (DD/MM/YYYY).\n");
            b.checkOutDate[0] = '\0';
            continue;
        }
        if (dateToInt(b.checkOutDate) < dateToInt(b.checkInDate)) {
            printf("  [!] Khong duoc nhap ngay trong qua khu.\n");
            b.checkOutDate[0] = '\0';
            continue;
        }
        if (dateToInt(b.checkOutDate) <= dateToInt(b.checkInDate)) {
            printf("  [!] Ngay check-out phai sau ngay check-in.\n");
            b.checkOutDate[0] = '\0';
        }
    } while (!isNotEmpty(b.checkOutDate));

    printf("\n  Danh sach phong trong:\n");
    printf("  +----------+----------+---------------+\n");
    printf("  | %-8s | %-8s | %13s |\n", "Ma phong", "Loai", "Gia/dem (d)");
    printf("  +----------+----------+---------------+\n");
    int found = 0;
    for (int i = 0; i < roomCount; i++) {
        if (rooms[i].status == ROOM_EMPTY) {
            printf("  | %-8.8s | %-8.8s | %13.0f |\n",
                   rooms[i].id, rooms[i].type, rooms[i].price);
            found++;
        }
    }
    printf("  +----------+----------+---------------+\n");
    if (found == 0) {
        printf("  [!] Khong con phong trong.\n");
        pauseScreen();
        return;
    }

    do {
        printf("  Chon ma phong: ");
        safeInput(b.roomId, sizeof(b.roomId));
        trimStr(b.roomId);
        int idx = findRoomById(rooms, roomCount, b.roomId);
        if (idx == -1 || rooms[idx].status != ROOM_EMPTY) {
            printf("  [!] Phong khong hop le hoac khong trong.\n");
            b.roomId[0] = '\0';
        }
    } while (!isNotEmpty(b.roomId));

    int cIdx = findCustomerById(customers, customerCount, b.customerId);
    int rIdx = findRoomById(rooms, roomCount, b.roomId);
    b.status = BOOKING_CHECKIN;

    printf("\n  Tao check-in cho booking %s? (y/n): ", b.bookingId);
    char confirm[5];
    safeInput(confirm, sizeof(confirm));
    if (confirm[0] != 'y' && confirm[0] != 'Y') {
        printf("  Da huy.\n");
        pauseScreen();
        return;
    }

    bookings[*bookingCount] = b;
    rooms[rIdx].status = ROOM_OCCUPIED;
    (*bookingCount)++;
    saveBookings(bookings, *bookingCount);
    saveRooms(rooms, roomCount);

    clearScreen();
    printCheckInReceipt(&bookings[*bookingCount - 1], &rooms[rIdx], &customers[cIdx]);
    pauseScreen();
}

/* ============================
   CHECK-OUT
   ============================ */
void checkOut(Booking bookings[], int bookingCount,
              Room rooms[], int roomCount,
              Customer customers[], int customerCount) {
    clearScreen();
    printBookingBanner("CHECK-OUT");

    char id[10];
    printf("\n  Nhap ma booking: ");
    safeInput(id, sizeof(id)); trimStr(id);

    int bIdx = findBookingById(bookings, bookingCount, id);
    if (bIdx == -1) {
        printf("  [!] Khong tim thay booking '%s'!\n", id);
        pauseScreen(); return;
    }
    if (bookings[bIdx].status != BOOKING_CHECKIN) {
        printf("  [!] Booking nay chua check-in!\n");
        printf("  Trang thai: %s\n", bookingStatusStr(bookings[bIdx].status));
        pauseScreen(); return;
    }

    int rIdx = findRoomById(rooms, roomCount, bookings[bIdx].roomId);
    int cIdx = findCustomerById(customers, customerCount, bookings[bIdx].customerId);
    if (rIdx == -1 || cIdx == -1) {
        printf("  [!] Loi du lieu phong hoac khach hang!\n");
        pauseScreen();
        return;
    }

    int nights      = calcNights(bookings[bIdx].checkInDate, bookings[bIdx].checkOutDate);
    float roomCost  = rooms[rIdx].price * nights;
    float discount  = rankDiscount(customers[cIdx].rank);
    float discAmt   = roomCost * discount;
    float roomFinal = roomCost - discAmt;
    Service services[MAX_SERVICES];
    UsedService usedServices[MAX_USED_SERVICES];
    int serviceCount = loadServices(services);
    int usedCount = loadUsedServices(usedServices);
    float serviceCost = calcServiceCost(usedServices, usedCount,
                                        services, serviceCount,
                                        bookings[bIdx].bookingId);
    float total = roomFinal + serviceCost;

    char billId[10];
    generateBillId(billId, bookings[bIdx].bookingId);

    clearScreen();
    printf("  ---------------------------------------------------\n");
    printf("  |%-49s|\n", "               XAC NHAN CHECK-OUT");
    printf("  ---------------------------------------------------\n");
    printf("  | %-18s | %26.26s |\n", "Booking", bookings[bIdx].bookingId);
    printf("  | %-18s | %26.26s |\n", "Bill", billId);
    printf("  | %-18s | %26.26s |\n", "Khach", customers[cIdx].name);
    printf("  | %-18s | %26.26s |\n", "Phong", rooms[rIdx].id);
    printf("  | %-18s | %26d |\n", "So dem", nights);
    printf("  | %-18s | %22.0f VND |\n", "Gia/dem", rooms[rIdx].price);
    printf("  | %-18s | %22.0f VND |\n", "Tien phong", roomCost);
    printf("  | %-18s | %22.0f VND |\n", "Giam gia", discAmt);
    printf("  | %-18s | %22.0f VND |\n", "Tien dich vu", serviceCost);
    printf("  | %-18s | %22.0f VND |\n", "Tong", total);
    printf("  ---------------------------------------------------\n");

    printf("  Xac nhan check-out? (y/n): ");
    char confirm[5];
    safeInput(confirm, sizeof(confirm));
    if (confirm[0] != 'y' && confirm[0] != 'Y') {
        printf("  Da huy.\n"); pauseScreen(); return;
    }

    bookings[bIdx].status = BOOKING_DONE;
    rooms[rIdx].status    = ROOM_EMPTY;

    saveBookings(bookings, bookingCount);
    saveRooms(rooms, roomCount);

    printf("\n  [OK] Check-out thanh cong. Vao muc 'Xem/In hoa don' de xem hoac in.\n");
    pauseScreen();
}

void cancelBooking(Booking bookings[], int bookingCount,
                   Room rooms[], int roomCount) {
    clearScreen();
    printBookingBanner("HUY BOOKING");

    char id[10];
    printf("  Nhap ma booking: ");
    safeInput(id, sizeof(id));
    trimStr(id);

    int idx = findBookingById(bookings, bookingCount, id);
    if (idx < 0) {
        printf("  [!] Khong tim thay booking.\n");
        pauseScreen();
        return;
    }
    if (bookings[idx].status != BOOKING_CHECKIN) {
        printf("  [!] Chi huy duoc booking dang o.\n");
        pauseScreen();
        return;
    }

    printf("  Xac nhan huy booking %s? (y/n): ", id);
    char confirm[5];
    safeInput(confirm, sizeof(confirm));
    if (confirm[0] != 'y' && confirm[0] != 'Y') {
        printf("  Da huy thao tac.\n");
        pauseScreen();
        return;
    }

    bookings[idx].status = BOOKING_CANCEL;
    int rIdx = findRoomById(rooms, roomCount, bookings[idx].roomId);
    if (rIdx >= 0) rooms[rIdx].status = ROOM_EMPTY;
    saveBookings(bookings, bookingCount);
    saveRooms(rooms, roomCount);
    printf("  [OK] Da huy booking %s.\n", id);
    pauseScreen();
}

/* ============================
   XEM DANH SACH BOOKING
   ============================ */
void listBookings(Booking bookings[], int count,
                  Customer customers[], int customerCount,
                  Room rooms[], int roomCount) {
    (void)customers;
    (void)customerCount;
    (void)rooms;
    (void)roomCount;

    clearScreen();
    printBookingTableHeader("DANH SACH BOOKING");

    if (count == 0) {
        printf("  (Chua co booking nao)\n");
        printBookingBorder();
        pauseScreen(); return;
    }

    for (int i = 0; i < count; i++)
        printBookingRow(&bookings[i]);

    printBookingBorder();

    int checkin=0, done=0, cancel=0;
    for (int i = 0; i < count; i++) {
        switch(bookings[i].status) {
            case 1: checkin++; break;
            case 2: done++;    break;
            case 3: cancel++;  break;
        }
    }
    printf("  Tong: %d  |  Dang o: %d  |  Da tra: %d  |  Huy: %d\n",
           count, checkin, done, cancel);
    pauseScreen();
}

/* ============================
   LUU FILE
   ============================ */
void saveBookings(Booking bookings[], int count) {
    FILE *fp = fopen(BOOKING_FILE, "w");
    if (!fp) { printf("  [!] Loi ghi file!\n"); return; }
    for (int i = 0; i < count; i++)
        fprintf(fp, "%s,%s,%s,%s,%s,%d\n",
                bookings[i].bookingId, bookings[i].customerId,
                bookings[i].roomId, bookings[i].checkInDate,
                bookings[i].checkOutDate, bookings[i].status);
    fclose(fp);
}

/* ============================
   DOC FILE
   ============================ */
int loadBookings(Booking bookings[]) {
    FILE *fp = fopen(BOOKING_FILE, "r");
    if (!fp) return 0;
    
    int count = 0;
    char line[256];
    
    while (count < MAX_BOOKINGS && fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "%9[^,],%9[^,],%9[^,],%19[^,],%19[^,],%d",
                  bookings[count].bookingId, bookings[count].customerId,
                  bookings[count].roomId, bookings[count].checkInDate,
                  bookings[count].checkOutDate, &bookings[count].status) == 6) {
            count++;
        }
    }
    fclose(fp);
    return count;
}

/* ============================
   MENU BOOKING
   ============================ */
void menuBooking(Booking bookings[], int *bookingCount,
                 Room rooms[], int *roomCount,
                 Customer customers[], int customerCount) {
    int choice;
    do {
        clearScreen();
        printHeader("CHECK-IN / CHECK-OUT");
        printf("| 1. Xem danh sach booking                               |\n");
        printf("| 2. Check-in                                            |\n");
        printf("| 3. Check-out                                           |\n");
        printf("| 4. Huy booking                                         |\n");
        printf("| 0. Quay lai                                            |\n");
        printLine(58);

        choice = inputInt("  Chon: ", 0, 4);

        switch (choice) {
            case 1: listBookings(bookings, *bookingCount, customers, customerCount, rooms, *roomCount); break;
            case 2: checkIn(bookings, bookingCount, rooms, *roomCount, customers, customerCount);      break;
            case 3: checkOut(bookings, *bookingCount, rooms, *roomCount, customers, customerCount);    break;
            case 4: cancelBooking(bookings, *bookingCount, rooms, *roomCount);                         break;
            case 0: break;
        }
    } while (choice != 0);
}
