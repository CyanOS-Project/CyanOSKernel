section .text

global _load_segments
_load_segments:
mov eax, [esp+8];
mov ds, ax;
mov es, ax;
mov fs, ax;
mov ss, ax;
mov gs, ax;

push dword [esp+4];
push far_jmp;
retf;
far_jmp: nop;
ret;