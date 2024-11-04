; ModuleID = 'expr'
source_filename = "expr"

@0 = private unnamed_addr constant [13 x i8] c"expr val:%d\0A\00", align 1

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %aa = alloca i32, align 4
  %b = alloca i32, align 4
  store i32 4, ptr %b, align 4
  %b1 = load i32, ptr %b, align 4
  store i32 5, ptr %b, align 4
  %b2 = load i32, ptr %b, align 4
  store i32 %b2, ptr %aa, align 4
  %aa3 = load i32, ptr %aa, align 4
  %aa4 = load i32, ptr %aa, align 4
  %b5 = load i32, ptr %b, align 4
  %0 = mul nsw i32 %b5, 9
  %1 = add nsw i32 %aa4, %0
  %2 = call i32 (ptr, ...) @printf(ptr @0, i32 %1)
  ret i32 0
}
