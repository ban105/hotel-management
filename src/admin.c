#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "admin.h"
#include "utils.h"

/* ============================================================
   TIEN ICH IN DUONG KE CHUAN 62 KY TU
   ============================================================ */
void adminPrintLine() {
    printf("  +----------------------------------------------------------+\n");
}



/* ============================================================
   SO SANH NGAY DD/MM/YYYY
   Tra ve: -1 neu d1 < d2 | 0 neu bang nhau | 1 neu d1 > d2
   ============================================================ */
int compareDates(const char *d1, const char *d2) {
    int day1, mon1, year1;
    int day2, mon2, year2;

    if (sscanf(d1, "%d/%d/%d", &day1, &mon1, &year1) != 3) return 0;
    if (sscanf(d2, "%d/%d/%d", &day2, &mon2, &year2) != 3) return 0;

    /* So sanh nam truoc */
    if (year1 != year2) return (year1 < year2) ? -1 : 1;
    /* Roi den thang */
    if (mon1  != mon2)  return (mon1  < mon2)  ? -1 : 1;
    /* Cuoi cung ngay */
    if (day1  != day2)  return (day1  < day2)  ? -1 : 1;
    return 0;
}

/* ============================================================
   KIEM TRA NGAY CO NAM TRONG KHOANG [from, to]
   ============================================================ */
int dateInRange(const char *d, const char *from, const char *to) {
    return (compareDates(d, from) >= 0 && compareDates(d, to) <= 0);
}

/* ============================================================
   DANG NHAP ADMIN
   ============================================================ */
int adminLogin() {
    clearScreen();
    adminPrintLine();
    printf("  |                  KHU VUC QUAN TRI VIEN                   |\n");
    adminPrintLine();
    printf("  | Yeu cau xac thuc de tiep tuc.                            |\n");
    adminPrintLine();

    /* An mat khau khi nhap */
    printf("  Nhap mat khau: ");
    char pw[30];
    safeInput(pw, sizeof(pw));

    if (strcmp(pw, ADMIN_PASSWORD) == 0) {
        printf("\n  [OK] Dang nhap thanh cong!\n");
        pauseScreen();
        return 1;
    }

    printf("\n  [!] Mat khau sai. Quay ve menu chinh.\n");
    pauseScreen();
    return 0;
}

/* ============================================================
   XEM DANH MUC DICH VU
   ============================================================ */
void adminViewServices(Service services[], int count) {
    clearScreen();
    adminPrintLine();
    printf("  |                DANH MUC DICH VU HIEN TAI                 |\n");
    adminPrintLine();

    if (count == 0) {
        printf("  | %-58s|\n", "  (Chua co dich vu nao)");
        adminPrintLine();
        pauseScreen();
        return;
    }

    printf("  | %-8s %-22s %10s  %-12s |\n",
           "Ma DV", "Ten dich vu", "Don gia", "Don vi");
    adminPrintLine();

    for (int i = 0; i < count; i++) {
        printf("  | %-8s %-22s %10.0f  %-12s |\n",
               services[i].serviceId,
               services[i].serviceName,
               services[i].price,
               services[i].unit);
    }
    adminPrintLine();
    printf("  Tong cong: %d dich vu.\n", count);
    pauseScreen();
}

/* ============================================================
   LUU DANH MUC DICH VU RA FILE (GHI DE)
   ============================================================ */
void adminSaveServices(Service services[], int count) {
    FILE *fp = fopen("data/services.txt", "w");
    if (!fp) {
        printf("  [!] Khong the mo file services.txt de luu.\n");
        return;
    }
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%s,%s,%.0f,%s\n",
                services[i].serviceId,
                services[i].serviceName,
                services[i].price,
                services[i].unit);
    }
    fclose(fp);
}

/* ============================================================
   TAO MA DICH VU TU DONG
   Tim so lon nhat hien co, cong 1
   ============================================================ */
static void generateServiceId(Service services[], int count, char *newId) {
    int maxNum = 0;
    for (int i = 0; i < count; i++) {
        int n = 0;
        sscanf(services[i].serviceId, "DV%d", &n);
        if (n > maxNum) maxNum = n;
    }
    snprintf(newId, 10, "DV%03d", maxNum + 1);
}

/* ============================================================
   THEM DICH VU MOI
   ============================================================ */
void adminAddService(Service services[], int *count) {
    clearScreen();
    adminPrintLine();
    printf("  |                   THEM DICH VU MOI                       |\n");
    adminPrintLine();

    if (*count >= MAX_SERVICES) {
        printf("  [!] Danh muc da day (%d dich vu).\n", MAX_SERVICES);
        pauseScreen();
        return;
    }

    Service s;
    memset(&s, 0, sizeof(Service));

    /* Tao ma tu dong */
    generateServiceId(services, *count, s.serviceId);
    printf("  Ma dich vu tu dong: %s\n", s.serviceId);
    adminPrintLine();

    /* Nhap ten */
    printf("  Ten dich vu       : ");
    safeInput(s.serviceName, sizeof(s.serviceName));
    if (!isNotEmpty(s.serviceName)) {
        printf("  [!] Ten khong duoc de trong.\n");
        pauseScreen();
        return;
    }

    /* Nhap don gia */
    s.price = inputFloat("  Don gia (VND)     : ", 1000);

    /* Nhap don vi */
    printf("  Don vi tinh       : ");
    safeInput(s.unit, sizeof(s.unit));
    if (!isNotEmpty(s.unit)) {
        printf("  [!] Don vi khong duoc de trong.\n");
        pauseScreen();
        return;
    }

    /* Xac nhan */
    printf("\n  Xac nhan them [%s] - %s - %.0f VND/%s ? (y/n): ",
           s.serviceId, s.serviceName, s.price, s.unit);
    char confirm[5];
    safeInput(confirm, sizeof(confirm));

    if (confirm[0] != 'y' && confirm[0] != 'Y') {
        printf("  Da huy.\n");
        pauseScreen();
        return;
    }

    /* Them vao mang va luu file */
    services[*count] = s;
    (*count)++;
    adminSaveServices(services, *count);

    printf("  [OK] Da them dich vu %s thanh cong!\n", s.serviceId);
    pauseScreen();
}

/* ============================================================
   SUA DICH VU
   ============================================================ */
void adminEditService(Service services[], int *count) {
    clearScreen();
    adminPrintLine();;
    printf("  |                   SUA DICH VU                           |\n");
    adminPrintLine();

    adminViewServices(services, *count);

    printf("  Nhap ma dich vu muon sua (VD: DV001): ");
    char svcId[10];
    safeInput(svcId, sizeof(svcId));

    /* Tim dich vu */
    int idx = -1;
    for (int i = 0; i < *count; i++) {
        if (strcmp(services[i].serviceId, svcId) == 0) {
            idx = i;
            break;
        }
    }

    if (idx < 0) {
        printf("  [!] Khong tim thay dich vu %s.\n", svcId);
        pauseScreen();
        return;
    }

    printf("\n  Dich vu hien tai: [%s] %s - %.0f VND/%s\n",
           services[idx].serviceId,
           services[idx].serviceName,
           services[idx].price,
           services[idx].unit);
    adminPrintLine();
    printf("  (Nhan Enter de giu nguyen gia tri cu)\n\n");

    /* Sua ten */
    char tmp[50];
    printf("  Ten moi [%s]: ", services[idx].serviceName);
    safeInput(tmp, sizeof(tmp));
    if (isNotEmpty(tmp)) {
        strncpy(services[idx].serviceName, tmp,
                sizeof(services[idx].serviceName) - 1);
    }

    /* Sua don gia */
    printf("  Don gia moi [%.0f]: ", services[idx].price);
    safeInput(tmp, sizeof(tmp));
    if (isNotEmpty(tmp)) {
        float newPrice = atof(tmp);
        if (newPrice >= 1000) services[idx].price = newPrice;
        else printf("  [!] Gia khong hop le, giu nguyen.\n");
    }

    /* Sua don vi */
    printf("  Don vi moi [%s]: ", services[idx].unit);
    safeInput(tmp, sizeof(tmp));
    if (isNotEmpty(tmp)) {
        strncpy(services[idx].unit, tmp, sizeof(services[idx].unit) - 1);
    }

    adminSaveServices(services, *count);
    printf("\n  [OK] Da cap nhat dich vu %s.\n", svcId);
    pauseScreen();
}

/* ============================================================
   XOA DICH VU
   ============================================================ */
void adminDeleteService(Service services[], int *count) {
    clearScreen();
    adminPrintLine();
    printf("  |                   XOA DICH VU                           |\n");
    adminPrintLine();

    adminViewServices(services, *count);

    printf("  Nhap ma dich vu muon xoa (VD: DV001): ");
    char svcId[10];
    safeInput(svcId, sizeof(svcId));

    int idx = -1;
    for (int i = 0; i < *count; i++) {
        if (strcmp(services[i].serviceId, svcId) == 0) {
            idx = i;
            break;
        }
    }

    if (idx < 0) {
        printf("  [!] Khong tim thay dich vu %s.\n", svcId);
        pauseScreen();
        return;
    }

    printf("\n  Xoa dich vu: [%s] %s - %.0f VND/%s\n",
           services[idx].serviceId,
           services[idx].serviceName,
           services[idx].price,
           services[idx].unit);
    printf("  Xac nhan xoa? (y/n): ");
    char confirm[5];
    safeInput(confirm, sizeof(confirm));

    if (confirm[0] != 'y' && confirm[0] != 'Y') {
        printf("  Da huy.\n");
        pauseScreen();
        return;
    }

    /* Dich cac phan tu phia sau len truoc */
    for (int i = idx; i < *count - 1; i++) {
        services[i] = services[i + 1];
    }
    (*count)--;

    adminSaveServices(services, *count);
    printf("  [OK] Da xoa dich vu %s.\n", svcId);
    pauseScreen();
}

/* ============================================================
   LUU HOA DON VAO FILE data/bills.txt (APPEND)
   Format: billId,bookingId,checkOutDate,roomCost,serviceCost,
           serviceCharge,vat,total
   ============================================================ */


/* ============================================================
   DOC DANH SACH HOA DON TU FILE data/bills.txt
   ============================================================ */
int loadBillRecords(BillRecord records[]) {
    FILE *fp = fopen("bills.txt", "r");
    if (!fp) return 0;

    int count = 0;
    char line[256];

    while (fgets(line, sizeof(line), fp) && count < MAX_BILLS) {
        trimStr(line);
        if (strlen(line) == 0) continue;

        BillRecord *r = &records[count];
        int parsed = sscanf(line, "%9[^,],%9[^,],%19[^,],%f,%f,%f,%f,%f",
                            r->billId,
                            r->bookingId,
                            r->checkOutDate,
                            &r->roomCost,
                            &r->serviceCost,
                            &r->serviceCharge,
                            &r->vat,
                            &r->total);
        if (parsed == 8) count++;
    }

    fclose(fp);
    return count;
}

/* ============================================================
   BAO CAO 1: DOANH THU TONG HOP
   ============================================================ */
void reportRevenueSummary() {
    clearScreen();

    BillRecord records[MAX_BILLS];
    int count = loadBillRecords(records);

    adminPrintLine();
    printf("  |              BAO CAO DOANH THU TONG HOP                  |\n");
    adminPrintLine();

    if (count == 0) {
        printf("  | %-57s|\n", "  Chua co hoa don nao duoc ghi nhan.");
        adminPrintLine();
        pauseScreen();
        return;
    }

    /* Tinh tong cac chi so */
    float totalRoom    = 0;
    float totalSvc     = 0;
    float totalCharge  = 0;
    float totalVat     = 0;
    float grandTotal   = 0;

    for (int i = 0; i < count; i++) {
        totalRoom   += records[i].roomCost;
        totalSvc    += records[i].serviceCost;
        totalCharge += records[i].serviceCharge;
        totalVat    += records[i].vat;
        grandTotal  += records[i].total;
    }

    printf("  | %-31s     %20s |\n", "Chi tieu", "Gia tri (VND)");
    adminPrintLine();
    printf("  | %-31s     %20.0f |\n", "Tong so hoa don da thanh toan:", (float)count);
    adminPrintLine();
    printf("  | %-31s     %20.0f |\n", "Tong doanh thu tien phong:", totalRoom);
    adminPrintLine();
    printf("  | %-31s     %20.0f |\n", "Tong doanh thu tien dich vu:", totalSvc);
    adminPrintLine();
    printf("  | %-31s     %20.0f |\n", "Tong phi dich vu (5%%):", totalCharge);
    adminPrintLine();
    printf("  | %-31s     %20.0f |\n", "Tong thue VAT (10%%):", totalVat);
    adminPrintLine();
    printf("  | %-31s     %20.0f |\n", "TONG DOANH THU THUC NHAN:", grandTotal);
    adminPrintLine();

    pauseScreen();
}

/* ============================================================
   BAO CAO 2: HIEU SUAT DICH VU (BEST SELLER)
   ============================================================ */
void reportServicePerformance(Service services[], int serviceCount) {
    clearScreen();

    adminPrintLine();
    printf("  |              THONG KE HIEU SUAT DICH VU                  |\n");
    adminPrintLine();

    /* Doc du lieu used_services */
    FILE *fp = fopen("data/used_services.txt", "r");
    if (!fp) {
        printf("  | %-58s|\n", "  Chua co du lieu dich vu da su dung.");
        adminPrintLine();
        pauseScreen();
        return;
    }

    /* Mang thong ke: moi phan tu tuong ung 1 dich vu */
    ServiceStat stats[MAX_SERVICES];
    int statCount = 0;

    /* Khoi tao thong ke tu danh muc dich vu */
    for (int i = 0; i < serviceCount; i++) {
        strncpy(stats[i].serviceId,   services[i].serviceId,
                sizeof(stats[i].serviceId) - 1);
        strncpy(stats[i].serviceName, services[i].serviceName,
                sizeof(stats[i].serviceName) - 1);
        stats[i].totalQty     = 0;
        stats[i].totalRevenue = 0;
    }
    statCount = serviceCount;

    /* Quet file used_services, cong don so luong */
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        trimStr(line);
        if (strlen(line) == 0) continue;

        char bkId[10], svcId[10];
        int  qty, status;
        if (sscanf(line, "%9[^,],%9[^,],%d,%d",
                   bkId, svcId, &qty, &status) != 4) continue;

        /* Chi tinh status == 1 (Active) */
        if (status != 1) continue;

        /* Tim vi tri trong mang stats */
        for (int i = 0; i < statCount; i++) {
            if (strcmp(stats[i].serviceId, svcId) == 0) {
                stats[i].totalQty += qty;
                /* Lay don gia tu mang services */
                for (int j = 0; j < serviceCount; j++) {
                    if (strcmp(services[j].serviceId, svcId) == 0) {
                        stats[i].totalRevenue += services[j].price * qty;
                        break;
                    }
                }
                break;
            }
        }
    }
    fclose(fp);

    /* Sap xep giam dan theo tong so luong (bubble sort don gian) */
    for (int i = 0; i < statCount - 1; i++) {
        for (int j = 0; j < statCount - i - 1; j++) {
            if (stats[j].totalQty < stats[j + 1].totalQty) {
                ServiceStat tmp = stats[j];
                stats[j]        = stats[j + 1];
                stats[j + 1]    = tmp;
            }
        }
    }

    /* In bang xep hang */
    printf("  | %3s  %-8s %-18s %8s %14s |\n",
           "STT", "Ma DV", "Ten dich vu", "Tong SL", "Doanh thu");
    adminPrintLine();

    int bestIdx = -1;
    int bestQty = 0;

    for (int i = 0; i < statCount; i++) {
        if (stats[i].totalQty == 0) continue; /* Bo qua dich vu chua ban */

        printf("  | %3d  %-8s %-18s %8d %14.0f |\n",
               i + 1,
               stats[i].serviceId,
               stats[i].serviceName,
               stats[i].totalQty,
               stats[i].totalRevenue);

        if (stats[i].totalQty > bestQty) {
            bestQty = stats[i].totalQty;
            bestIdx = i;
        }
    }

    adminPrintLine();

    /* In Best Seller */
    if (bestIdx >= 0) {
        printf("  | BEST SELLER: %-44s|\n", "");
        printf("  | >> [%s] %-20s - %d luot - %.0f VND   |\n",
               stats[bestIdx].serviceId,
               stats[bestIdx].serviceName,
               stats[bestIdx].totalQty,
               stats[bestIdx].totalRevenue);
        adminPrintLine();
    } else {
        printf("  | %-58s|\n", "  Chua co dich vu nao duoc su dung.");
        adminPrintLine();
    }

    pauseScreen();
}

/* ============================================================
   BAO CAO 3: DOANH THU THEO KHOANG THOI GIAN
   ============================================================ */
void reportRevenueByDateRange() {
    clearScreen();

    adminPrintLine();
    printf("  |            DOANH THU THEO KHOANG THOI GIAN               |\n");
    adminPrintLine();

    /* Nhap khoang thoi gian */
    char fromDate[20], toDate[20];

    printf("    Nhap ngay bat dau  (DD/MM/YYYY): ");
    safeInput(fromDate, sizeof(fromDate));
    if (!isValidDate(fromDate)) {
        printf("  [!] Ngay khong hop le.\n");
        pauseScreen();
        return;
    }

    printf("    Nhap ngay ket thuc (DD/MM/YYYY): ");
    safeInput(toDate, sizeof(toDate));
    if (!isValidDate(toDate)) {
        printf("  [!] Ngay khong hop le.\n");
        pauseScreen();
        return;
    }

    if (compareDates(fromDate, toDate) > 0) {
        printf("  [!] Ngay bat dau phai truoc ngay ket thuc.\n");
        pauseScreen();
        return;
    }

    /* Doc va loc hoa don */
    BillRecord records[MAX_BILLS];
    int total = loadBillRecords(records);

    float sumRoom    = 0;
    float sumSvc     = 0;
    float sumCharge  = 0;
    float sumVat     = 0;
    float sumTotal   = 0;
    int   matched    = 0;

    adminPrintLine();
    printf("  | %-10s %-10s %10s %10s %12s |\n",
           "Ma HD", "Ma BK", "Tien phong", "Dich vu", "Tong");
    adminPrintLine();

    for (int i = 0; i < total; i++) {
        if (!dateInRange(records[i].checkOutDate, fromDate, toDate)) continue;

        printf("  | %-10s %-10s %10.0f %10.0f %12.0f |\n",
               records[i].billId,
               records[i].bookingId,
               records[i].roomCost,
               records[i].serviceCost,
               records[i].total);

        sumRoom   += records[i].roomCost;
        sumSvc    += records[i].serviceCost;
        sumCharge += records[i].serviceCharge;
        sumVat    += records[i].vat;
        sumTotal  += records[i].total;
        matched++;
    }

    adminPrintLine();

    if (matched == 0) {
        printf("  | %-58s|\n",
               " Khong co hoa don nao trong khoang thoi gian nay.");
       adminPrintLine();;
    } else {
        printf("  | Khoang thoi gian : %-38s|\n",
               "");
        printf("  |   Tu: %-10s  Den: %-31s   |\n", fromDate, toDate);
        adminPrintLine();
        printf("  | So hoa don phu hop          : %24d   |\n", matched);
        adminPrintLine();
        printf("  | Tong tien phong             : %22.0f VND |\n", sumRoom);
        adminPrintLine();
        printf("  | Tong tien dich vu           : %22.0f VND |\n", sumSvc);
        adminPrintLine();
        printf("  | Tong phi dich vu (5%%)       : %22.0f VND |\n", sumCharge);
        adminPrintLine();
        printf("  | Tong thue VAT (10%%)         : %22.0f VND |\n", sumVat);
        adminPrintLine();
        printf("  | TONG DOANH THU GIAI DOAN    : %22.0f VND |\n", sumTotal);
        adminPrintLine();
    }

    pauseScreen();
}

/* ============================================================
   MENU QUAN TRI VIEN
   ============================================================ */
void menuAdmin(Room rooms[], int *roomCount,
               Customer customers[], int *customerCount,
               Booking bookings[], int *bookingCount,
               UsedService usedServices[], int *usedCount,
               Service services[], int *serviceCount) {

    /* Xac thuc truoc khi vao */
    if (!adminLogin()) return;

    int choice;
    do {
        clearScreen();
        adminPrintLine();
        printf("  |                MENU QUAN TRI VIEN                        |\n");
        adminPrintLine();
        printf("  | 1. Quan ly danh muc dich vu                              |\n");
        printf("  | 2. Bao cao doanh thu tong hop                            |\n");
        printf("  | 3. Thong ke hieu suat dich vu (Best Seller)              |\n");
        printf("  | 4. Doanh thu theo khoang thoi gian                       |\n");
        printf("  | 0. Quay lai menu chinh                                   |\n");
        adminPrintLine();

        choice = inputInt("  Chon: ", 0, 4);

        switch (choice) {
            case 1:
                /* Sub-menu quan ly dich vu */
                {
                    int sub;
                    do {
                        clearScreen();
                        adminPrintLine();
                        printf("  |           QUAN LY DANH MUC DICH VU                       |\n");
                        adminPrintLine();
                        printf("  | 1. Xem danh muc dich vu                                  |\n");
                        printf("  | 2. Them dich vu moi                                      |\n");
                        printf("  | 3. Sua dich vu                                           |\n");
                        printf("  | 4. Xoa dich vu                                           |\n");
                        printf("  | 0. Quay lai                                              |\n");
                        adminPrintLine();
                        sub = inputInt("  Chon: ", 0, 4);

                        switch (sub) {
                            case 1:
                                clearScreen();
                                adminViewServices(services, *serviceCount);
                                break;
                            case 2:
                                adminAddService(services, serviceCount);
                                break;
                            case 3:
                                adminEditService(services, serviceCount);
                                break;
                            case 4:
                                adminDeleteService(services, serviceCount);
                                break;
                        }
                    } while (sub != 0);
                }
                break;

            case 2:
                reportRevenueSummary();
                break;

            case 3:
                reportServicePerformance(services, *serviceCount);
                break;

            case 4:
                reportRevenueByDateRange();
                break;

            case 0:
                break;
        }
    } while (choice != 0);
}