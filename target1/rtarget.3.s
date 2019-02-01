leaq 0x8(%rsp), %rax
leaq 0x4(%rsp), %rcx
leaq 0x4(%rsp), %rdx
leaq 0x4(%rsp), %rbx
leaq 0x4(%rsp), %rsp
leaq 0x4(%rsp), %rbp
leaq 0x4(%rsp), %rsi
leaq 0x4(%rsp), %rdi
leaq 0x10, %rax

addq 0x10, %rax
addq 0x08, %rcx
addq 0x20, %rdx
addq 0x10, %rbx
addq 0x10, %rsp
addq 0x10, %rbp
addq 0x10, %rsi
addq 0x10, %rdi

add $0x10, %al
add $0x37, %bl
add $0x10, %cl
add $0x10, %dl
add $0x10, %rax
add $0x10, %eax

mov %rax, (%rsp)
mov %rcx, (%rsp)
mov %rdx, (%rsp)
mov %rbx, (%rsp)
mov %rsp, (%rsp)
mov %rbp, (%rsp)
mov %rsi, (%rsp)
mov %rdi, (%rsp)

