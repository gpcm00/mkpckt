#include <stdbool.h>
#include <stdio.h>

#define INIT_CMD(o, desc)	{o, NULL, desc, false}

#define LEN(arr)			(sizeof(arr)/sizeof(arr[0]))

struct optional_args {
	char opt;
	char* arg;
	char* desc;
	bool fill;
};

static struct optional_args full_list[] = {
	INIT_CMD('i', "Convert input to packet"),
	INIT_CMD('o', "Output file. STDOUT by default"),	
	INIT_CMD('e', "Convert data using system's endianness"),
	INIT_CMD('h', "Help"),
};

static char* file_path = NULL;

static int update_list(int i, int argc, char** argv);

int parse_args(int argc, char** argv)
{	
	int i;
	for (i = 1; i < argc; i++) {
		i = update_list(i, argc, argv);
		if (i == -1) {
			return -1;	
		}
	}
	return i;
}

char* get_file()
{
	return file_path;
}

char* get_input()
{
	return full_list[0].arg;
}

char* get_output()
{
	return full_list[1].arg;
}

unsigned char get_endianness()
{
	return (full_list[2].fill)? 0:1;
}

unsigned char get_help()
{
	return (full_list[3].fill)? 1:0;
}

void print_optional_args()
{
	for (unsigned int i = 0; i < LEN(full_list); i++) {
		printf("%c: %s\n", full_list[i].opt, full_list[i].desc);
	}	
}

static int update_list(int i, int argc, char** argv)
{
	char* current = argv[i];
	if (current[0] != '-') {
		if (file_path) {
			return -1;
		}
		file_path = current;
		return i;
	}
	
	switch (current[1]) {
		case 'i':
			if (full_list[0].fill) {
				return -1;
			}
			if (++i >= argc) {
				return -1;
			}
			full_list[0].fill = true;
			full_list[0].arg = argv[i];
			return i;

		case 'o':
			if (full_list[1].fill) {
				return -1;
			}
			if (++i >= argc) {
				return -1;
			}
			full_list[1].fill = true;
			full_list[1].arg = argv[i];
			return i;
			
		case 'e':
			if (full_list[2].fill) {
				return -1;
			}
			full_list[2].fill = true;
			return i;
		
		case 'h':
			if (full_list[3].fill) {
				return -1;
			}
			full_list[3].fill = true;
			return i;
		
		default:
			return -1;		
	}
}
