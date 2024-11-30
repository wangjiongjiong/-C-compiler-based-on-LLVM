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
  store i32 19, ptr %b, align 4
  %b12 = load i32, ptr %b, align 4
  br label %last

last:                                             ; preds = %else, %last9
  br label %"for,init"

cond7:                                            ; preds = %then
  %aa10 = load i32, ptr %aa, align 4
  %3 = icmp ne i32 %aa10, 5
  %4 = sext i1 %3 to i32
  %5 = icmp ne i32 %4, 0
  br i1 %5, label %then8, label %last9

then8:                                            ; preds = %cond7
  store i32 6, ptr %aa, align 4
  %aa11 = load i32, ptr %aa, align 4
  br label %last9

last9:                                            ; preds = %then8, %cond7
  br label %last

"for,init":                                       ; preds = %last
  %i = alloca i32, align 4
  store i32 0, ptr %i, align 4
  %i13 = load i32, ptr %i, align 4
  br label %"for,cond"

"for,cond":                                       ; preds = %"for,inc", %"for,init"
  %i14 = load i32, ptr %i, align 4
  %6 = icmp slt i32 %i14, 100
  %7 = sext i1 %6 to i32
  %8 = icmp ne i32 %7, 0
  br i1 %8, label %"for,body", label %"for,last"

"for,inc":                                        ; preds = %"for,last27", %then20
  %i35 = load i32, ptr %i, align 4
  %9 = add nsw i32 %i35, 1
  store i32 %9, ptr %i, align 4
  %i36 = load i32, ptr %i, align 4
  br label %"for,cond"

"for,body":                                       ; preds = %"for,cond"
  store i32 3, ptr %b, align 4
  %b15 = load i32, ptr %b, align 4
  %aa16 = load i32, ptr %aa, align 4
  %i17 = load i32, ptr %i, align 4
  %10 = add nsw i32 %aa16, %i17
  store i32 %10, ptr %aa, align 4
  %aa18 = load i32, ptr %aa, align 4
  br label %cond19

"for,last":                                       ; preds = %"for,cond"
  %aa37 = load i32, ptr %aa, align 4
  %b38 = load i32, ptr %b, align 4
  %11 = mul nsw i32 %b38, 9
  %12 = add nsw i32 %aa37, %11
  %13 = call i32 (ptr, ...) @printf(ptr @0, i32 %12)
  ret i32 0

cond19:                                           ; preds = %"for,body"
  %i22 = load i32, ptr %i, align 4
  %14 = icmp sge i32 %i22, 1
  %15 = sext i1 %14 to i32
  %16 = icmp ne i32 %15, 0
  br i1 %16, label %then20, label %last21

then20:                                           ; preds = %cond19
  br label %"for,inc"

last21:                                           ; preds = %for.continue.death, %cond19
  br label %"for,init23"

for.continue.death:                               ; No predecessors!
  br label %last21

"for,init23":                                     ; preds = %last21
  %j = alloca i32, align 4
  store i32 1, ptr %j, align 4
  %j28 = load i32, ptr %j, align 4
  br label %"for,cond24"

"for,cond24":                                     ; preds = %"for,inc25", %"for,init23"
  %j29 = load i32, ptr %j, align 4
  %17 = icmp slt i32 %j29, 10
  %18 = sext i1 %17 to i32
  %19 = icmp ne i32 %18, 0
  br i1 %19, label %"for,body26", label %"for,last27"

"for,inc25":                                      ; preds = %"for,body26"
  %j33 = load i32, ptr %j, align 4
  %20 = add nsw i32 %j33, 2
  store i32 %20, ptr %j, align 4
  %j34 = load i32, ptr %j, align 4
  br label %"for,cond24"

"for,body26":                                     ; preds = %"for,cond24"
  %b30 = load i32, ptr %b, align 4
  %j31 = load i32, ptr %j, align 4
  %21 = mul nsw i32 %b30, %j31
  store i32 %21, ptr %b, align 4
  %b32 = load i32, ptr %b, align 4
  br label %"for,inc25"

"for,last27":                                     ; preds = %"for,cond24"
  br label %"for,inc"
}
