	.text
	.file	"test_functionCloner.ll"
	.globl	koo
	.p2align	4, 0x90
	.type	koo,@function
koo:                                    # @koo
	.cfi_startproc
# BB#0:                                 # %entry
                                        # kill: %EDI<def> %EDI<kill> %RDI<def>
	leal	10(%rdi), %eax
	retq
.Lfunc_end0:
	.size	koo, .Lfunc_end0-koo
	.cfi_endproc

	.globl	bar
	.p2align	4, 0x90
	.type	bar,@function
bar:                                    # @bar
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rax
.Ltmp0:
	.cfi_def_cfa_offset 16
	addl	$10, %edi
	callq	*data_0x8c(%rip)
	popq	%rcx
	retq
.Lfunc_end1:
	.size	bar, .Lfunc_end1-bar
	.cfi_endproc

	.globl	main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbx
.Ltmp1:
	.cfi_def_cfa_offset 16
.Ltmp2:
	.cfi_offset %rbx, -16
	movl	$80, %edi
	callq	bar
	movl	%eax, %ebx
	movl	$_test_.1, %edi
	xorl	%eax, %eax
	movl	%ebx, %esi
	callq	printf
	movl	%ebx, %eax
	popq	%rbx
	retq
.Lfunc_end2:
	.size	main, .Lfunc_end2-main
	.cfi_endproc

	.type	_test_.1,@object        # @_test_.1
	.section	.rodata,"a",@progbits
_test_.1:
	.asciz	"Value:%d\n"
	.size	_test_.1, 10

	.type	data_0x8c,@object       # @data_0x8c
	.p2align	3
data_0x8c:
	.quad	koo
	.size	data_0x8c, 8


	.section	".note.GNU-stack","",@progbits
