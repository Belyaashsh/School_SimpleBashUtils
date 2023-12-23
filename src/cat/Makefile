CC = gcc
CFLAGS = -Wall -Werror -Wextra

all: clean s21_cat

s21_cat: clean
	$(CC) $(CFLAGS) cat.c -o s21_cat

clean:
	rm -rf s21_cat

rebuild: clean all