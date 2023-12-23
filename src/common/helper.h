#include <stdlib.h>
#ifndef SRC_COMMON_HELPER_H_
#define SRC_COMMON_HELPER_H_


typedef struct flags
{
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
  int h;
  int s;
  int f;
  int o;
  int b;
  int t;
  int em;
  int is_option;
}flags;


void flags_init(flags *f){
  f->e = 0;
  f->l = 0;
  f->i = 0;
  f->h = 0;
  f->v = 0;
  f->c = 0;
  f->o = 0;
  f->b = 0;
  f->n = 0;
  f->t = 0;
  f->f = 0;
  f->s = 0;
  f->em = 0;
  f->is_option = 0;
}


#endif // SRC_COMMON_HELPER_H_