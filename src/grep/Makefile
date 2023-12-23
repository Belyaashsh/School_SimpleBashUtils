CC = gcc
CFLAGS = -Wall -Werror -Wextra

all: clean s21_grep

s21_grep: clean
	$(CC) $(CFLAGS) grep.c -o s21_grep

clean:
	rm -rf s21_grep

rebuild: clean all