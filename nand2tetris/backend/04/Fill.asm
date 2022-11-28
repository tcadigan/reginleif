// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input. 
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel. When no key is pressed, the
// program clears the screen, i.e. writes "white" in every pixel.

        @KBD
        D=M
        @SETWHITE
        D;JLE
        
(SETBLACK)
        @8191
        D=A
        @R1
        M=D
        @BLACK
        0;JMP

(SETWHITE)
        @8191
        D=A
        @R2
        M=D

(WHITE)
        @R2
        D=M
        @WHITECHECK
        D;JLT
        @SCREEN
        A=D+A
        M=0
        @R2
        M=D-1

(WHITECHECK)
        @KBD
        D=M
        @WHITE
        D;JLE
        @SETBLACK
        0;JMP

(BLACK)
        @R1
        D=M
        @BLACKCHECK
        D;JLT
        @SCREEN
        A=D+A
        M=-1
        @R1
        M=D-1

(BLACKCHECK)
        @KBD
        D=M
        @BLACK
        D;JGT
        @SETWHITE
        0;JMP       
        
