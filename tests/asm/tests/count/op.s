

.global _start

.text

nop
_start:
lui x1, 0
addi x2, x0, 100
addi x3, x0, 1
loop:
beq x1, x2, done
add x1, x1, x3
jal x0, loop

done:
lui x4, 0xABCDE
lui x5, 0x11223
sw x5,0(x4)

