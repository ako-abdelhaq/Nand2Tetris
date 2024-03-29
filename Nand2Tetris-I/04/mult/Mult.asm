// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)
//
// This program only needs to handle arguments that satisfy
// R0 >= 0, R1 >= 0, and R0*R1 < 32768.

// Put your code here.


@R2
M = 0			// R2 = 0

@i
M = 1			// i = 1

(LOOP)			// for( i = 1 ; i - R1 > 0 ; i++ )
	@i
	D = M 		// D = i
	
	@R1
	D = D - M 	// D = i - R1
	
	@STOP
	D;JGT		
	
	@R2
	D = M     	// D = R2
	
	@R0
	D = D + M 	// D = R2 + R0
	
	@R2
	M = D		// R2 = R2 + R0
	
	@i
	M = M + 1	// i++
	
	@LOOP
	0;JMP

(STOP)			// Infinite Loop
	@STOP
	0;JMP