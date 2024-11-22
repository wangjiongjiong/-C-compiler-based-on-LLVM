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
  %0 = icmp sle i32 %b5, 10
  %1 = sext i1 %0 to i32
  %2 = icmp ne i32 %1, 0
  br i1 %2, label %then, label %else

then:                                             ; preds = %cond
  store i32 30, ptr %aa, align 4
  %aa6 = load i32, ptr %aa, align 4
  br label %cond7

else:                                             ; preds = %cond
  store i32 10, ptr %b, align 4
  %b12 = load i32, ptr %b, align 4
  br label %last

last:                                             ; preds = %else, %last9
  %aa13 = load i32, ptr %aa, align 4
  %b14 = load i32, ptr %b, align 4
  %3 = mul nsw i32 %b14, 9
  %4 = add nsw i32 %aa13, %3
  %5 = call i32 (ptr, ...) @printf(ptr @0, i32 %4)
  ret i32 0

cond7:                                            ; preds = %then
  %aa10 = load i32, ptr %aa, align 4
  %6 = icmp ne i32 %aa10, 5
  %7 = sext i1 %6 to i32
  %8 = icmp ne i32 %7, 0
  br i1 %8, label %then8, label %last9

then8:                                            ; preds = %cond7
  store i32 6, ptr %aa, align 4
  %aa11 = load i32, ptr %aa, align 4
  br label %last9

last9:                                            ; preds = %then8, %cond7
  br label %last
}
