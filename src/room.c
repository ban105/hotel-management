#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "room.h"
#include "booking.h"
#include "utils.h"

/* =============================================
   ROOM.C - Quan ly phong khach san
   ============================================= */

#define ROOM_FILE "data/rooms.txt"


/* ============================
   HAM HIEN THI NOI BO
   ============================ */

/* Chuyen status thanh chuoi */
static const char* statusStr(int status) {
    switch (status) {
        case ROOM_EMPTY:    return "Trong";
        case ROOM_OCCUPIED: return "Dang thue";
        default:            return "Trong";
    }
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
            bookings[i].status == BOOKING_CHECKIN) {
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
    int empty = 0, occupied = 0;
    for (int i = 0; i < count; i++) {
        if      (rooms[i].status == ROOM_EMPTY)    empty++;
        else if (rooms[i].status == ROOM_OCCUPIED) occupied++;
    }

    printRoomTableHeader("DANH SACH PHONG");
    for (int i = 0; i < count; i++) {
        printRoomRow(i + 1, &rooms[i]);
    }
    printRoomTableFooter();
    printf("  Tong: %d phong  |  Trong: %d  |  Dang thue: %d\n",
           count, empty, occupied);
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
        if (rooms[count].status != ROOM_OCCUPIED) {
            rooms[count].status = ROOM_EMPTY;
        }
        count++;
    }
    fclose(fp);
    return count;
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
        printf("| 2. Them phong moi                                      |\n");
        printf("| 3. Xoa phong                                           |\n");
        printf("| 0. Quay lai menu chinh                                 |\n");
        printLine(58);

        choice = inputInt("  Chon chuc nang: ", 0, 3);

        switch (choice) {
            case 1: listRooms(rooms, *count);        break;
            case 2: addRoom(rooms, count);           break;
            case 3: deleteRoom(rooms, count);        break;
            case 0: break;
        }
    } while (choice != 0);
}
