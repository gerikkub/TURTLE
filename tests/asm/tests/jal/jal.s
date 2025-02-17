

.global _start

.text

nop
_start:
addi x1, x0, 1
_loop:
slli x1, x1, 1
jal x2, _loop
