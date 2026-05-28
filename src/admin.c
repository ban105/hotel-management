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

#define ADMIN_FULL_DIV "  +----------------------------------------------------------+\n"
#define ADMIN_SERVICE_TABLE_DIV "  +----------+------------------------+------------+---------+\n"
#define ADMIN_SUMMARY_TABLE_DIV "  +-----------------------------------+----------------------+\n"
#define ADMIN_PERFORMANCE_TABLE_DIV "  +-----+----------+--------------------+----------+----------------+\n"
#define ADMIN_PERFORMANCE_FULL_DIV "  +-----------------------------------------------------------------+\n"
#define ADMIN_REVENUE_TABLE_DIV "  +------------+------------+------------+------------+--------------+\n"
#define ADMIN_REVENUE_FULL_DIV "  +------------------------------------------------------------------+\n"

static void adminPrintBoxHeader(const char *border, int innerWidth, const char *title) {
    int titleLen = strlen(title);
    if (titleLen > innerWidth) titleLen = innerWidth;
    int left = (innerWidth - titleLen) / 2;
    int right = innerWidth - titleLen - left;

    printf(border);
    printf("  | ");
    for (int i = 0; i < left; i++) printf(" ");
    printf("%.*s", innerWidth, title);
    for (int i = 0; i < right; i++) printf(" ");
    printf(" |\n");
    printf(border);
}

static int parseMinFloat(const char *text, float min, float *value) {
    char *end;
    float parsed = strtof(text, &end);

    if (end == text) return 0;
    while (*end != '\0') {
        if (*end != ' ' && *end != '\t') return 0;
        end++;
    }
    if (parsed < min) return 0;

    *value = parsed;
    return 1;
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

int systemLogin(User *user) {
    char password[30];
    int roleChoice;
    int attempts = 0;

    clearScreen();
    adminPrintLine();
    printf("  |                    DANG NHAP HE THONG                    |\n");
    adminPrintLine();
    printf("  | 1. Le tan                                                |\n");
    printf("  | 2. Admin                                                 |\n");
    printf("  | 0. Thoat                                                 |\n");
    adminPrintLine();

    roleChoice = inputInt("  Chon vai tro: ", 0, 2);
    if (roleChoice == 0) return 0;

    while (attempts < 3) {
        printf("  Mat khau: ");
        safeInput(password, sizeof(password));
        trimStr(password);

        if (roleChoice == 2 && strcmp(password, ADMIN_PASSWORD) == 0) {
            strcpy(user->username, "admin");
            strcpy(user->password, password);
            user->role = ROLE_ADMIN;
            printf("\n  [OK] Dang nhap Admin thanh cong!\n");
            pauseScreen();
            return 1;
        }

        if (roleChoice == 1 && strcmp(password, RECEPTION_PASSWORD) == 0) {
            strcpy(user->username, "letan");
            strcpy(user->password, password);
            user->role = ROLE_RECEPTIONIST;
            printf("\n  [OK] Dang nhap Le tan thanh cong!\n");
            pauseScreen();
            return 1;
        }

        attempts++;
        printf("  [!] Mat khau sai. Con %d lan thu.\n", 3 - attempts);
    }

    printf("\n  [!] Nhap sai qua 3 lan. Thoat chuong trinh.\n");
    pauseScreen();
    return 0;
}

/* ============================================================
   XEM DANH MUC DICH VU
   ============================================================ */
void adminViewServices(Service services[], int count) {
    clearScreen();
    adminPrintBoxHeader(ADMIN_FULL_DIV, 56, "DANH MUC DICH VU HIEN TAI");

    if (count == 0) {
        printf("  | %-56s |\n", "(Chua co dich vu nao)");
        adminPrintLine();
        pauseScreen();
        return;
    }

    printf(ADMIN_SERVICE_TABLE_DIV);
    printf("  | %-8s | %-22s | %10s | %-7s |\n",
           "Ma DV", "Ten dich vu", "Don gia", "Don vi");
    printf(ADMIN_SERVICE_TABLE_DIV);

    for (int i = 0; i < count; i++) {
        printf("  | %-8.8s | %-22.22s | %10.0f | %-7.7s |\n",
               services[i].serviceId,
               services[i].serviceName,
               services[i].price,
               services[i].unit);
    }
    printf(ADMIN_SERVICE_TABLE_DIV);
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
        services[idx].serviceName[sizeof(services[idx].serviceName) - 1] = '\0';
    }

    /* Sua don gia */
    printf("  Don gia moi [%.0f]: ", services[idx].price);
    safeInput(tmp, sizeof(tmp));
    if (isNotEmpty(tmp)) {
        float newPrice;
        if (parseMinFloat(tmp, 1000, &newPrice)) services[idx].price = newPrice;
        else printf("  [!] Gia khong hop le, giu nguyen.\n");
    }

    /* Sua don vi */
    printf("  Don vi moi [%s]: ", services[idx].unit);
    safeInput(tmp, sizeof(tmp));
    if (isNotEmpty(tmp)) {
        strncpy(services[idx].unit, tmp, sizeof(services[idx].unit) - 1);
        services[idx].unit[sizeof(services[idx].unit) - 1] = '\0';
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

    FILE *usedFp = fopen("data/used_services.txt", "r");
    if (usedFp) {
        char line[256];
        while (fgets(line, sizeof(line), usedFp)) {
            char bkId[10], usedSvcId[10];
            int qty, status;
            if (sscanf(line, "%9[^,],%9[^,],%d,%d",
                       bkId, usedSvcId, &qty, &status) == 4 &&
                strcmp(usedSvcId, svcId) == 0) {
                fclose(usedFp);
                printf("  [!] Dich vu %s da tung duoc su dung, khong the xoa vi se anh huong hoa don.\n", svcId);
                printf("      Hay sua ten/gia neu can cap nhat danh muc.\n");
                pauseScreen();
                return;
            }
        }
        fclose(usedFp);
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
    FILE *fp = fopen("data/bills.txt", "r");
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
        if (parsed == 8) {
            int duplicate = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(records[i].billId, r->billId) == 0 ||
                    strcmp(records[i].bookingId, r->bookingId) == 0) {
                    duplicate = 1;
                    break;
                }
            }
            if (!duplicate) count++;
        }
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

    printf("  | %-31.31s     %20.20s |\n", "Chi tieu", "Gia tri (VND)");
    adminPrintLine();
    printf("  | %-31.31s     %20.0f |\n", "Tong so hoa don da thanh toan:", (float)count);
    adminPrintLine();
    printf("  | %-31.31s     %20.0f |\n", "Tong doanh thu tien phong:", totalRoom);
    adminPrintLine();
    printf("  | %-31.31s     %20.0f |\n", "Tong doanh thu tien dich vu:", totalSvc);
    adminPrintLine();
    printf("  | %-31.31s     %20.0f |\n", "Tong phi dich vu (5%):", totalCharge);
    adminPrintLine();
    printf("  | %-31.31s     %20.0f |\n", "Tong thue VAT (10%):", totalVat);
    adminPrintLine();
    printf("  | %-31.31s     %20.0f |\n", "TONG DOANH THU THUC NHAN:", grandTotal);
    adminPrintLine();

    pauseScreen();
}

/* ============================================================
   BAO CAO 2: HIEU SUAT DICH VU (BEST SELLER)
   ============================================================ */
void reportServicePerformance(Service services[], int serviceCount) {
    clearScreen();

    adminPrintBoxHeader(ADMIN_PERFORMANCE_FULL_DIV, 63, "THONG KE HIEU SUAT DICH VU");

    /* Doc du lieu used_services */
    FILE *fp = fopen("data/used_services.txt", "r");
    if (!fp) {
        printf("  | %-63s |\n", "Chua co du lieu dich vu da su dung.");
        printf(ADMIN_PERFORMANCE_FULL_DIV);
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
        stats[i].serviceId[sizeof(stats[i].serviceId) - 1] = '\0';
        strncpy(stats[i].serviceName, services[i].serviceName,
                sizeof(stats[i].serviceName) - 1);
        stats[i].serviceName[sizeof(stats[i].serviceName) - 1] = '\0';
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
    printf(ADMIN_PERFORMANCE_TABLE_DIV);
    printf("  | %3s | %-8s | %-18s | %8s | %14s |\n",
           "STT", "Ma DV", "Ten dich vu", "Tong SL", "Doanh thu");
    printf(ADMIN_PERFORMANCE_TABLE_DIV);

    int bestIdx = -1;
    int bestQty = 0;

    for (int i = 0; i < statCount; i++) {
        if (stats[i].totalQty == 0) continue; /* Bo qua dich vu chua ban */

        printf("  | %3d | %-8.8s | %-18.18s | %8d | %14.0f |\n",
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

    printf(ADMIN_PERFORMANCE_TABLE_DIV);

    /* In Best Seller */
    if (bestIdx >= 0) {
        printf(ADMIN_PERFORMANCE_FULL_DIV);
        printf("  | %-63s |\n", "BEST SELLER");
        printf("  | >> [%-5.5s] %-18.18s - %3d luot - %9.0f VND %-6s |\n",
               stats[bestIdx].serviceId,
               stats[bestIdx].serviceName,
               stats[bestIdx].totalQty,
               stats[bestIdx].totalRevenue,
               "");
        printf(ADMIN_PERFORMANCE_FULL_DIV);
    } else {
        printf("  | %-63s |\n", "Chua co dich vu nao duoc su dung.");
        printf(ADMIN_PERFORMANCE_FULL_DIV);
    }

    pauseScreen();
}

/* ============================================================
   BAO CAO 3: DOANH THU THEO KHOANG THOI GIAN
   ============================================================ */
void reportRevenueByDateRange() {
    clearScreen();

    adminPrintBoxHeader(ADMIN_REVENUE_FULL_DIV, 64, "DOANH THU THEO KHOANG THOI GIAN");

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

    printf(ADMIN_REVENUE_TABLE_DIV);
    printf("  | %-10s | %-10s | %10s | %10s | %12s |\n",
           "Ma HD", "Ma BK", "Tien phong", "Dich vu", "Tong");
    printf(ADMIN_REVENUE_TABLE_DIV);

    for (int i = 0; i < total; i++) {
        if (!dateInRange(records[i].checkOutDate, fromDate, toDate)) continue;

        printf("  | %-10.10s | %-10.10s | %10.0f | %10.0f | %12.0f |\n",
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

    printf(ADMIN_REVENUE_TABLE_DIV);

    if (matched == 0) {
        printf("  | %-64s |\n",
               " Khong co hoa don nao trong khoang thoi gian nay.");
       printf(ADMIN_REVENUE_FULL_DIV);
    } else {
        char rangeLine[80];
        snprintf(rangeLine, sizeof(rangeLine), "Khoang thoi gian: Tu %s den %s", fromDate, toDate);
        printf(ADMIN_REVENUE_FULL_DIV);
        printf("  | %-64.64s |\n", rangeLine);
        printf("  | %-38s %25d |\n", "So hoa don phu hop:", matched);
        printf("  | %-38s %21.0f VND |\n", "Tong tien phong:", sumRoom);
        printf("  | %-38s %21.0f VND |\n", "Tong tien dich vu:", sumSvc);
        printf("  | %-38s %21.0f VND |\n", "Tong phi dich vu (5%):", sumCharge);
        printf("  | %-38s %21.0f VND |\n", "Tong thue VAT (10%):", sumVat);
        printf("  | %-38s %21.0f VND |\n", "TONG DOANH THU GIAI DOAN:", sumTotal);
        printf(ADMIN_REVENUE_FULL_DIV);
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
               Service services[], int *serviceCount,
               Employee employees[], int *employeeCount) {
    (void)rooms;
    (void)roomCount;
    (void)customers;
    (void)customerCount;
    (void)bookings;
    (void)bookingCount;
    (void)usedServices;
    (void)usedCount;

    int choice;
    do {
        clearScreen();
        adminPrintLine();
        printf("  |                   MENU QUAN TRI VIEN                     |\n");
        adminPrintLine();
        printf("  | 1. Quan ly danh muc dich vu                              |\n");
        printf("  | 2. Bao cao doanh thu tong hop                            |\n");
        printf("  | 3. Thong ke hieu suat dich vu (Best Seller)              |\n");
        printf("  | 4. Doanh thu theo khoang thoi gian                       |\n");
        printf("  | 5. Quan ly Nhan vien                                     |\n");
        printf("  | 0. Quay lai menu chinh                                   |\n");
        adminPrintLine();

        choice = inputInt("  Chon: ", 0, 5);

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
            
            case 5:
                menuEmployee(employees, employeeCount);
                break;    

            case 0:
                break;
        }
    } while (choice != 0);
}
