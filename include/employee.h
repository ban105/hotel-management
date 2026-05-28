#ifndef EMPLOYEE_H
#define EMPLOYEE_H

/* ============================================================
   HANG SO
   ============================================================ */
#define MAX_EMPLOYEES    100
#define EMPLOYEE_ACTIVE  1
#define EMPLOYEE_INACTIVE 0

/* ============================================================
   STRUCT NHAN VIEN
   ============================================================ */
typedef struct {
    char  empId[10];      
    char  name[50];       
    char  position[30];   
    char  phone[15];      
    float salary;         
    int   status;         
} Employee;

/* ============================================================
   FILE I/O
   ============================================================ */
int  loadEmployees(Employee employees[]);
void saveEmployees(Employee employees[], int count);

/* ============================================================
   CAC HAM CHUC NANG
   ============================================================ */
void listEmployees(Employee employees[], int count);
void addEmployee(Employee employees[], int *count);
void editEmployee(Employee employees[], int count);
void toggleEmployeeStatus(Employee employees[], int count);

/* Tim nhan vien theo ma */
int findEmployeeById(Employee employees[], int count, const char *id);

/* Tao ma nhan vien tu dong */
void generateEmployeeId(Employee employees[], int count, char *newId);

/* ============================================================
   MENU QUAN LY NHAN VIEN
   ============================================================ */
void menuEmployee(Employee employees[], int *count);

#endif /* EMPLOYEE_H */