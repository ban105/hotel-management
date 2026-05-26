#ifndef BOOKING_H
#define BOOKING_H

#include "types.h"
#include "room.h"
#include "customer.h"

#define BOOKING_PENDING  0
#define BOOKING_CHECKIN  1
#define BOOKING_DONE     2
#define BOOKING_CANCEL   3
#define MAX_BOOKINGS     100

void addBooking(Booking bookings[], int *bookingCount,
                Room rooms[], int roomCount,
                Customer customers[], int customerCount);

void cancelBooking(Booking bookings[], int bookingCount,
                   Room rooms[], int roomCount);

void listBookings(Booking bookings[], int count,
                  Customer customers[], int customerCount,
                  Room rooms[], int roomCount);

void checkIn(Booking bookings[], int bookingCount,
             Room rooms[], int roomCount,
             Customer customers[], int customerCount);

void checkOut(Booking bookings[], int bookingCount,
              Room rooms[], int roomCount,
              Customer customers[], int customerCount);

int   findBookingById(Booking bookings[], int count, const char *id);
void  generateBookingId(Booking bookings[], int count, char *id);
void  generateBillId(char *billId, const char *bookingId);
int   calcNights(const char *checkIn, const char *checkOut);
void  getCurrentDateTime(char *dateStr, char *timeStr);
void  printCheckInReceipt(Booking *b, Room *r, Customer *c);

void saveBookings(Booking bookings[], int count);
int  loadBookings(Booking bookings[]);

void menuBooking(Booking bookings[], int *bookingCount,
                 Room rooms[], int *roomCount,
                 Customer customers[], int customerCount);

#endif
