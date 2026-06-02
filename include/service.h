#ifndef SERVICE_H
#define SERVICE_H

#include <stdio.h>
#include "types.h"
#include "room.h"
#include "customer.h"
#include "booking.h"

/* ============================================================
   CAC HAM DANH MUC DICH VU
   ============================================================ */
int   loadServices(Service services[]);
void  listServices(Service services[], int count);
int   findServiceById(Service services[], int count, const char *id);

/* ============================================================
   CAC HAM DICH VU DA GOI
   ============================================================ */
void  addUsedService(UsedService usedServices[], int *usedCount,
                     Booking bookings[], int bookingCount,
                     Room rooms[], int roomCount,
                     Customer customers[], int customerCount,
                     Service services[], int serviceCount);

void  deleteUsedService(UsedService usedServices[], int *usedCount,
                        Booking bookings[], int bookingCount,
                        Customer customers[], int customerCount,
                        Service services[], int serviceCount);

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

#endif 
