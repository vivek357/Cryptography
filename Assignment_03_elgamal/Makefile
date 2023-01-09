CC     := gcc
MACROS := 
CFLAGS := -I. -Wundef -Wall -Wextra -O3 $(MACROS)


all:
	@$(CC) $(CFLAGS) big_int1024.c elgamal.c -o ./big_elgamal.out

clean:
	@rm -f *.out
