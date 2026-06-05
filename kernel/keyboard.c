#include <n7OS/keyboard.h>
#include <n7OS/cpu.h>
#include <n7OS/irq.h>
#include <stdio.h>

extern void handler_clavier();

// buffer circulaire pour stocker les caractères tapés au clavier
char c[256];

// index de lecture depuis le clavier et d'écriture dans le buffer
int index_lec;

// index de lecture depuis le buffer et d'écriture dans la console
int index_ecr;

// variables pour stocker l'état des touches Shift et Alt
int shift;
int alt;

// initialisation du clavier : on initialise les variables, on active l'interruption du clavier et on définit le handler de l'interruption du clavier
void init_keyboard() {
    index_ecr = 0;
    index_lec = 0;
    shift = 0;
    alt = 0;
    memset(c, 0, sizeof(c));
    outb(inb(0x21)&(~(1 << KEYB_PIC_IRQ)), 0x21);
    init_irq_entry(0x21, (uint32_t) handler_clavier);
}

// handler de l'interruption du clavier 
void handler_clavier_C () {
    outb(0x20, 0x20);
    uint32_t character = inb(0x60);
    // si la touche est pressée (le bit de poids fort n'est pas à 1), on ajoute le caractère correspondant dans le buffer circulaire en fonction de l'état des touches Shift et Alt
    if (!IS_KEY_RELEASED(character)) {
        if (character == KEY_TAB) {
            character = 0x9;
        }
        if (character == SHIFT_PRESSED) {
            shift = 1;
        } else if (character == ALT_PRESSED) {
            alt = 1;
        } else if (character == 0xF) {
            c[(index_lec++)%256] = '\t';
        } else {
            if (shift) {
                // si Shift est pressé, on utilise la table de correspondance des caractères avec Shift
                c[(index_lec++)%256] = scancode_map_shift[character];
            } else if (alt) {
                // si Alt est pressé, on utilise la table de correspondance des caractères avec Alt
                c[(index_lec++)%256] = scancode_map_alt[character];
            } else {
                c[(index_lec++)%256] = scancode_map[character];
            }
        }
    // si la touche est relâchée, on met à jour l'état des touches Shift et Alt
    } else {
        if (character == SHIFT_RELEASED) {
            shift = 0;
        } else if (character == ALT_RELEASED) {
            alt = 0;
        }
    }
}

// fonction pour lire un caractère du buffer circulaire, retourne -1 si le buffer est vide
char kgetch() {
    if (index_ecr == index_lec) {
        return -1;
    }
    return c[(index_ecr++)%256];
}