#ifndef PROCESSUS_H
#define PROCESSUS_H

#include <inttypes.h>
#define NB_PROC 255
#define STACK_SIZE 1024

typedef enum {ELU, PRET, BLOCKE, SUPPRIME, VIDE} PROCESS_STATE;
typedef uint32_t pid_t;

typedef void* (*fnptr)(void);

struct process_t {
    char name[32];        // nom du programme
    fnptr function;       // adresse de la fonction
    uint32_t *stack;      // pile
    PROCESS_STATE state;  // état
    pid_t pid;            // identifiant
    uint32_t regs[5];     // registres sauvegardés
    uint32_t wakeup_time; // temps de réveil pour les processus endormis
};

// process_table en externe pour pouvoir débloquer les processus blockés dans le handler du timer
extern struct process_t process_table[NB_PROC-1];

// allouer un PID libre dans la table des processus
pid_t allouer_pid();

// initialiser la table des processus et créer les processus initiaux
void init_processes();

// créer un processus et retourner son PID
pid_t creer(char name[32], fnptr function);

// supprimer un processus de la table des processus (le mettre dans l'état SUPPRIME)
void terminer();

// arrêter un processus en cours d'exécution (le mettre dans l'état PRET)
void arreter();

// debloquer un processus bloqué (le mettre dans l'état PRET)
void debloquer(pid_t pid);

// retourne le PID du processus courant
pid_t getpid_process();

// créer un nouveau processus en dupliquant un processus existant (le processus parent) et retourner le PID du processus enfant
pid_t fork_process(const char *name, fnptr function);

// faire dormir un processus pendant un certain nombre de millisecondes
int sleep_process(int duration);

// scheduler pour choisir le prochain processus à exécuter
void scheduler();

#endif