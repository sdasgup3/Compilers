My projects involving compiler frameworks like GCC and LLVM.

###Cheetsheet
- ```echo | gcc -E -dM -``` gives the preprocessor macros defined by the compiler.   ```__x86_64__``` is one of those. Which is set/reset while compiling using ```gcc -m64 / gcc -m32```

#### What's wrong with casting malloc's return value?
Suppose that you call malloc but forget to #include <stdlib.h>. The compiler is likely to assume that malloc is a function returning int, which is of course incorrect, and will lead to trouble. Now, if your call to malloc is of the form
```c
char *p = malloc(10);
```
the compiler will notice that you're seemingly assigning an integer to a pointer, and will likely emit a warning of the form _"assignment of pointer from integer lacks a cast"_, which will alert you to the problem. (The problem is of course that you forgot to ```#include <stdlib.h>```, not that you forgot to use a cast.) If, on the other hand, your call to malloc includes a cast:

```c
	char *p = (char *)malloc(10);
```

the compiler is likely to assume that you know what you're doing, that you really do want to convert the int returned by malloc to a pointer, and the compiler therefore probably won't warn you. But of course malloc does not return an int, so trying to convert the int that it doesn't return to a pointer is likely to lead to a different kind of trouble, which will be harder to track down.
(Of course, compilers are increasingly likely--especially under C99--to emit warnings whenever functions are called without prototypes in scope, and such a warning would alert you to the lack of <stdlib.h> whether casts were used or not.) 

With the following we dont need to change the  typename in malloc everytime the type of returned pointer changes
```c
int *sieve = malloc(sizeof (*sieve) * 10); //rather than int *sieve = malloc(sizeof(int) * 10);
```
