#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bill.h"
#include "utils.h"

#define MAX_BILLS 500

/* ============================================================
   HAM BO SUNG: GHI DU LIEU THUAN VAO data/bills.txt DE BAO CAO DOANH THU
   ============================================================ */
static int billRecordExists(const char *billId, const char *bookingId) {
    FILE *fp = fopen("data/bills.txt", "r");
    if (!fp) return 0;

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        char oldBillId[10], oldBookingId[10];
        if (sscanf(line, "%9[^,],%9[^,]", oldBillId, oldBookingId) == 2) {
            if (strcmp(oldBillId, billId) == 0 ||
                strcmp(oldBookingId, bookingId) == 0) {
                fclose(fp);
                return 1;
            }
        }
    }

    fclose(fp);
    return 0;
}

int saveBillRecord(Bill *bill, Booking *booking) {
    if (!booking || booking->status != BOOKING_DONE) {
        printf("  [!] Chi ghi doanh thu khi booking da check-out.\n");
        return 0;
    }

    if (billRecordExists(bill->billId, bill->bookingId)) {
        printf("  [!] Hoa don %s / booking %s da duoc ghi nhan doanh thu truoc do.\n",
               bill->billId, bill->bookingId);
        return 0;
    }

    FILE *fp = fopen("data/bills.txt", "a");
    if (!fp) {
        printf("  [!] Khong the mo hoac tao file data/bills.txt de ghi nhan doanh thu!\n");
        return 0;
    }
    char dateStr[20], timeStr[20];
    getCurrentDateTime(dateStr, timeStr);
    fprintf(fp, "%s,%s,%s,%.0f,%.0f,%.0f,%.0f,%.0f\n",
            bill->billId,
            bill->bookingId,
            booking->checkOutDate,
            bill->roomCost,
            bill->serviceCost,
            bill->serviceCharge,
            bill->vat,
            bill->total);

    fclose(fp);
    return 1;
}

void sanitizeBillRecords(Booking bookings[], int bookingCount) {
    FILE *fp = fopen("data/bills.txt", "r");
    if (!fp) return;

    char lines[MAX_BILLS][256];
    int keep = 0;
    char line[256];

    while (fgets(line, sizeof(line), fp) && keep < MAX_BILLS) {
        char billId[10], bookingId[10];
        if (sscanf(line, "%9[^,],%9[^,]", billId, bookingId) != 2) continue;

        int idx = findBookingById(bookings, bookingCount, bookingId);
        if (idx < 0) continue;
        if (bookings[idx].status != BOOKING_DONE) continue;

        strcpy(lines[keep++], line);
    }
    fclose(fp);

    fp = fopen("data/bills.txt", "w");
    if (!fp) return;
    for (int i = 0; i < keep; i++) fputs(lines[i], fp);
    fclose(fp);
}

static void printBillAmountLine(FILE *stream, const char *label, float amount) {
    fprintf(stream, "| %-35.35s %14.0f VND |\n", label, amount);
}

/* ============================================================
   TAO MA HOA DON TU MA BOOKING
   VD: BK001 -> HD001
   ============================================================ */
void generateBillIdFromBooking(char *billId, const char *bookingId) {
    int num = 0;
    if (strlen(bookingId) >= 3) {
        const char *p = bookingId;
        while (*p && !(*p >= '0' && *p <= '9')) p++;
        num = atoi(p);
    }
    snprintf(billId, 10, "HD%03d", num);
}

/* ============================================================
   TAO HOA DON
   ============================================================ */
int createBill(Bill *bill,
               Booking *booking,
               Room *room,
               Customer *customer,
               UsedService usedServices[], int usedCount,
               Service services[], int serviceCount) {

    if (!bill || !booking || !room || !customer) return 0;

    generateBillIdFromBooking(bill->billId, booking->bookingId);
    strncpy(bill->bookingId, booking->bookingId, sizeof(bill->bookingId) - 1);
    bill->bookingId[sizeof(bill->bookingId) - 1] = '\0';

    int nights = calcNights(booking->checkInDate, booking->checkOutDate);
    if (nights <= 0) nights = 1;

    float discount = rankDiscount(customer->rank);

    bill->roomCost = room->price * nights * (1.0f - discount);

    bill->serviceCost = calcServiceCost(usedServices, usedCount,
                                        services, serviceCount,
                                        booking->bookingId);

    bill->serviceCharge = 0.0f;
    bill->vat = 0.0f;
    bill->total = bill->roomCost + bill->serviceCost;

    return 1;
}

/* ============================================================
   IN HOA DON RA STREAM (stdout hoac file)
   ============================================================ */
void printBill(Bill *bill,
               Booking *booking,
               Room *room,
               Customer *customer,
               UsedService usedServices[], int usedCount,
               Service services[], int serviceCount,
               FILE *stream) {

    char dateStr[20], timeStr[20];
    getCurrentDateTime(dateStr, timeStr);

    int nights     = calcNights(booking->checkInDate, booking->checkOutDate);
    if (nights <= 0) nights = 1;
    float discount = rankDiscount(customer->rank);

    #define LINE_D "+--------------------------------------------------------+\n"
    #define LINE_E "+========================================================+\n"

    fprintf(stream, "\n");
    fprintf(stream, LINE_E);
    fprintf(stream, "|           KHACH SAN HOTEL MANAGEMENT SYSTEM            |\n");
    fprintf(stream, "|                  HOA DON THANH TOAN                    |\n");
    fprintf(stream, LINE_E);
    fprintf(stream, "| Ma hoa don : %-16.16s  Ngay: %-16.16s  |\n",
            bill->billId, dateStr);
    fprintf(stream, "| Ma booking : %-40.40s  |\n", booking->bookingId);

    /* --- Khach hang --- */
    fprintf(stream, LINE_D);
    fprintf(stream, "| THONG TIN KHACH HANG                                   |\n");
    fprintf(stream, LINE_D);
    fprintf(stream, "| Ho ten    : %-41.41s  |\n", customer->name);
    fprintf(stream, LINE_D);
    fprintf(stream, "| Ma KH     : %-41.41s  |\n", customer->id);
    fprintf(stream, LINE_D);
    fprintf(stream, "| CCCD      : %-41.41s  |\n", customer->cccd);
    fprintf(stream, LINE_D);
    fprintf(stream, "| SDT       : %-41.41s  |\n", customer->phone);
    fprintf(stream, LINE_D);
    fprintf(stream, "| Hang KH   : %-11.11s   Giam gia : %3.0f%%              |\n",
            rankStr(customer->rank), discount * 100);

    /* --- Phong --- */
    fprintf(stream, LINE_D);
    fprintf(stream, "| THONG TIN PHONG                                        |\n");
    fprintf(stream, LINE_D);
    fprintf(stream, "| Phong     : %-10.10s        Loai : %-17.17s |\n",
            room->id, room->type);
    fprintf(stream, LINE_D);
    fprintf(stream, "| Check-in  : %-41.41s  |\n", booking->checkInDate);
    fprintf(stream, LINE_D);
    fprintf(stream, "| Check-out : %-41.41s  |\n", booking->checkOutDate);
    fprintf(stream, LINE_D);
    fprintf(stream, "| So dem    : %-5d        Don gia : %10.0f VND/dem  |\n",
            nights, room->price);

    /* --- Chi tiet tien phong --- */
    fprintf(stream, LINE_D);
    fprintf(stream, "| CHI TIET TIEN PHONG                                    |\n");
    fprintf(stream, LINE_D);
    fprintf(stream, "| %-15.15s  %10.0f x %2d dem =   %9.0f VND |\n",
            room->type, room->price, nights, room->price * nights);
    if (discount > 0) {
        fprintf(stream, LINE_D);
        fprintf(stream, "| Giam gia %-8.8s (%3.0f%%)             -  %9.0f VND  |\n",
                rankStr(customer->rank),
                discount * 100,
                room->price * nights * discount);
    }
    fprintf(stream, LINE_D);
    fprintf(stream, "| Tien phong sau giam                  =  %9.0f VND  |\n",
            bill->roomCost);

    /* --- Chi tiet dich vu --- */
    fprintf(stream, LINE_D);
    fprintf(stream, "| CHI TIET DICH VU                                       |\n");
    fprintf(stream, LINE_D);

    int hasSvc = 0;
    for (int i = 0; i < usedCount; i++) {
        if (strcmp(usedServices[i].bookingId, booking->bookingId) != 0) continue;
        if (usedServices[i].status != SERVICE_ACTIVE) continue;

        int svcIdx = findServiceById(services, serviceCount,
                                     usedServices[i].serviceId);
        if (svcIdx < 0) continue;

        float sub = services[svcIdx].price * usedServices[i].quantity;
        fprintf(stream, "| %-17.17s  %9.0f x %2d %-4.4s  = %7.0f VND  |\n",
                services[svcIdx].serviceName,
                services[svcIdx].price,
                usedServices[i].quantity,
                services[svcIdx].unit,
                sub);
        fprintf(stream, LINE_D);
        hasSvc = 1;
    }

    if (!hasSvc) {
        fprintf(stream, "| %-55s|\n", "(Khong co dich vu)");
        fprintf(stream, LINE_D);
    }

    printBillAmountLine(stream, "Tong tien dich vu", bill->serviceCost);

    /* --- Tong hop --- */
    fprintf(stream, LINE_D);
    fprintf(stream, "| TONG HOP THANH TOAN                                    |\n");
    fprintf(stream, LINE_D);
    printBillAmountLine(stream, "Tien phong", bill->roomCost);
    fprintf(stream, LINE_D);
    printBillAmountLine(stream, "Tien dich vu", bill->serviceCost);
    fprintf(stream, LINE_E);
    printBillAmountLine(stream, "TONG THANH TOAN", bill->total);
    fprintf(stream, LINE_E);
    fprintf(stream, "|            Cam on quy khach! Hen gap lai.              |\n");
    fprintf(stream, LINE_E);

    #undef LINE_D
    #undef LINE_E
}

/* ============================================================
   XUAT HOA DON RA FILE output/HDxxx.txt
   ============================================================ */
int exportBillToFile(Bill *bill,
                     Booking *booking,
                     Room *room,
                     Customer *customer,
                     UsedService usedServices[], int usedCount,
                     Service services[], int serviceCount) {

    system("mkdir output 2>nul");

    char filename[64];
    snprintf(filename, sizeof(filename), "output/%s.txt", bill->billId);

    FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("  [!] Khong the tao file %s\n", filename);
        return 0;
    }

    printBill(bill, booking, room, customer,
              usedServices, usedCount, services, serviceCount,
              fp);

    fclose(fp);
    printf("  [OK] Da xuat hoa don ra file: %s\n", filename);
    return 1;
}

void viewBillByBooking(Booking bookings[], int bookingCount,
                       Room rooms[], int roomCount,
                       Customer customers[], int customerCount,
                       UsedService usedServices[], int usedCount,
                       Service services[], int serviceCount) {
    clearScreen();
    printHeader("XEM/IN HOA DON");

    char id[10];
    printf("  Nhap ma booking: ");
    safeInput(id, sizeof(id));
    trimStr(id);

    int bIdx = findBookingById(bookings, bookingCount, id);
    if (bIdx < 0) {
        printf("  [!] Khong tim thay booking.\n");
        pauseScreen();
        return;
    }
    if (bookings[bIdx].status == BOOKING_CANCEL) {
        printf("  [!] Booking da huy, khong co hoa don.\n");
        pauseScreen();
        return;
    }

    int rIdx = findRoomById(rooms, roomCount, bookings[bIdx].roomId);
    int cIdx = findCustomerById(customers, customerCount, bookings[bIdx].customerId);
    if (rIdx < 0 || cIdx < 0) {
        printf("  [!] Loi du lieu phong/khach.\n");
        pauseScreen();
        return;
    }

    Bill bill;
    memset(&bill, 0, sizeof(Bill));
    if (!createBill(&bill, &bookings[bIdx], &rooms[rIdx], &customers[cIdx],
                    usedServices, usedCount, services, serviceCount)) {
        printf("  [!] Khong tao duoc hoa don.\n");
        pauseScreen();
        return;
    }

    printBill(&bill, &bookings[bIdx], &rooms[rIdx], &customers[cIdx],
              usedServices, usedCount, services, serviceCount, stdout);

    if (bookings[bIdx].status == BOOKING_CHECKIN) {
        printf("\n  [Luu y] Booking dang o: chi xem hoa don tam tinh, chua duoc in file.\n");
        pauseScreen();
        return;
    }

    printf("\n  In hoa don ra file? (y/n): ");
    char cf[5];
    safeInput(cf, sizeof(cf));
    if (cf[0] == 'y' || cf[0] == 'Y') {
        if (exportBillToFile(&bill, &bookings[bIdx], &rooms[rIdx], &customers[cIdx],
                             usedServices, usedCount, services, serviceCount)) {
            saveBillRecord(&bill, &bookings[bIdx]);
        }
    }
    pauseScreen();
}

