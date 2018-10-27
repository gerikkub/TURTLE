

.global _start

.text

nop
_start:
addi x1, x0, 4
add x2, x0, x0
_loop:
addi x2, x2, 1
bltu x2, x1, _loop
addi x3, x3, 1
jal x0, _start

