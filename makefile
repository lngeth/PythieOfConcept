GCC=gcc -Wextra -Wall -std=c99

all: projet_LN clean

projet_LN: projet_LN.o
	$(GCC) $< -o $@ -lm

%.o: %.c
	gcc -c $< -o $@

clean:
	rm *.o
