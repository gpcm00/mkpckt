#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef union data_t
{
    int as_int;
    float as_float;
} data_t;


struct packet {
    uint8_t header[4];
    uint16_t port;
    uint8_t type;           // if bit n is a 1 then payload[n] is an int
                            // otherwise it is a float

    uint8_t padding;        // keep data_t 32-bit aligned

    data_t payload[8];

    uint8_t signature[12];
} __attribute__((packed));

extern char raw_bytes[sizeof(struct packet)];

int main() {
    struct packet* packet = (struct packet*)raw_bytes;

    printf("Header:\n");

    for (int i = 0; i < 4; i++) {
        printf("%02X ", packet->header[i]);
    }

    printf("\n\nPort: %d\n", packet->port);

    printf("\n\nPayload:\n");

    for (int i = 0; i < 8; i++) {
        if (packet->type & (1<<(i))) {
            printf("%d, ", packet->payload[i].as_int);
        } else {
            printf("%.5f, ", packet->payload[i].as_float);
        }
    }

    printf("\n\n%s\n", packet->signature);

    return 0;
}