#ifndef CUSTOMER_H
#define CUSTOMER_H

/* =============================================
   CUSTOMER.H - Module quan ly khach hang
   ============================================= */

#include "types.h"

/* ---- HĂ m CRUD ---- */
void addCustomer(Customer customers[], int *count);
void editCustomer(Customer customers[], int count);
void deleteCustomer(Customer customers[], int count);
void listCustomers(Customer customers[], int count);
void searchCustomer(Customer customers[], int count);
void updateCustomerRank(Customer customers[], int count);
const char* rankStr(int rank);
float rankDiscount(int rank);

int findCustomerById(Customer customers[], int count, const char *id);
int findCustomerByPhone(Customer customers[], int count, const char *phone);
void printCustomer(Customer *c);

/* ---- File I/O ---- */
void saveCustomers(Customer customers[], int count);
int loadCustomers(Customer customers[]);

/* ---- Menu ---- */
void menuCustomer(Customer customers[], int *count);

#endif /* CUSTOMER_H */