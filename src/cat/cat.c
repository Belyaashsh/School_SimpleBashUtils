#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../common/helper.h"

void print_file(FILE *f);
void print_symbol(int c, int *pr, int b, int e, int n, int s, int t, int v,
                  int *line, int *is_empty);
int flag_checker(int argc, char **argv, int *b, int *e, int *n, int *s, int *t,
                 int *v);

void s21_cat(int file_num, int argc, char **argv, int b, int e, int n, int s,
             int t, int v);

int main(int argc, char **argv) {
  int b = 0;
  int e = 0;
  int n = 0;
  int s = 0;
  int t = 0;
  int v = 0;
  int pars = flag_checker(argc, argv, &b, &e, &n, &s, &t, &v);
  s21_cat(pars, argc, argv, b, e, n, s, t, v);
  return 0;
}

void print_file(FILE *f) { // Печать файла
  if (f != NULL) {
    int c = fgetc(f);
    while (c != EOF) {
      putc(c, stdout);
      c = fgetc(f);
    }
  }
}

int flag_checker(int argc, char **argv, int *b, int *e, int *n, int *s, int *t,
                 int *v) { // Проверка флагов
  char ch = 0;

  struct option flags[] = {{"number-nonblank", no_argument, 0, 'b'},
                           {"ends", no_argument, 0, 'e'},
                           {"GNU-ends", no_argument, 0, 'E'},
                           {"number", no_argument, 0, 'n'},
                           {"squeeze-blank", no_argument, 0, 's'},
                           {"tabs", no_argument, 0, 't'},
                           {"GNU-tabs", no_argument, 0, 'T'},
                           {"show-noprint", no_argument, 0, 'v'}};

  while (1) {
    ch = getopt_long(argc, argv, "beEnstTv", flags, 0);
    switch (ch) {
      case '?':
        return -1;
        break;
      case 'b':
        *b = 1;
        *n = 0;
        break;
      case 'e':
        *e = 1;
        *v = 1;
        break;
      case 'E':
        *e = 1;
        break;
      case 'n':
        if (*b != 1) *n = 1;
        break;
      case 's':
        *s = 1;
        break;
      case 't':
        *t = 1;
        *v = 1;
        break;
      case 'T':
        *t = 1;
        break;
      case 'v':
        *v = 1;
      default:
        break;
    }
    if (ch == -1) break;
  }
  return optind;
}

void s21_cat(int file_num, int argc, char **argv, int b, int e, int n, int s,
             int t, int v) { // Функция, открывающая файле и вызывающая функцию печати символа с учетом флагов
  if (file_num == -1) {
    exit(1);
  }
  for (int i = file_num; i < argc; i++) {
    FILE *f = fopen(argv[i], "r");
    if (f == NULL) {
      printf("s21_cat: %s: File not found\n", argv[i]);
    } else {
      int line = 0;
      int is_empty = 0;
      int c = fgetc(f), pr = '\n';
      while (c != EOF) {
        print_symbol(c, &pr, b, e, n, s, t, v, &line, &is_empty);
        c = fgetc(f);
      }
    }
    fclose(f);
  }
}

void print_symbol(int c, int *pr, int b, int e, int n, int s, int t, int v,
                  int *line, int *is_empty) { // Функция печати символа, с учетом флагов
  if (!(s && *pr == '\n' && c == '\n' && *is_empty)) {
    if (*pr == '\n' && c == '\n')
      *is_empty = 1;
    else
      *is_empty = 0;
    if (b) {
    }
    if (n && *pr == '\n') {
      *line += 1;
      printf("%6d\t", *line);
    }

    if (b && *pr == '\n' && c != '\n') {
      *line += 1;
      printf("%6d\t", *line);
    }
    // if (b && *pr == '\n' && c != '\n') {
    //   *line += 1;
    //   printf("%6d\t", *line);
    // } else if (b && (e) && c == '\n' && *pr == '\n') {
    //   printf("      \t");
    // }

    if (e && c == '\n') printf("$");
    if (t && c == '\t') {
      printf("^");
      c = '\t' + 64;
    }

    if (v) {
      if ((c > 127) && (c <= 159)) printf("M-^");
      if (((c < 32) && c != '\n' && c != '\t') || (c == 127)) printf("^");
      if (((c < 32) || (c > 126 && c <= 159)) && c != '\n' && c != '\t') {
        if (c > 126)
          c = c - 128 + 64;
        else
          c += 64;
      }
    }
    fputc(c, stdout);
  }
  *pr = c;
}
