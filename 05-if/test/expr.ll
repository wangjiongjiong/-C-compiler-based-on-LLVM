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
  store i32 1, ptr %aa, align 4
  %aa2 = load i32, ptr %aa, align 4
  store i32 5, ptr %b, align 4
  %b3 = load i32, ptr %b, align 4
  store i32 %b3, ptr %aa, align 4
  %aa4 = load i32, ptr %aa, align 4
  br label %cond

cond:                                             ; preds = %entry
  %b5 = load i32, ptr %b, align 4
  %0 = icmp ne i32 %b5, 0
  br i1 %0, label %then, label %else

then:                                             ; preds = %cond
  store i32 30, ptr %aa, align 4
  %aa6 = load i32, ptr %aa, align 4
  br label %last

else:                                             ; preds = %cond
  store i32 10, ptr %b, align 4
  %b7 = load i32, ptr %b, align 4
  br label %last

last:                                             ; preds = %else, %then
  %aa8 = load i32, ptr %aa, align 4
  %b9 = load i32, ptr %b, align 4
  %1 = mul nsw i32 %b9, 9
  %2 = add nsw i32 %aa8, %1
  %3 = call i32 (ptr, ...) @printf(ptr @0, i32 %2)
  ret i32 0
}
