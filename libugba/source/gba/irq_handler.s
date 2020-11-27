// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

    .section .iwram, "ax", %progbits
    .code 32

    .set MEM_IO_ADDR, 0x04000000
    .set OFFSET_IE, 0x200
    .set OFFSET_IF, 0x202
    .set OFFSET_IME, 0x208

    .global IRQ_GlobalInterruptHandler

IRQ_GlobalInterruptHandler:
    // Get the pending interrupts that the user actually cares about. If
    // something isn't set in IE, ignore it.

    mov     r0, #MEM_IO_ADDR        // r0 = MEM_IO_ADDR
    ldr     r1, [r0, #OFFSET_IE]    // r1 = REG_IE | (REG_IF << 16)
    and     r1, r1, r1, lsr #16     // r1 = REG_IE & REG_IF

    // Iterate from BIT(0) to BIT(13)

    .extern IRQ_VectorTable

    ldr     r3, =IRQ_VectorTable
    mov     r2, #1

iterate_interrupt_table:
        tst     r1, r2
        bne     interrupt_found

        mov     r2, r2, lsl #1
        add     r3, r3, #4

        // Check if we have reached the end of the table. If so, exit loop.
        cmp     r2, #(1 << 14)
        bne     iterate_interrupt_table

    // If no interrupt handlers have to be called, clear all bits in the IF and
    // BIOS flags register.

    add     r3, r0, #(OFFSET_IF & 0xFF00)
    orr     r3, r3, #(OFFSET_IF & 0xFF)
    ldrh    r1, [r3]
    strh    r1, [r3]

    ldrh    r2, [r0, #-8] // The BIOS register is mirrored at 0x03FFFFF8
    orr     r2, r2, r1
    strh    r2, [r0, #-8]

    bx      lr

    // This point is reached if there is at least one bit set in IF & IE
interrupt_found:
    // r0 = REG_BASE
    // r2 = IRQ bit of the current vector
    // r3 = Pointer to vector to jump to

    // Write bit to IF and the BIOS register to acknowledge this interrupt, but
    // leave the others alone.
    add     r1, r0, #(OFFSET_IF & 0xFF00)
    orr     r1, r1, #(OFFSET_IF & 0xFF)
    strh    r2, [r1]

    ldrh    r1, [r0, #-8] // The BIOS register is mirrored at 0x03FFFFF8
    orr     r1, r1, r2
    strh    r1, [r0, #-8]

    // If the interrupt handler is null, exit handler
    ldr     r3, [r3]
    cmp     r3, #0
    bxeq    lr

    // If this point is reached, there is a valid interrupt handler

    // r0 = REG_BASE
    // r3 = Vector to jump to

    // Clear IME so that we don't get any nested interrupt during the handler of
    // the current one. It isn't needed to save the old value because there is
    // no way to reach the global interrupt handler unless IME = 1.
    add     r2, r0, #(OFFSET_IME & 0xFF00)
    orr     r2, r2, #(OFFSET_IME & 0xFF)
    mov     r1, #0
    strh    r1, [r2]

    // Save r0, lr and spsr for later
    mrs     r1, spsr
    stmfd   sp!, {r0-r1, lr} // MEM_IO_ADDR, spsr, lr

    .equ    MODE_IRQ, 0x12
    .equ    MODE_SYSTEM, 0x1F
    .equ    MODE_MASK, 0x1F

    // Set CPU mode to system (like user, but privileged, so we can go back to
    // mode IRQ later)
    mrs     r2, cpsr
    //bic     r2, r2, #MODE_MASK // Not needed for MODE_SYSTEM
    orr     r2, r2, #MODE_SYSTEM
    msr     cpsr, r2

    // Call interrupt handler
    push    {lr}

    mov     lr, pc
    bx      r3

    pop     {lr}

    // Set CPU mode to IRQ
    mrs     r2, cpsr
    bic     r2, r2, #MODE_MASK
    orr     r2, r2, #MODE_IRQ
    msr     cpsr, r2

    // Restore preserved registers
    ldmfd	sp!, {r0-r1, lr} // MEM_IO_ADDR, spsr, lr
    msr     spsr, r1

    // Re-enable IME
    mov     r1, #1
    str     r1, [r0, #OFFSET_IME]

    bx      lr

    .end
