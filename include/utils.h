#ifndef UTILS_H
#define UTILS_H

void clearScreen();
void pauseScreen();
void clearInputBuffer();
void printLine(int length);
void printHeader(const char *title);
int isNotEmpty(const char *str);
int isNumeric(const char *str);
int isValidDate(const char *date);
void toLowerStr(char *str);
void trimStr(char *str);
int safeInput(char *buffer, int size);
int inputInt(const char *prompt, int min, int max);
float inputFloat(const char *prompt,float min);
#endif
