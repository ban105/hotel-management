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

/* KIEM TRA NGAY NHAP VAO CO PHAI LA QUA KHU KHONG */
static int isPastDate(const char *dateStr) {
    int inputD, inputM, inputY;
    if (sscanf(dateStr, "%d/%d/%d", &inputD, &inputM, &inputY) != 3) {
        return 1; 
    }

    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    int curD = tm->tm_mday;
    int curM = tm->tm_mon + 1;        
    int curY = tm->tm_year + 1900;    

    if (inputY < curY) {
        return 1; 
    }
    if (inputY == curY) {
        if (inputM < curM) {
            return 1; 
        }
        if (inputM == curM) {
            if (inputD < curD) {
                return 1; 
            }
        }
    }

    return 0; 
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
        case 0: return "Da dat";  
        case 1: return "Dang o";  
        case 2: return "Da tra";  
        case 3: return "Da huy";  
        default: return "Da huy"; 
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

    printf("\n");
    printLine(52);
    printf("|          XAC NHAN CHECK-IN THANH CONG            |\n");
    printLine(52);
    printf("| %-20s : %-25.25s |\n", "Ma booking",  b->bookingId);
    printf("| %-20s : %-25.25s |\n", "Ma hoa don",  billId);
    printLine(52);
    printf("| %-20s : %-25.25s |\n", "Ho ten KH",    c->name);
    printf("| %-20s : %-25.25s |\n", "Ma KH",       c->id);
    if (rankDiscount(c->rank) > 0) {
        char rankBuf[30];
        sprintf(rankBuf, "%s (giam %.0f%%)", rankStr(c->rank), discount*100);
        printf("| %-20s : %-25.25s |\n", "Hang", rankBuf);
    } else {
        printf("| %-20s : %-25.25s |\n", "Hang", rankStr(c->rank));
    }
    printLine(52);
    char roomBuf[30];
    sprintf(roomBuf, "%s (%s)", r->id, r->type);
    printf("| %-20s : %-25.25s |\n", "Phong",      roomBuf);
    char priceBuf[30];
    sprintf(priceBuf, "%.0f dong/dem", r->price);
    printf("| %-20s : %-25.25s |\n", "Gia phong", priceBuf);
    printLine(52);
    char ciStr[30];
    sprintf(ciStr, "%s - 13:00", b->checkInDate);
    printf("| %-20s : %-25.25s |\n", "Check-in",  ciStr);
    char coStr[30];
    sprintf(coStr, "%s (du kien)", b->checkOutDate);
    printf("| %-20s : %-25.25s |\n", "Check-out", coStr);
    char nightBuf[20];
    sprintf(nightBuf, "%d dem", nights);
    printf("| %-20s : %-25.25s |\n", "So dem",    nightBuf);
    printLine(52);
    char rcBuf[30];
    sprintf(rcBuf, "%.0f dong", roomCost);
    printf("| %-20s : %-25.25s |\n", "Tien phong",  rcBuf);
    if (discAmt > 0) {
        char daBuf[30];
        sprintf(daBuf, "%.0f dong", discAmt);
        printf("| %-20s : %-25.25s |\n", "Giam gia", daBuf);
    }
    char estBuf[30];
    sprintf(estBuf, "%.0f dong", estimate);
    printf("| %-20s : %-25.25s |\n", "Du kien tra", estBuf);
    printLine(52);
    printf("|   Luu y: Chua bao gom dich vu phat sinh          |\n");
    printLine(52);
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

static int bookingDatesOverlap(const char *newIn, const char *newOut,
                               const char *oldIn, const char *oldOut) {
    return dateToInt(newIn) < dateToInt(oldOut) &&
           dateToInt(newOut) > dateToInt(oldIn);
}

static int isRoomFreeInDateRange(Booking bookings[], int bookingCount,
                                 const char *roomId,
                                 const char *checkIn,
                                 const char *checkOut) {
    for (int i = 0; i < bookingCount; i++) {
        if (strcmp(bookings[i].roomId, roomId) != 0) continue;
        if (bookings[i].status == BOOKING_DONE ||
            bookings[i].status == BOOKING_CANCEL) continue;

        if (bookingDatesOverlap(checkIn, checkOut,
                                bookings[i].checkInDate,
                                bookings[i].checkOutDate)) {
            return 0;
        }
    }
    return 1;
}

/* ============================
   DAT PHONG MOI
   ============================ */
void addBooking(Booking bookings[], int *bookingCount,
                Room rooms[], int roomCount,
                Customer customers[], int customerCount) {
    if (*bookingCount >= MAX_BOOKINGS) {
        printf("  [!] Da dat toi da so booking!\n");
        pauseScreen(); return;
    }

    clearScreen();
    printBookingBanner(" DAT PHONG MOI ");

    Booking b;
    generateBookingId(bookings, *bookingCount, b.bookingId);
    printf("\n  Ma booking: %s (tu dong)\n\n", b.bookingId);

    do {
        printf("  Ma khach hang (KH001...): ");
        safeInput(b.customerId, sizeof(b.customerId));
        trimStr(b.customerId);
        if (findCustomerById(customers, customerCount, b.customerId) == -1) {
            printf("  [!] Khong tim thay khach hang '%s'!\n", b.customerId);
            b.customerId[0] = '\0';
        }
    } while (!isNotEmpty(b.customerId));

    int cIdx = findCustomerById(customers, customerCount, b.customerId);
    printf("  Khach hang : %s - Hang %s\n\n",
           customers[cIdx].name, rankStr(customers[cIdx].rank));

    do {
        printf("  Ngay check-in (DD/MM/YYYY) : ");
        safeInput(b.checkInDate, sizeof(b.checkInDate));
        trimStr(b.checkInDate);

        if (!isValidDate(b.checkInDate)) {
            printf("  [!] Dinh dang ngay thang khong hop le!\n");
        } else if (isPastDate(b.checkInDate)) {
            printf("  [!] Khong duoc dat phong trong qua khu!\n");
            b.checkInDate[0] = '\0';
        }
    } while (!isValidDate(b.checkInDate));

    do {
        printf("  Ngay check-out (DD/MM/YYYY): ");
        safeInput(b.checkOutDate, sizeof(b.checkOutDate));
        trimStr(b.checkOutDate);

        if (!isValidDate(b.checkOutDate)) {
            printf("  [!] Dinh dang ngay thang khong hop le!\n");
        } else if (isPastDate(b.checkOutDate)) {
            printf("  [!] Ngay check-out khong the o trong qua khu!\n");
        } else if (dateToInt(b.checkOutDate) <= dateToInt(b.checkInDate)) {
            printf("  [!] Ngay check-out phai sau ngay check-in!\n");
        } else {
            break;
        }
    } while (1);

    printf("\n  Danh sach phong trong tu %s den %s:\n",
           b.checkInDate, b.checkOutDate);
    printf("  +----------+----------+---------------+\n");
    printf("  | %-8s | %-8s | %13s |\n", "Ma phong", "Loai", "Gia/dem (d)");
    printf("  +----------+----------+---------------+\n");
    
    int hasEmpty = 0;
    for (int i = 0; i < roomCount; i++) {
        if (rooms[i].status != ROOM_MAINTAIN &&
            isRoomFreeInDateRange(bookings, *bookingCount, rooms[i].id,
                                  b.checkInDate, b.checkOutDate)) {
            printf("  | %-8.8s | %-8.8s | %13.0f |\n",
                   rooms[i].id, rooms[i].type, rooms[i].price);
            hasEmpty++;
        }
    }
    printf("  +----------+----------+---------------+\n");
    if (hasEmpty == 0) {
        printf("  [!] Khong co phong trong trong khoang ngay nay!\n");
        pauseScreen(); return;
    }
    printf("  Co %d phong co the dat.\n\n", hasEmpty);

    do {
        printf("  Ma phong (P101...): ");
        safeInput(b.roomId, sizeof(b.roomId));
        trimStr(b.roomId);
        
        int rIdx = findRoomById(rooms, roomCount, b.roomId);
        if (rIdx == -1) {
            printf("  [!] Khong tim thay phong '%s'!\n", b.roomId);
            b.roomId[0] = '\0'; continue;
        }
        if (rooms[rIdx].status == ROOM_MAINTAIN) {
            printf("  [!] Phong '%s' dang bao tri!\n", b.roomId);
            b.roomId[0] = '\0'; continue;
        }

        if (!isRoomFreeInDateRange(bookings, *bookingCount, b.roomId,
                                   b.checkInDate, b.checkOutDate)) {
            printf("  [!] Phong '%s' da co booking trung khoang ngay nay!\n", b.roomId);
            b.roomId[0] = '\0';
        }
        
    } while (!isNotEmpty(b.roomId));

    int rIdx = findRoomById(rooms, roomCount, b.roomId);
    printf("  Phong      : %s (%s) - %.0f dong/dem\n\n",
           rooms[rIdx].id, rooms[rIdx].type, rooms[rIdx].price);

    int nights = calcNights(b.checkInDate, b.checkOutDate);
    float total = rooms[rIdx].price * nights;
    float disc  = total * rankDiscount(customers[cIdx].rank);
    printf("\n  So dem du kien : %d dem\n", nights);
    printf("  Tam tinh        : %.0f dong", total);
    if (disc > 0) printf(" (giam %.0f = con %.0f)", disc, total - disc);
    printf("\n");

    b.status = BOOKING_PENDING;

    printf("\n  Xac nhan dat phong? (y/n): ");
    char confirm[5];
    safeInput(confirm, sizeof(confirm));
    if (confirm[0] != 'y' && confirm[0] != 'Y') {
        printf("  Da huy.\n"); pauseScreen(); return;
    }

    bookings[*bookingCount] = b;
    (*bookingCount)++;
    saveBookings(bookings, *bookingCount);

    printf("\n  [OK] Da dat phong thanh cong! Ma booking: %s\n", b.bookingId);
    pauseScreen();
}

/* ============================
   HUY DAT PHONG
   ============================ */
void cancelBooking(Booking bookings[], int bookingCount,
                   Room rooms[], int roomCount) {
    clearScreen();
    printBookingBanner("HUY DAT PHONG");

    char id[10];
    printf("\n  Nhap ma booking can huy: ");
    safeInput(id, sizeof(id)); trimStr(id);

    int idx = findBookingById(bookings, bookingCount, id);
    if (idx == -1) {
        printf("  [!] Khong tim thay booking '%s'!\n", id);
        pauseScreen(); return;
    }
    if (bookings[idx].status != BOOKING_PENDING) {
        printf("  [!] Chi huy duoc booking chua check-in!\n");
        printf("  Trang thai hien tai: %s\n",
               bookingStatusStr(bookings[idx].status));
        pauseScreen(); return;
    }

    printf("\n  Booking can huy:\n");
    printBookingBorder();
    printBookingRow(&bookings[idx]);
    printBookingBorder();

    printf("  Xac nhan huy? (y/n): ");
    char confirm[5];
    safeInput(confirm, sizeof(confirm));
    if (confirm[0] != 'y' && confirm[0] != 'Y') {
        printf("  Da huy thao tac.\n"); pauseScreen(); return;
    }

    bookings[idx].status = BOOKING_CANCEL;
    int rIdx = findRoomById(rooms, roomCount, bookings[idx].roomId);
    int hasCheckedInSameRoom = 0;
    for (int i = 0; i < bookingCount; i++) {
        if (i == idx) continue;
        if (strcmp(bookings[i].roomId, bookings[idx].roomId) == 0 &&
            bookings[i].status == BOOKING_CHECKIN) {
            hasCheckedInSameRoom = 1;
            break;
        }
    }
    if (rIdx >= 0 && rooms[rIdx].status != ROOM_MAINTAIN &&
        !hasCheckedInSameRoom) {
        rooms[rIdx].status = ROOM_EMPTY;
        saveRooms(rooms, roomCount);
    }
    saveBookings(bookings, bookingCount);
    printf("  [OK] Da huy booking %s!\n", id);
    pauseScreen();
}

/* ============================
   CHECK-IN
   ============================ */
void checkIn(Booking bookings[], int bookingCount,
             Room rooms[], int roomCount,
             Customer customers[], int customerCount) {
    clearScreen();
    printBookingBanner("CHECK-IN");

    char id[10];
    printf("\n  Nhap ma booking: ");
    safeInput(id, sizeof(id)); trimStr(id);
    printf("  ");printLine(62);
    int bIdx = findBookingById(bookings, bookingCount, id);
    if (bIdx == -1) {
        printf("  [!] Khong tim thay booking '%s'!\n", id);
        pauseScreen(); return;
    }
    if (bookings[bIdx].status != BOOKING_PENDING) {
        printf("  [!] Booking nay khong the check-in!\n");
        printf("  Trang thai: %s\n", bookingStatusStr(bookings[bIdx].status));
        pauseScreen(); return;
    }

    int rIdx = findRoomById(rooms, roomCount, bookings[bIdx].roomId);
    int cIdx = findCustomerById(customers, customerCount, bookings[bIdx].customerId);

    if (rIdx == -1 || cIdx == -1) {
        printf("  [!] Loi du lieu phong hoac khach hang!\n");
        pauseScreen(); return;
    }

    printf("\n  Thong tin check-in:\n");
    printBookingBorder();
    printBookingRow(&bookings[bIdx]);
    printBookingBorder();
    printf("  Khach hang : %s\n", customers[cIdx].name);
    printf("  Phong      : %s (%s)\n", rooms[rIdx].id, rooms[rIdx].type);

    printf("\n  Xac nhan check-in? (y/n): ");
    char confirm[5];
    safeInput(confirm, sizeof(confirm));
    if (confirm[0] != 'y' && confirm[0] != 'Y') {
        printf("  Da huy.\n"); pauseScreen(); return;
    }

    bookings[bIdx].status  = BOOKING_CHECKIN;
    rooms[rIdx].status     = ROOM_OCCUPIED;

    saveBookings(bookings, bookingCount);
    saveRooms(rooms, roomCount);

    clearScreen();
    printCheckInReceipt(&bookings[bIdx], &rooms[rIdx], &customers[cIdx]);
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

    int nights    = calcNights(bookings[bIdx].checkInDate, bookings[bIdx].checkOutDate);
    float roomCost = rooms[rIdx].price * nights;
    float discount = rankDiscount(customers[cIdx].rank);
    float discAmt  = roomCost * discount;
    float total    = roomCost - discAmt;

    char billId[10];
    generateBillId(billId, bookings[bIdx].bookingId);

    printf("\n");
    clearScreen();
    printf("  ");printLine(52);
    printf("  |                THONG TIN CHECK-OUT               |\n");
    printf("  ");printLine(52);
    
    printf("  | %-14s : %-31.31s |\n", "Ma booking", bookings[bIdx].bookingId);
    printf("  | %-14s : %-31.31s |\n", "Ma hoa don", billId);
    
    printf("  ");printLine(52);
    printf("  | %-14s : %-31.31s |\n", "Ho ten KH", customers[cIdx].name);
    printf("  | %-14s : %-31.31s |\n", "Ma KH", bookings[bIdx].customerId);
    
    char rankInfo[50];
    sprintf(rankInfo, "%s (giam %.0f%%)", rankStr(customers[cIdx].rank), discount * 100);
    printf("  | %-14s : %-31.31s |\n", "Hang", rankInfo);
    
    printf("  ");printLine(52);
    char roomTypeInfo[50];
    sprintf(roomTypeInfo, "%s (%s)", rooms[rIdx].id, rooms[rIdx].type);
    printf("  | %-14s : %-31.31s |\n", "Phong", roomTypeInfo);
    printf("  | %-14s : %-22.0f dong/dem |\n", "Gia phong", rooms[rIdx].price);
    
    printf("  ");printLine(52);
    char checkInWithTime[40];
    char checkOutWithTime[40];
    sprintf(checkInWithTime, "%s - 13:00", bookings[bIdx].checkInDate);
    sprintf(checkOutWithTime, "%s - 12:00", bookings[bIdx].checkOutDate);

    printf("  | %-14s : %-31.31s |\n", "Check-in", checkInWithTime);
    printf("  | %-14s : %-31.31s |\n", "Check-out", checkOutWithTime);
    
    char nightStr[20];
    sprintf(nightStr, "%d dem", nights);
    printf("  | %-14s : %-31.31s |\n", "So dem", nightStr);
    
    printf("  ");printLine(52);
    printf("  | %-14s : %-26.0f dong |\n", "Tien phong", roomCost);
    printf("  | %-14s : %-26.0f dong |\n", "Giam gia", discAmt);
    printf("  | %-14s: %-26.0f dong |\n", "Tong thanh toan", total);
    
    printf("  ");printLine(52);
    // Dong ghi chu ve hoa don tam thoi.
    printf("  | Luu y: Hoa don chua bao gom phi dich vu va thue  |\n");
    printf("  ");printLine(52);

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

    Service services[MAX_SERVICES];
    UsedService usedServices[MAX_USED_SERVICES];
    int serviceCount = loadServices(services);
    int usedCount = loadUsedServices(usedServices);

    Bill bill;
    memset(&bill, 0, sizeof(Bill));
    if (createBill(&bill,
                   &bookings[bIdx],
                   &rooms[rIdx],
                   &customers[cIdx],
                   usedServices, usedCount,
                   services, serviceCount)) {
        saveBillRecord(&bill, &bookings[bIdx]);
    } else {
        printf("  [!] Check-out thanh cong nhung khong the ghi nhan doanh thu.\n");
    }

    printf("\n  [OK] Check-out thanh cong! Cam on quy khach.\n");
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

    int pending=0, checkin=0, done=0, cancel=0;
    for (int i = 0; i < count; i++) {
        switch(bookings[i].status) {
            case 0: pending++; break;
            case 1: checkin++; break;
            case 2: done++;    break;
            case 3: cancel++;  break;
        }
    }
    printf("  Tong: %d  |  Da dat: %d  |  Dang o: %d  |  Da tra: %d  |  Huy: %d\n",
           count, pending, checkin, done, cancel);
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
        printHeader("QUAN LY DAT PHONG");
        printf("| 1. Xem danh sach booking                               |\n");
        printf("| 2. Dat phong moi                                       |\n");
        printf("| 3. Huy dat phong                                       |\n");
        printf("| 4. Check-in                                            |\n");
        printf("| 5. Check-out                                           |\n");
        printf("| 0. Quay lai                                            |\n");
        printLine(58);

        choice = inputInt("  Chon: ", 0, 5);

        switch (choice) {
            case 1: listBookings(bookings, *bookingCount, customers, customerCount, rooms, *roomCount); break;
            case 2: addBooking(bookings, bookingCount, rooms, *roomCount, customers, customerCount);    break;
            case 3: cancelBooking(bookings, *bookingCount, rooms, *roomCount);                          break;
            case 4: checkIn(bookings, *bookingCount, rooms, *roomCount, customers, customerCount);     break;
            case 5: checkOut(bookings, *bookingCount, rooms, *roomCount, customers, customerCount);    break;
            case 0: break;
        }
    } while (choice != 0);
}
