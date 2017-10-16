@_test_.1 = internal constant [10 x i8] c"Value:%d\0A\00" 
%0 = type <{ i32 (i32)*}>
@data_0x8c = internal constant %0 <{ i32 (i32)* @koo }>


define i32 @koo(i32) {
entry:
  %1 =  add i32 %0, 10
  ret i32 %1
}


define i32 @bar(i32) {
entry:
  %1 =  add i32 %0, 10
  %2 = load i64, i64* inttoptr (i64 add (i64 ptrtoint (%0* @data_0x8c to i64), i64 0) to i64*)
  %3 = inttoptr i64 %2 to i32 (i32)*
  %4 = call i32 %3(i32 %1)
  ret i32 %4
}


define i32 @main() {

entry:
  %0 = call i32 @bar(i32 80);
  call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([10 x i8], [10 x i8]* @_test_.1, i32 0, i32 0), i32 %0)

  ret i32 %0
}


declare i32 @printf(i8* noalias, ...)
