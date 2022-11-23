GCC=gcc -Wextra -Wall -std=c99

all: projet_LN clean

projet_LN: projet_LN.o
	gcc -c $< -o $@

%.o: %.c
	$(GCC) $< -o $@

clean:
	rm *.o
