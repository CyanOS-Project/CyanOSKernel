extern kernel_boot_stage2
KERNEL_BASE equ 0xC0000000

; Stack
section .bss
align 16

resb 16384 ; 16 KiB
__stack_top:
;stack_top equ (__stack_top - KERNEL_BASE)
; bootstrap stage 1
section .bootstrap
global _kernel_boot_stage1
_kernel_boot_stage1:
mov ecx, __stack_top
sub ecx, KERNEL_BASE
mov esp, ecx
push ebx
push eax
call kernel_boot_stage2
