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
#define SERVICE_CHARGE_RATE 0.05f  
#define VAT_RATE            0.10f  

/* ============================================================
   STRUCT HOA DON
   ============================================================ */
typedef struct {
    char  billId[10];     
    char  bookingId[10];  
    float roomCost;       
    float serviceCost;    
    float serviceCharge;  
    float vat;            
    float total;          
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

/* Ghi nhan hoa don vao data/bills.txt de bao cao doanh thu */
int  saveBillRecord(Bill *bill, Booking *booking);

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
