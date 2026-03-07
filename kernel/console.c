#include <n7OS/console.h>
#include <n7OS/cpu.h>

uint16_t *scr_tab;
int ligne_colonne[2] = {0, 0};

int position () {
    return ligne_colonne[0]*VGA_WIDTH+ligne_colonne[1];
}

void incremente_ligne () {
    if (ligne_colonne[0] < VGA_HEIGHT-1) {
        ligne_colonne[0]++;
    } else {
        ligne_colonne[0]=0;
    }
}

void incremente_colonne () {
    if (ligne_colonne[1] < VGA_WIDTH-1) {
        ligne_colonne[1]++;
    } else {
        ligne_colonne[1]=0;
    }
}

void incremente_position () {
    if (ligne_colonne[1] < VGA_WIDTH-1) {
        incremente_colonne();
    } else {
        ligne_colonne[1]=0;
        incremente_ligne();
    }
}

void console_cursor() {
    int pos= (uint16_t) position();
    outb(CMD_LOW, PORT_CMD);
    outb(pos, PORT_DATA);
    outb(CMD_HIGH, PORT_CMD);
    outb(pos >> 8, PORT_DATA);
}

void init_console() {
    scr_tab= (uint16_t *) SCREEN_ADDR;
}

void console_putchar(const char c) {
    if (c>=32 && c<=127) {
        scr_tab[position()]= CHAR_COLOR<<8|c;
        incremente_position();
    } else {
        switch (c) {
            case 8:
                ligne_colonne[1]--;
                break;
            case 9:
                for (int i=0; i<8; i++)
                    incremente_position();
                break;
            case 10:
                ligne_colonne[1]=0;
                incremente_ligne();
                break;
            case 12:
                for (int i=0; i<VGA_WIDTH*VGA_HEIGHT; i++)
                    scr_tab[i]= CHAR_COLOR<<8|32;
                ligne_colonne[0]=0;
                ligne_colonne[1]=0;
                break;
            case 13:
                ligne_colonne[1]=0;
                break;
            default:
                break;  
        }
    }
    console_cursor();
}

void console_putbytes(const char *s, int len) {
    for (int i= 0; i<len; i++) {
        console_putchar(s[i]);
    }
}