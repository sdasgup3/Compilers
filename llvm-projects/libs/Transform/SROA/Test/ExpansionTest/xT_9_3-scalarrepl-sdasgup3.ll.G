; ModuleID = 'xT_9_3-scalarrepl-sdasgup3.bc'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.student_detail = type { i32, [20 x i8], float, %struct.student_college_detail }
%struct.student_college_detail = type { i32, [50 x i8] }

@main.stu_data = private unnamed_addr constant { i32, [20 x i8], float, { i32, [50 x i8], [2 x i8] } } { i32 1, [20 x i8] c"Raju\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00", float 9.050000e+01, { i32, [50 x i8], [2 x i8] } { i32 71145, [50 x i8] c"Anna University\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00", [2 x i8] undef } }, align 4
@.str = private unnamed_addr constant [13 x i8] c" Id is: %d \0A\00", align 1
@.str1 = private unnamed_addr constant [22 x i8] c" Percentage is: %f \0A\0A\00", align 1
@.str2 = private unnamed_addr constant [21 x i8] c" College Id is: %d \0A\00", align 1
@stu_data = common global %struct.student_detail zeroinitializer, align 4

; Function Attrs: nounwind uwtable
define i32 @main() #0 {
entry:
  %stu_data = alloca %struct.student_detail, align 4
  %0 = bitcast %struct.student_detail* %stu_data to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %0, i8* bitcast ({ i32, [20 x i8], float, { i32, [50 x i8], [2 x i8] } }* @main.stu_data to i8*), i64 84, i32 4, i1 false)
  %id = getelementptr inbounds %struct.student_detail* %stu_data, i32 0, i32 0
  %1 = load i32* %id, align 4
  %call = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([13 x i8]* @.str, i32 0, i32 0), i32 %1)
  %percentage = getelementptr inbounds %struct.student_detail* %stu_data, i32 0, i32 2
  %2 = load float* %percentage, align 4
  %conv = fpext float %2 to double
  %call1 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([22 x i8]* @.str1, i32 0, i32 0), double %conv)
  %clg_data = getelementptr inbounds %struct.student_detail* %stu_data, i32 0, i32 3
  %college_id = getelementptr inbounds %struct.student_college_detail* %clg_data, i32 0, i32 0
  %3 = load i32* %college_id, align 4
  %call2 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([21 x i8]* @.str2, i32 0, i32 0), i32 %3)
  ret i32 0
}

; Function Attrs: nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture, i8* nocapture readonly, i64, i32, i1) #1

declare i32 @printf(i8*, ...) #2

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind }
attributes #2 = { "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = metadata !{metadata !"clang version 3.5 (trunk 201464)"}
