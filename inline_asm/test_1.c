#include<stdio.h>

int main () {
  
  int a=10, b=20;
  printf("%d-%d", a, b);
        asm ("movl %1, %%eax; movl %%eax, %0;"
             :"=r"(b)        /* output */
             :"r"(a)         /* input */
             :"%eax"         /* clobbered register */
             );       

  printf("%d-%d", a, b);

  return 0;
}
