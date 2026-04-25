#include <n7OS/keyboard.h>
#include <n7OS/cpu.h>
#include <n7OS/irq.h>
#include <stdio.h>

extern void handler_clavier();

char c[256];
int index_lec;
int index_ecr;
int shift;

void init_keyboard() {
    index_ecr = 0;
    index_lec = 0;
    shift = 0;
    memset(c, 0, sizeof(c));
    outb(inb(0x21)&(~(1 << KEYB_PIC_IRQ)), 0x21);
    init_irq_entry(0x21, (uint32_t) handler_clavier);
}

void handler_clavier_C () {
    outb(0x20, 0x20);
    uint32_t character = inb(0x60);
    if (!IS_KEY_RELEASED(character)) {
        if (character == KEY_TAB) {
            character = 0x9;
        }
        if (character == SHIFT_PRESSED) {
            shift = 1;
        } else if (character == 0xF) {
            c[(index_lec++)%256] = '\t';
        } else {
            if (shift) {
                c[(index_lec++)%256] = scancode_map_shift[character];
            } else {
                c[(index_lec++)%256] = scancode_map[character];
            }
        }
    } else {
        if (character == SHIFT_RELEASED) {
            shift = 0;
        }
    }
}

char kgetch() {
    if (index_ecr == index_lec) {
        return -1;
    }
    return c[(index_ecr++)%256];
}