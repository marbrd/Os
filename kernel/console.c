#include <n7OS/console.h>
#include <n7OS/cpu.h>

uint16_t *scr_tab;

/** Position du curseur dans le tableau de d'écran */
int ligne_colonne[2] = {1, 0};
int sauvegarde_ligne_colonne[2];

/** Retourne la position du curseur dans le tampon d'écran */
int position () {
    return ligne_colonne[0]*VGA_WIDTH+ligne_colonne[1];
}

/** Incrémente la ligne du curseur */
void incremente_ligne () {
    if (ligne_colonne[0] < VGA_HEIGHT-1) {
        ligne_colonne[0]++;
    } else {
        ligne_colonne[0]=1;
    }
}

/** Incrémente la colonne du curseur */
void incremente_colonne () {
    if (ligne_colonne[1] < VGA_WIDTH-1) {
        ligne_colonne[1]++;
    } else {
        ligne_colonne[1]=0;
    }
}

/** décrémente la ligne du curseur */
void decremente_ligne () {
    if (ligne_colonne[0] > 1) {
        ligne_colonne[0]--;
    } else {
        ligne_colonne[0]=VGA_HEIGHT-1;
    }
}

/** décrémente la colonne du curseur */
void decremente_colonne () {
    if (ligne_colonne[1] > 0) {
        ligne_colonne[1]--;
    } else {
        ligne_colonne[1]=VGA_WIDTH-1;
    }
}

/** Incrémente la position du curseur */
void incremente_position () {
    if (ligne_colonne[1] < VGA_WIDTH-1) {
        incremente_colonne();
    } else {
        ligne_colonne[1]=0;
        incremente_ligne();
    }
}

/** décrémente la position du curseur */
void decremente_position () {
    if (ligne_colonne[1] > 0) {
        decremente_colonne();
    } else {
        ligne_colonne[1]=VGA_WIDTH-1;
        decremente_ligne();
    }
}

/** Affiche le curseur sur l'écran */
void console_cursor() {
    int pos= (uint16_t) position();
    outb(CMD_LOW, PORT_CMD);
    outb(pos, PORT_DATA);
    outb(CMD_HIGH, PORT_CMD);
    outb(pos >> 8, PORT_DATA);
}

/** Initialise la console */
void init_console() {
    scr_tab= (uint16_t *) SCREEN_ADDR;
}

/** Affiche un caractère sur la console */
void console_putchar(const char c) {
    if (c>=32 && c<=127) {
        scr_tab[position()]= CHAR_COLOR<<8|c;
        incremente_position();
    } else {
        switch (c) {
            case 8: // backspace
                if (position()>80) {
                    decremente_position();
                }
                scr_tab[position()]= CHAR_COLOR<<8|32;
                break;
            case 9: // tab
                for (int i=0; i<8; i++)
                    incremente_position();
                break;
            case 10: // line feed
                ligne_colonne[1]=0;
                incremente_ligne();
                break;
            case 12: // form feed
                for (int i=0; i<VGA_WIDTH*VGA_HEIGHT; i++)
                    scr_tab[i]= CHAR_COLOR<<8|32;
                ligne_colonne[0]=1;
                ligne_colonne[1]=0;
                break;
            case 13: // carriage return
                incremente_ligne();
                ligne_colonne[1]=0;
                break;
            default:
                break;  
        }
    }
    console_cursor();
}

/** Affiche une chaîne de caractères sur la console */
void console_putbytes(const char *s, int len) {
    for (int i= 0; i<len; i++) {
        console_putchar(s[i]);
    }
}

void timer_setup() {
    memcpy(sauvegarde_ligne_colonne, ligne_colonne, sizeof(ligne_colonne));
    ligne_colonne[0]=0;
    ligne_colonne[1]=72;
}

void timer_cleanup() {
    memcpy(ligne_colonne, sauvegarde_ligne_colonne, sizeof(sauvegarde_ligne_colonne));
    console_cursor();
}