##Command Usage
- C program to LLVM code  (.ll) using clang
`$ clang -S -emit-llvm myfile.c -o myfile.ll`

- C program to llvm bytecode
`$ clang -c -emit-llvm myfile.c -o myfile.bc`

- The bytecode files are not human readable. However you can transform from bytecode to text and 
vice versa using some LLVM tools:
`$ llvm-dis myfile.bc -o myfile.ll`
`$ llvm-as myfile.ll -o myfile.bc`

- For building a llvm based project outside the llvm build framework
  - Copy llvm-src/projects/sample
```
$ mkdir build
$ cd build
$ ../sample/configure --with-llvmsrc=/home/sdasgup3/Documents/llvm/llvm.src/ --with-llvmobj=/home/sdasgup3/Documents/llvm/llvm.obj/
$ make ENABLE_OPTIMIZED=1   
```
- Run an LLVM Pass With Clang
  - `clang -Xclang -load -Xclang mypass.so *.c`
  - An [example](https://github.com/sdasgup3/Programming/tree/master/Compiler/LLVM/LLVMProjects/lib/Analysis/opModifier)


##Building LLVM
- Prechecks
```
module load gcc
check the $PATH variable that it should not cpntain clang
setenv LD_LIBRARY_PATH /software/gcc-4.8.2/lib64:/software/gcc-4.8.2/lib

and to ensure that it contains GLIBCXX_3.4.15 do
strings /software/gcc-4.8.2/lib/libstdc++.so.6 | grep GLIBC
```
- Steps
```
svn co http://llvm.org/svn/llvm-project/llvm/trunk llvm-src
cd llvm-src/tools/
svn co http://llvm.org/svn/llvm-project/cfe/trunk clang
cd ../projects/
svn co http://llvm.org/svn/llvm-project/compiler-rt/trunk compiler-rt
svn co http://llvm.org/svn/llvm-project/test-suite/trunk test-suite
cd ../..
mkdir llvm-build
cd !$
../llvm.src/configure  --prefix=/home/sdasgup3/work/llvm-install --enable-targets=all --enable-jit  --disable-optimized --enable-assertions
../llvm-src/configure                                                                               --disable-optimized --enable-assertions
../llvm-src/configure CC=/software/gcc-4.8.2/bin/gcc CXX=/software/gcc-4.8.2/bin/g++ --with-gcc-toolchain=/software/gcc-4.8.2 --with-extra-ld-options="-Wl,-rpath,/software/gcc-4.8.2/lib64" --enable-jit --enable-targets=all --enable-optimized --enable-assertions
make update
make
llvm[0]: ***** Note: Debug build can be 10 times slower than an
llvm[0]: ***** optimized build. Use make ENABLE_OPTIMIZED=1 to
llvm[0]: ***** make an optimized build. Alternatively you can
llvm[0]: ***** configure with --enable-optimized.

make check-all
make ENABLE_OPTIMIZED=1 -j 2
make ENABLE_OPTIMIZED=1 check-all
make ENABLE_OPTIMIZED=1 install
```

##Build  test-suite

- Steps
```
cd llvm-src/projects
svn co http://llvm.org/svn/llvm-project/test-suite/trunk test-suite
../llvm-src/configure CC=/software/gcc-4.8.2/bin/gcc CXX=/software/gcc-4.8.2/bin/g++ 
--with-gcc-toolchain=/software/gcc-4.8.2 --with-extra-ld-options=-Wl,-rpath,/software/gcc-4.8.2/lib64 
--enable-jit --enable-targets=all --enable-optimized --enable-assertions 
--with-llvmgccdir=/home/sdasgup3/llvm/llvm-gcc4.2-2.9-x86_64-linux/
```

- In llvm-src/projects/test-suite/Makefile.rules, change
```
# Default optimization level:
OPTFLAGS := -O3

to 

# Default optimization level:
OPTFLAGS := -O0

AND
change 

ifeq ($(ENABLE_PROFILING),1)
  ENABLE_OPTIMIZED = 1
endif

to 

#ifeq ($(ENABLE_PROFILING),1)
ENABLE_OPTIMIZED = 1
#endif
```

- Then in llvm-src/projects/test-suite/MultiSource, put TEST.pre.Makefile // Code give in Repo

- Go to llvm-build/projects/test-suite
```
make TEST=pre [clean] // to run SingleSource and Multisourse together or 
cd SingleSource;  make TEST=pre [clean] // to run SingleSource Only
```

- The result is that in llvm-build/projects/test-suite/*...*/Output/*.linked.rbc will be created which is the  clang compiled linked bc of the source code.
_Note that: if you put other instrcutions in  TEST.pre.Makefile then those will also be executed. For exampe if you intend to run opt pass on the linked.rbc._

##Ruunig spec2006

- Steps
```
cp -rf llvm-src/projects/test-suite/External/SPEC/CINT2006/*  llvm-src/projects/test-suite/External/SPEC/CINT2006/ ; to copy the dir structure
cd llvm-src/projects/test-suite/External/SPEC/CINT2006
ls |& tee log // will put all the 12 spec dir in log file ; do log and remove Makefile
foreach file (`cat log`)
  cp -rf /home/sdasgup3/speccpu2006/benchspec/CPU2006/$file/* $file/ 
end
cd llvm-build/projects/test-suite/External/SPEC/CINT2006
```
- edit the following in */Makefile
```
LEVEL = ../../../..

to 

LEVEL = ../../../..
SPEC_BENCH_DIR = /home/sdasgup3/llvm/llvm-llvmpa/llvm-src/projects/test-suite/External/SPEC/CINT2006/<testcase name>
```
- ```make TEST=pre```

**Note: for CFP2006, use the same above methods.**

## Links ##
- For more about various LLVM tools: http://llvm.org/docs/CommandGuide/index.html
- For moe information about creating a LLVM project :  http://llvm.org/releases/3.7.1/docs/Projects.html
- Frequently asked Questions: http://llvm.org/releases/3.1/docs/FAQ.html
- Building llvm with using clang: http://btorpey.github.io/blog/2015/01/02/building-clang/
- Using cmake to build llvm:  http://llvm.org/docs/CMake.html#embedding-llvm-in-your-project
- Run an LLVM Pass Automatically with Clang: http://adriansampson.net/blog/clangpass.html
- LLVM for Grad Students: http://adriansampson.net/blog/llvm.html
- Build testsuite: http://llvm.org/docs/TestSuiteMakefileGuide.html
