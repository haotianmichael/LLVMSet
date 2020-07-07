	.section	__TEXT,__text,regular,pure_instructions
	.macosx_version_min 10, 14
	.globl	_main                   ## -- Begin function main
	.p2align	4, 0x90
_main:                                  ## @main
	.cfi_startproc
## %bb.0:                               ## %entry
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movl	$0, -4(%rbp)
	movq	$0, -16(%rbp)
	movq	$0, -24(%rbp)
	leaq	L_.str(%rip), %rdi
	leaq	-16(%rbp), %rsi
	leaq	-24(%rbp), %rdx
	movb	$0, %al
	callq	_scanf
	movq	-16(%rbp), %rcx
	movl	%eax, -28(%rbp)         ## 4-byte Spill
	movq	%rcx, %rax
	xorl	%r8d, %r8d
	movl	%r8d, %edx
	divq	-24(%rbp)
	leaq	L_.str.1(%rip), %rdi
	movq	%rax, %rsi
	movb	$0, %al
	callq	_printf
	movl	$1, %r8d
	movl	%eax, -32(%rbp)         ## 4-byte Spill
	movl	%r8d, %eax
	addq	$32, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__cstring,cstring_literals
L_.str:                                 ## @.str
	.asciz	"%lld %lld"

L_.str.1:                               ## @.str.1
	.asciz	"64-bit division is %lld\n"


.subsections_via_symbols
