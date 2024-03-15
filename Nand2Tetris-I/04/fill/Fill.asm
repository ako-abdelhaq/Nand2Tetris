// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

// Put your code here.


(LOOP)				// main loop

	@KBD
	D = M			// D = Keyboard
	
	@Reset
	D;JEQ			// if( Keyboard = 0 ) then reset all pixels to 0
	
	@i
	M = 0			// i = 0

	@8192
	D = A			// The maximum number of pixels

	@n
	M = D			// n is set to cover all the pixels

	@SCREEN
	D = A			// D = Screen

	@address
	M = D			// address = 16384
	
	(Black)			// Black Color  ( i.e. Set all pixels to 1 )
		
		@KBD		// Keep on reading from the keyboard
		D = M		// D = keyboard
		
		@LOOP
		D;JEQ		// if( keyboard = 0 ) then comeback to the main loop i.e. If no key is pressed then return to the main loop and start again
		
		@i
		D = M 		// D = i
		
		@n
		D = D - M 	// D = i - R1
		
		@LOOP		
		D;JGT		// if all the pixels have been set to 1 then return to the main loop
		
		@address
		A = M
		M = -1		// RAM[address] = 1111 1111 1111 1111  i.e. set all the pixels to 1
		
		@i
		M = M + 1	// i++
		
		@address
		M = M + 1;	// address = address + 1
		
	@Black
	0;JMP

(Reset)				// Reset ( Set all the pixels to 0 )

	@i
	M = 0			// i = 0

	@8192
	D = A

	@n
	M = D			// n = 8192

	@SCREEN
	D = A

	@address
	M = D			// address = 16384
	
	(White)			// for( i = 0 ; i - R1 > 0 ; i++ )
		
		@KBD
		D = M
		
		@LOOP		// if any key of the keyboard is pressed then go to the main loop and start listening again
		D;JNE
		
		@i
		D = M 		// D = i
		
		@n
		D = D - M 	// D = i - R1
		
		@LOOP		// if the screen is filled with white then go back to the main loop
		D;JGT
		
		@address
		A = M
		M = 0		// RAM[address] = 0000 0000 0000 0000 
		
		@i
		M = M + 1	// i++
		
		@address
		M = M + 1;	// go to the next address
		
	@White
	0;JMP