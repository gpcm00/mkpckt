#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <mkpckt.h>
#include <parse_args.h>


void print_usage(char* progname)
{
	printf(	"usage: %s <file> [-i extra_packet] [-h] [-e]\n\n"
			"file is not required if extra_packet is present\n"
			"if both file and extra_packet are present, then"
			" extra_packet is concatenated after file.\n\n", progname);
	print_optional_args();
}

int main(int argc, char** argv) 
{
	if (parse_args(argc, argv) < 2) {
		print_usage(argv[0]);
		exit(EXIT_FAILURE);
	}
	char* ifile = get_file();
	char* iextra = get_input();
	char* ofile = get_output();

	convert_endianness(get_endianness());
	if (get_help()) {
		print_usage(argv[0]);
	}
		
	FILE* ostream = (ofile)? fopen(ofile, "w+") : stdout;
	if (!ostream) {
		perror("can't open file %s");
		exit(EXIT_FAILURE);
	}
	
	return mkpckt(ifile, iextra, ostream);
}
