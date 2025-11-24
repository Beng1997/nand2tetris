// push constant 17

@17
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 17

@17
D=A
@SP
A=M
M=D
@SP
M=M+1
// eq

@SP
AM=M-1
D=M
A=A-1
D=M-D
@TRUE0
D;JEQ
@SP
A=M-1
M=0
@END1
0;JMP
(TRUE0)
@SP
A=M-1
M=-1
(END1)
// push constant 17

@17
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 16

@16
D=A
@SP
A=M
M=D
@SP
M=M+1
// eq

@SP
AM=M-1
D=M
A=A-1
D=M-D
@TRUE2
D;JEQ
@SP
A=M-1
M=0
@END3
0;JMP
(TRUE2)
@SP
A=M-1
M=-1
(END3)
// push constant 16

@16
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 17

@17
D=A
@SP
A=M
M=D
@SP
M=M+1
// eq

@SP
AM=M-1
D=M
A=A-1
D=M-D
@TRUE4
D;JEQ
@SP
A=M-1
M=0
@END5
0;JMP
(TRUE4)
@SP
A=M-1
M=-1
(END5)
// push constant 892

@892
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 891

@891
D=A
@SP
A=M
M=D
@SP
M=M+1
// lt

@SP
AM=M-1
D=M
A=A-1
D=M-D
@TRUE6
D;JLT
@SP
A=M-1
M=0
@END7
0;JMP
(TRUE6)
@SP
A=M-1
M=-1
(END7)
// push constant 891

@891
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 892

@892
D=A
@SP
A=M
M=D
@SP
M=M+1
// lt

@SP
AM=M-1
D=M
A=A-1
D=M-D
@TRUE8
D;JLT
@SP
A=M-1
M=0
@END9
0;JMP
(TRUE8)
@SP
A=M-1
M=-1
(END9)
// push constant 891

@891
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 891

@891
D=A
@SP
A=M
M=D
@SP
M=M+1
// lt

@SP
AM=M-1
D=M
A=A-1
D=M-D
@TRUE10
D;JLT
@SP
A=M-1
M=0
@END11
0;JMP
(TRUE10)
@SP
A=M-1
M=-1
(END11)
// push constant 32767

@32767
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 32766

@32766
D=A
@SP
A=M
M=D
@SP
M=M+1
// gt

@SP
AM=M-1
D=M
A=A-1
D=M-D
@TRUE12
D;JGT
@SP
A=M-1
M=0
@END13
0;JMP
(TRUE12)
@SP
A=M-1
M=-1
(END13)
// push constant 32766

@32766
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 32767

@32767
D=A
@SP
A=M
M=D
@SP
M=M+1
// gt

@SP
AM=M-1
D=M
A=A-1
D=M-D
@TRUE14
D;JGT
@SP
A=M-1
M=0
@END15
0;JMP
(TRUE14)
@SP
A=M-1
M=-1
(END15)
// push constant 32766

@32766
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 32766

@32766
D=A
@SP
A=M
M=D
@SP
M=M+1
// gt

@SP
AM=M-1
D=M
A=A-1
D=M-D
@TRUE16
D;JGT
@SP
A=M-1
M=0
@END17
0;JMP
(TRUE16)
@SP
A=M-1
M=-1
(END17)
// push constant 57

@57
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 31

@31
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 53

@53
D=A
@SP
A=M
M=D
@SP
M=M+1
// add

@SP
AM=M-1
D=M
A=A-1
M=D+M
// push constant 112

@112
D=A
@SP
A=M
M=D
@SP
M=M+1
// sub

@SP
AM=M-1
D=M
A=A-1
M=M-D
// neg

@SP
A=M-1
M=-M
// and

@SP
AM=M-1
D=M
A=A-1
M=D&M
// push constant 82

@82
D=A
@SP
A=M
M=D
@SP
M=M+1
// or

@SP
AM=M-1
D=M
A=A-1
M=D|M
// not

@SP
A=M-1
M=!M
