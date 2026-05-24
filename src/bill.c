#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bill.h"
#include "utils.h"

/* ============================================================
   HÀM BỔ SUNG: GHI DỮ LIỆU THUẦN VÀO bills.txt ĐỂ BÁO CÁO DOANH THU
   ============================================================ */
void saveBillRecord(Bill *bill, Booking *booking) {
    // Mở file ở chế độ "a" (append) tại gốc thư mục thực thi (output/)
    FILE *fp = fopen("bills.txt", "a");
    if (!fp) {
        printf("  [!] Khong the mo hoac tao file bills.txt de ghi nhan doanh thu!\n");
        return;
    }

    // Lấy ngày check-out thực tế của hóa đơn
    char dateStr[20], timeStr[20];
    getCurrentDateTime(dateStr, timeStr);

    // Ghi dữ liệu dạng CSV (phân tách bằng dấu phẩy)
    // Format khớp hoàn toàn với sscanf của hàm quản lý doanh thu
    fprintf(fp, "%s,%s,%s,%.0f,%.0f,%.0f,%.0f,%.0f\n",
            bill->billId,
            bill->bookingId,
            booking->checkOutDate, // Sử dụng ngày check-out của booking
            bill->roomCost,
            bill->serviceCost,
            bill->serviceCharge,
            bill->vat,
            bill->total);

    fclose(fp);
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

    int nights = calcNights(booking->checkInDate, booking->checkOutDate);
    if (nights <= 0) nights = 1;

    float discount = rankDiscount(customer->rank);

    bill->roomCost = room->price * nights * (1.0f - discount);

    bill->serviceCost = calcServiceCost(usedServices, usedCount,
                                        services, serviceCount,
                                        booking->bookingId);

    bill->serviceCharge = (bill->roomCost + bill->serviceCost) * SERVICE_CHARGE_RATE;

    bill->vat = (bill->roomCost + bill->serviceCost + bill->serviceCharge) * VAT_RATE;

    bill->total = bill->roomCost + bill->serviceCost
                + bill->serviceCharge + bill->vat;

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
    fprintf(stream, "| Ma hoa don : %-16s  Ngay: %-16s  |\n",
            bill->billId, dateStr);
    fprintf(stream, "| Ma booking : %-40s  |\n", booking->bookingId);

    /* --- Khach hang --- */
    fprintf(stream, LINE_D);
    fprintf(stream, "| THONG TIN KHACH HANG                                   |\n");
    fprintf(stream, LINE_D);
    fprintf(stream, "| Ho ten    : %-41s  |\n", customer->name);
    fprintf(stream, LINE_D);
    fprintf(stream, "| Ma KH     : %-41s  |\n", customer->id);
    fprintf(stream, LINE_D);
    fprintf(stream, "| CCCD      : %-41s  |\n", customer->cccd);
    fprintf(stream, LINE_D);
    fprintf(stream, "| SDT       : %-41s  |\n", customer->phone);
    fprintf(stream, LINE_D);
    fprintf(stream, "| Hang KH   : %-12s   Giam gia : %3.0f%%              |\n",
            rankStr(customer->rank), discount * 100);

    /* --- Phong --- */
    fprintf(stream, LINE_D);
    fprintf(stream, "| THONG TIN PHONG                                        |\n");
    fprintf(stream, LINE_D);
    fprintf(stream, "| Phong     : %-10s        Loai : %-18s |\n",
            room->id, room->type);
    fprintf(stream, LINE_D);
    fprintf(stream, "| Check-in  : %-41s  |\n", booking->checkInDate);
    fprintf(stream, LINE_D);
    fprintf(stream, "| Check-out : %-41s  |\n", booking->checkOutDate);
    fprintf(stream, LINE_D);
    fprintf(stream, "| So dem    : %-5d       Don gia : %10.0f VND/dem    |\n",
            nights, room->price);

    /* --- Chi tiet tien phong --- */
    fprintf(stream, LINE_D);
    fprintf(stream, "| CHI TIET TIEN PHONG                                    |\n");
    fprintf(stream, LINE_D);
    fprintf(stream, "| %-15s  %10.0f x %2d dem =   %9.0f VND |\n",
            room->type, room->price, nights, room->price * nights);
    if (discount > 0) {
        fprintf(stream, LINE_D);
        fprintf(stream, "| Giam gia %-8s (%3.0f%%)             -  %9.0f VND  |\n",
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
        fprintf(stream, "| %-17s  %9.0f x %2d %-4s  = %7.0f VND  |\n",
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

    fprintf(stream, "| Tong tien dich vu:                         %6.0f VND  |\n",
            bill->serviceCost);

    /* --- Tong hop --- */
    fprintf(stream, LINE_D);
    fprintf(stream, "| TONG HOP THANH TOAN                                    |\n");
    fprintf(stream, LINE_D);
    fprintf(stream, "| Tien phong                           %10.0f VND    |\n",
            bill->roomCost);
    fprintf(stream, LINE_D);
    fprintf(stream, "| Tien dich vu                        %11.0f VND    |\n",
            bill->serviceCost);
    fprintf(stream, LINE_D);
    fprintf(stream, "| Phi dich vu (5%%)                    %11.0f VND    |\n",
            bill->serviceCharge);
    fprintf(stream, LINE_D);
    fprintf(stream, "| Thue VAT (10%%)                      %11.0f VND    |\n",
            bill->vat);
    fprintf(stream, LINE_E);
    fprintf(stream, "| TONG THANH TOAN                     %11.0f VND    |\n",
            bill->total);
    fprintf(stream, LINE_E);
    fprintf(stream, "|            Cam on quy khach! Hen gap lai.              |\n");
    fprintf(stream, LINE_E);

    #undef LINE_D
    #undef LINE_E
}

/* ============================================================
   HIEN HOA DON TREN MAN HINH TERMINAL
   ============================================================ */
void displayBill(Bill *bill,
                 Booking *booking,
                 Room *room,
                 Customer *customer,
                 UsedService usedServices[], int usedCount,
                 Service services[], int serviceCount) {

    printBill(bill, booking, room, customer,
              usedServices, usedCount, services, serviceCount,
              stdout);
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

/* ============================================================
   XEM LAI HOA DON THEO MA BOOKING VÀ TỰ ĐỘNG ĐỒNG BỘ DOANH THU
   ============================================================ */
void viewBillByBooking(Booking bookings[], int bookingCount,
                       Room rooms[], int roomCount,
                       Customer customers[], int customerCount,
                       UsedService usedServices[], int usedCount,
                       Service services[], int serviceCount) {

    clearScreen();
    printHeader("XEM HOA DON");

    char bkId[10];
    printf("  Nhap ma booking: ");
    safeInput(bkId, sizeof(bkId));

    int bkIdx = findBookingById(bookings, bookingCount, bkId);
    if (bkIdx < 0) {
        printf("  [!] Khong tim thay booking %s\n", bkId);
        pauseScreen();
        return;
    }

    if (bookings[bkIdx].status == 0) { 
        printf("\n  [!] Canh bao: Booking %s dang o trang thai CHO CHECK-IN.\n", bkId);
        printf("  Khach hang chua nhan phong, khong the tao hoac xem hoa don.\n");
        pauseScreen();
        return;
    }

    int rmIdx = findRoomById(rooms, roomCount, bookings[bkIdx].roomId);
    int cuIdx = findCustomerById(customers, customerCount, bookings[bkIdx].customerId);

    if (rmIdx < 0 || cuIdx < 0) {
        printf("  [!] Khong tim thay thong tin phong hoac khach hang.\n");
        pauseScreen();
        return;
    }

    Bill bill;
    memset(&bill, 0, sizeof(Bill));

    int ok = createBill(&bill,
                        &bookings[bkIdx],
                        &rooms[rmIdx],
                        &customers[cuIdx],
                        usedServices, usedCount,
                        services, serviceCount);

    if (!ok) {
        printf("  [!] Khong the tao hoa don.\n");
        pauseScreen();
        return;
    }

    displayBill(&bill,
                &bookings[bkIdx],
                &rooms[rmIdx],
                &customers[cuIdx],
                usedServices, usedCount,
                services, serviceCount);

    /* ========================================================
       SỬA TẠI ĐÂY: KHI XUẤT FILE SẼ ĐỒNG THỜI GHI VÀO bills.txt
       ======================================================== */
    if (bookings[bkIdx].status == 1) { 
        printf("\n  [Luu y] Phong nay chua lam thu tuc Check-out.\n");
        printf("  Day chi la phieu tam thoi tren man hinh, khong the ket xuat file hoa don.\n");
    } 
    else if (bookings[bkIdx].status == 2) { 
        printf("\n  Xuat hoa don ra file de in an va cap nhat doanh thu? (y/n): ");
        char confirm[5];
        safeInput(confirm, sizeof(confirm));

        if (confirm[0] == 'Y' || confirm[0] == 'y') {
            // 1. Xuất hóa đơn giao diện khung bảng vào thư mục output/HDxxx.txt cho khách
            exportBillToFile(&bill,
                             &bookings[bkIdx],
                             &rooms[rmIdx],
                             &customers[cuIdx],
                             usedServices, usedCount,
                             services, serviceCount);
                             
            // 2. TỰ ĐỘNG GHI 1 DÒNG DỮ LIỆU THUẦN VÀO bills.txt PHỤC VỤ ADMIN
            saveBillRecord(&bill, &bookings[bkIdx]);
            printf("  [OK] Da ghi nhan doanh thu vao file he thong (bills.txt).\n");
        }
    }

    pauseScreen();
}

/* ============================================================
   MENU QUAN LY HOA DON
   ============================================================ */
void menuBill(Booking bookings[], int bookingCount,
              Room rooms[], int roomCount,
              Customer customers[], int customerCount,
              UsedService usedServices[], int usedCount,
              Service services[], int serviceCount) {

    int choice;
    do {
        clearScreen();
        printHeader("QUAN LY HOA DON");
        printf("| 1. Xem / In hoa don theo booking                       |\n");
        printf("| 0. Quay lai menu chinh                                 |\n");
        printLine(58);

        choice = inputInt("  Chon: ", 0, 1);

        switch (choice) {
            case 1:
                viewBillByBooking(bookings, bookingCount,
                                  rooms, roomCount,
                                  customers, customerCount,
                                  usedServices, usedCount,
                                  services, serviceCount);
                break;
            default:
                break;
        }
    } while (choice != 0);
}