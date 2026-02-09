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
    uint8_t type;

    uint8_t padding;        // keep data_t 32-bit aligned

    data_t payload[8];

    uint8_t signature[12];
} __attribute__((packed));

extern char raw_bytes[sizeof(struct packet)];

int main() {
    struct packet* packet = (struct packet*)raw_bytes;

    for (int i = 0; i < 8; i++) {
        if (packet->type & (1<<(i))) {
            printf("%d \n", packet->payload[i].as_int);
        } else {
            printf("%f \n", packet->payload[i].as_float);
        }
    }

    printf("%s\n", packet->signature);
}