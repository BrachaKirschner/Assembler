macr macro_b
lea r6
endmacr
.entry NOT_DEFINED
.extern EXTERN_LABEL
MAIN: add r2, r3
jsr NOT_DEFINED
LOOP: prn #100
lea STR, r2
EXTERN_LABEL: inc r5
prn #999999
mov *r5, r2
sub r1, #6
cmp r4, STR
bne EXTERN_LABEL
add r7, r11
clr
sub r3, r3
.entry LOOP
jmp LOOP
END: stop
STR: .string "xyz"
STR: .data 50, -5
END: .data 120