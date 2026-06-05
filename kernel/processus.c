#include "n7OS/processus.h"
#include "n7OS/mem.h"
#include "n7OS/time.h"
#include "n7OS/cpu.h"
#include "../bin/processus1.h"
#include "../bin/processus2.h"
#include "../bin/processus3.h"
#include "../bin/idle.h"
#include "../bin/miniShell.h"
#include <string.h>

int process_table_indx;

pid_t current_process;

// les processus sont stockés dans une table des processus (process_table) de taille NB_PROC-1
// le choix des processus à exécuter est fait par le scheduler qui parcourt la table des processus pour trouver un processus dans l'état PRET et le mettre dans l'état ELU
// la recherche d'un processus dans la table des processus est faite de manière circulaire à partir du processus suivant du processus courant pour assurer une répartition équitable du temps CPU entre les processus
struct process_t process_table[NB_PROC-1];

extern ctx_sw(uint32_t* old_reg, uint32_t* new_reg);

// initialiser la table des processus et créer les processus initiaux
void init_processes() {
    process_table_indx = 0;
    memset(process_table, 0, sizeof(process_table));
    // initialiser tous les processus de la table des processus à l'état VIDE et le temps de réveil à -1
    for (int i = 0; i < NB_PROC - 1; i++) {
        process_table[i].state = VIDE;
        process_table[i].wakeup_time = -1; 
    }
    current_process = 0;
    pid_t idle_pid = creer("idle", idle);
    creer("processus1", processus1);
    creer("processus2", processus2);
    creer("processus3", processus3);
    creer("miniShell", miniShell);
    current_process = idle_pid;
    scheduler();
}

// allouer un PID libre dans la table des processus
pid_t allouer_pid() {
    for (int i = 0; i < NB_PROC - 1; i++) {
        // on cherche un processus dans la table des processus qui est dans l'état SUPPRIME (pour réutiliser son PID) ou VIDE 
        if (process_table[i].state == SUPPRIME || process_table[i].state == VIDE) {
            return i;
        }
    }
    return -1;
}

// créer un processus
pid_t creer(char name[32], fnptr function) {
    pid_t Pid ;
    // trouver un PID libre dans la table des processus
    if ((Pid = allouer_pid()) == -1) {
        return -1;
    }
    strncpy(process_table[Pid].name, name, 32); // insérer le nom du processus dans la table des processus
    process_table[Pid].pid = Pid; // assigner le PID au processus
    process_table[Pid].stack = (uint32_t*) malloc(STACK_SIZE*sizeof(uint32_t)); // allouer une pile pour le processus
    process_table[Pid].state = PRET; // mettre le processus dans l'état PRET pour qu'il puisse être sélectionné par le scheduler
    process_table[Pid].function = function; // assigner la fonction à exécuter
    process_table[Pid].stack[STACK_SIZE - 1] = (uint32_t) function; // mettre l'adresse de la fonction au sommet de la pile
    process_table[Pid].regs[0] = 0; // initialiser les registres à 0 sauf le pointeur de pile
    process_table[Pid].regs[1] = (uint32_t) &process_table[Pid].stack[STACK_SIZE - 1]; // initialiser le pointeur de pile
    process_table[Pid].regs[2] = 0;
    process_table[Pid].regs[3] = 0;
    process_table[Pid].regs[4] = 0;
    return Pid;
}

// supprimer un processus de la table des processus (le mettre dans l'état SUPPRIME)
void terminer(pid_t pid) {
    process_table[pid].state = SUPPRIME;
    scheduler();
}

// arrêter un processus en cours d'exécution (le mettre dans l'état PRET)
void arreter(pid_t pid) {
    if (process_table[pid].state == ELU) {
        process_table[pid].state = PRET;
    }
}   

// debloquer un processus bloqué (le mettre dans l'état PRET)
void debloquer(pid_t pid) {
    if (process_table[pid].state == BLOCKE) {
        process_table[pid].state = PRET;
    }
}

// retourne le PID du processus courant
pid_t getpid_process() {
    return current_process;
}

// créer un nouveau processus en dupliquant un processus existant (le processus parent)
pid_t fork_process(const char *name, fnptr function) {
    return creer(name, function);
}

// Mettre le processus en sommeil pendant une durée spécifiée (en millisecondes)
int sleep_process(int duration){
    process_table[current_process].state = BLOCKE;
    process_table[current_process].wakeup_time = timer + duration; // Définir le temps de réveil
    scheduler(); // Appeler le scheduler pour choisir un autre processus à exécuter
    return 0;
}

void scheduler(){
    // comme mentionné au début, le scheduler parcourt la table des processus de manière circulaire à partir du processus suivant du processus courant (i commence de 1)
    for (int i = 1; i < NB_PROC; i++){
        pid_t new_pid = (current_process + i) % NB_PROC;
        if (process_table[new_pid].state == PRET && new_pid != 0) {
            int indx_current = current_process;
            arreter(current_process); 
            process_table[new_pid].state = ELU;
            sti();
            current_process = new_pid;
            // pour le debug : afficher le changement de contexte
            // printf("Changement de contexte : %s -> %s\n", process_table[indx_current].name, process_table[new_pid].name);
            // effectuer le changement de contexte entre le processus courant et le nouveau processus sélectionné par le scheduler
            ctx_sw(process_table[indx_current].regs, process_table[new_pid].regs);
            return;
        }
    }
}

// savoir si un processus avec un nom donné existe déjà dans la table des processus
fnptr process_exists(char *name) {
    for (int i = 0; i < NB_PROC - 1; i++) {
        if (process_table[i].state != SUPPRIME && process_table[i].state != VIDE && strcmp(process_table[i].name, name) == 0) {
            return process_table[i].function;
        }
    }
    return NULL;
}

// afficher la liste des processus avec leur PID, nom et état
void ps() {
    printf("PID   Name               State\n");
    printf("-------------------------------\n");
    for (int i = 0; i < NB_PROC-1; i++) {
        if (process_table[i].state != SUPPRIME && process_table[i].state != VIDE) {
            printf("%-5d %-18s ", process_table[i].pid, process_table[i].name);
            switch (process_table[i].state) {
                case PRET:
                    printf("PRET\n");
                    break;
                case BLOCKE:
                    printf("BLOCKE\n");
                    break;
                case ELU:
                    printf("ELU\n");
                    break;
                default:
                    printf("UNKNOWN\n");
            }
        }
    }
}