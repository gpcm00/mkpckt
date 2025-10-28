#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <mkpckt.h>
#include <parse_args.h>


void print_usage(char* progname)
{
	printf(	"usage: %s <message> [-f file] [-h] [-e] [-o output_file]\n\n"
			"message is not required if file is present\n"
			"if both file and message are present, then"
			" message is concatenated after file.\n"
			"Options:\n", progname);
	print_optional_args();
}

int main(int argc, char** argv) 
{
	if (parse_args(argc, argv) < 2) {
		print_usage(argv[0]);
		exit(EXIT_FAILURE);
	}
	char* ifile = get_file();
	char* imessage = get_input();
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
	
	return mkpckt(ifile, imessage, ostream);
}
