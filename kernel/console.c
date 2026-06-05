#include <n7OS/console.h>
#include <n7OS/cpu.h>
#include <string.h>
#include <stdio.h>

uint16_t *scr_tab;

// variable globale pour stocker la position du curseur (ligne et colonne)
int ligne_colonne[2];

// sauvegarde de la position du curseur pour le timer
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
    ligne_colonne[0] = 0;
    ligne_colonne[1] = 0;
    // efface la première ligne (car elle n'est pas incluse dans form feed) et affiche le nom du système d'exploitation
    for (int i=0; i<80; i++) {
        scr_tab[i]= CHAR_COLOR<<8|32;
    }
    console_putbytes("\f", strlen("\f"));
    ligne_colonne[0] = 0;
    ligne_colonne[1] = 35;
    console_putbytes("Marouanux", strlen("Marouanux"));
    // positionne le curseur à la prmière ligne après le nom du système d'exploitation
    ligne_colonne[0] = 1;
    ligne_colonne[1] = 0;
}

// efface la première moitié de l'écran
void clear_first_half() {
    for (int i=80; i<80*12; i++) {
        scr_tab[i]= CHAR_COLOR<<8|32;
    }
    ligne_colonne[0]=1;
    ligne_colonne[1]=0;
    console_cursor();
}

// efface la deuxième moitié de l'écran
void clear_second_half() {
    for (int i=80*12; i<80*25; i++) {
        scr_tab[i]= CHAR_COLOR<<8|32;
    }
    ligne_colonne[0]=12;
    ligne_colonne[1]=0;
    console_cursor();
}


/** Affiche un caractère sur la console */
void console_putchar(const char c) {
    if (c>=32 && c<=127) {
        scr_tab[position()]= CHAR_COLOR<<8|c;
        incremente_position();
    } else {
        switch (c) {
            case 8: // backspace : Déplacele curseur d’une colonne en arrière
                if (position()>80) {
                    decremente_position();
                }
                scr_tab[position()]= CHAR_COLOR<<8|32;
                break;
            case 9: // tab : Déplace le curseur de 8 colonnes en avant
                for (int i=0; i<8; i++)
                    incremente_position();
                break;
            case 10: // line feed : Déplace le curseur à la ligne suivante, colone 0
                ligne_colonne[1]=0;
                incremente_ligne();
                break;
            case 12: // form feed : Efface l'écran et positionne le curseur à la première ligne (ligne 1 pas 0 car la ligne 0 est réservée pour le nom du système d'exploitation et le timer)
                for (int i=80; i<VGA_WIDTH*VGA_HEIGHT; i++)
                    scr_tab[i]= CHAR_COLOR<<8|32;
                ligne_colonne[0]=1;
                ligne_colonne[1]=0;
                break;
            case 13: // carriage return : Déplace le curseur au début de la ligne courante
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
        int pos = position();
        // si on atteint le milieu de l'écran, on efface la deuxième moitié et on continue à afficher à partir de la ligne 13
        if (pos == VGA_WIDTH*12) {
            clear_second_half();
        // si on atteint la fin de l'écran, on efface la première moitié et on continue à afficher à partir de la ligne 1
        } else if (pos == 80) {
            clear_first_half();
        }
        console_putchar(s[i]);
    }
}

// positionne le curseur pour afficher le timer à la fin de la première ligne
void timer_setup() {
    memcpy(sauvegarde_ligne_colonne, ligne_colonne, sizeof(ligne_colonne));
    ligne_colonne[0]=0;
    ligne_colonne[1]=72;
}

// restaure la position du curseur après avoir affiché le timer
void timer_cleanup() {
    memcpy(ligne_colonne, sauvegarde_ligne_colonne, sizeof(sauvegarde_ligne_colonne));
    console_cursor();
}