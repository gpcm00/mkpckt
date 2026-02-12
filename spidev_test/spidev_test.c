#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <linux/spi/spidev.h>

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifndef SPIDEV_PATH
#define SPIDEV_PATH "/dev/spidev2.0"
#endif

#define MAX_INPUT_SZ 256

void print_buffer(uint8_t* buffer, uint32_t len, char* header) 
{	
	printf("%s:\n", header);
	for (uint32_t i = 0; i < len; i++) {
		printf("0x%02X", buffer[i]);
		if (((i+1) % 16) == 0) {
			putc('\n', stdout);
		} else {
			putc(' ', stdout);
		}
	}
	putc('\n', stdout);
}

/*	main pseudo code:
	
	if (piped_over_stdin) {
		txbuffer = read(stdin)
	} else {
		txbuffer = read(file pointed by argv[1])
	}
	
	ioctl(SPI_IOC_MESSAGE, &txbuffer, &rxbuffer)	
	// ioctl(SPI_IOC_MESSAGE) reads and writes at the same time 
	// since spi is full-duplex

	print(txbuffer)
	print(rxbuffer) 		
******************************************************************/

int main(int argc, char** argv)
{
	int spidev_fd, txbuffer_fd;
	uint8_t *txbuffer = NULL;
   	uint8_t	*rxbuffer = NULL;
	uint32_t xfer_len;
	struct stat txbuffer_stat;
	struct spi_ioc_transfer xfer;

    int exit_code = EXIT_SUCCESS;
	
	uint8_t bits = 8;
	uint32_t speed = 1000000;

    // piped instead of a file
	if (!isatty(0)) {
		txbuffer = (uint8_t*)malloc(MAX_INPUT_SZ);
        if (txbuffer == NULL) {
            perror("malloc(txbuffer)");
            exit_code = EXIT_FAILURE;
            goto Error_Exit_Function;
            
        }

		xfer_len = read(0, txbuffer, MAX_INPUT_SZ);
        if (xfer_len <= 0) {
            perror("read(stdin)");
            exit_code = EXIT_FAILURE;
            goto Error_Clean_TxBuffer;        
        }

    // read file
	} else {
		if (argc != 2) {
			fprintf(stderr, "usage: %s <transmit buffer>\n", argv[0]);
			exit_code = EXIT_FAILURE;
            goto Error_Exit_Function;
		}

		if (access(argv[1], R_OK) < 0) {
			fprintf(stderr, "access(%s, R_OK): %s\n", argv[1], strerror(errno));
			exit_code = EXIT_FAILURE;
            goto Error_Exit_Function;
		}
		
		txbuffer_fd = open(argv[1], O_RDONLY);
		if (txbuffer_fd < 0) {
			fprintf(stderr, "open(%s, O_RDONLY): %s\n", argv[1], strerror(errno));
			exit_code = EXIT_FAILURE;
            goto Error_Exit_Function;
		}

		// only allocate as much data as needed
		if (fstat(txbuffer_fd, &txbuffer_stat) < 0) {
			perror("fstat");
            close(txbuffer_fd);
			exit_code = EXIT_FAILURE;
            goto Error_Exit_Function;
		}

		// check size of file
		xfer_len = txbuffer_stat.st_size;
        if (xfer_len == 0) {
            fprintf(stderr, "Empty file\n");
            close(txbuffer_fd);
			exit_code = EXIT_FAILURE;
            goto Error_Exit_Function;
        }
		
		// initialize buffers
		txbuffer = (uint8_t*)malloc(xfer_len);
		if (txbuffer == NULL) {
			perror("malloc(txbuffer)");
			close(txbuffer_fd);
			exit_code = EXIT_FAILURE;
            goto Error_Exit_Function;
		}
	
		// get data from file
		size_t total_rd = read(txbuffer_fd, txbuffer, xfer_len);
		if (total_rd <= 0) {
			perror("read(txbuffer)");
			close(txbuffer_fd);
			exit_code = EXIT_FAILURE;
            goto Error_Clean_TxBuffer;
		}

		close(txbuffer_fd);	// done with file
	}

	// initialize rxbuffer
    rxbuffer = (uint8_t*)malloc(xfer_len);
	if (rxbuffer == NULL) {
		perror("malloc(rxbuffer)");
		exit_code = EXIT_FAILURE;
        goto Error_Clean_TxBuffer;
	}

	// open spidev device
	spidev_fd = open(SPIDEV_PATH, O_RDWR);
	if (spidev_fd < 0) {
		perror("open(/dev/spidev)");
		exit_code = EXIT_FAILURE;
        goto Error_Clean_RxBuffer;
	}
	
	// initialize spi ioc
	memset(&xfer, 0, sizeof(xfer));
	xfer.tx_buf = (uintptr_t)txbuffer;
	xfer.rx_buf = (uintptr_t)rxbuffer;
	xfer.len = xfer_len;
	xfer.delay_usecs = 0;
	xfer.speed_hz = speed;
	xfer.bits_per_word = bits;

	// set bus info
	uint8_t mode = 0;
	if (ioctl(spidev_fd, SPI_IOC_WR_MODE, &mode) < 0	  ||
	    ioctl(spidev_fd, SPI_IOC_WR_BITS_PER_WORD, &bits) < 0 ||
	    ioctl(spidev_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0 ) {
		perror("SPIDEV ioct configuration");
		exit_code = EXIT_FAILURE;
        goto Error_Clean_FileDesc;
	}

	// finally get data
	if (ioctl(spidev_fd, SPI_IOC_MESSAGE(1), &xfer) < 0) {
		perror("SPIDEV transfer");
		exit_code = EXIT_FAILURE;
        goto Error_Clean_FileDesc;
	}

	print_buffer(txbuffer, xfer_len, "Tx Buffer");
	print_buffer(rxbuffer, xfer_len, "Rx Buffer");

Error_Clean_FileDesc: close(spidev_fd);
Error_Clean_RxBuffer: free(rxbuffer);
Error_Clean_TxBuffer: free(txbuffer);
Error_Exit_Function:  exit(exit_code);

}