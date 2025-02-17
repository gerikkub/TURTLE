
/* start address for the initialization values of the .data section. 
defined in linker script */
.word  _sidata
/* start address for the .data section. defined in linker script */  
.word  _sdata
/* end address for the .data section. defined in linker script */
.word  _edata
/* start address for the .bss section. defined in linker script */
.word  _sbss
/* end address for the .bss section. defined in linker script */
.word  _ebss
/* stack used for SystemInit_ExtMemCtl; always internal RAM used */

.word _estack


.global Reset_Handler
.global _start

  .section  .isr_vector..Reset_Handler
  .type  Reset_Handler, %function
Reset_Handler:  
  j _start

ISR_Handler:
  j ISR_Handler

.extern main

.section .text
_start:

.option push
.option norelax
 #auipc gp, %pcrel_hi(__global_pointer$)
 #addi gp, gp, %pcrel_lo(1b)
#.option pop

  la sp, _estack

  mv a0, zero
  la a1, _sdata
  la a2, _edata
  j LoopCopyDataInit

CopyDataInit:
  la  t0, _sidata
  add t0, t0, a0
  add t1, a1, a0
  lw  t2, 0(t0)
  sw  t2, 0(t1)
  addi a0, a0, 4

LoopCopyDataInit:
  add t0, a1, a0
  blt t0, a2, CopyDataInit

  la a0, _sbss
  la a1, _ebss
  j LoopFillZerosbss

FillZerobss:
  sw x0, 0(a0)
  addi a0, a0, 4

LoopFillZerosbss:
  blt a0, a1, FillZerobss

  call main

EndLoop:
  j EndLoop

.option pop



