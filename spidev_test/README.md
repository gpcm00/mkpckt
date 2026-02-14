# Simple SPI Userspace Transfer Tool

Minimal userspace SPI test utility for sending raw byte buffers over an SPI bus and reading the simultaneous response (full-duplex transfer).

Designed for quick bus bring-up, debugging, and custom packet testing.

---

## Features

- Sends arbitrary byte buffers over SPI
- Reads response data simultaneously (full-duplex SPI behavior)
- Accepts input from:
  - stdin (pipe)
  - file input
- Compile-time configurable SPI device path
- Minimal dependencies (glibc + Linux spidev)

---

## Requirements

- Linux system with spidev enabled
- SPI device exposed under `/dev/spidevX.Y`
- GCC or compatible compiler

---

## Build

Default build:

    # Optional:
       ./compile_spidev_test.sh /dev/spidev1.0

    # or to use the default /dev/spidev2.0
       ./compile_spidev_test.sh


---

## Default SPI Settings

| Setting | Value |
|---|---|
| Mode | 0 |
| Bits per word | 8 |
| Speed | 1 MHz |

---

## Example Use Cases

- SPI device bring-up
- Bus validation
- Sending custom command frames
- Testing firmware SPI handlers
- Rapid hardware debugging

---

## Notes

- Stdin input is limited to 256 bytes.
- File input reads entire file into memory.
- SPI transfer length equals input length.
- Requires read/write permission on SPI device node.

---

## License

Use freely for testing, debugging, and development.

---

## Usage

### Pipe data via stdin

    ./mkpkt "\10 \b1 \f-1.4 \xDEADBEEF" | ./spidev_test

---

### Send data from file

    ./spidev_test packet.bin

