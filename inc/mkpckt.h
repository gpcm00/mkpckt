#ifndef MKPCKT_H
#define MKPCK_H
#include <stdio.h>

int mkpckt(char* path, char* extra, FILE* ostream);
void convert_endianness(char endiannes); 

#endif
