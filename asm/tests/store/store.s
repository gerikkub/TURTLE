

.global _start

.text

.word 0xAABBCCDD
_start:
addi x1, x0, 1
slli x1, x1, 31
lw x2, 0(x0)

sw x2, 0(x1)
sb x2, 5(x1)
sh x2, 7(x1)

