
all: fib sum

fib:
	gcc -m32 -no-pie -nostdlib -o fib fib.c
	gcc -m32 -o load load.c -lm
	./load fib

sum:
	gcc -m32 -no-pie -nostdlib -o sum sum.c
	gcc -m32 -o load load.c -lm
	./load sum

clean:
	rm -f fib sum load
