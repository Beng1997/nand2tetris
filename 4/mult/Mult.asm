// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/4/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)
// The algorithm is based on repetitive addition.

//// Replace this comment with your code.
//R2 WILL BE ZERO
//UNTIL R1 IS NOT ZERO R2 WILL BE INCREASED BY R0
//A=2
@R2
M=0
//IF R1 IS 0 GO TO THE END
//DESRIBING ITERATION
(LOOP)
//CHECK IF R1 IS 0
@R1 
D=M
@END
D;JEQ
@R0
D=M
//NOW WELL LOAD R2
@R2
//ADDING R0 TO R2 ONE TIME
M=D+M
//NOW DECREASE 1 FROM R1
@R1
M=M-1
//JUMPING TO THE START OF THE LOOP
@LOOP
0;JEQ

//JUST LET THE PROGRAM RUN FOREVER
(END)
0;JMP


