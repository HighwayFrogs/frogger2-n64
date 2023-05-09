; SNMAIN.S
; entry point code for N64

; v2.05 20.2.98 by Gil - added GP initialisation from linker-generated symbol,
;                        cleared .sbss section
; v2.03 2.2.98 by Gil - moved C++ constructors to ctors.c
; v2.02 16.1.98 by Gil - added C++ constructors etc

        opt     c+

; macro to zero a section for initialising bss areas
; assumes the section is a multiple of 4 bytes long
; the parameter (sec) must have been declared in a section statement

zerosect        macro   sec
	local   l1,l2

	la      t0,sect(\sec)
	la      t1,sectend(\sec)
	beq     t0,t1,l2        ; in case section is empty
	nop
l1:
	addiu   t0,t0,4
	sltu    at,t0,t1
	bne     at,zero,l1
	sw      zero,-4(t0)
l2:
	endm


; 4k header area for front of cartridge

	section .header
	dcb     4096,0


; declaration of sections we're going to initialise to 0

	section .bss
        section .sbss

; start up code

	xref    boot

	section .start

	la      sp,$80400000-16 ; initialise stack pointer

	zerosect .bss   ; clear out .bss section
	zerosect .sbss	; clear out .sbss section

; set GP from linker value

        xref    __SN_GP_BASE

        la      gp,__SN_GP_BASE

; run game

	jal     boot    ; go execute users program
	nop

; safety trap

	break   $402    ; just in case it returns


