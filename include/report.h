#ifndef REPORT_H
#define REPORT_H

#include "types.h"

void showStatistics(Room rooms[], int roomCount,
                    Customer customers[], int customerCount,
                    Booking bookings[], int bookingCount,
                    UsedService usedServices[], int usedCount,
                    Service services[], int serviceCount);

#endif /* REPORT_H */
