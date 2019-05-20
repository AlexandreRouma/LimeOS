.extern _cpp_switch_task

.global _ASM_SWITCH_TASK_ISR
_ASM_SWITCH_TASK_ISR:
    pushf
    pusha

    push %esp
    call _cpp_switch_task # Modify the states
    # pop %esp
    mov %eax, %esp

    

    popa
    popf

    

    # ret
    iret

