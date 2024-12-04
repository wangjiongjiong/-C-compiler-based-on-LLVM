; ModuleID = 'expr'
source_filename = "expr"

@0 = private unnamed_addr constant [13 x i8] c"expr val:%d\0A\00", align 1

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %aa = alloca i32, align 4
  store i32 1, ptr %aa, align 4
  %aa1 = load i32, ptr %aa, align 4
  %b = alloca i32, align 4
  store i32 1, ptr %b, align 4
  %b2 = load i32, ptr %b, align 4
  %aa3 = load i32, ptr %aa, align 4
  %0 = icmp ne i32 %aa3, 0
  br i1 %0, label %trueBB, label %nextBB

nextBB:                                           ; preds = %entry
  %b6 = load i32, ptr %b, align 4
  %1 = icmp ne i32 %b6, 0
  br i1 %1, label %nextBB4, label %falseBB

trueBB:                                           ; preds = %entry
  br label %mergeBB

mergeBB:                                          ; preds = %trueBB, %mergeBB5
  %2 = phi i32 [ %9, %mergeBB5 ], [ 1, %trueBB ]
  store i32 %2, ptr %aa, align 4
  %aa8 = load i32, ptr %aa, align 4
  %aa9 = load i32, ptr %aa, align 4
  %b10 = load i32, ptr %b, align 4
  %3 = add nsw i32 %aa9, %b10
  %4 = call i32 (ptr, ...) @printf(ptr @0, i32 %3)
  ret i32 0

nextBB4:                                          ; preds = %nextBB
  %aa7 = load i32, ptr %aa, align 4
  %5 = icmp ne i32 %aa7, 0
  %6 = zext i1 %5 to i32
  br label %mergeBB5

falseBB:                                          ; preds = %nextBB
  br label %mergeBB5

mergeBB5:                                         ; preds = %falseBB, %nextBB4
  %7 = phi i32 [ %6, %nextBB4 ], [ 0, %falseBB ]
  %8 = icmp ne i32 %7, 0
  %9 = zext i1 %8 to i32
  br label %mergeBB
}
