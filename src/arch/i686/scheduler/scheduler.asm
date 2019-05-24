.extern _cpp_switch_task
.extern _cpp_switch_task_no_count

/*
    When this function is called, the stack must be in the state of an ISR

            EFLAGS
              CS
    esp -->  EIP
*/

.global _ASM_SWITCH_TASK_ISR
_ASM_SWITCH_TASK_ISR:
    pushf
    pusha

    push %esp
    call _cpp_switch_task # Modify the states
    mov %eax, %esp

    popa
    popf

    # Switch task
    sti
    iret

.global _ASM_SWITCH_TASK_YIELD
_ASM_SWITCH_TASK_YIELD:
    pushf
    pusha

    push %esp
    call _cpp_switch_task_no_count # Modify the states
    mov %eax, %esp

    popa
    popf

    sti
    
    # Switch task
    iret

.global _ASM_YIELD
_ASM_YIELD:
    cli
    pushf
    push %cs
    call _ASM_SWITCH_TASK_YIELD
    # No need for a return statement as the iret will do all the work
