#include <inttypes.h>
#include <n7OS/time.h>
#include <n7OS/cpu.h>
#include <n7OS/irq.h>
#include <stdio.h>
#include <n7OS/processus.h>

extern void handler_timer();

// variable globale pour stocker le temps écoulé en millisecondes depuis le démarrage du système
volatile int timer;

// initialiser le timer pour générer une interruption toutes les millisecondes
void init_timer() {
    timer = 0;
    duree_exec();
    // le timer matériel du PC fonctionne à une fréquence de 1,19 MHz
    uint32_t frequence = 1190/1;
    outb(0x34,0x43);
    outb(frequence&0xFF, 0x40);
    outb(frequence>>8, 0x40);
    outb(inb(0x21)&0xfe, 0x21);
    init_irq_entry(0x20, (uint32_t) handler_timer);
}

// handler de l'interruption du timer
void handler_timer_C() {
    outb(0x20, 0x20);
    timer++;
    // afficher la durée d'exécution du système toutes les secondes
    if (timer%1000==0) {
        duree_exec();
    }
    // réveiller les processus dont le temps est écoulé
    for (int i = 0; i < NB_PROC-1; i++) {
        if (process_table[i].state == BLOCKE && 
            process_table[i].wakeup_time <= timer) {
            process_table[i].state = PRET;
        }
    }
    // appeler le scheduler pour choisir un autre processus à exécuter toutes les 10 ms
    if (timer%10==0) {
        scheduler();
    }
}

// afficher la durée d'exécution du système au format hh:mm:ss
void duree_exec() {
    int heures, minutes, secondes;
    int timer_s = timer/1000;
    heures = timer_s/3600;
    minutes = (timer_s%3600)/60;
    secondes = timer_s%60;
    // pose le curseur à la position appropriée pour afficher la durée d'exécution du système
    timer_setup();
    printf("%02d:%02d:%02d", heures, minutes, secondes);
    // restaurer la position du curseur pour continuer à afficher les autres informations
    timer_cleanup();
}

