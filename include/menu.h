#ifndef MENU_H
#define MENU_H

#include "room.h"
#include "customer.h"
#include "booking.h"
#include "service.h"
#include "employee.h"
#include "bill.h"

/* Hien man hinh chao mung */
void showWelcome();

/*
 * Menu chinh - cap nhat Day 4:
 * Them usedServices, usedCount, services, serviceCount
 */
void mainMenu(Room rooms[], int *roomCount,
              Customer customers[], int *customerCount,
              Booking bookings[], int *bookingCount,
              UsedService usedServices[], int *usedCount,
              Service services[], int *serviceCount,
              Employee employees[], int *employeeCount);

#endif /* MENU_H */