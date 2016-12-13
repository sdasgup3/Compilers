#include<stdio.h>

int main () {
  
  int x=10, five_times_x = 0;
  printf("%d-%d", x, five_times_x);

  // t = leal(a,b,c) == t = a + b*c
   asm ("leal (%1,%1,4), %0"
             : "=r" (five_times_x)
             : "r" (x) 
             );

  printf("%d-%d", x, five_times_x);

  return 0;
}
