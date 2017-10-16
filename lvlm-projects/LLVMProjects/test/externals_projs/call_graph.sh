#!/usr/bin/bash
clang -S -emit-llvm calls.c

# You can use this instead if LLVM was compiled with xdot in PATH
# @LLVM_TOOLS_BINARY_DIR@/opt -view-callgraph calls.ll > /dev/null

# Otherwise this will work
opt -dot-callgraph calls.ll > /dev/null
dot -Tps callgraph.dot -o callgraph.ps
