section .text
extern interrupt_dispatcher
%macro ISR_ENTRY 1     
dd isr%1
%endmacro

%macro ISR_NOERRCODE 1     
	isr%1:
		push dword 0
		push dword %1
		pusha
		call interrupt_dispatcher
		popa
	 	add esp, 8
	 	iret
%endmacro

%macro ISR_ERRCODE 1
	isr%1:
		push dword %1
		pusha
		call interrupt_dispatcher
		popa
		add esp, 8
		iret
%endmacro

ISR_NOERRCODE 	0
ISR_NOERRCODE 	1
ISR_NOERRCODE 	2
ISR_NOERRCODE 	3
ISR_NOERRCODE 	4
ISR_NOERRCODE 	5
ISR_NOERRCODE 	6
ISR_NOERRCODE 	7
ISR_ERRCODE  	8
ISR_NOERRCODE 	9
ISR_ERRCODE   	10
ISR_ERRCODE   	11
ISR_ERRCODE   	12
ISR_ERRCODE   	13
ISR_ERRCODE   	14
ISR_NOERRCODE 	15
ISR_NOERRCODE 	16
ISR_ERRCODE   	17
ISR_NOERRCODE 	18
ISR_NOERRCODE 	19
ISR_NOERRCODE 	20
ISR_NOERRCODE 	256



global  isr_vector
isr_vector:
ISR_ENTRY 	0
ISR_ENTRY 	1
ISR_ENTRY 	2
ISR_ENTRY 	3
ISR_ENTRY 	4
ISR_ENTRY 	5
ISR_ENTRY 	6
ISR_ENTRY 	7
ISR_ENTRY  	8
ISR_ENTRY 	9
ISR_ENTRY   10
ISR_ENTRY   11
ISR_ENTRY   12
ISR_ENTRY   13
ISR_ENTRY   14
ISR_ENTRY 	15
ISR_ENTRY 	16
ISR_ENTRY   17
ISR_ENTRY 	18
ISR_ENTRY 	19
ISR_ENTRY 	20

global generic_isr
generic_isr:
ISR_ENTRY 	256



