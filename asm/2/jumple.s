.section .data

.section .bss
	.lcomm jesus, 1

.section .text
	NL_char:
		.byte 12

.global _start

_start:

	movb $65, jesus

	spout:
		movl $4, %eax
		movl $1, %ebx
		movl $jesus, %ecx
		movl $1, %edx
		int $0x80
		incb jesus

		cmpb $91, jesus
		jne spout

	#Print NL char
	movl $4, %eax
	movl $1, %ebx
	movl $NL_char, %ecx
	movl $1, %edx
	int $0x80
	
	#Exit
	movl $1, %eax
	movl $0, %ebx
	int $0x80
