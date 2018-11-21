ldr r0, =0x20200004
ldr r1, =0x1
lsl r1, #18
str r1, [r0] ; We have now set pin 16 to an output pin!
ldr r4, =0x1
lsl r4, #16 ; To active
ldr r5, =0x0
lsl r5, #16 ; To end
ldr r2, =0x20200028 ; Address of clearing pins
ldr r3, =0x2020001C ; Address of turning on pins
str r4, [r2] ; First we clear pins
loop:
str r4, [r3] ; Turn on pin
ldr r12,=0xFFFFFF
wait:
sub r12,r12,#1
cmp r12,#0
bne wait
str r4, [r2]
ldr r12,=0xFFFFFF
waittwo:
sub r12,r12,#1
cmp r12,#0
bne waittwo
b loop
andeq r0,r0,r0
