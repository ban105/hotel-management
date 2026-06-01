#ifndef MENU_H
#define MENU_H

#include "room.h"
#include "customer.h"
#include "booking.h"
#include "service.h"
#include "bill.h"

/* Hien man hinh chao mung */
void showWelcome();

void mainMenu(Room rooms[], int *roomCount,
              Customer customers[], int *customerCount,
              Booking bookings[], int *bookingCount,
              UsedService usedServices[], int *usedCount,
              Service services[], int *serviceCount);

#endif /* MENU_H */
