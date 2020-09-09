section .text

global _load_segments
_load_segments:
mov eax, [esp+4];
mov ds, ax;
mov es, ax;
mov fs, ax;
mov ss, ax;
mov gs, ax;

push far_jmp;
iret;
far_jmp: nop;
ret;