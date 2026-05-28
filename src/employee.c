#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "employee.h"
#include "utils.h"

#define EMPLOYEE_FILE "data/employees.txt"
#define EMP_DIV   "  +-----------------------------------------------------------------------------+\n"
#define EMP_MENU_DIV "  +----------------------------------------------------------+\n"
#define EMP_TABLE_DIV "  +--------+--------------------+--------------+------------+----------+--------+\n"

/* ============================================================
   TIEN ICH IN KHUNG
   ============================================================ */
static void empHeader(const char *title) {
    printf(EMP_DIV);
    int len   = strlen(title);
    int total = 77;
    int pad   = (total - len) / 2;
    printf("  |");
    for (int i = 0; i < pad; i++) printf(" ");
    printf("%s", title);
    for (int i = 0; i < total - pad - len; i++) printf(" ");
    printf("|\n");
    printf(EMP_DIV);
}

static void empMenuHeader(const char *title) {
    printf(EMP_MENU_DIV);
    int len = strlen(title);
    int total = 58;
    int pad = (total - len) / 2;
    printf("  |");
    for (int i = 0; i < pad; i++) printf(" ");
    printf("%s", title);
    for (int i = 0; i < total - pad - len; i++) printf(" ");
    printf("|\n");
    printf(EMP_MENU_DIV);
}

/* ============================================================
   TAO MA NHAN VIEN TU DONG
   ============================================================ */
void generateEmployeeId(Employee employees[], int count, char *newId) {
    int maxNum = 0;
    for (int i = 0; i < count; i++) {
        int n = 0;
        sscanf(employees[i].empId, "NV%d", &n);
        if (n > maxNum) maxNum = n;
    }
    snprintf(newId, 10, "NV%03d", maxNum + 1);
}

/* ============================================================
   TIM NHAN VIEN THEO MA
   ============================================================ */
int findEmployeeById(Employee employees[], int count, const char *id) {
    for (int i = 0; i < count; i++)
        if (strcmp(employees[i].empId, id) == 0) return i;
    return -1;
}

static int parseNonNegativeFloat(const char *text, float *value) {
    char *end;
    float parsed = strtof(text, &end);

    if (end == text) return 0;
    while (*end != '\0') {
        if (!isspace((unsigned char)*end)) return 0;
        end++;
    }
    if (parsed < 0) return 0;

    *value = parsed;
    return 1;
}

/* ============================================================
   DOC FILE
   ============================================================ */
int loadEmployees(Employee employees[]) {
    FILE *fp = fopen(EMPLOYEE_FILE, "r");
    if (!fp) return 0;

    int  count = 0;
    char line[256];

    while (fgets(line, sizeof(line), fp) && count < MAX_EMPLOYEES) {
        trimStr(line);
        if (strlen(line) == 0) continue;

        Employee *e = &employees[count];
        int parsed = sscanf(line, "%9[^,],%49[^,],%29[^,],%14[^,],%f,%d",
                            e->empId,
                            e->name,
                            e->position,
                            e->phone,
                            &e->salary,
                            &e->status);
        if (parsed == 6) count++;
    }

    fclose(fp);
    return count;
}

/* ============================================================
   LUU FILE
   ============================================================ */
void saveEmployees(Employee employees[], int count) {
    FILE *fp = fopen(EMPLOYEE_FILE, "w");
    if (!fp) {
        printf("  [!] Khong the mo file de luu nhan vien.\n");
        return;
    }
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%s,%s,%s,%s,%.0f,%d\n",
                employees[i].empId,
                employees[i].name,
                employees[i].position,
                employees[i].phone,
                employees[i].salary,
                employees[i].status);
    }
    fclose(fp);
}

/* ============================================================
   XEM DANH SACH NHAN VIEN
   ============================================================ */
void listEmployees(Employee employees[], int count) {
    clearScreen();
    empHeader("DANH SACH NHAN VIEN");

    if (count == 0) {
        printf("  | %-75s |\n", "  (Chua co nhan vien nao)");
        printf(EMP_DIV);
        pauseScreen();
        return;
    }

    printf(EMP_TABLE_DIV);
    printf("  | %-6s | %-18s | %-12s | %-10s | %8s | %-6s |\n",
           "Ma NV", "Ho ten", "Chuc vu", "SDT", "Luong", "T.Thai");
    printf(EMP_TABLE_DIV);

    int active = 0, inactive = 0;

    for (int i = 0; i < count; i++) {
        const char *statusStr = (employees[i].status == EMPLOYEE_ACTIVE)
                                ? "On " : "Off";
        printf("  | %-6.6s | %-18.18s | %-12.12s | %-10.10s | %8.0f | %-6.6s |\n",
               employees[i].empId,
               employees[i].name,
               employees[i].position,
               employees[i].phone,
               employees[i].salary,
               statusStr);

        if (employees[i].status == EMPLOYEE_ACTIVE) active++;
        else inactive++;
    }

    printf(EMP_TABLE_DIV);
    printf("  Tong: %d  |  Dang lam: %d  |  Da nghi: %d\n",
           count, active, inactive);

    pauseScreen();
}

/* ============================================================
   THEM NHAN VIEN MOI
   ============================================================ */
void addEmployee(Employee employees[], int *count) {
    clearScreen();
    empHeader("THEM NHAN VIEN MOI");

    if (*count >= MAX_EMPLOYEES) {
        printf("  [!] Da dat toi da so nhan vien!\n");
        pauseScreen();
        return;
    }

    Employee e;
    memset(&e, 0, sizeof(Employee));

    /* Tao ma tu dong */
    generateEmployeeId(employees, *count, e.empId);
    printf("  Ma nhan vien (tu dong): %s\n", e.empId);
    printf(EMP_DIV);

    /* Nhap ho ten */
    printf("  Ho ten           : ");
    safeInput(e.name, sizeof(e.name));
    if (!isNotEmpty(e.name)) {
        printf("  [!] Ten khong duoc de trong.\n");
        pauseScreen(); return;
    }

    /* Nhap chuc vu */
    printf("  Chuc vu\n");
    printf("  (Le tan / Buong phong / Bep / Bao ve / Quan ly): ");
    safeInput(e.position, sizeof(e.position));
    if (!isNotEmpty(e.position)) {
        printf("  [!] Chuc vu khong duoc de trong.\n");
        pauseScreen(); return;
    }

    /* Nhap so dien thoai */
    printf("  So dien thoai    : ");
    safeInput(e.phone, sizeof(e.phone));
    if (!isNotEmpty(e.phone)) {
        printf("  [!] SDT khong duoc de trong.\n");
        pauseScreen(); return;
    }
    if (!isNumeric(e.phone)) {
        printf("  [!] SDT chi duoc chua chu so.\n");
        pauseScreen(); return;
    }

    /* Nhap luong */
    e.salary = inputFloat("  Luong co ban (VND): ", 0);

    /* Mac dinh dang lam viec */
    e.status = EMPLOYEE_ACTIVE;

    /* Xac nhan */
    printf(EMP_DIV);
    printf("  Ma  : %s\n", e.empId);
    printf("  Ten : %s\n", e.name);
    printf("  CV  : %s\n", e.position);
    printf("  SDT : %s\n", e.phone);
    printf("  Luong: %.0f VND\n", e.salary);
    printf(EMP_DIV);
    printf("  Xac nhan them? (y/n): ");
    char confirm[5];
    safeInput(confirm, sizeof(confirm));

    if (confirm[0] != 'y' && confirm[0] != 'Y') {
        printf("  Da huy.\n");
        pauseScreen(); return;
    }

    employees[*count] = e;
    (*count)++;
    saveEmployees(employees, *count);

    printf("  [OK] Da them nhan vien %s - %s!\n", e.empId, e.name);
    pauseScreen();
}

/* ============================================================
   CAP NHAT THONG TIN NHAN VIEN (CHUC VU VA LUONG)
   ============================================================ */
void editEmployee(Employee employees[], int count) {
    clearScreen();
    empHeader("CAP NHAT THONG TIN NHAN VIEN");

    /* Hien danh sach de chon */
    listEmployees(employees, count);
    if (count == 0) return;

    printf("  Nhap ma nhan vien can sua (VD: NV001): ");
    char empId[10];
    safeInput(empId, sizeof(empId));

    int idx = findEmployeeById(employees, count, empId);
    if (idx < 0) {
        printf("  [!] Khong tim thay nhan vien %s.\n", empId);
        pauseScreen(); return;
    }

    if (employees[idx].status == EMPLOYEE_INACTIVE) {
        printf("  [!] Nhan vien %s da nghi viec, khong the sua thong tin.\n", empId);
        pauseScreen(); return;
    }

    printf("\n  Thong tin hien tai:\n");
    printf(EMP_DIV);
    printf("  Ten   : %s\n", employees[idx].name);
    printf("  CV    : %s\n", employees[idx].position);
    printf("  SDT   : %s\n", employees[idx].phone);
    printf("  Luong : %.0f VND\n", employees[idx].salary);
    printf(EMP_DIV);
    printf("  (Nhan Enter de giu nguyen gia tri cu)\n\n");

    /* Sua chuc vu */
    char tmp[50];
    printf("  Chuc vu moi [%s]: ", employees[idx].position);
    safeInput(tmp, sizeof(tmp));
    if (isNotEmpty(tmp))
        strncpy(employees[idx].position, tmp,
                sizeof(employees[idx].position) - 1);
    employees[idx].position[sizeof(employees[idx].position) - 1] = '\0';

    /* Sua so dien thoai */
    printf("  SDT moi [%s]: ", employees[idx].phone);
    safeInput(tmp, sizeof(tmp));
    if (isNotEmpty(tmp)) {
        if (!isNumeric(tmp)) {
            printf("  [!] SDT khong hop le, giu nguyen.\n");
        } else {
            strncpy(employees[idx].phone, tmp,
                    sizeof(employees[idx].phone) - 1);
        }
    }
    employees[idx].phone[sizeof(employees[idx].phone) - 1] = '\0';

    /* Sua luong */
    printf("  Luong moi [%.0f]: ", employees[idx].salary);
    safeInput(tmp, sizeof(tmp));
    if (isNotEmpty(tmp)) {
        float newSalary;
        if (parseNonNegativeFloat(tmp, &newSalary))
            employees[idx].salary = newSalary;
        else
            printf("  [!] Luong khong hop le, giu nguyen.\n");
    }

    saveEmployees(employees, count);
    printf("\n  [OK] Da cap nhat thong tin nhan vien %s.\n", empId);
    pauseScreen();
}

/* ============================================================
   THAY DOI TRANG THAI LAM VIEC
   ============================================================ */
void toggleEmployeeStatus(Employee employees[], int count) {
    clearScreen();
    empHeader("THAY DOI TRANG THAI LAM VIEC");

    listEmployees(employees, count);
    if (count == 0) return;

    printf("  Nhap ma nhan vien (VD: NV001): ");
    char empId[10];
    safeInput(empId, sizeof(empId));

    int idx = findEmployeeById(employees, count, empId);
    if (idx < 0) {
        printf("  [!] Khong tim thay nhan vien %s.\n", empId);
        pauseScreen(); return;
    }

    /* Hien trang thai hien tai */
    const char *curStatus = (employees[idx].status == EMPLOYEE_ACTIVE)
                            ? "Dang lam viec" : "Da nghi viec";
    const char *newStatus = (employees[idx].status == EMPLOYEE_ACTIVE)
                            ? "Nghi viec" : "Di lam lai";

    printf("\n  Nhan vien : %s - %s\n", employees[idx].empId, employees[idx].name);
    printf("  Trang thai hien tai: %s\n", curStatus);
    printf("  Thay doi thanh     : %s\n", newStatus);
    printf(EMP_DIV);
    printf("  Xac nhan? (y/n): ");
    char confirm[5];
    safeInput(confirm, sizeof(confirm));

    if (confirm[0] != 'y' && confirm[0] != 'Y') {
        printf("  Da huy.\n");
        pauseScreen(); return;
    }

    /* Doi trang thai */
    employees[idx].status = (employees[idx].status == EMPLOYEE_ACTIVE)
                            ? EMPLOYEE_INACTIVE : EMPLOYEE_ACTIVE;

    saveEmployees(employees, count);
    printf("  [OK] Da doi trang thai nhan vien %s sang '%s'.\n",
           empId, newStatus);
    pauseScreen();
}

/* ============================================================
   MENU QUAN LY NHAN VIEN
   ============================================================ */
void menuEmployee(Employee employees[], int *count) {
    int choice;
    do {
        clearScreen();
        empMenuHeader("QUAN LY NHAN VIEN");
        printf("  | %-57s|\n", "1. Xem danh sach nhan vien");
        printf("  | %-57s|\n", "2. Them nhan vien moi");
        printf("  | %-57s|\n", "3. Cap nhat thong tin (Chuc vu / Luong)");
        printf("  | %-57s|\n", "4. Thay doi trang thai lam viec");
        printf("  | %-57s|\n", "0. Quay lai menu Admin");
        printf(EMP_MENU_DIV);

        choice = inputInt("  Chon: ", 0, 4);

        switch (choice) {
            case 1:
                listEmployees(employees, *count);
                break;
            case 2:
                addEmployee(employees, count);
                break;
            case 3:
                editEmployee(employees, *count);
                break;
            case 4:
                toggleEmployeeStatus(employees, *count);
                break;
            case 0:
                break;
        }
    } while (choice != 0);
}

