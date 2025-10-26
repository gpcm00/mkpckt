#ifndef PARSE_CMD_H
#define PARSE_CMD_H

int parse_args(int argc, char** argv);
char* get_file();
char* get_input();
char* get_output();
unsigned char get_endianness();
unsigned char get_help();
void print_optional_args();

#endif
