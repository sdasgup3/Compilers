; ModuleID = 'test.c'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nounwind uwtable
define i32 @fib(i32 %n) #0 {
entry:
  %retval = alloca i32, align 4
  %n.addr = alloca i32, align 4
  %f = alloca i32, align 4
  store i32 %n, i32* %n.addr, align 4
  call void @llvm.dbg.declare(metadata !{i32* %n.addr}, metadata !12), !dbg !13
  %0 = load i32* %n.addr, align 4, !dbg !14
  %cmp = icmp ult i32 %0, 2, !dbg !14
  br i1 %cmp, label %if.then, label %if.end, !dbg !14

if.then:                                          ; preds = %entry
  %1 = load i32* %n.addr, align 4, !dbg !16
  store i32 %1, i32* %retval, !dbg !16
  br label %return, !dbg !16

if.end:                                           ; preds = %entry
  call void @llvm.dbg.declare(metadata !{i32* %f}, metadata !17), !dbg !18
  %2 = load i32* %n.addr, align 4, !dbg !18
  %sub = sub i32 %2, 1, !dbg !18
  %call = call i32 @fib(i32 %sub), !dbg !18
  %3 = load i32* %n.addr, align 4, !dbg !18
  %sub1 = sub i32 %3, 2, !dbg !18
  %call2 = call i32 @fib(i32 %sub1), !dbg !18
  %add = add i32 %call, %call2, !dbg !18
  store i32 %add, i32* %f, align 4, !dbg !18
  %4 = load i32* %f, align 4, !dbg !19
  store i32 %4, i32* %retval, !dbg !19
  br label %return, !dbg !19

return:                                           ; preds = %if.end, %if.then
  %5 = load i32* %retval, !dbg !20
  ret i32 %5, !dbg !20
}

; Function Attrs: nounwind readnone
declare void @llvm.dbg.declare(metadata, metadata) #1

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!9, !10}
!llvm.ident = !{!11}

!0 = metadata !{i32 786449, metadata !1, i32 12, metadata !"clang version 3.4.2 (tags/RELEASE_34/dot2-final)", i1 false, metadata !"", i32 0, metadata !2, metadata !2, metadata !3, metadata !2, metadata !2, metadata !""} ; [ DW_TAG_compile_unit ] [/home/sdasgup3/GitHub/Programming/Compiler/LLVM/LLVMProjects.obj/test.c] [DW_LANG_C99]
!1 = metadata !{metadata !"test.c", metadata !"/home/sdasgup3/GitHub/Programming/Compiler/LLVM/LLVMProjects.obj"}
!2 = metadata !{i32 0}
!3 = metadata !{metadata !4}
!4 = metadata !{i32 786478, metadata !1, metadata !5, metadata !"fib", metadata !"fib", metadata !"", i32 1, metadata !6, i1 false, i1 true, i32 0, i32 0, null, i32 256, i1 false, i32 (i32)* @fib, null, null, metadata !2, i32 1} ; [ DW_TAG_subprogram ] [line 1] [def] [fib]
!5 = metadata !{i32 786473, metadata !1}          ; [ DW_TAG_file_type ] [/home/sdasgup3/GitHub/Programming/Compiler/LLVM/LLVMProjects.obj/test.c]
!6 = metadata !{i32 786453, i32 0, null, metadata !"", i32 0, i64 0, i64 0, i64 0, i32 0, null, metadata !7, i32 0, null, null, null} ; [ DW_TAG_subroutine_type ] [line 0, size 0, align 0, offset 0] [from ]
!7 = metadata !{metadata !8, metadata !8}
!8 = metadata !{i32 786468, null, null, metadata !"unsigned int", i32 0, i64 32, i64 32, i64 0, i32 0, i32 7} ; [ DW_TAG_base_type ] [unsigned int] [line 0, size 32, align 32, offset 0, enc DW_ATE_unsigned]
!9 = metadata !{i32 2, metadata !"Dwarf Version", i32 4}
!10 = metadata !{i32 1, metadata !"Debug Info Version", i32 1}
!11 = metadata !{metadata !"clang version 3.4.2 (tags/RELEASE_34/dot2-final)"}
!12 = metadata !{i32 786689, metadata !4, metadata !"n", metadata !5, i32 16777217, metadata !8, i32 0, i32 0} ; [ DW_TAG_arg_variable ] [n] [line 1]
!13 = metadata !{i32 1, i32 0, metadata !4, null}
!14 = metadata !{i32 2, i32 0, metadata !15, null}
!15 = metadata !{i32 786443, metadata !1, metadata !4, i32 2, i32 0, i32 0} ; [ DW_TAG_lexical_block ] [/home/sdasgup3/GitHub/Programming/Compiler/LLVM/LLVMProjects.obj/test.c]
!16 = metadata !{i32 3, i32 0, metadata !15, null}
!17 = metadata !{i32 786688, metadata !4, metadata !"f", metadata !5, i32 5, metadata !8, i32 0, i32 0} ; [ DW_TAG_auto_variable ] [f] [line 5]
!18 = metadata !{i32 5, i32 0, metadata !4, null}
!19 = metadata !{i32 6, i32 0, metadata !4, null}
!20 = metadata !{i32 7, i32 0, metadata !4, null}
