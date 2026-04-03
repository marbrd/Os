#include <inttypes.h>
#include <n7OS/irq.h>


void init_irq_entry(int irq_num, uint32_t addr) {
    idt_entry_t *entry = &idt[irq_num]; 
    entry->offset_inf = addr & 0xFFFF; // partie basse de l'adresse du gestionnaire d'interruption
    entry->sel_segment = KERNEL_CS; // segment de code du noyau
    entry->zero = 0;
    entry->type_attr = 0x8E; // Présent, DPL=0, Type=14 (Interrupt Gate)
    entry->offset_sup = (addr >> 16) & 0xFFFF;  // partie haute de l'adresse du gestionnaire d'interruption
}