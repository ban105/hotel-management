#ifndef BILL_H
#define BILL_H

#include <stdio.h>
#include "types.h"
#include "room.h"
#include "customer.h"
#include "booking.h"
#include "service.h"

/* ============================================================
   HANG SO THUE VA PHI
   ============================================================ */
#define SERVICE_CHARGE_RATE 0.05f  /* Phi dich vu 5%  */
#define VAT_RATE            0.10f  /* Thue VAT 10%    */

/* ============================================================
   STRUCT HOA DON
   ============================================================ */
typedef struct {
    char  billId[10];     /* Ma hoa don: HD001...       */
    char  bookingId[10];  /* Ma dat phong lien ket      */
    float roomCost;       /* Tien phong sau giam gia    */
    float serviceCost;    /* Tong tien dich vu (active) */
    float serviceCharge;  /* Phi dich vu 5%             */
    float vat;            /* Thue VAT 10%               */
    float total;          /* Tong thanh toan cuoi       */
} Bill;

/* ============================================================
   CAC HAM HOA DON
   ============================================================ */

/* Tao hoa don, tra ve 1 neu thanh cong */
int  createBill(Bill *bill,
                Booking *booking,
                Room *room,
                Customer *customer,
                UsedService usedServices[], int usedCount,
                Service services[], int serviceCount);

/* In hoa don ra stream bat ky (stdout hoac file) */
void printBill(Bill *bill,
               Booking *booking,
               Room *room,
               Customer *customer,
               UsedService usedServices[], int usedCount,
               Service services[], int serviceCount,
               FILE *stream);

/* Xuat hoa don ra file output/HDxxx.txt */
int  exportBillToFile(Bill *bill,
                      Booking *booking,
                      Room *room,
                      Customer *customer,
                      UsedService usedServices[], int usedCount,
                      Service services[], int serviceCount);

/* Hien hoa don tren man hinh terminal */
void displayBill(Bill *bill,
                 Booking *booking,
                 Room *room,
                 Customer *customer,
                 UsedService usedServices[], int usedCount,
                 Service services[], int serviceCount);

/* Xem lai hoa don theo ma booking */
void viewBillByBooking(Booking bookings[], int bookingCount,
                       Room rooms[], int roomCount,
                       Customer customers[], int customerCount,
                       UsedService usedServices[], int usedCount,
                       Service services[], int serviceCount);

/* Tao ma hoa don tu ma booking: BK001 -> HD001 */
void generateBillIdFromBooking(char *billId, const char *bookingId);

/* Menu quan ly hoa don */
void menuBill(Booking bookings[], int bookingCount,
              Room rooms[], int roomCount,
              Customer customers[], int customerCount,
              UsedService usedServices[], int usedCount,
              Service services[], int serviceCount);

#endif /* BILL_H */