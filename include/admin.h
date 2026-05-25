#ifndef ADMIN_H
#define ADMIN_H
#include "employee.h"
#include <stdio.h>
#include "types.h"
#include "room.h"
#include "customer.h"
#include "booking.h"
#include "service.h"
#include "bill.h"


#define ADMIN_PASSWORD     "admin123"
#define MAX_BILLS          200
#define ADMIN_LINE_LEN     62


typedef struct {
    char billId[10];        
    char bookingId[10];     
    char checkOutDate[20];  
    float roomCost;         
    float serviceCost;      
    float serviceCharge;    
    float vat;              
    float total;            
} BillRecord;


typedef struct {
    char serviceId[10];    
    char serviceName[30];  
    int  totalQty;         
    float totalRevenue;    
} ServiceStat;


int adminLogin();



/* Xem toan bo danh muc dich vu */
void adminViewServices(Service services[], int count);

/* Them dich vu moi vao danh muc (append file + cap nhat mang) */
void adminAddService(Service services[], int *count);

/* Sua don gia hoac ten dich vu */
void adminEditService(Service services[], int *count);

/* Xoa dich vu khoi danh muc */
void adminDeleteService(Service services[], int *count);

/* Luu lai danh muc dich vu ra file (ghi de) */
void adminSaveServices(Service services[], int count);

/* ============================================================
   LUU HOA DON RA FILE data/bills.txt (goi sau check-out)
   ============================================================ */

/* Luu 1 hoa don vao file data/bills.txt (append)
 * Goi tu checkOut() sau khi tao Bill thanh cong */
void saveBillRecord(Bill *bill, Booking *booking);

/* Doc toan bo hoa don tu file data/bills.txt */
int loadBillRecords(BillRecord records[]);

/* ============================================================
   BAO CAO THONG KE
   ============================================================ */

/* Bao cao 1: Doanh thu tong hop tu data/bills.txt */
void reportRevenueSummary();

/* Bao cao 2: Hieu suat dich vu (best seller) */
void reportServicePerformance(Service services[], int serviceCount);

/* Bao cao 3: Doanh thu theo khoang thoi gian */
void reportRevenueByDateRange();

/* ============================================================
   TIEN ICH NOI BO
   ============================================================ */

/* So sanh ngay DD/MM/YYYY, tra ve -1/0/1 */
int compareDates(const char *d1, const char *d2);

/* Kiem tra ngay d nam trong khoang [from, to] */
int dateInRange(const char *d, const char *from, const char *to);

/* In dong ke cho bao cao */
void adminPrintLine();
void adminPrintDoubleLine();

/* ============================================================
   MENU QUAN TRI VIEN
   ============================================================ */
void menuAdmin(Room rooms[], int *roomCount,
               Customer customers[], int *customerCount,
               Booking bookings[], int *bookingCount,
               UsedService usedServices[], int *usedCount,
               Service services[], int *serviceCount,
               Employee employees[], int *employeeCount);

#endif 