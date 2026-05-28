#ifndef ADMIN_H
#define ADMIN_H

#include <stdio.h>
#include "types.h"
#include "room.h"
#include "customer.h"
#include "booking.h"
#include "service.h"
#include "bill.h"
#include "employee.h"

#define ADMIN_PASSWORD     "admin123"
#define RECEPTION_PASSWORD "letan123"
#define MAX_BILLS          200
#define ADMIN_LINE_LEN     62

typedef struct {
    char  billId[10];
    char  bookingId[10];
    char  checkOutDate[20];
    float roomCost;
    float serviceCost;
    float serviceCharge;
    float vat;
    float total;
} BillRecord;

typedef struct {
    char  serviceId[10];
    char  serviceName[30];
    int   totalQty;
    float totalRevenue;
} ServiceStat;

int systemLogin(User *user);

void adminViewServices(Service services[], int count);
void adminAddService(Service services[], int *count);
void adminEditService(Service services[], int *count);
void adminDeleteService(Service services[], int *count);
void adminSaveServices(Service services[], int count);

int  saveBillRecord(Bill *bill, Booking *booking);
int  loadBillRecords(BillRecord records[]);

void reportRevenueSummary();
void reportServicePerformance(Service services[], int serviceCount);
void reportRevenueByDateRange();

int compareDates(const char *d1, const char *d2);
int dateInRange(const char *d, const char *from, const char *to);

void adminPrintLine();
void adminPrintDoubleLine();

void menuAdmin(Room rooms[], int *roomCount,
               Customer customers[], int *customerCount,
               Booking bookings[], int *bookingCount,
               UsedService usedServices[], int *usedCount,
               Service services[], int *serviceCount,
               Employee employees[], int *employeeCount);

#endif
