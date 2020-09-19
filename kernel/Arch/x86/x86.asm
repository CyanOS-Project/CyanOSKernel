section .text

global _load_segments
_load_segments:

mov eax, [esp+16];
mov fs, ax;

mov eax, [esp+12];
mov gs, ax;

mov eax, [esp+8];
mov ds, ax;
mov es, ax;
mov ss, ax;



push dword [esp+4];
push far_jmp;
retf;
far_jmp: nop;
ret;