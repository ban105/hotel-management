#ifndef UTILS_H
#define UTILS_H

void clearScreen();
void pauseScreen();
void clearInputBuffer();
void printLine(int lenght);
void printHeader(const char *title);
int isNotEmpty(const char *str);
int isNumberic(const char *str);
int isvalidDate(const char *date);
void toLowerStr(char *str);
void safeInput(char *buffer, int size);
int inputInt(const char *prompt, int min, int max);
float inputFloat(const char *prompt,float min);
#endif