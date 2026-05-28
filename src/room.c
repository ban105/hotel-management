#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "room.h"
#include "booking.h"
#include "utils.h"

/* =============================================
   ROOM.C - Quan ly phong khach san
   ============================================= */

#define ROOM_FILE "data/rooms.txt"

static int equalsIgnoreCase(const char *a, const char *b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) {
            return 0;
        }
        a++;
        b++;
    }
    return *a == '\0' && *b == '\0';
}

/* ============================
   HAM HIEN THI NOI BO
   ============================ */

/* Chuyen status thanh chuoi */
static const char* statusStr(int status) {
    switch (status) {
        case ROOM_EMPTY:    return "Trong";
        case ROOM_OCCUPIED: return "Dang thue";
        case ROOM_MAINTAIN: return "Bao tri";
        default:            return "Khong ro";
    }
}

/* In trang thai phong (dung cho cac module khac) */
void printRoomStatus(int status) {
    printf("%s", statusStr(status));
}

/* In vien bang */
static void printTableBorder() {
    printf("  +-----+----------+----------+---------------+------------+\n");
}

/* In header bang */
static void printRoomTableHeader(const char *title) {
    printf("  +--------------------------------------------------------+\n");
    if (title != NULL) {
        int titleLen = strlen(title);
        int pad = (56 - titleLen) / 2;
        printf("  |");
        for (int i = 0; i < pad; i++) printf(" ");
        printf("%s", title);
        for (int i = 0; i < 56 - pad - titleLen; i++) printf(" ");
        printf("|\n");
    }
    printf("  +-----+----------+----------+---------------+------------+\n");
    printf("  | %-3s | %-8s | %-8s | %13s | %-10s |\n",
           "STT", "Ma phong", "Loai", "Gia/dem (d)", "Trang thai");
    printf("  +-----+----------+----------+---------------+------------+\n");
}

/* In footer bang */
static void printRoomTableFooter() {
    printTableBorder();
}

/* In 1 dong phong trong bang */
static void printRoomRow(int stt, Room *r) {
    printf("  | %-3d | %-8.8s | %-8.8s | %13.0f | %-10.10s |\n",
           stt, r->id, r->type, r->price, statusStr(r->status));
}

/* In thong tin phong don le */
void printRoom(Room *r) {
    printRoomTableHeader("DANH SACH PHONG");
    printRoomRow(1, r);
    printRoomTableFooter();
}

/* ============================
   TIM PHONG THEO ID
   Tra ve index, hoac -1 neu khong tim thay
   ============================ */
int findRoomById(Room rooms[], int count, const char *id) {
    for (int i = 0; i < count; i++) {
        if (strcmp(rooms[i].id, id) == 0) return i;
    }
    return -1;
}

/* ============================
   KIEM TRA PHONG CO TRONG KHONG
   ============================ */
int isRoomAvailable(Room rooms[], int count, const char *id) {
    int idx = findRoomById(rooms, count, id);
    if (idx == -1) return 0;
    return rooms[idx].status == ROOM_EMPTY;
}

/* ============================
   THEM PHONG
   ============================ */
void addRoom(Room rooms[], int *count) {
    if (*count >= MAX_ROOMS) {
        printf("  [!] Da dat toi da so phong!\n");
        pauseScreen();
        return;
    }

    Room r;
    clearScreen();
    printHeader("THEM PHONG MOI");

    /* Nhap ma phong - khong duoc trung */
    do {
        printf("  Ma phong (VD: P101): ");
        safeInput(r.id, sizeof(r.id));
        trimStr(r.id);
        if (!isNotEmpty(r.id)) {
            printf("  [!] Ma phong khong duoc de trong!\n");
            r.id[0] = '\0';
            continue;
        }
        if (findRoomById(rooms, *count, r.id) != -1) {
            printf("  [!] Ma phong da ton tai! Nhap ma khac.\n");
            r.id[0] = '\0';
        }
    } while (!isNotEmpty(r.id));

    /* Chon loai phong */
    printf("\n  Loai phong:\n");
    printf("    1. Single  (1 giuong don)\n");
    printf("    2. Double  (1 giuong doi)\n");
    printf("    3. VIP     (cao cap)\n");
    int typeChoice = inputInt("  Chon (1-3): ", 1, 3);
    switch (typeChoice) {
        case 1: strcpy(r.type, "Single"); break;
        case 2: strcpy(r.type, "Double"); break;
        case 3: strcpy(r.type, "VIP");    break;
    }

    /* Nhap gia */
    printf("\n");
    r.price = inputFloat("  Gia/dem (dong): ", 1);

    /* Mac dinh: Trong */
    r.status = ROOM_EMPTY;

    rooms[*count] = r;
    (*count)++;
    saveRooms(rooms, *count);

    printf("\n  [OK] Da them phong %s (%s) - %.0f dong/dem\n",
           r.id, r.type, r.price);
    pauseScreen();
}

/* ============================
   SUA PHONG
   ============================ */
void editRoom(Room rooms[], int count) {
    if (count == 0) {
        printf("  [!] Chua co phong nao!\n");
        pauseScreen();
        return;
    }

    clearScreen();
    printHeader("SUA THONG TIN PHONG");

    char id[10];
    printf("  Nhap ma phong can sua: ");
    safeInput(id, sizeof(id));
    trimStr(id);

    int idx = findRoomById(rooms, count, id);
    if (idx == -1) {
        printf("  [!] Khong tim thay phong '%s'!\n", id);
        pauseScreen();
        return;
    }

    printf("\n  Thong tin hien tai:\n");
    printRoom(&rooms[idx]);

    printf("\n  --- Nhap thong tin moi ---\n");

    /* Sua loai phong */
    printf("  Loai phong hien tai: %s\n", rooms[idx].type);
    printf("    1. Single  2. Double  3. VIP  0. Giu nguyen\n");
    int typeChoice = inputInt("  Chon: ", 0, 3);
    if (typeChoice == 1) strcpy(rooms[idx].type, "Single");
    else if (typeChoice == 2) strcpy(rooms[idx].type, "Double");
    else if (typeChoice == 3) strcpy(rooms[idx].type, "VIP");

    /* Sua gia */
    printf("  Gia hien tai: %.0f dong\n", rooms[idx].price);
    float newPrice = inputFloat("  Gia moi (0 = giu nguyen): ", 0);
    if (newPrice > 0) rooms[idx].price = newPrice;

    saveRooms(rooms, count);
    printf("\n  [OK] Da cap nhat phong %s!\n", id);
    pauseScreen();
}

/* ============================
   XOA PHONG
   ============================ */
void deleteRoom(Room rooms[], int *count) {
    if (*count == 0) {
        printf("  [!] Chua co phong nao!\n");
        pauseScreen();
        return;
    }

    clearScreen();
    printHeader("XOA PHONG");

    char id[10];
    printf("  Nhap ma phong can xoa: ");
    safeInput(id, sizeof(id));
    trimStr(id);

    int idx = findRoomById(rooms, *count, id);
    if (idx == -1) {
        printf("  [!] Khong tim thay phong '%s'!\n", id);
        pauseScreen();
        return;
    }

    if (rooms[idx].status == ROOM_OCCUPIED) {
        printf("  [!] Phong dang co khach! Khong the xoa.\n");
        pauseScreen();
        return;
    }

    Booking bookings[MAX_BOOKINGS];
    int bookingCount = loadBookings(bookings);
    for (int i = 0; i < bookingCount; i++) {
        if (strcmp(bookings[i].roomId, id) == 0 &&
            (bookings[i].status == BOOKING_PENDING ||
             bookings[i].status == BOOKING_CHECKIN)) {
            printf("  [!] Phong dang co booking active! Khong the xoa.\n");
            pauseScreen();
            return;
        }
    }

    printf("\n  Phong se bi xoa:\n");
    printRoom(&rooms[idx]);

    printf("  Xac nhan xoa phong %s? (y/n): ", id);
    char confirm[5];
    safeInput(confirm, sizeof(confirm));
    if (confirm[0] != 'y' && confirm[0] != 'Y') {
        printf("  Da huy thao tac.\n");
        pauseScreen();
        return;
    }

    /* Doi cac phan tu sau len 1 vi tri */
    for (int i = idx; i < *count - 1; i++) {
        rooms[i] = rooms[i + 1];
    }
    (*count)--;
    saveRooms(rooms, *count);

    printf("  [OK] Da xoa phong %s!\n", id);
    pauseScreen();
}

/* ============================
   XEM DANH SACH PHONG
   ============================ */
void listRooms(Room rooms[], int count) {
    clearScreen();
    

    if (count == 0) {
        printf("  (Chua co phong nao)\n");
        pauseScreen();
        return;
    }

    /* Thong ke nhanh */
    int empty = 0, occupied = 0, maintain = 0;
    for (int i = 0; i < count; i++) {
        if      (rooms[i].status == ROOM_EMPTY)    empty++;
        else if (rooms[i].status == ROOM_OCCUPIED) occupied++;
        else                                        maintain++;
    }

    printRoomTableHeader("DANH SACH PHONG");
    for (int i = 0; i < count; i++) {
        printRoomRow(i + 1, &rooms[i]);
    }
    printRoomTableFooter();
    printf("  Tong: %d phong  |  Trong: %d  |  Dang thue: %d  |  Bao tri: %d\n",
           count, empty, occupied, maintain);
    pauseScreen();
}

/* ============================
   TIM KIEM PHONG
   ============================ */
void searchRoom(Room rooms[], int count) {
    clearScreen();
    printHeader("TIM KIEM PHONG");

    if (count == 0) {
        printf("  (Chua co phong nao)\n");
        pauseScreen();
        return;
    }

    printf("  Tim theo:\n");
    printf("    1. Ma phong\n");
    printf("    2. Loai phong\n");
    printf("    3. Trang thai\n");
    int choice = inputInt("  Chon (1-3): ", 1, 3);

    int found = 0;
    printf("\n  Ket qua tim kiem:\n");
    printRoomTableHeader("DANH SACH PHONG");

    if (choice == 1) {
        char id[10];
        printf("  Nhap ma phong: ");
        safeInput(id, sizeof(id));
        trimStr(id);
        int idx = findRoomById(rooms, count, id);
        if (idx != -1) {
            printRoomRow(1, &rooms[idx]);
            found = 1;
        }

    } else if (choice == 2) {
        char type[20];
        printf("  Nhap loai phong (Single/Double/VIP): ");
        safeInput(type, sizeof(type));
        trimStr(type);
        for (int i = 0; i < count; i++) {
            if (equalsIgnoreCase(rooms[i].type, type)) {
                printRoomRow(++found, &rooms[i]);
            }
        }

    } else if (choice == 3) {
        printf("  Trang thai:\n");
        printf("    0. Trong\n    1. Dang thue\n    2. Bao tri\n");
        int status = inputInt("  Chon (0-2): ", 0, 2);
        for (int i = 0; i < count; i++) {
            if (rooms[i].status == status) {
                printRoomRow(++found, &rooms[i]);
            }
        }
    }

    printRoomTableFooter();
    if (found == 0)
        printf("  Khong tim thay ket qua.\n");
    else
        printf("  Tim thay %d phong.\n", found);

    pauseScreen();
}

/* ============================
   CAP NHAT TRANG THAI PHONG
   ============================ */
void updateRoomStatus(Room rooms[], int count) {
    clearScreen();
    printHeader("CAP NHAT TRANG THAI PHONG");

    char id[10];
    printf("  Nhap ma phong: ");
    safeInput(id, sizeof(id));
    trimStr(id);

    int idx = findRoomById(rooms, count, id);
    if (idx == -1) {
        printf("  [!] Khong tim thay phong '%s'!\n", id);
        pauseScreen();
        return;
    }

    Booking bookings[MAX_BOOKINGS];
    int bookingCount = loadBookings(bookings);
    for (int i = 0; i < bookingCount; i++) {
        if (strcmp(bookings[i].roomId, id) == 0 &&
            (bookings[i].status == BOOKING_PENDING ||
             bookings[i].status == BOOKING_CHECKIN)) {
            printf("  [!] Phong %s dang co booking active, khong the doi trang thai thu cong.\n", id);
            printf("      Hay huy booking hoac check-out truoc khi cap nhat trang thai phong.\n");
            pauseScreen();
            return;
        }
    }

    printf("\n  Trang thai hien tai: [%s]\n", statusStr(rooms[idx].status));
    printf("  Chon trang thai moi:\n");
    printf("    0. Trong\n");
    printf("    2. Bao tri\n");

    int newStatus = inputInt("  Chon (0-2): ", 0, 2);
    if (newStatus == ROOM_OCCUPIED) {
        printf("  [!] Trang thai 'Dang thue' chi duoc cap nhat qua chuc nang check-in.\n");
        pauseScreen();
        return;
    }

    if (newStatus == rooms[idx].status) {
        printf("  Trang thai khong thay doi.\n");
    } else {
        rooms[idx].status = newStatus;
        saveRooms(rooms, count);
        printf("  [OK] Da cap nhat: %s -> %s\n", id, statusStr(newStatus));
    }
    pauseScreen();
}

/* ============================
   LUU PHONG VAO FILE
   ============================ */
void saveRooms(Room rooms[], int count) {
    FILE *fp = fopen(ROOM_FILE, "w");
    if (fp == NULL) {
        printf("  [!] Loi: Khong the ghi file %s\n", ROOM_FILE);
        return;
    }
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%s,%s,%.0f,%d\n",
                rooms[i].id, rooms[i].type,
                rooms[i].price, rooms[i].status);
    }
    fclose(fp);
}

/* ============================
   DOC PHONG TU FILE
   Tra ve so phong doc duoc
   ============================ */
int loadRooms(Room rooms[]) {
    FILE *fp = fopen(ROOM_FILE, "r");
    if (fp == NULL) return 0;

    int count = 0;
    while (count < MAX_ROOMS &&
           fscanf(fp, "%9[^,],%19[^,],%f,%d\n",
                  rooms[count].id,
                  rooms[count].type,
                  &rooms[count].price,
                  &rooms[count].status) == 4) {
        count++;
    }
    fclose(fp);
    return count;
}

/* ============================
   XEM DANH SACH PHONG TRONG
   ============================ */
void listAvailableRooms(Room rooms[], int count) {
    clearScreen();

    int found = 0;
    printRoomTableHeader("DANH SACH PHONG TRONG");
    for (int i = 0; i < count; i++) {
        if (rooms[i].status == ROOM_EMPTY) {
            printRoomRow(++found, &rooms[i]);
        }
    }
    printRoomTableFooter();

    if (found == 0)
        printf("  Hien tai khong co phong trong nao.\n");
    else
        printf("  Co %d phong trong san sang phuc vu.\n", found);

    pauseScreen();
}

/* ============================
   MENU QUAN LY PHONG
   ============================ */
void menuRoom(Room rooms[], int *count) {
    int choice;
    do {
        clearScreen();
        printHeader("QUAN LY PHONG");
        printf("| 1. Xem danh sach phong                                 |\n");
        printf("| 2. Xem phong trong                                     |\n");
        printf("| 3. Them phong moi                                      |\n");
        printf("| 4. Sua thong tin phong                                 |\n");
        printf("| 5. Xoa phong                                           |\n");
        printf("| 6. Tim kiem phong                                      |\n");
        printf("| 7. Cap nhat trang thai                                 |\n");
        printf("| 0. Quay lai menu chinh                                 |\n");
        printLine(58);

        choice = inputInt("  Chon chuc nang: ", 0, 7);

        switch (choice) {
            case 1: listRooms(rooms, *count);           break;
            case 2: listAvailableRooms(rooms, *count);  break;
            case 3: addRoom(rooms, count);               break;
            case 4: editRoom(rooms, *count);             break;
            case 5: deleteRoom(rooms, count);            break;
            case 6: searchRoom(rooms, *count);           break;
            case 7: updateRoomStatus(rooms, *count);     break;
            case 0: break;
        }
    } while (choice != 0);
}
