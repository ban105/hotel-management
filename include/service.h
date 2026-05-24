#ifndef SERVICE_H
#define SERVICE_H

#include <stdio.h>
#include "types.h"
#include "room.h"
#include "customer.h"
#include "booking.h"

/* ============================================================
   HANG SO
   ============================================================ */
#define MAX_SERVICES      50
#define MAX_USED_SERVICES 500
#define MAX_CART_ITEMS    20

#define SERVICE_ACTIVE    1
#define SERVICE_CANCELED  0

/* ============================================================
   STRUCT DANH MUC DICH VU (doc tu file tinh)
   ============================================================ */
typedef struct {
    char  serviceId[10];   /* Ma dich vu: DV001... */
    char  serviceName[30]; /* Ten dich vu           */
    float price;           /* Don gia               */
    char  unit[10];        /* Don vi: cai, lan, kg  */
} Service;

/* ============================================================
   STRUCT DICH VU KHACH DA GOI (luu vao file dong)
   ============================================================ */
typedef struct {
    char bookingId[10]; /* Gan voi booking nao  */
    char serviceId[10]; /* Ma dich vu           */
    int  quantity;      /* So luong             */
    int  status;        /* 1=Active, 0=Canceled */
} UsedService;

/* ============================================================
   CAC HAM DANH MUC DICH VU
   ============================================================ */
int   loadServices(Service services[]);
void  listServices(Service services[], int count);
int   findServiceById(Service services[], int count, const char *id);
void  printService(Service *s);

/* ============================================================
   CAC HAM DICH VU DA GOI
   ============================================================ */
void  addUsedService(UsedService usedServices[], int *usedCount,
                     Booking bookings[], int bookingCount,
                     Room rooms[], int roomCount,
                     Customer customers[], int customerCount,
                     Service services[], int serviceCount);

void  cancelUsedService(UsedService usedServices[], int usedCount,
                        Service services[], int serviceCount);

void  viewUsedServicesByBooking(UsedService usedServices[], int usedCount,
                                Service services[], int serviceCount,
                                Booking bookings[], int bookingCount,
                                Customer customers[], int customerCount);

float calcServiceCost(UsedService usedServices[], int usedCount,
                      Service services[], int serviceCount,
                      const char *bookingId);

void  saveUsedServices(UsedService usedServices[], int count);
int   loadUsedServices(UsedService usedServices[]);

void  menuService(UsedService usedServices[], int *usedCount,
                  Booking bookings[], int bookingCount,
                  Room rooms[], int roomCount,
                  Customer customers[], int customerCount,
                  Service services[], int serviceCount);

#endif /* SERVICE_H */