#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "customer.h"
#include "booking.h"
#include "utils.h"

/* =============================================
   CUSTOMER.C - Quan ly khach hang
   ============================================= */

#define CUSTOMER_FILE "data/customers.txt"

/* ============================
   TIEN ICH HANG THANH VIEN
   ============================ */
const char* rankStr(int rank) {
    switch (rank) {
        case RANK_BRONZE: return "Bronze";
        case RANK_SILVER: return "Silver";
        case RANK_GOLD:   return "Gold";
        default:          return "Bronze";
    }
}

float rankDiscount(int rank) {
    switch (rank) {
        case RANK_SILVER: return 0.05f;
        case RANK_GOLD:   return 0.15f;
        default:          return 0.0f;
    }
}

static void printTableBorder() {
    printf("  +--------+----------------------+---------------+------------------+--------+----------+\n");
}

static void printCustomerTableHeader() {
    printTableBorder();
    printf("  | %-6s | %-20s | %-13s | %-16s | %-6s | %-6s |\n",
           "Ma KH", "Ho ten", "Dien thoai", "CCCD", "Hang", "Giam gia");
    printTableBorder();
}

static void printCustomerTableFooter() {
    printTableBorder();
}

static void printCustomerRow(Customer *c) {
    float disc = rankDiscount(c->rank) * 100;
    char discStr[10];
    if (disc > 0) sprintf(discStr, "%.0f%%", disc);
    else          sprintf(discStr, "-");
    printf("  | %-6s | %-20.20s | %-13.13s | %-16.16s | %-6.6s | %-8.8s |\n",
           c->id, c->name,
           c->phone, c->cccd, rankStr(c->rank), discStr);
}

void printCustomer(Customer *c) {
    printCustomerTableHeader();
    printCustomerRow(c);
    printCustomerTableFooter();
}

/* ============================
   TIM KHACH HANG
   ============================ */
int findCustomerById(Customer customers[], int count, const char *id) {
    for (int i = 0; i < count; i++)
        if (strcmp(customers[i].id, id) == 0) return i;
    return -1;
}

int findCustomerByPhone(Customer customers[], int count, const char *phone) {
    for (int i = 0; i < count; i++)
        if (strcmp(customers[i].phone, phone) == 0) return i;
    return -1;
}

/* Tao ma khach hang tu dong. */
static void generateCustomerId(Customer customers[], int count, char *id) {
    int maxNum = 0;
    for (int i = 0; i < count; i++) {
        int num = 0;
        sscanf(customers[i].id, "KH%d", &num);
        if (num > maxNum) maxNum = num;
    }
    sprintf(id, "KH%03d", maxNum + 1);
}

/* ============================
   THEM KHACH HANG
   ============================ */
void addCustomer(Customer customers[], int *count) {
    if (*count >= MAX_CUSTOMERS) {
        printf("  [!] Da dat toi da so khach hang!\n");
        pauseScreen();
        return;
    }

    Customer c;
    clearScreen();
    printHeader("THEM KHACH HANG MOI");

    generateCustomerId(customers, *count, c.id);
    printf("\n  Ma khach hang: %s (tu dong)\n\n", c.id);

    do {
        printf("  Ho ten: ");
        safeInput(c.name, sizeof(c.name));
        trimStr(c.name);
        if (!isNotEmpty(c.name))
            printf("  [!] Ho ten khong duoc de trong!\n");
    } while (!isNotEmpty(c.name));

    do {
        printf("  So dien thoai: ");
        safeInput(c.phone, sizeof(c.phone));
        trimStr(c.phone);
        if (!isNotEmpty(c.phone)) {
            printf("  [!] Khong duoc de trong!\n"); continue;
        }
        if (!isNumeric(c.phone)) {
            printf("  [!] Chi chua chu so!\n"); c.phone[0] = '\0'; continue;
        }
        if (findCustomerByPhone(customers, *count, c.phone) != -1) {
            printf("  [!] So dien thoai da ton tai!\n"); c.phone[0] = '\0';
        }
    } while (!isNotEmpty(c.phone));

    do {
        printf("  So CCCD: ");
        safeInput(c.cccd, sizeof(c.cccd));
        trimStr(c.cccd);
        if (!isNotEmpty(c.cccd)) {
            printf("  [!] Khong duoc de trong!\n"); continue;
        }
        if (!isNumeric(c.cccd)) {
            printf("  [!] Chi chua chu so!\n"); c.cccd[0] = '\0';
        }
    } while (!isNotEmpty(c.cccd));

    printf("\n  Hang thanh vien:\n");
    printf("    0. Bronze (mac dinh - khong giam gia)\n");
    printf("    1. Silver (giam 5%%)\n");
    printf("    2. Gold   (giam 15%%)\n");
    c.rank = inputInt("  Chon hang (0-2): ", 0, 2);

    customers[*count] = c;
    (*count)++;
    saveCustomers(customers, *count);

    printf("\n  [OK] Da them: %s - %s - Hang %s\n",
           c.id, c.name, rankStr(c.rank));
    pauseScreen();
}


/* ============================
   XOA KHACH HANG
   ============================ */
void deleteCustomer(Customer customers[], int *count,
                    Booking bookings[], int bookingCount) {
    clearScreen();
    printHeader("XOA KHACH HANG");

    char id[15];
    printf("\n  Nhap ma khach hang can xoa (vi du: KH001): ");
    safeInput(id, sizeof(id));
    trimStr(id);

    int idx = findCustomerById(customers, *count, id);
    if (idx == -1) {
        printf("  [!] Khong tim thay khach hang '%s'!\n", id);
        pauseScreen();
        return;
    }

    printf("\n  Khach hang se bi xoa:\n");
    printCustomer(&customers[idx]);

    for (int i = 0; i < bookingCount; i++) {
        if (strcmp(bookings[i].customerId, id) == 0) {
            if (bookings[i].status == BOOKING_CHECKIN) {
                printf("  [!] Khach hang dang check-in, khong the xoa.\n");
            } else {
                printf("  [!] Khach hang da co booking lich su, khong the xoa vi se mat lien ket hoa don.\n");
            }
            pauseScreen();
            return;
        }
    }

    printf("  Xac nhan xoa? (y/n): ");
    char confirm[5];
    safeInput(confirm, sizeof(confirm));
    if (confirm[0] != 'y' && confirm[0] != 'Y') {
        printf("  Da huy thao tac.\n");
        pauseScreen();
        return;
    }

    for (int i = idx; i < *count - 1; i++) {
        customers[i] = customers[i + 1];
    }
    (*count)--;
    saveCustomers(customers, *count);

    printf("  [OK] Da xoa khach hang %s khoi he thong!\n", id);
    pauseScreen();
}

/* ============================
   XEM DANH SACH KHACH HANG
   ============================ */
void listCustomers(Customer customers[], int count) {
    clearScreen();
    printf("  +--------------------------------------------------------------------------------------+\n");
    printf("  |                               DANH SACH KHACH HANG                                   |\n");
    printf("  +--------------------------------------------------------------------------------------+\n");

    if (count == 0) {
        printf("  (Chua co khach hang nao)\n");
        pauseScreen(); return;
    }

    printCustomerTableHeader();

    int bronze = 0, silver = 0, gold = 0;
    for (int i = 0; i < count; i++) {
        if      (customers[i].rank == RANK_BRONZE) bronze++;
        else if (customers[i].rank == RANK_SILVER) silver++;
        else                                       gold++;
    }

    for (int i = 0; i < count; i++)
        printCustomerRow(&customers[i]);

    printCustomerTableFooter();
    printf("  Tong: %d  |  Bronze: %d  |  Silver: %d  |  Gold: %d\n",
           count, bronze, silver, gold);
    pauseScreen();
}

/* ============================
   LUU VAO FILE
   ============================ */
void saveCustomers(Customer customers[], int count) {
    FILE *fp = fopen(CUSTOMER_FILE, "w");
    if (!fp) { 
        printf("  [!] Loi cap nhat tep tin du lieu khach hang!\n"); 
        return; 
    }
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%s,%s,%s,%s,%d\n",
                customers[i].id, customers[i].name,
                customers[i].phone, customers[i].cccd,
                customers[i].rank);
    }
    fclose(fp);
}

/* Doc khach hang tu file. */
int loadCustomers(Customer customers[]) {
    FILE *fp = fopen(CUSTOMER_FILE, "r");
    if (!fp) return 0;
    int count = 0;
    while (count < MAX_CUSTOMERS) {
        if (feof(fp)) break; 
        
        int r = fscanf(fp, "%9[^,],%49[^,],%14[^,],%19[^,],%d\n",
                       customers[count].id,
                       customers[count].name,
                       customers[count].phone,
                       customers[count].cccd,
                       &customers[count].rank);
        if (r == 5) {
            count++;
        } else if (r == 4) {
            customers[count].rank = RANK_BRONZE;
            count++;
        } else {
            break; 
        }
    }
    fclose(fp);
    return count;
}

/* ============================
   MENU QUAN LY KHACH HANG
   ============================ */
void menuCustomer(Customer customers[], int *count,
                  Booking bookings[], int bookingCount) {
    int choice;
    do {
        clearScreen();
        printHeader("QUAN LY KHACH HANG");
        printf("| 1. Xem danh sach khach hang                            |\n");
        printf("| 2. Them khach hang moi                                 |\n");
        printf("| 3. Xoa khach hang                                      |\n");
        printf("| 0. Quay lai menu chinh                                 |\n");
        printLine(58);

        choice = inputInt("  Chon chuc nang: ", 0, 3);

        switch (choice) {
            case 1: listCustomers(customers, *count);     break;
            case 2: addCustomer(customers, count);        break;
            case 3: deleteCustomer(customers, count,
                                    bookings, bookingCount); break;
            case 0: break;
        }
    } while (choice != 0);
}
