#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pauseScreen() {
    printf("\n  Nhan Enter de tiep tuc...");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void printLine(int length) {
    for (int i = 0; i < length; i++) printf("-");
    printf("\n");
}

void printHeader(const char *title) {
    int len = 58;
    printLine(len);
    int titleLen = strlen(title);
    int padding = (len - titleLen - 2) / 2;
    printf("|");
    for (int i = 0; i < padding; i++) printf(" ");
    printf("%s", title);
    for (int i = 0; i < len - padding - titleLen - 2; i++) printf(" ");
    printf("|\n");
    printLine(len);
}

int isNotEmpty(const char *str) {
    if (str == NULL) return 0;
    int i = 0;
    while (str[i] != '\0') {
        if (!isspace((unsigned char)str[i])) return 1;
        i++;
    }
    return 0;
}

int isNumeric(const char *str) {
    if (str == NULL || *str == '\0') return 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit((unsigned char)str[i])) return 0;
    }
    return 1;
}

int isValidDate(const char *date) {
    int d, m, y;

    /* Kiem tra format DD/MM/YYYY */
    if (sscanf(date, "%d/%d/%d", &d, &m, &y) != 3) return 0;

    /* Kiem tra nam hop le */
    if (y < 1900 || y > 2100) return 0;

    /* Kiem tra thang hop le */
    if (m < 1 || m > 12) return 0;

    /* So ngay toi da cua tung thang */
    int maxDay;
    switch (m) {
        case 1: case 3: case 5: case 7:
        case 8: case 10: case 12:
            maxDay = 31;
            break;
        case 4: case 6: case 9: case 11:
            maxDay = 30;
            break;
        case 2:
            /* Kiem tra nam nhuan:
               Chia het 400 -> nhuan
               Chia het 100 nhung khong chia het 400 -> khong nhuan
               Chia het 4   nhung khong chia het 100 -> nhuan */
            if ((y % 400 == 0) || (y % 4 == 0 && y % 100 != 0))
                maxDay = 29;
            else
                maxDay = 28;
            break;
        default:
            return 0;
    }

    /* Kiem tra ngay hop le */
    if (d < 1 || d > maxDay) return 0;

    return 1;
}

void toLowerStr(char *str) {
    for (int i = 0; str[i]; i++)
        str[i] = tolower((unsigned char)str[i]);
}

void trimStr(char *str) {
    int end = strlen(str) - 1;
    while (end >= 0 && isspace((unsigned char)str[end])) end--;
    str[end + 1] = '\0';
    int start = 0;
    while (str[start] && isspace((unsigned char)str[start])) start++;
    if (start > 0) memmove(str, str + start, strlen(str) - start + 1);
}

int safeInput(char *buffer, int size) {
    if (fgets(buffer, size, stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';
        return 1;
    }
    if (size > 0) buffer[0] = '\0';
    return 0;
}

int inputInt(const char *prompt, int min, int max) {
    int value;
        char buf[20];
    while (1) {
        printf("%s", prompt);
        if (!safeInput(buf, sizeof(buf))) {
            return min;
        }
        if (isNumeric(buf)) {
            value = atoi(buf);
            if (value >= min && value <= max) return value;
        }
        printf("  [!] Gia tri khong hop le\n  Nhap lai trong [%d - %d]: ", min, max);
    }
}

float inputFloat(const char *prompt, float min) {
    char buf[30];
    float value;
    while (1) {
        printf("%s", prompt);
        if (!safeInput(buf, sizeof(buf))) {
            return min;
        }
        if (sscanf(buf, "%f", &value) == 1 && value >= min) {
            return value;
        }
        printf("  [!] Gia tri khong hop le. Vui long nhap lai: ");
    }
}
