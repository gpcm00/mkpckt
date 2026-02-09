#include <mkpckt.h>

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <errno.h>

#ifdef DEBUG
#define INLINE_FUNCTION
#else
#define INLINE_FUNCTION	inline
#endif

#define _check_(cond, label)     if(cond) goto label

typedef enum {
    STR_UNKNOWN = -1,
    STR_NAME,
    STR_HEX,
	STR_BYTE,
	STR_WORD,
    STR_INT,
    STR_FLOAT,
    STR_CTRL,

    N_STR_TYPES,    
} strtype_t;

union data_t {
	size_t n;
	int32_t dw;		
	int16_t w;
	int8_t b;

	float f;
};

static bool change_endianness = true;

// char to hex
static INLINE_FUNCTION char c2h(char c) 
{
	if (c >= '0' && c <= '9') return c - '0';
	if (c >= 'A' && c <= 'F') return c - 'A' + 10;
	if (c >= 'a' && c <= 'f') return c - 'a' + 10;
	return 0;
}

// change the endianess of a 16-bit number
static INLINE_FUNCTION short worder(short w)
{
	return (change_endianness)? __builtin_bswap16(w) : w;
}

// change the endianess of a 32-bit number
static INLINE_FUNCTION int dworder(int dw)
{
	return (change_endianness)? __builtin_bswap32(dw) : dw;
}


static char* alloc_fbuffer(char* path);
static bool parse_buffer(char* buffer, FILE* ostream);
static size_t parse_next(char* buffer, char** next, strtype_t* type);
static size_t parse_special(char* buffer, strtype_t* type);
static size_t parse_name(char* buffer);
static char* remove_spaces(char* buffer);
static char* get_next_line(char* buffer);
static size_t strtohex(char* c, size_t sz);
static bool write_to_file(void* c, size_t len, FILE* ostream);

/* converts the contents of the file in path and the extra buffer into
 * the pckt format. output is written to ostream
 **********************************************************************/
int mkpckt(char* path, char* extra, FILE* ostream)
{
	char* fbuffer = NULL;
	if (path) {
    	fbuffer = alloc_fbuffer(path);
		if (!fbuffer) {
			return -1;
		}
		if (!parse_buffer(fbuffer, ostream)) {
			free(fbuffer);
			return -1;
		}
		free(fbuffer);
	}
	
	if (extra) {
		return (parse_buffer(extra, ostream))? 0 : -1;
	}

	return 0;
}

/* sets the global flag that signals endianness should be opposite of
 * the endianness of the current system
 **********************************************************************/
void convert_endianness(char endianess)
{
	change_endianness = (endianess != 0);		
}

/* reads the file from path and allocates its contents into a buffer
 * the return is a dynamically allocated buffer that needs to freed
 **********************************************************************/
static char *alloc_fbuffer(char* path)
{
    char* fbuffer = NULL;
    size_t size = 0; 
    FILE* fstream = fopen(path, "r");

    if (fstream == NULL) {
        return NULL;
    }

    _check_(fseek(fstream, 0L, SEEK_END) < 0, Clean_Fstream);
    
    size = ftell(fstream);
    _check_(size <= 0, Clean_Fstream);

    _check_(fseek(fstream, 0L, SEEK_SET) < 0, Clean_Fstream);

    fbuffer = (char*)malloc(size+1);
    _check_(fbuffer == NULL, Clean_Fstream);

    if (fread(fbuffer, 1, size, fstream) < size) {
        free(fbuffer);
        fbuffer = NULL;
    } else {
        fbuffer[size] = '\0';
    }

Clean_Fstream:
    fclose(fstream);
    return fbuffer;
}

/* converts the buffer in mkpckt format into a bytestream. output is 
 * written into ostream
 **********************************************************************/
static bool parse_buffer(char* buffer, FILE* ostream)
{
	size_t len = 0;
	char* next = NULL;
	char* current;
	strtype_t type = STR_UNKNOWN;
	union data_t num;	
	current = buffer;    

	while ((len = parse_next(current, &next, &type)) > 0) {
		switch (type) {
			case STR_NAME:
				if (!write_to_file(next, len, ostream)) {
					return false; 
				}
				break;
	
			case STR_HEX:
				num.n = strtohex(next+2, len-2);
				if (!write_to_file(next+2, num.n, ostream)) {
					return false;
				}
				break;
				
			case STR_BYTE:
				num.b = strtol(next+2, NULL, 10);
				if (!write_to_file(&num.b, sizeof(int8_t), ostream)) {
					return false;
				}
				break;

			case STR_WORD:
				num.w = strtol(next+2, NULL, 10);
				num.w = worder(num.w);
				if (!write_to_file(&num.w, sizeof(int16_t), ostream)) {
					return false;
				}
				break;

			case STR_INT:
				num.dw = strtol(next+1, NULL, 10);
				num.dw = dworder(num.dw);
				if (!write_to_file(&num.dw, sizeof(int32_t), ostream)) {
					return false;
				}
				break;

			case STR_FLOAT:
				num.f = strtof(next+2, NULL);
				num.dw = dworder(num.dw);
				if (!write_to_file(&num.f, sizeof(float), ostream)) {
					return false;
				}
				break;				

			case STR_CTRL:
				if (!write_to_file(next+1, 1, ostream)) {
					return false;
				}
				break;
			
			default:
				break;
		}
		current = next + len;
	}
	
	return true;
}

/* parse the next data on the buffer removing spaces and comments
 **********************************************************************/
static size_t parse_next(char* buffer, char** next, strtype_t* type)
{
	buffer = remove_spaces(buffer);	
    
	while (*buffer == '#') {
        buffer = get_next_line(buffer);
		buffer = remove_spaces(buffer);
    }
		
	
	if (*buffer == '\0') {
		return 0;
	}    

    *next = buffer; 
    
    if (*buffer == '\\') {
        return parse_special(buffer, type);
    }
	
	*type = STR_NAME;

    return parse_name(buffer);
}

/* parse words that start with \ and convert them accordingly 
 **********************************************************************/
static size_t parse_special(char* buffer, strtype_t* type)
{
    size_t sz = 1;
  	switch (buffer[sz]) {
		case 'x':
			*type = STR_HEX;
			while (isxdigit(buffer[++sz]));
			return sz;

		case 'b':
			*type = STR_BYTE;
			if (buffer[sz+1] == '-') {
				sz++;
			}
			while (isdigit(buffer[++sz]));
			return sz;

		case 'w':
			*type = STR_WORD;
			if (buffer[sz+1] == '-') {
				sz++;
			}
			while (isdigit(buffer[++sz]));
			return sz;
	
		case 'f':
			*type = STR_FLOAT;
			if (buffer[sz+1] == '-') {
				sz++;
			}
			while (isdigit(buffer[++sz]));
			if (buffer[sz] == '.') {
				while (isdigit(buffer[++sz]));
			}
			return sz;

		case '-':
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			*type = STR_INT;
			while (isdigit(buffer[++sz]));
			return sz;

		default:
			*type = STR_CTRL;
			return sz;
	} 
}

/* just parse the normal word and print it as it is
 **********************************************************************/
static size_t parse_name(char* buffer)
{
	size_t sz = 0;
	while (isgraph(buffer[sz])) {
		if (buffer[sz] == '\\') {
			break;
		}
		sz++;
	}

	return sz;
}

/* remove space tabs and newlines
 **********************************************************************/
static char* remove_spaces(char* buffer)
{
	while (*buffer == ' ' || *buffer == '\n' || *buffer == '\t') {
		buffer++;
	}
	return buffer;
}

/* ignores everything until a newline is reached
 **********************************************************************/
static char* get_next_line(char* buffer)
{
	while (*(buffer++) != '\n') {
		if (*buffer == '\0') {
			break;
		}
	}
	
	return buffer;
}

/* converts a sequence of hex numbers given in string format into
 * actual hex numbers. the output is in byte stream format, so the
 * hex values are preserved no matter the endianness
 **********************************************************************/
static size_t strtohex(char* str, size_t sz)
{
	size_t i = 0;
	size_t n_bytes = sz / 2;
	for (; i < n_bytes; i++) {
		str[i] = c2h(str[2*i+1]) | c2h(str[2*i]) << 4;
	}

	if (sz % 2) {
		str[i] = c2h(str[2*i]);
		i++;
	}
	
	return i;
}

/* writes a fixed amount of data into a file and handle error if error
 **********************************************************************/
static bool write_to_file(void* c, size_t len, FILE* ostream)
{
	size_t total = 0;
	do {
		size_t w = fwrite(c + total, 1, len - total, ostream);
		if (w == 0 && ferror(ostream)) {
			if (errno == EINTR) {
				clearerr(ostream);
				continue;
			} else {
				return false;
			}
		}
		total += w;
	} while (total < len);
	return true;
}
