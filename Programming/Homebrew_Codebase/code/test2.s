.org $0

start:
    ; Start with some immediate loads
	mov a, $01
	mov b, $02
	mov c, $03
	
	; Then transfer values between a and between (a = $02, b = $01, c = $01)
	mov c, a
	mov a, b
	mov b, c  
	
	; Now for some memory ops (a should be $42)
	mov [$0100], $42
	mov a, [$0100]
	
	; Push and pop (a and b should be $42)
	push a
	pop b
	
	; Inc and dec (a = $41, b = $43, c = $02)
	dec a
	inc b
	inc c
	
	; Add (a = $86, b= $41)
	add a, b
	sub b, c
	
	