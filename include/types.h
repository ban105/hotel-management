#ifndef TYPES_H
#define TYPES_H

#define MAX_ROOMS       100
#define MAX_CUSTOMERS   100
#define MAX_BOOKINGS    100
#define MAX_SERVICES    50
#define MAX_USED_SERVICES 500
#define MAX_CART_ITEMS    20

#define ROOM_EMPTY      0   
#define ROOM_OCCUPIED   1   

#define BOOKING_CHECKIN  1  
#define BOOKING_DONE     2  
#define BOOKING_CANCEL   3

#define SERVICE_ACTIVE    1
#define SERVICE_CANCELED  0

typedef struct
{
    char id[10];
    char type[20];
    float price;
    int status;
}Room;

#define RANK_BRONZE  0
#define RANK_SILVER  1
#define RANK_GOLD    2

typedef struct 
{
   char id[10];
   char name[50];
   char phone[15];
   char cccd[20];
   int rank;
}Customer;

typedef struct {
    char bookingId[10];     
    char customerId[10];    
    char roomId[10];        
    char checkInDate[20];   
    char checkOutDate[20];  
    int status;             
} Booking;

typedef struct {
    char  serviceId[10];
    char  serviceName[30];
    float price;
    char  unit[10];
} Service;

typedef struct {
    char bookingId[10];
    char serviceId[10];
    int  quantity;
    int  status;
} UsedService;

#endif 



