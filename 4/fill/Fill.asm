// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/4/Fill.asm

// Runs an infinite loop that listens to the keyboard input. 
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel. When no key is pressed, 
// the screen should be cleared.

//// Replace this comment with your code.

//CREATE A COLOR VARIABLE
@COLOR
M=0 //SET TO 0 AS DEFAULT
//LIKE WITH MULT WE WILL DESCRIBE THE LOOP GOING FROM SCREEN TO KBD
//FIRST THE CONDITION OF THE KEYBOARD LOOP
(LOOP)
//LOAD SCREEN ADDRESS INTO ADDRESS
@SCREEN
D=A
@ADDRESS
M=D 
//CHECK IF ANYTHING IS BEING PRESSED
@KBD
D=M
@BLACK
//LOADED THE BLACK FUNCTION TO CURRENT ADDRESS
D;JGT
@COLOR
M=0
@COLORING
0;JMP

(BLACK)
@COLOR
M=-1


(COLORING) // COLOR ENTIRE SCREEN WITH BLACK OR WHITE
//CALL FOR COLOR 
@COLOR 
D=M //SETS THE CURRENT COLOR
@ADDRESS
A=M //WE LOADED THE VALUE FOR THE ADRESS WE NEED TO PAINT TO CURRENT ADDRESS
M=D //THE COLORING

@ADDRESS
M=M+1
D=M //NEED THIS TO CHECK IF ITS BIGGER THAN KBD ADRESS

@KBD
D=A-D
//LOAD COLORING ADRESS
@COLORING
D;JGT //IF POSITIVE CONTINUE TO COLOR






@LOOP
0;JMP //INFINITE LOOP