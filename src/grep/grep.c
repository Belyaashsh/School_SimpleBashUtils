#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../common/helper.h"

#define NMAX 99999
#define RESET "\033[0m"
#define RED "\033[1;31m"
#define WHITE "\033[1;37m"

void flag_checker(int argc, char **argv, flags *f, int *regex_flags,
                  char *patt);
void e_pattern(char *optarg, int *e_count, char *patt, flags *f);
void f_pattern(char *optarg, int *e_count, char *patt, flags *f);
void s21_grep(char **argv, flags f, int *index, regex_t *preg, int f_count);
void grepchik(FILE *file, flags f, regex_t *preg, int f_count, char *filename);
void print_in_color(char *line, regmatch_t match);
void parser(int argc, char **argv, flags *f, int *regex_flags);
int file_check(int argc, char **argv, flags f, int *index);
void patt_check(int argc, char **argv, flags f, char *patt, int *index,
                regex_t *preg);

int main(int argc, char **argv) {
  flags f;
  flags_init(&f);
  int regex_flags = 0;
  char *patt = malloc(sizeof(char) * NMAX);
  int *index = malloc(sizeof(int) * NMAX);
  regex_t preg_stor;
  regex_t *preg = &preg_stor;

  flag_checker(argc, argv, &f, &regex_flags, patt);
  int f_count = file_check(argc, argv, f, index);

  patt_check(argc, argv, f, patt, index, preg);

  s21_grep(argv, f, index, preg, f_count);
  regfree(preg);
  free(patt);
  free(index);
  return 0;
}

void flag_checker(int argc, char **argv, flags *f, int *regex_flags,
                  char *patt) { // Проверка флагов
  char ch = 0;
  int e_count = 0;

  while (1) {
    ch = getopt_long(argc, argv, "-e:ivclnf:hso", NULL, NULL);
    opterr = 0;
    switch (ch) {
      case '?':
        break;
      case 'e':
        f->e = 1;
        f->is_option = 1;
        *regex_flags |= REG_EXTENDED;
        e_pattern(optarg, &e_count, patt, f);
        break;
      case 'i':
        f->i = 1;
        f->is_option = 1;
        *regex_flags |= REG_ICASE;
        break;
      case 'v':
        f->v = 1;
        f->is_option = 1;
        break;
      case 'c':
        f->c = 1;
        f->is_option = 1;
        f->n = 0;
        break;
      case 'l':
        f->l = 1;
        f->is_option = 1;
        f->n = 0;
        break;
      case 'n':
        if ((f->c != 1) && (f->l != 1)) {
          f->n = 1;
          f->is_option = 1;
        }
        break;
      case 'f':
        f->f = 1;
        f->is_option = 1;
        f_pattern(optarg, &e_count, patt, f);
        break;
      case 'h':
        f->h = 1;
        f->is_option = 1;
        break;
      case 's':
        f->s = 1;
        f->is_option = 1;
        break;
      case 'o':
        f->o = 1;
        f->is_option = 1;
        break;
      default:
        break;
    }
    if (ch == -1) break;
  }
}
void e_pattern(char *optarg, int *e_count, char *patt, flags *f) {
  if (*e_count) { // Функция для обработки флана -е
    char *ch = "|";
    strcat(patt, ch);
  }
  if (!*optarg) {
    optarg = ".";
    f->em++;
  }
  strcat(patt, optarg);
  *e_count += 1;
}

void f_pattern(char *optarg, int *e_count, char *patt, flags *f) {
  FILE *fp = fopen(optarg, "r"); // Функция для обработки флага -f
  if (fp == NULL && !f->s) {
    printf("PATTERN_FILE NOT FOUND\n");
    exit(1);
  }
  char *line = NULL;
  size_t length = 0;
  while (getline(&line, &length, fp) > 0) {
    int line_len = strlen(line);
    if (line[line_len - 1] == '\n') {
      line[line_len - 1] = '\0';
    }
    if (line[0] > 31 && line[0] < 127) e_pattern(line, e_count, patt, f);
  }
  free(line);
  fclose(fp);
}

void s21_grep(char **argv, flags f, int *index, regex_t *preg, int f_count) { // Функция, открывающая файл и вызывающая функцию печати.
  int j = 0;
  for (int i = 0; i < f_count; i++) {
    j = index[i];
    char *filename = argv[j];
    FILE *file = fopen(argv[j], "r");
    if (file == NULL) {
      if (!f.s)
        printf("s21_grep: %s: File not found\n", argv[j]);
      else
        continue;
    } else {
      grepchik(file, f, preg, f_count, filename);
    }
    fclose(file);
    j++;
  }
}

void grepchik(FILE *file, flags f, regex_t *preg, int f_count, char *filename) { // Функция печати
  char *line = NULL;
  size_t length = 0;
  int counter = 0;
  int match_counter = 0;
  int line_num = 1;
  regmatch_t match[1] = {0};
  int exec = 0;
  char last = 0;

  while (getline(&line, &length, file) > 0) {
    exec = regexec(preg, line, 1, match, 0);

    if (f.n && ((!f.v && !exec) || (f.v && exec))) {
      if (f_count > 1 && !f.h) {
        printf("%s:%d:", filename, line_num);
      } else
        printf("%d:", line_num);
    }

    if (f.o && !f.c && !f.l && (!exec && !f.v)) {
      if (f_count > 1) printf("%s:", filename);
      for (int i = match->rm_so; i < match->rm_eo; i++) {
        printf("%c", line[i]);
      }
      match_counter++;
      printf("\n");
    } else if (f.o && !f.c && !f.l && (exec && f.v)) {
      match_counter++;
      if (f_count > 1)
        printf("%s:%s", filename, line);
      else
        printf("%s", line);
    }

    if (f.v && !f.c && !f.l && !f.o) {
      if (exec) {
        match_counter++;
        if (f_count > 1 && !f.h && !f.n)
          printf("%s:%s", filename, line);
        else
          printf("%s", line);
      }
    } else if (!exec && !f.c && !f.l && !f.o) {
      match_counter++;
      if (f_count > 1 && !f.h && !f.n)
        printf("%s:%s", filename, line);
      else
        printf("%s", line);
    }

    if ((!f.v && !exec) || (f.v && exec)) {
      last = line[strlen(line) - 1];
    }

    if (f.v && f.c) {
      if (exec) counter++;
    } else if (!exec && f.c)
      counter++;
    line_num++;

    if (f.l && ((!f.v && !exec) || (f.v && exec))) break;
  }

  if (f.c && !f.l) {
    if (f_count > 1 && !f.h)
      printf("%s:%d%s", filename, counter, "\n");
    else
      printf("%d%s", counter, "\n");
  } else if (f.c && f.l && f_count <= 1 && ((!f.v && !exec) || (f.v && exec))) {
    printf("%d%s", counter, "\n");
    printf("%s%s", filename, "\n");
  } else if (f.l && !f.c && ((!f.v && !exec) || (f.v && exec))) {
    printf("%s%s", filename, "\n");
  } else if (f.c && f.l && f_count > 1) {
    printf("%s:%d%s", filename, counter, "\n");
  }
  if (last != '\n' && match_counter != 0 && !f.c && !f.l) printf("\n");

  free(line);
}

int file_check(int argc, char **argv, flags f, int *index) { // Функция для поиска имен файолов среди аргументов
  int counter = 0;
  int j = 0;
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] != '-') {
      int len = (int)strlen(argv[i - 1]);
      if (argv[i - 1][0] == '-') {
        if ((argv[i - 1][len - 1] != 'e') && (argv[i - 1][len - 1] != 'f') &&
            (f.e == 1 || f.f == 1)) {
          counter++;
          index[j] = i;
          j++;
        }
      } else {
        if (argv[i - 1][len - 1] != 'p') {
          counter++;
          index[j] = i;
          j++;
        }
      }
    }
  }
  return counter;
}

void patt_check(int argc, char **argv, flags f, char *patt, int *index,
                regex_t *preg) { // Функция поиска паттерна для grep и компиляции регулярного выражения
  char **pen = &argv[1];

  if (f.is_option && (!f.e && !f.f)) {
    for (int i = 1; i < argc; i++) {
      if (argv[i][0] != '-') {
        if (i < index[0]) {
          pen = &argv[i];
        }
      }
    }
  } else {
    pen = &argv[1];
  }
  int regex = 0;
  if (f.i) regex |= REG_ICASE;
  if (f.e || f.f) regex |= REG_EXTENDED;
  if (f.e || f.f) {
    regcomp(preg, patt, regex);
  } else {
    regcomp(preg, *pen, regex);
  }
}
