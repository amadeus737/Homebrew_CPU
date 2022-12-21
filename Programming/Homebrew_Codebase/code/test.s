.arch "homebrew.arch"
.org $0

start:	
	; Set up a sequence where a shifts one bit left and b shifts one bit right
	mov a, %00000001;
	mov b, %10000000;

	mov a, %00000010;
	mov b, %01000000;

	mov a, %00000100;
	mov b, %00100000;
	
	mov a, %00001000;
	mov b, %00010000;

	mov a, %00010000;
	mov b, %00001000;

	mov a, %00100000;
	mov b, %00000100;

	mov a, %01000000;
	mov b, %00000010;
	
	mov a, %10000000;
	mov b, %00000001;
	
	; Reverse the sequence now
	mov a, %01000000;
	mov b, %00000010;
	
	mov a, %00100000;
	mov b, %00000100;
	
	mov a, %00010000;
	mov b, %00001000;

	mov a, %00001000;
	mov b, %00010000;
	
	mov a, %00000100;
	mov b, %00100000;
	
	mov a, %00000010;
	mov b, %01000000;
	
	mov a, %00000001;
	mov b, %10000000;
