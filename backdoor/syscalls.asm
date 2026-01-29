; syscalls.asm
section .data
    wSystemCall dd 0
    qSyscallIns dq 0

section .text
    global SetSyscallConfig
    global NtAllocateVirtualMemory
    global NtWriteVirtualMemory
    global NtCreateThreadEx
    global NtProtectVirtualMemory

SetSyscallConfig:
    mov [rel wSystemCall], ecx
    mov [rel qSyscallIns], rdx
    ret

NtAllocateVirtualMemory:
    mov r10, rcx
    mov eax, [rel wSystemCall]
    jmp [rel qSyscallIns]
    ret

NtWriteVirtualMemory:
    mov r10, rcx
    mov eax, [rel wSystemCall]
    jmp [rel qSyscallIns]
    ret

NtCreateThreadEx:
    mov r10, rcx
    mov eax, [rel wSystemCall]
    jmp [rel qSyscallIns]
    ret

; --- LE BLOC MANQUANT ---
NtProtectVirtualMemory:
    mov r10, rcx
    mov eax, [rel wSystemCall]
    jmp [rel qSyscallIns]
    ret