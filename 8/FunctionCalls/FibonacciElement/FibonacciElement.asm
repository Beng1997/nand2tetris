@256
D=A
@SP
M=D
@Sys.init$ret.0
D=A
@SP
A=M
M=D
@SP
M=M+1
@LCL
D=M
@SP
A=M
M=D
@SP
M=M+1
@ARG
D=M
@SP
A=M
M=D
@SP
M=M+1
@THIS
D=M
@SP
A=M
M=D
@SP
M=M+1
@THAT
D=M
@SP
A=M
M=D
@SP
M=M+1
@SP
D=M
@5
D=D-A
@0
D=D-A
@ARG
M=D
@SP
D=M
@LCL
M=D
@Sys.init
0;JMP
(Sys.init$ret.0)
// function Main.fibonacci 0

(Main.fibonacci)
// 	push argument 0

@ARG
D=M
@0
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1
// 	push constant 2

@2
D=A
@SP
A=M
M=D
@SP
M=M+1
// 	lt                     

@SP
AM=M-1
D=M
A=A-1
D=M-D
@TRUE1
D;JLT
@SP
A=M-1
M=0
@END2
0;JMP
(TRUE1)
@SP
A=M-1
M=-1
(END2)
// 	if-goto N_LT_2        

@SP
AM=M-1
D=M
@Main.fibonacci$N_LT_2
D;JNE
// 	goto N_GE_2

@Main.fibonacci$N_GE_2
0;JMP
// label N_LT_2               // if n < 2 returns n

(Main.fibonacci$N_LT_2)
// 	push argument 0        

@ARG
D=M
@0
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1
// 	return

@LCL
D=M
@R13
M=D
@5
A=D-A
D=M
@R14
M=D
@SP
AM=M-1
D=M
@ARG
A=M
M=D
@ARG
D=M+1
@SP
M=D
@R13
AM=M-1
D=M
@THAT
M=D
@R13
AM=M-1
D=M
@THIS
M=D
@R13
AM=M-1
D=M
@ARG
M=D
@R13
AM=M-1
D=M
@LCL
M=D
@R14
A=M
0;JMP
// label N_GE_2               // if n >= 2 returns fib(n - 2) + fib(n - 1)

(Main.fibonacci$N_GE_2)
// 	push argument 0

@ARG
D=M
@0
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1
// 	push constant 2

@2
D=A
@SP
A=M
M=D
@SP
M=M+1
// 	sub

@SP
AM=M-1
D=M
A=A-1
M=M-D
// 	call Main.fibonacci 1  // computes fib(n - 2)

@Main.fibonacci$ret.3
D=A
@SP
A=M
M=D
@SP
M=M+1
@LCL
D=M
@SP
A=M
M=D
@SP
M=M+1
@ARG
D=M
@SP
A=M
M=D
@SP
M=M+1
@THIS
D=M
@SP
A=M
M=D
@SP
M=M+1
@THAT
D=M
@SP
A=M
M=D
@SP
M=M+1
@SP
D=M
@5
D=D-A
@1
D=D-A
@ARG
M=D
@SP
D=M
@LCL
M=D
@Main.fibonacci
0;JMP
(Main.fibonacci$ret.3)
// 	push argument 0

@ARG
D=M
@0
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1
// 	push constant 1

@1
D=A
@SP
A=M
M=D
@SP
M=M+1
// 	sub

@SP
AM=M-1
D=M
A=A-1
M=M-D
// 	call Main.fibonacci 1  // computes fib(n - 1)

@Main.fibonacci$ret.4
D=A
@SP
A=M
M=D
@SP
M=M+1
@LCL
D=M
@SP
A=M
M=D
@SP
M=M+1
@ARG
D=M
@SP
A=M
M=D
@SP
M=M+1
@THIS
D=M
@SP
A=M
M=D
@SP
M=M+1
@THAT
D=M
@SP
A=M
M=D
@SP
M=M+1
@SP
D=M
@5
D=D-A
@1
D=D-A
@ARG
M=D
@SP
D=M
@LCL
M=D
@Main.fibonacci
0;JMP
(Main.fibonacci$ret.4)
// 	add                    // returns fib(n - 1) + fib(n - 2)

@SP
AM=M-1
D=M
A=A-1
M=D+M
// 	return

@LCL
D=M
@R13
M=D
@5
A=D-A
D=M
@R14
M=D
@SP
AM=M-1
D=M
@ARG
A=M
M=D
@ARG
D=M+1
@SP
M=D
@R13
AM=M-1
D=M
@THAT
M=D
@R13
AM=M-1
D=M
@THIS
M=D
@R13
AM=M-1
D=M
@ARG
M=D
@R13
AM=M-1
D=M
@LCL
M=D
@R14
A=M
0;JMP
// function Sys.init 0

(Sys.init)
// 	push constant 4

@4
D=A
@SP
A=M
M=D
@SP
M=M+1
// 	call Main.fibonacci 1

@Main.fibonacci$ret.5
D=A
@SP
A=M
M=D
@SP
M=M+1
@LCL
D=M
@SP
A=M
M=D
@SP
M=M+1
@ARG
D=M
@SP
A=M
M=D
@SP
M=M+1
@THIS
D=M
@SP
A=M
M=D
@SP
M=M+1
@THAT
D=M
@SP
A=M
M=D
@SP
M=M+1
@SP
D=M
@5
D=D-A
@1
D=D-A
@ARG
M=D
@SP
D=M
@LCL
M=D
@Main.fibonacci
0;JMP
(Main.fibonacci$ret.5)
// label END  

(Sys.init$END)
// 	goto END  // loops infinitely

@Sys.init$END
0;JMP
