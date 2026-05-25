#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "customer.h"
#include "utils.h"

/* =============================================
   CUSTOMER.C - Fix lỗi lệch bảng, trùng lặp & lưu file
   ============================================= */

#define CUSTOMER_FILE "data/customers.txt"

/* ============================
   HÀM TIỆN ÍCH HẠNG THÀNH VIÊN
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

/* =========================================================================
   HÀM PHỤ TRỢ: TỰ ĐỘNG BÙ KHOẢNG TRẮNG CHỐNG LỆCH TIẾNG VIỆT CÓ DẤU
   ========================================================================= */
static int countUtf8Characters(const char *str) {
    int count = 0;
    while (*str) {
        if ((*str & 0xC0) != 0x80) {
            count++;
        }
        str++;
    }
    return count;
}

static void printCellWithPadding(const char *str, int width) {
    int visibleChars = countUtf8Characters(str);
    int paddingNeeded = width - visibleChars;
    printf("%s", str);
    for (int i = 0; i < paddingNeeded; i++) {
        printf(" ");
    }
}


static void printTableBorder() {
    
    printf("  +--------+----------------------+---------------+------------------+--------+----------+\n");
}


static void printFunctionBanner(const char *title) {
    printf("  +--------------------------------------------------------------------------------------+\n");
    if (title != NULL) {
        int titleLen = strlen(title);
        int pad = (86 - titleLen) / 2;
        printf("  |");
        for (int i = 0; i < pad; i++) printf(" ");
        printf("%s", title);
        for (int i = 0; i < 86 - pad - titleLen; i++) printf(" ");
        printf("|\n");
    }
    printf("  +--------------------------------------------------------------------------------------+\n");
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
    
    // Cột Ma KH: %-6s, Họ tên: dùng hàm padding với độ rộng cố định 20, các cột sau khớp định dạng header
    printf("  | %-6s | ", c->id);
    printCellWithPadding(c->name, 20);
    printf(" | %-13s | %-16s | %-6s | %-6s   |\n",
           c->phone, c->cccd, rankStr(c->rank), discStr);
}

void printCustomer(Customer *c) {
    printCustomerTableHeader();
    printCustomerRow(c);
    printCustomerTableFooter();
}

/* ============================
   TÌM KHÁCH HÀNG
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

/* ============================
   TẠO MÃ KHÁCH HÀNG TỰ ĐỘNG
   ============================ */
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
   THÊM KHÁCH HÀNG
   ============================ */
void addCustomer(Customer customers[], int *count) {
    if (*count >= MAX_CUSTOMERS) {
        printf("  [!] Da dat toi da so khach hang!\n");
        pauseScreen();
        return;
    }

    Customer c;
    clearScreen();
    printFunctionBanner("THEM KHACH HANG MOI");

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
   SỬA KHÁCH HÀNG
   ============================ */
void editCustomer(Customer customers[], int count) {
    if (count == 0) {
        printf("  [!] Chua co khach hang nao!\n");
        pauseScreen(); return;
    }

    clearScreen();
    printFunctionBanner("SUA THONG TIN KHACH HANG");

    char id[10];
    printf("\n  Nhap ma khach hang: ");
    safeInput(id, sizeof(id));
    trimStr(id);

    int idx = findCustomerById(customers, count, id);
    if (idx == -1) {
        printf("  [!] Khong tim thay '%s'!\n", id);
        pauseScreen(); return;
    }

    printf("\n  Thong tin hien tai:\n");
    printCustomer(&customers[idx]);
    printf("\n  --- Nhap thong tin moi (Enter = giu nguyen) ---\n\n");

    char buf[50];

    printf("  Ho ten [%s]: ", customers[idx].name);
    safeInput(buf, sizeof(buf)); trimStr(buf);
    if (isNotEmpty(buf)) strcpy(customers[idx].name, buf);

    printf("  So dien thoai [%s]: ", customers[idx].phone);
    safeInput(buf, sizeof(buf)); trimStr(buf);
    if (isNotEmpty(buf)) {
        if (!isNumeric(buf))
            printf("  [!] Khong hop le, giu nguyen.\n");
        else if (findCustomerByPhone(customers, count, buf) != -1 &&
                 strcmp(buf, customers[idx].phone) != 0)
            printf("  [!] Da ton tai, giu nguyen.\n");
        else
            strcpy(customers[idx].phone, buf);
    }

    printf("  So CCCD [%s]: ", customers[idx].cccd);
    safeInput(buf, sizeof(buf)); trimStr(buf);
    if (isNotEmpty(buf)) {
        if (!isNumeric(buf)) printf("  [!] Khong hop le, giu nguyen.\n");
        else strcpy(customers[idx].cccd, buf);
    }

    printf("  Hang hien tai: %s\n", rankStr(customers[idx].rank));
    printf("    0. Bronze  1. Silver (5%%)  2. Gold (15%%)  9. Giu nguyen\n");
    int rankChoice = inputInt("  Chon: ", 0, 9);
    if (rankChoice >= 0 && rankChoice <= 2)
        customers[idx].rank = rankChoice;

    saveCustomers(customers, count);
    printf("\n  [OK] Da cap nhat khach hang %s!\n", id);
    pauseScreen();
}

/* ============================
   XÓA KHÁCH HÀNG
   ============================ */
void deleteCustomer(Customer customers[], int count) {
    clearScreen();
    printFunctionBanner("XOA KHACH HANG");

    char id[15];
    printf("\n  Nhap ma khach hang can xoa (vi du: KH008): ");
    safeInput(id, sizeof(id)); trimStr(id);

    // --- BƯỚC 1: XÓA KHÁCH HÀNG TRONG FILE CUSTOMERS.TXT ---
    FILE *fCust = fopen("data/customers.txt", "r");
    if (fCust == NULL) {
        printf("  [!] Khong the mo file data/customers.txt!\n");
        pauseScreen(); return;
    }

    typedef struct {
        char maKH[20], hoTen[50], dienThoai[20], cccd[20];
        int hang; // Hạng thành viên dạng số của bạn (1: Silver, 2: Gold...)
    } CustTemp;

    CustTemp cList[500];
    int cCount = 0;
    char line[256];
    int foundCustomer = 0;

    // Đọc chính xác theo định dạng lưu file khách hàng của bạn
    while (fgets(line, sizeof(line), fCust)) {
        if (sscanf(line, "%[^,],%[^,],%[^,],%[^,],%d", 
                   cList[cCount].maKH, cList[cCount].hoTen, cList[cCount].dienThoai, 
                   cList[cCount].cccd, &cList[cCount].hang) == 5) {
            
            if (strcmp(cList[cCount].maKH, id) == 0) {
                foundCustomer = 1; 
                continue; // Bỏ qua không nạp vào mảng => Xóa
            }
            cCount++;
        }
    }
    fclose(fCust);

    if (!foundCustomer) {
        printf("  [!] Khong tim thay khach hang '%s' hoac ma nay khong ton tai trong file txt!\n", id);
        pauseScreen(); return;
    }

    // Ghi đè lại file khách hàng sạch dữ liệu lỗi
    FILE *fCustWrite = fopen("data/customers.txt", "w");
    if (fCustWrite != NULL) {
        for (int i = 0; i < cCount; i++) {
            fprintf(fCustWrite, "%s,%s,%s,%s,%d\n", 
                    cList[i].maKH, cList[i].hoTen, cList[i].dienThoai, cList[i].cccd, cList[i].hang);
        }
        fclose(fCustWrite);
    }

    // --- BƯỚC 2: CẬP NHẬT FILE BOOKING THEO MÃ SỐ (ĐỔI THÀNH TRẠNG THÁI 3) ---
    FILE *fBook = fopen("data/bookings.txt", "r");
    if (fBook != NULL) {
        typedef struct {
            char maBK[15], maKH[15], maPhong[15], checkIn[15], checkOut[15];
            int trangThaiNum; // Lưu trạng thái dạng số (0: Da dat, 1: Dang o, 3: Da huy)
        } BookTemp;

        BookTemp bList[1000];
        int bCount = 0;

        while (fgets(line, sizeof(line), fBook)) {
            // Đọc trạng thái cuối cùng dưới dạng số %d
            if (sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^,],%d", 
                       bList[bCount].maBK, bList[bCount].maKH, bList[bCount].maPhong, 
                       bList[bCount].checkIn, bList[bCount].checkOut, &bList[bCount].trangThaiNum) == 6) {
                
                // Nếu booking thuộc về khách vừa xóa, chuyển số trạng thái thành 3 (Hủy)
                if (strcmp(bList[bCount].maKH, id) == 0) {
                    bList[bCount].trangThaiNum = 3; 
                }
                bCount++;
            }
        }
        fclose(fBook);

        // Ghi lại file bookings.txt chuẩn hóa số 100%
        FILE *fBookWrite = fopen("data/bookings.txt", "w");
        if (fBookWrite != NULL) {
            for (int i = 0; i < bCount; i++) {
                fprintf(fBookWrite, "%s,%s,%s,%s,%s,%d\n", 
                        bList[i].maBK, bList[i].maKH, bList[i].maPhong, 
                        bList[i].checkIn, bList[i].checkOut, bList[i].trangThaiNum);
            }
            fclose(fBookWrite);
            printf("  [-] Cac phong dat lien quan den ma %s da duoc tu dong HUY (Ma so 3)!\n", id);
        }
    }

    printf("  [OK] Da xoa hoan toan khach hang %s khoi he thong!\n", id);
    pauseScreen();
}

/* ============================
   XEM DANH SÁCH KHÁCH HÀNG
   ============================ */
void listCustomers(Customer customers[], int count) {
    clearScreen();
    printFunctionBanner("DANH SACH KHACH HANG");

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
   TÌM KIẾM KHÁCH HÀNG
   ============================ */
void searchCustomer(Customer customers[], int count) {
    clearScreen();
    printFunctionBanner("TIM KIEM KHACH HANG");

    if (count == 0) {
        printf("  (Chua co khach hang nao)\n");
        pauseScreen(); return;
    }

    printf("  Tim theo:\n");
    printf("    1. Ma khach hang\n");
    printf("    2. Ho ten\n");
    printf("    3. So dien thoai\n");
    printf("    4. Hang thanh vien\n");
    int choice = inputInt("  Chon (1-4): ", 1, 4);

    int found = 0;
    printf("\n  Ket qua:\n");

    if (choice == 1) {
        char id[10];
        printf("  Nhap ma KH: ");
        safeInput(id, sizeof(id)); trimStr(id);
        printCustomerTableHeader();
        int idx = findCustomerById(customers, count, id);
        if (idx != -1) { printCustomerRow(&customers[idx]); found = 1; }

    } else if (choice == 2) {
        char name[50];
        printf("  Nhap ho ten: ");
        safeInput(name, sizeof(name)); trimStr(name);
        printCustomerTableHeader();
        for (int i = 0; i < count; i++) {
            char tmpC[50], tmpN[50];
            strcpy(tmpC, customers[i].name);
            strcpy(tmpN, name);
            toLowerStr(tmpC); toLowerStr(tmpN);
            if (strstr(tmpC, tmpN)) { printCustomerRow(&customers[i]); found++; }
        }

    } else if (choice == 3) {
        char phone[15];
        printf("  Nhap so dien thoai: ");
        safeInput(phone, sizeof(phone)); trimStr(phone);
        printCustomerTableHeader();
        int idx = findCustomerByPhone(customers, count, phone);
        if (idx != -1) { printCustomerRow(&customers[idx]); found = 1; }

    } else if (choice == 4) {
        printf("  Hang:  0. Bronze  1. Silver  2. Gold\n");
        int rank = inputInt("  Chon (0-2): ", 0, 2);
        printCustomerTableHeader();
        for (int i = 0; i < count; i++)
            if (customers[i].rank == rank) { printCustomerRow(&customers[i]); found++; }
    }

    printCustomerTableFooter();
    if (found == 0) printf("  Khong tim thay ket qua.\n");
    else            printf("  Tim thay %d khach hang.\n", found);
    pauseScreen();
}

/* ============================
   NÂNG/HẠ HẠNG THÀNH VIÊN
   ============================ */
void updateCustomerRank(Customer customers[], int count) {
    clearScreen();
    printFunctionBanner("CAP NHAT HANG THANH VIEN");

    char id[10];
    printf("\n  Nhap ma khach hang: ");
    safeInput(id, sizeof(id)); trimStr(id);

    int idx = findCustomerById(customers, count, id);
    if (idx == -1) {
        printf("  [!] Khong tim thay '%s'!\n", id);
        pauseScreen(); return;
    }

    printf("\n  Khach hang: %s - %s\n",
           customers[idx].id, customers[idx].name);
    printf("  Hang hien tai: %s", rankStr(customers[idx].rank));
    if (rankDiscount(customers[idx].rank) > 0)
        printf(" (giam %.0f%%)", rankDiscount(customers[idx].rank) * 100);
    printf("\n\n");

    printf("  Chon hang moi:\n");
    printf("    0. Bronze  - Khong giam gia\n");
    printf("    1. Silver  - Giam 5%%\n");
    printf("    2. Gold    - Giam 15%%\n");

    int newRank = inputInt("  Chon (0-2): ", 0, 2);
    if (newRank == customers[idx].rank) {
        printf("  Hang khong thay doi.\n");
    } else {
        customers[idx].rank = newRank;
        saveCustomers(customers, count);
        printf("  [OK] Da nang hang %s -> %s!\n", id, rankStr(newRank));
    }
    pauseScreen();
}

/* ============================
   LƯU VÀO FILE
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

/* ============================
   ĐỌC TỪ FILE (Đã xử lý chống lặp vô hạn bản ghi cuối)
   ============================ */
int loadCustomers(Customer customers[]) {
    FILE *fp = fopen(CUSTOMER_FILE, "r");
    if (!fp) return 0;
    int count = 0;
    while (count < MAX_CUSTOMERS) {
        // Kiểm tra EOF nghiêm ngặt để chặn đứng việc đọc lặp lại dòng cuối cùng
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
   MENU QUẢN LÝ KHÁCH HÀNG
   ============================ */
void menuCustomer(Customer customers[], int *count) {
    int choice;
    do {
        clearScreen();
        printHeader("QUAN LY KHACH HANG");
        printf("| 1. Xem danh sach khach hang                            |\n");
        printf("| 2. Them khach hang moi                                 |\n");
        printf("| 3. Sua thong tin khach hang                            |\n");
        printf("| 4. Xoa khach hang                                      |\n");
        printf("| 5. Tim kiem khach hang                                 |\n");
        printf("| 6. Cap nhat hang thanh vien                            |\n");
        printf("| 0. Quay lai menu chinh                                 |\n");
        printLine(58);

        choice = inputInt("  Chon chuc nang: ", 0, 6);

        switch (choice) {
            case 1: listCustomers(customers, *count);     break;
            case 2: addCustomer(customers, count);        break;
            case 3: editCustomer(customers, *count);      break;
            case 4: deleteCustomer(customers, *count);    break;
            case 5: searchCustomer(customers, *count);    break;
            case 6: updateCustomerRank(customers, *count); break;
            case 0: break;
        }
    } while (choice != 0);
}