

.global _start

.text

nop
_start:
addi x1, x0, 4
add x1, x1, x1
lw x2, 0(x0)
lb x3, -4(x1)
lbu x4, -4(x1)
