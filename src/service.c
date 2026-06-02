#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "service.h"
#include "utils.h"

#define SERVICE_TABLE_DIV "  +----------+------------------------+------------+---------+\n"
#define USED_SERVICE_CONFIRM_DIV "  +----------+------------------------+-------+--------------+\n"
#define USED_SERVICE_CONFIRM_FULL_DIV "  +----------------------------------------------------------+\n"
#define SERVICE_TABLE_WIDTH 58
#define USED_SERVICE_TABLE_WIDTH 58

static void printServiceTableHeader(const char *title) {
    int titleLen = strlen(title);
    if (titleLen > SERVICE_TABLE_WIDTH) titleLen = SERVICE_TABLE_WIDTH;

    int leftPadding = (SERVICE_TABLE_WIDTH - titleLen) / 2;
    int rightPadding = SERVICE_TABLE_WIDTH - titleLen - leftPadding;

    printf("\n  +");
    for (int i = 0; i < SERVICE_TABLE_WIDTH; i++) printf("-");
    printf("+\n");
    printf("  |");
    for (int i = 0; i < leftPadding; i++) printf(" ");
    printf("%.*s", titleLen, title);
    for (int i = 0; i < rightPadding; i++) printf(" ");
    printf("|\n");
    printf("  +");
    for (int i = 0; i < SERVICE_TABLE_WIDTH; i++) printf("-");
    printf("+\n");
}

static void printUsedServiceTitle(const char *title) {
    int titleLen = strlen(title);
    if (titleLen > USED_SERVICE_TABLE_WIDTH) titleLen = USED_SERVICE_TABLE_WIDTH;

    int leftPadding = (USED_SERVICE_TABLE_WIDTH - titleLen) / 2;
    int rightPadding = USED_SERVICE_TABLE_WIDTH - titleLen - leftPadding;

    printf(USED_SERVICE_CONFIRM_FULL_DIV);
    printf("  |");
    for (int i = 0; i < leftPadding; i++) printf(" ");
    printf("%.*s", titleLen, title);
    for (int i = 0; i < rightPadding; i++) printf(" ");
    printf("|\n");
    printf(USED_SERVICE_CONFIRM_FULL_DIV);
}

/* ============================================================
   DOC DANH MUC DICH VU TU FILE TINH
   ============================================================ */
int loadServices(Service services[]) {
    FILE *fp = fopen("data/services.txt", "r");
    if (!fp) {
        /* Neu chua co file, tao du lieu mac dinh */
        printf("  [!] Khong tim thay data/services.txt, tao du lieu mac dinh...\n");

        /* Tao file mac dinh */
        fp = fopen("data/services.txt", "w");
        if (!fp) {
            printf("  [!] Khong the tao file services.txt\n");
            return 0;
        }
        fprintf(fp, "DV001,Nuoc suoi,15000,chai\n");
        fprintf(fp, "DV002,Giat la,50000,kg\n");
        fprintf(fp, "DV003,Thuc an phong,80000,suat\n");
        fprintf(fp, "DV004,Nuoc uong cao cap,35000,chai\n");
        fprintf(fp, "DV005,Dich vu spa,200000,lan\n");
        fprintf(fp, "DV006,Thue xe,150000,ngay\n");
        fprintf(fp, "DV007,Dat ve may bay,50000,ve\n");
        fprintf(fp, "DV008,Goi do an them,60000,mon\n");
        fclose(fp);

        fp = fopen("data/services.txt", "r");
        if (!fp) return 0;
    }

    int count = 0;
    char line[256];

    while (fgets(line, sizeof(line), fp) && count < MAX_SERVICES) {
        /* Bo qua dong trong hoac comment */
        trimStr(line);
        if (strlen(line) == 0) continue;

        Service *s = &services[count];
        int parsed = sscanf(line, "%9[^,],%29[^,],%f,%9[^\n]",
                            s->serviceId,
                            s->serviceName,
                            &s->price,
                            s->unit);

        if (parsed == 4) {
            count++;
        }
    }

    fclose(fp);
    return count;
}

/* ============================================================
   HIEN DANH SACH DICH VU CO SAN
   ============================================================ */
void listServices(Service services[], int count) {
    if (count == 0) {
        printf("  Khong co dich vu nao.\n");
        return;
    }

    printf(SERVICE_TABLE_DIV);
    printf("  | %-8s | %-22s | %10s | %-7s |\n",
           "Ma DV", "Ten dich vu", "Don gia", "Don vi");
    printf(SERVICE_TABLE_DIV);

    for (int i = 0; i < count; i++) {
        printf("  | %-8.8s | %-22.22s | %10.0f | %-7.7s |\n",
               services[i].serviceId,
               services[i].serviceName,
               services[i].price,
               services[i].unit);
    }
    printf(SERVICE_TABLE_DIV);
}

/* ============================================================
   TIM DICH VU THEO MA
   ============================================================ */
int findServiceById(Service services[], int count, const char *id) {
    for (int i = 0; i < count; i++) {
        if (strcmp(services[i].serviceId, id) == 0)
            return i;
    }
    return -1;
}

static int findActiveUsedService(UsedService usedServices[], int usedCount,
                                 const char *bookingId,
                                 const char *serviceId) {
    for (int i = 0; i < usedCount; i++) {
        if (usedServices[i].status == SERVICE_ACTIVE &&
            strcmp(usedServices[i].bookingId, bookingId) == 0 &&
            strcmp(usedServices[i].serviceId, serviceId) == 0) {
            return i;
        }
    }
    return -1;
}

static void mergeActiveUsedServices(UsedService usedServices[], int *usedCount) {
    for (int i = 0; i < *usedCount; i++) {
        if (usedServices[i].status != SERVICE_ACTIVE) continue;

        for (int j = i + 1; j < *usedCount; ) {
            if (usedServices[j].status == SERVICE_ACTIVE &&
                strcmp(usedServices[i].bookingId, usedServices[j].bookingId) == 0 &&
                strcmp(usedServices[i].serviceId, usedServices[j].serviceId) == 0) {
                usedServices[i].quantity += usedServices[j].quantity;
                for (int k = j; k < *usedCount - 1; k++) {
                    usedServices[k] = usedServices[k + 1];
                }
                (*usedCount)--;
            } else {
                j++;
            }
        }
    }
}

static int listActiveUsedServicesByBooking(UsedService usedServices[], int usedCount,
                                           Service services[], int serviceCount,
                                           const char *bookingId) {
    int found = 0;
    float total = 0;

    printf(USED_SERVICE_CONFIRM_DIV);
    printf("  | %-8s | %-22s | %5s | %12s |\n",
           "Ma DV", "Ten DV", "SL", "Thanh tien");
    printf(USED_SERVICE_CONFIRM_DIV);

    for (int i = 0; i < usedCount; i++) {
        if (usedServices[i].status != SERVICE_ACTIVE ||
            strcmp(usedServices[i].bookingId, bookingId) != 0) {
            continue;
        }

        int svcIdx = findServiceById(services, serviceCount, usedServices[i].serviceId);
        if (svcIdx < 0) continue;

        float subtotal = services[svcIdx].price * usedServices[i].quantity;
        total += subtotal;
        found++;

        printf("  | %-8.8s | %-22.22s | %5d | %12.0f |\n",
               usedServices[i].serviceId,
               services[svcIdx].serviceName,
               usedServices[i].quantity,
               subtotal);
    }

    printf(USED_SERVICE_CONFIRM_DIV);
    if (found > 0) {
        printf("  | %-32s %19.0f VND |\n", "Tong tien dich vu:", total);
        printf(USED_SERVICE_CONFIRM_FULL_DIV);
    }

    return found;
}

/* ============================================================
   GOI DICH VU CHO BOOKING DANG CHECK-IN
   ============================================================ */
void addUsedService(UsedService usedServices[], int *usedCount,
                    Booking bookings[], int bookingCount,
                    Room rooms[], int roomCount,
                    Customer customers[], int customerCount,
                    Service services[], int serviceCount) {
    (void)rooms;
    (void)roomCount;

    clearScreen();
    printHeader("GOI DICH VU PHONG");

    /* --- Nhap ma booking --- */
    char bkId[10];
    printf("  Nhap ma booking (VD: BK001): ");
    safeInput(bkId, sizeof(bkId));
    trimStr(bkId);

    int bkIdx = findBookingById(bookings, bookingCount, bkId);
    if (bkIdx < 0) {
        printf("  [!] Khong tim thay booking %s\n", bkId);
        pauseScreen();
        return;
    }

    /* Chi cho phep goi dich vu khi dang check-in */
    if (bookings[bkIdx].status != BOOKING_CHECKIN) {
        printf("  [!] Booking nay khong o trang thai 'Dang o'.\n");
        printf("      Chi co the goi dich vu khi khach dang thue phong.\n");
        pauseScreen();
        return;
    }

    /* --- Hien thong tin booking --- */
    int cuIdx = findCustomerById(customers, customerCount, bookings[bkIdx].customerId);
    printf("\n  Booking: %s\n", bookings[bkIdx].bookingId);
    if (cuIdx >= 0) printf("  Khach  : %s (%s)\n", customers[cuIdx].name, customers[cuIdx].id);

    /* --- Hien danh sach dich vu --- */
    printf("\n");
    printServiceTableHeader("DANH SACH DICH VU");
    listServices(services, serviceCount);

    /* --- Gio hang tam (tempCart) --- */
    UsedService tempCart[MAX_CART_ITEMS];
    int cartCount = 0;

    printf("\n  Nhap dich vu muon goi (nhap 0 de ket thuc):\n");

    while (cartCount < MAX_CART_ITEMS) {
        char svcId[10];
        printf("\n  Ma dich vu: ");
        safeInput(svcId, sizeof(svcId));

        /* Nhap 0 de thoat vong lap */
        if (strcmp(svcId, "0") == 0) break;

        int svcIdx = findServiceById(services, serviceCount, svcId);
        if (svcIdx < 0) {
            printf("  [!] Ma dich vu khong ton tai. Thu lai.\n");
            continue;
        }

        /* Kiem tra da them dich vu nay vao gio chua */
        int dupInCart = 0;
        for (int i = 0; i < cartCount; i++) {
            if (strcmp(tempCart[i].serviceId, svcId) == 0) {
                dupInCart = i + 1; /* luu vi tri +1 */
                break;
            }
        }

        int qty;
        printf("  So luong (%s): ", services[svcIdx].unit);
        char qtyStr[10];
        safeInput(qtyStr, sizeof(qtyStr));
        if (!isNumeric(qtyStr) || (qty = atoi(qtyStr)) <= 0) {
            printf("  [!] So luong khong hop le.\n");
            continue;
        }

        if (dupInCart > 0) {
            /* Cong don so luong neu cung dich vu */
            tempCart[dupInCart - 1].quantity += qty;
            printf("  Da cap nhat: %s x%d\n",
                   services[svcIdx].serviceName,
                   tempCart[dupInCart - 1].quantity);
        } else {
            /* Them moi vao gio */
            strncpy(tempCart[cartCount].bookingId, bkId, sizeof(tempCart[cartCount].bookingId) - 1);
            tempCart[cartCount].bookingId[sizeof(tempCart[cartCount].bookingId) - 1] = '\0';
            strncpy(tempCart[cartCount].serviceId, svcId, sizeof(tempCart[cartCount].serviceId) - 1);
            tempCart[cartCount].serviceId[sizeof(tempCart[cartCount].serviceId) - 1] = '\0';
            tempCart[cartCount].quantity = qty;
            tempCart[cartCount].status   = SERVICE_ACTIVE;
            cartCount++;
            printf("  Da them: %s x%d\n", services[svcIdx].serviceName, qty);
        }
    }

    if (cartCount == 0) {
        printf("  Khong co dich vu nao duoc chon.\n");
        pauseScreen();
        return;
    }

    /* --- Hien man hinh xac nhan --- */
    printUsedServiceTitle("XAC NHAN GOI DICH VU");
    printf("  | %-8s | %-22s | %5s | %12s |\n",
           "Ma DV", "Ten DV", "SL", "Thanh tien");
    printf(USED_SERVICE_CONFIRM_DIV);

    float cartTotal = 0;
    for (int i = 0; i < cartCount; i++) {
        int svcIdx = findServiceById(services, serviceCount, tempCart[i].serviceId);
        if (svcIdx < 0) continue;
        float subtotal = services[svcIdx].price * tempCart[i].quantity;
        cartTotal += subtotal;
        printf("  | %-8.8s | %-22.22s | %5d | %12.0f |\n",
               tempCart[i].serviceId,
               services[svcIdx].serviceName,
               tempCart[i].quantity,
               subtotal);
    }
    {
        printf(USED_SERVICE_CONFIRM_FULL_DIV);
        printf("  | %-32s %19.0f VND |\n", "Tam tinh dich vu:", cartTotal);
        printf(USED_SERVICE_CONFIRM_FULL_DIV);
    }

    printf("\n  Xac nhan luu? (y/n): ");
    char confirm[5];
    safeInput(confirm, sizeof(confirm));

    if (confirm[0] != 'Y' && confirm[0] != 'y') {
        printf("  Da huy, khong luu.\n");
        pauseScreen();
        return;
    }

    int newItemCount = 0;
    for (int i = 0; i < cartCount; i++) {
        if (findActiveUsedService(usedServices, *usedCount,
                                  tempCart[i].bookingId,
                                  tempCart[i].serviceId) < 0) {
            newItemCount++;
        }
    }

    /* Kiem tra du cho trong mang usedServices */
    if (*usedCount + newItemCount > MAX_USED_SERVICES) {
        printf("  [!] Bo nho day, khong the luu them dich vu.\n");
        pauseScreen();
        return;
    }

    /* --- Luu vao mang usedServices --- */
    for (int i = 0; i < cartCount; i++) {
        int oldIdx = findActiveUsedService(usedServices, *usedCount,
                                           tempCart[i].bookingId,
                                           tempCart[i].serviceId);
        if (oldIdx >= 0) {
            usedServices[oldIdx].quantity += tempCart[i].quantity;
        } else {
            usedServices[*usedCount] = tempCart[i];
            (*usedCount)++;
        }
    }
    mergeActiveUsedServices(usedServices, usedCount);

    saveUsedServices(usedServices, *usedCount);
    printf("\n  [OK] Da luu %d dich vu thanh cong!\n", cartCount);
    pauseScreen();
}

/* ============================================================
   XOA DICH VU DA GOI NHAM
   ============================================================ */
void deleteUsedService(UsedService usedServices[], int *usedCount,
                       Booking bookings[], int bookingCount,
                       Customer customers[], int customerCount,
                       Service services[], int serviceCount) {
    clearScreen();
    printUsedServiceTitle("XOA DICH VU DA GOI");

    char bkId[10];
    printf("  Nhap ma booking (VD: BK001): ");
    safeInput(bkId, sizeof(bkId));
    trimStr(bkId);

    int bkIdx = findBookingById(bookings, bookingCount, bkId);
    if (bkIdx < 0) {
        printf("  [!] Khong tim thay booking %s\n", bkId);
        pauseScreen();
        return;
    }

    if (bookings[bkIdx].status != BOOKING_CHECKIN) {
        printf("  [!] Chi xoa dich vu khi booking dang o.\n");
        pauseScreen();
        return;
    }

    int cuIdx = findCustomerById(customers, customerCount, bookings[bkIdx].customerId);
    printf("\n  Booking: %s\n", bookings[bkIdx].bookingId);
    if (cuIdx >= 0) printf("  Khach  : %s (%s)\n", customers[cuIdx].name, customers[cuIdx].id);

    printf("\n  Dich vu da goi:\n");
    if (listActiveUsedServicesByBooking(usedServices, *usedCount,
                                        services, serviceCount, bkId) == 0) {
        printf("  Booking nay chua goi dich vu nao.\n");
        pauseScreen();
        return;
    }

    char svcId[10];
    printf("\n  Nhap ma dich vu can xoa: ");
    safeInput(svcId, sizeof(svcId));
    trimStr(svcId);

    int usedIdx = findActiveUsedService(usedServices, *usedCount, bkId, svcId);
    if (usedIdx < 0) {
        printf("  [!] Khong tim thay dich vu %s trong booking %s.\n", svcId, bkId);
        pauseScreen();
        return;
    }

    int svcIdx = findServiceById(services, serviceCount, svcId);
    if (svcIdx >= 0) {
        printf("  Se xoa: %s x%d\n",
               services[svcIdx].serviceName,
               usedServices[usedIdx].quantity);
    }

    printf("  Xac nhan xoa dich vu %s? (y/n): ", svcId);
    char confirm[5];
    safeInput(confirm, sizeof(confirm));
    if (confirm[0] != 'y' && confirm[0] != 'Y') {
        printf("  Da huy thao tac.\n");
        pauseScreen();
        return;
    }

    for (int i = usedIdx; i < *usedCount - 1; i++) {
        usedServices[i] = usedServices[i + 1];
    }
    (*usedCount)--;
    saveUsedServices(usedServices, *usedCount);

    printf("  [OK] Da xoa dich vu %s khoi booking %s.\n", svcId, bkId);
    pauseScreen();
}

/* ============================================================
   TINH TONG TIEN DICH VU CHO MOT BOOKING
   ============================================================ */
float calcServiceCost(UsedService usedServices[], int usedCount,
                      Service services[], int serviceCount,
                      const char *bookingId) {
    float total = 0;
    for (int i = 0; i < usedCount; i++) {
        if (strcmp(usedServices[i].bookingId, bookingId) != 0) continue;
        if (usedServices[i].status != SERVICE_ACTIVE) continue;

        int svcIdx = findServiceById(services, serviceCount, usedServices[i].serviceId);
        if (svcIdx >= 0) {
            total += services[svcIdx].price * usedServices[i].quantity;
        }
    }
    return total;
}

/* ============================================================
   LUU DICH VU DA GOI RA FILE
   ============================================================ */
void saveUsedServices(UsedService usedServices[], int count) {
    FILE *fp = fopen("data/used_services.txt", "w");
    if (!fp) {
        printf("  [!] Khong the mo file de luu dich vu.\n");
        return;
    }
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%s,%s,%d,%d\n",
                usedServices[i].bookingId,
                usedServices[i].serviceId,
                usedServices[i].quantity,
                usedServices[i].status);
    }
    fclose(fp);
}

/* ============================================================
   DOC DICH VU DA GOI TU FILE
   ============================================================ */
int loadUsedServices(UsedService usedServices[]) {
    FILE *fp = fopen("data/used_services.txt", "r");
    if (!fp) return 0; 

    int count = 0;
    char line[256];

    while (fgets(line, sizeof(line), fp) && count < MAX_USED_SERVICES) {
        trimStr(line);
        if (strlen(line) == 0) continue;

        UsedService *us = &usedServices[count];
        int parsed = sscanf(line, "%9[^,],%9[^,],%d,%d",
                            us->bookingId,
                            us->serviceId,
                            &us->quantity,
                            &us->status);

        if (parsed == 4) count++;
    }

    fclose(fp);
    return count;
}

/* ============================================================
   MENU QUAN LY DICH VU
   ============================================================ */
void menuService(UsedService usedServices[], int *usedCount,
                 Booking bookings[], int bookingCount,
                 Room rooms[], int roomCount,
                 Customer customers[], int customerCount,
                 Service services[], int serviceCount) {

    int choice;
    do {
        /* Luon tai lai danh muc dich vu tu file de dong bo thay doi moi nhat */
        serviceCount = loadServices(services);

        clearScreen();
        printHeader("QUAN LY DICH VU");
        printf("| 1. Goi dich vu cho phong                               |\n");
        printf("| 2. Xem danh sach dich vu co san                        |\n");
        printf("| 3. Xoa dich vu da goi                                  |\n");
        printf("| 0. Quay lai menu chinh                                 |\n");
        printLine(58);

        choice = inputInt("  Chon: ", 0, 3);

        switch (choice) {
            case 1:
                addUsedService(usedServices, usedCount,
                               bookings, bookingCount,
                               rooms, roomCount,
                               customers, customerCount,
                               services, serviceCount);
                break;
            case 2:
                clearScreen();
                printServiceTableHeader("DANH SACH DICH VU");
                listServices(services, serviceCount);
                pauseScreen();
                break;
            case 3:
                deleteUsedService(usedServices, usedCount,
                                  bookings, bookingCount,
                                  customers, customerCount,
                                  services, serviceCount);
                break;
            default:
                break;
        }
    } while (choice != 0);
}
