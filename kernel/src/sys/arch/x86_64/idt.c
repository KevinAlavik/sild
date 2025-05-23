//#include "sys/log.h"
#include "sys/arch/x86_64/pic.h"
#include <sys/arch/x86_64/idt.h>
#include <sys/log.h>

__attribute__((aligned(0x10))) 
static idt_entry_t idt[256];

static idtr_t idtr;

void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags) {
    idt_entry_t* descriptor = &idt[vector];

    descriptor->isr_low        = (uint64_t)isr & 0xFFFF;
    descriptor->kernel_cs      = 0x28;
    descriptor->ist            = 0;
    descriptor->attributes     = flags;
    descriptor->isr_mid        = ((uint64_t)isr >> 16) & 0xFFFF;
    descriptor->isr_high       = ((uint64_t)isr >> 32) & 0xFFFFFFFF;
    descriptor->reserved       = 0;
}

static int vectors[256];

extern void* isr_stub_table[];

void idt_init() {
    idtr.base = (uintptr_t)&idt[0];
    idtr.limit = (uint16_t)sizeof(idt_entry_t) * 256 - 1;

    for (uint16_t vector = 0; vector <= 256; vector++) {
        if (vector == 0x80)
            continue; // We skip the syscall handler, since it should be called from user space.
        idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
        vectors[vector] = 1;
    }

    uint16_t vector = 0x80;
    idt_set_descriptor(vector, isr_stub_table[vector], 0xEE);
    vectors[vector] = 1;

    pic_init();
    pic_unmask_irq(1);
    
    __asm__ volatile ("lidt %0" : : "m"(idtr)); // load the new IDT
    __asm__ volatile ("sti"); // set the interrupt flag

    //logln(progress, "kinit stage 1", "IDT initialized! Time to receive interrupts!\n");
    log("idt - initialized\n");
}