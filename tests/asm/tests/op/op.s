

.global _start

.text

nop
_start:
addi x1, x0, 5
addi x5, x0, 0xC
add x2, x1, x1
slt x3, x1, x2
sll x4, x3, x1
xor x5, x1, x5
