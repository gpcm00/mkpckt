# Make Packet

This tool generates sequences of bytes and lets you mix floats, hex strings, 32-bit integers, 16-bit integers, 8-bit integers, and characters in one command.

# Usage

        make            # compile code
        ./mkpckt -h     # print help

# Syntax
Any text after "#" is a comment and will be ignored

Spaces and newlines are also ignored unless you use "\\" before the space, new line or tab. For example

        Hello World     # parsed as 'HelloWorld'
        Hello\ World    # parsed as 'Hello World'

Any number after "\\" is parsed as a 32-bit integer value. For example

        i = \10         # parsed as i=0x0000000A
        i = \-1         # parsed as i=0xFFFFFFFF

Note: In comments, hex values represent the actual output bytes, not ASCII characters containing hex digits.

Use "\\f" for float, "\\w" for words (16-bit numbers), and "\\b" for bytes (8-bit number). For example

        \f1.00          # parsed as 0x3F800000
        \f-1.00         # parsed as 0xBF800000
        \w128           # parsed as 0x0080
        \w-128          # parsed as 0xFF80
        \b64            # parsed as 0x40
        \b-64           # parsed as 0xC0

You can use "\x" to specify raw hex byte values for output. For example

        \x1234        # parsed as 0x12 followed by 0x34

# Examples 

Try this to print hello world on your terminal

        ./mkpckt "\x48656c6c6f20576f726c640a"

Try piping to xxd with the -i flag to inspect the output

        ./mkpckt "\f3.14 \b234 \w65123" | xxd -i
        # result: 0x40, 0x48, 0xf5, 0xc3, 0xea, 0xfe, 0x63

Run the same command with -e flag and see how the endianness changes

        ./mkpckt -e "\f3.14 \b234 \w65123" | xxd -i
        # result: 0xc3, 0xf5, 0x48, 0x40, 0xea, 0x63, 0xfe

See ./tst/test.txt and inspect the output of the following command

        ./mkpckt -f ./tst/test.txt | xxd

Run it with the endian flag and check how the output changed

        ./mkpckt -f ./tst/test.txt -e | xxd

----------

Finally, have a look at ./tst/phex.c to see a simple program of how to generate an array of raw bytes. 

Then compile everything with ./tst/compile_main_test.sh. Inspect raw_bytes.c to see what was generated. 

main_test.c contains the main code, which just prints the content of a data structure called packet.

After compiling the tests, run

        ./tst/test_packet

This allows data structures to be filled using the contents of packet.txt. Although this is not the reason why I made this tool, it is one of the ways it is useful.