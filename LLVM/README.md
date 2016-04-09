##command-usage
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
### Using makefile system
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
### Build using cmake
- [Example Script](https://github.com/sdasgup3/Compilers/blob/master/LLVM/BuildInfra/config-llvm-cmake.sh)

### Build from current git version
```
% git clone http://llvm.org/git/llvm.git
% cd llvm/tools
% git clone http://llvm.org/git/clang.git
% cd llvm/projects
% git clone http://llvm.org/git/compiler-rt.git
% cd llvm/projects
% git clone http://llvm.org/git/test-suite.git
%cmake -G "Unix Makefiles" -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DLLVM_ENABLE_ASSERTIONS=ON  -DCMAKE_BUILD_TYPE="Release" -DCMAKE_INSTALL_PREFIX=/home/sdasgup3/Install/llvm.install/ -DLLVM_TARGETS_TO_BUILD="all" ../llvm/
%make -j 16 ; make install

/* To update the repo
** Assumption that cwd=llvm.obj
*/
pushd .
cd /home/sdasgup3/Install/llvm
git pull --rebase
cd tools/clang/
git pull --rebase
cd ../../projects/compiler-rt/
git pull --rebase
cd ../../projects/test-suite/
git pull --rebase
popd

make -j 16
make install -j 16
```

##Build-test-suite
- Outside llvm build tree
```
tar -xvf testsuite.3.5.0.src.tar.xz
mkdir testsuite.3.5.0.obj
cd !$
../test-suite.3.5.0.src/configure --with-llvmsrc=<> --with-llvmobj=<> -disable-optimized --enable-assertions
make
```
- Inside llvm build tree 
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

###Multisource build process
```
//generate bc
lang -S arg.c -o Output/arg.ll -emit-llvm
lvm-as Output/arg.ll -o Output/arg.bc

//linking
lvm-link Output/arg.bc Output/gram.bc -o Output/make_dparser.linked.rbc

//Optimize with compile time opts
RunSafely.sh 
--show-errors -t "/home/sdasgup3/Documents/llvmpa/llvmpa.tests/klee-test-suite.obj/tools/timeit" 500
/dev/null // Input to Runsafely
Output/make_dparser.linked.bc.opt  // Output
otp //commands 
-std-compile-opts -info-output-file=/home/sdasgup3/Documents/llvmpa/llvmpa.tests/klee-test-suite.obj/MultiSource/Applications/d/Output/make_dparser.linked.bc.info -stats -time-passes  Output/make_dparser.linked.rbc -o Output/make_dparser.linked.bc

//Optimize with compile time opts
/home/sdasgup3/Documents/llvmpa/llvmpa.tests/klee-test-suite.src/RunSafely.sh --show-errors -t "/home/sdasgup3/Documents/llvmpa/llvmpa.tests/klee-test-suite.obj/tools/timeit" 500 /dev/null Output/make_dparser.llvm.bc.opt \
          /home/sdasgup3/Documents/llvm/llvm.obj/Release+Asserts/bin/opt -std-link-opts  \
                -info-output-file=/home/sdasgup3/Documents/llvmpa/llvmpa.tests/klee-test-suite.obj/MultiSource/Applications/d/Output/make_dparser.llvm.bc.info \
                        -stats -time-passes Output/make_dparser.linked.bc  -o Output/make_dparser.llvm.bc

//Generate native executable using gcc
g++ -o Output/make_dparser.native Output/arg.o Output/gram.o 

//Run it
/home/sdasgup3/Documents/llvmpa/llvmpa.tests/klee-test-suite.src/RunSafely.sh -t "/home/sdasgup3/Documents/llvmpa/llvmpa.tests/klee-test-suite.obj/tools/timeit-target" 500 /dev/null Output/make_dparser.out-nat Output/make_dparser.native -v /home/sdasgup3/Documents/llvmpa/llvmpa.tests/klee-test-suite.src/MultiSource/Applications/d/grammar.g

//Use llc to produce assemble .s 
/home/sdasgup3/Documents/llvmpa/llvmpa.tests/klee-test-suite.src/RunSafely.sh --show-errors -t "/home/sdasgup3/Documents/llvmpa/llvmpa.tests/klee-test-suite.obj/tools/timeit" 500 /dev/null Output/make_dparser.llc.s.llc \
          /home/sdasgup3/Documents/llvm/llvm.obj/Release+Asserts/bin/llc -asm-verbose=false -O3  Output/make_dparser.llvm.bc -o Output/make_dparser.llc.s -info-output-file=/home/sdasgup3/Documents/llvmpa/llvmpa.tests/klee-test-suite.obj/MultiSource/Applications/d/Output/make_dparser.llc.s.info -stats -time-passes
//Use gcc to generate executable
gcc Output/make_dparser.llc.s -o Output/make_dparser.llc  -lm   -m64 -fomit-frame-pointer
//Execute it
/home/sdasgup3/Documents/llvmpa/llvmpa.tests/klee-test-suite.src/RunSafely.sh -t "/home/sdasgup3/Documents/llvmpa/llvmpa.tests/klee-test-suite.obj/tools/timeit-target" 500 /dev/null Output/make_dparser.out-llc Output/make_dparser.llc -v /home/sdasgup3/Documents/llvmpa/llvmpa.tests/klee-test-suite.src/MultiSource/Applications/d/grammar.g

//Run lli
/home/sdasgup3/Documents/llvmpa/llvmpa.tests/klee-test-suite.src/RunSafely.sh -t "/home/sdasgup3/Documents/llvmpa/llvmpa.tests/klee-test-suite.obj/tools/timeit-target" 500 /dev/null Output/make_dparser.out-jit /home/sdasgup3/Documents/llvm/llvm.obj/Release+Asserts/bin/lli -info-output-file=/home/sdasgup3/Documents/llvmpa/llvmpa.tests/klee-test-suite.obj/MultiSource/Applications/d/Output/make_dparser.out-jit.info -stats -time-passes -force-interpreter=false --disable-core-files  Output/make_dparser.llvm.bc -v /home/sdasgup3/Documents/llvmpa/llvmpa.tests/klee-test-suite.src/MultiSource/Applications/d/grammar.g
```

## Links ##
- For more about various LLVM tools: http://llvm.org/docs/CommandGuide/index.html
- For moe information about creating a LLVM project :  http://llvm.org/releases/3.7.1/docs/Projects.html
- Frequently asked Questions: http://llvm.org/releases/3.1/docs/FAQ.html
- Building llvm with using clang: http://btorpey.github.io/blog/2015/01/02/building-clang/
- Using cmake to build llvm:  http://llvm.org/docs/CMake.html#embedding-llvm-in-your-project
- Run an LLVM Pass Automatically with Clang: http://adriansampson.net/blog/clangpass.html
- LLVM for Grad Students: http://adriansampson.net/blog/llvm.html
- Build testsuite: http://llvm.org/docs/TestSuiteMakefileGuide.html

