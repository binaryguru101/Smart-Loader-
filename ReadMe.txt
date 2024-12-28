Simple Smart Loader

How to run:

To run the sum.c program, type:

make sum
To run the fib.c program, type:

make fib
This will compile the respective .c file along with load.c and execute their object files with the loader.

Compilation commands:

gcc -m32 -no-pie -nostdlib -o sum sum.c
gcc -m32 -o loader loader.c -lm
./loader sum


If you wish to run it manually, enter the above commands in order. Error checking is implemented so that you'll receive an error if you try to execute ./loader <wrong input>. The loader will also handle non-ELF files gracefully.

Features of the loader:

Entry Point: When the loader starts, it will display the entry point address of the ELF file.
Page Information: The size of each page and the virtual memory address of the pages will be shown.
Program End Stats: After the program ends, the loader will display:
Total Page Faults: The number of page faults encountered during execution.
Total Internal Fragmentation (Kb): The internal fragmentation (in KB) in memory.
Total Number of Pages Created: The total number of pages created during execution.



Contributors:
Ronit: Implemented the SIGSEGV_HANDLER to handle segmentation faults.
Ujjwal: Handled error checks, implemented subsidiary functions, and created the Makefile