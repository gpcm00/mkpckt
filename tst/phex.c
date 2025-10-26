#include <stdio.h>

int main() 
{
	int c;
	while ((c = getchar()) != EOF) { 
		printf("%02X ", (unsigned char)c);
	}
	putchar('\n');
	return 0;
}
