#ifndef STD_STRING_H
#define STD_STRING_H

unsigned long long strlen(void *str);

/* String leading zeros */
char* strldz(void *str, int n);

char* ntoa(int n, unsigned char base);
char* htoa(int n);
char* itoa(int n);
char* btoa(int n);

#endif
