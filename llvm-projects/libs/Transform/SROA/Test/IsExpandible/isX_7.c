#include "stdio.h"
#include "stdlib.h"


int main()
{
  typedef struct {
    int r;
  } One;

  One i ;

  printf("%d ", NULL  ==  &i);
 
  return 0;
}

