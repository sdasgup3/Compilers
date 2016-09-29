#!/bin/bash

echo "Running on has_cycle.c"
clang -emit-llvm -c has_cycle.c
opt -load ../../build/lib/LLVMcycleDetect.so -cycle-detect -debug-pass=Structure has_cycle.bc > /dev/null

echo "Running on no_cycle.c"
clang -emit-llvm -c no_cycle.c
opt -load ../../build/lib/LLVMcycleDetect.so  -cycle-detect -debug-pass=Structure no_cycle.bc > /dev/null
