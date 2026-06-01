#ifndef ROOM_H
#define ROOM_H

/* =============================================
   ROOM.H - Module quan ly phong
   ============================================= */

#include "types.h"

/* ---- Ham CRUD ---- */
void addRoom(Room rooms[], int *count);
void deleteRoom(Room rooms[], int *count);
void listRooms(Room rooms[], int count);

/* ---- Ham tien ich ---- */
int findRoomById(Room rooms[], int count, const char *id);
void printRoom(Room *room);

/* ---- File I/O ---- */
void saveRooms(Room rooms[], int count);
int loadRooms(Room rooms[]);

/* ---- Menu ---- */
void menuRoom(Room rooms[], int *count);

#endif /* ROOM_H */
