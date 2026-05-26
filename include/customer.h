#ifndef CUSTOMER_H
#define CUSTOMER_H

#include "types.h"

/* CRUD */
void addCustomer(Customer customers[], int *count);
void editCustomer(Customer customers[], int count);
void deleteCustomer(Customer customers[], int *count,
                    Booking bookings[], int bookingCount);
void listCustomers(Customer customers[], int count);
void searchCustomer(Customer customers[], int count);
void updateCustomerRank(Customer customers[], int count);
const char* rankStr(int rank);
float rankDiscount(int rank);

/* Lookup and output helpers */
int findCustomerById(Customer customers[], int count, const char *id);
int findCustomerByPhone(Customer customers[], int count, const char *phone);
void printCustomer(Customer *c);

/* File I/O */
void saveCustomers(Customer customers[], int count);
int loadCustomers(Customer customers[]);

/* Menu */
void menuCustomer(Customer customers[], int *count,
                  Booking bookings[], int bookingCount);

#endif
