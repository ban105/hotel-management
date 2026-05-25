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
    char  empId[10];      /* Ma nhan vien: NV001...     */
    char  name[50];       /* Ho ten                     */
    char  position[30];   /* Chuc vu: Le tan, Buong phong... */
    char  phone[15];      /* So dien thoai              */
    float salary;         /* Luong co ban               */
    int   status;         /* 1=Active, 0=Inactive       */
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