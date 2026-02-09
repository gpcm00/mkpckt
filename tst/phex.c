#include <stdio.h>
#include <stdint.h>

int main(int argc, char** argv) 
{
	int c = 0;
	int i = 0;
	char* var_name = (argc > 1)? argv[1] : "raw_bytes";
	
	printf("char %s[] = {\n\t", var_name);

	while ((c = getchar()) != EOF) { 
		printf("0x%02X, ", (unsigned char)c);
		if ((++i % 8) == 0) {
			printf("\n\t");
		}
	}
	printf("\n};\n");
	return 0;
}
