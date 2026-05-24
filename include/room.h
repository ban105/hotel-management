#ifndef ROOM_H
#define ROOM_H

/* =============================================
   ROOM.H - Module quản lý phòng
   ============================================= */

#include "types.h"

/* ---- Hàm CRUD ---- */
void addRoom(Room rooms[], int *count);
void editRoom(Room rooms[], int count);
void deleteRoom(Room rooms[], int *count);
void listRooms(Room rooms[], int count);
void listAvailableRooms(Room rooms[], int count);
void searchRoom(Room rooms[], int count);
void updateRoomStatus(Room rooms[], int count);

/* ---- Hàm tiện ích ---- */
int findRoomById(Room rooms[], int count, const char *id);
int isRoomAvailable(Room rooms[], int count, const char *id);
void printRoom(Room *room);
void printRoomStatus(int status);

/* ---- File I/O ---- */
void saveRooms(Room rooms[], int count);
int loadRooms(Room rooms[]);

/* ---- Menu ---- */
void menuRoom(Room rooms[], int *count);

#endif /* ROOM_H */