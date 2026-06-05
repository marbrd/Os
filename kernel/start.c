#include <n7OS/cpu.h>
#include <inttypes.h>
#include <n7OS/processor_structs.h>
#include <n7OS/console.h>
#include <unistd.h>
#include <n7OS/paging.h>
#include <n7OS/irq.h>
#include <n7OS/time.h>
#include <n7OS/sys.h>
#include <unistd.h>
#include <n7OS/processus.h>

void kernel_start(void)
{
    // initialisation de la console
    init_console();

    // initialisation du tas du noyau
    init_kheap();

    // initialiser la pagination
    initialise_paging();

    // initialiser les interruptions
    init_irq();

    // initier les appels système
    init_syscall();

    // initier le clavier
    init_keyboard();

    // tests de la pagination

    // afficher l'état de la mémoire physique avant l'allocation de la page
    // print_mem(); // afficher l'état de la mémoire physique

    // tester l'allocation d'une page de mémoire physique à une adresse virtuelle
    alloc_page_entry(0xA000000, 1, 1); // allouer une page pour l'adresse 0xA000000
    uint32_t *ptr = (uint32_t *) 0xA000000;
    int page_defaut = *ptr;
    page_defaut++;

    // afficher l'état de la mémoire physique après l'allocation de la page
    // print_mem(); // afficher l'état de la mémoire physique après l'allocation de la page

    // test d'écriture et de lecture sur la page allouée
    // *ptr = 0xDEADBEEF;
    // uint32_t val = *ptr;
    // if (val == 0xDEADBEEF) {
    //     printf("valeur lue correctement : 0x%X\n", val);
    // } else {
    //     printf("valeur lue incorrectement : 0x%X\n", val);
    // }

    // en decommentant les lignes suivantes, on doit déclencher une exception de page fault car l'adresse 0xB000000 n'est jamais mappée
    // uint32_t *bad_ptr = (uint32_t *) 0xB000000;  // jamais mappée
    // int val = *bad_ptr;  // doit déclencher une exception de page fault

    // afficher le nom du système d'exploitation
    printf("              __  __                                                 \n");
    printf("             |  \\/  | __ _ _ __ ___  _   _  __ _ _ __  _   ___  __   \n");
    printf("             | |\\/| |/ _` | '__/ _ \\| | | |/ _` | '_ \\| | | \\ \\/ /   \n");
    printf("             | |  | | (_| | | | (_) | |_| | (_| | | | | |_| |>  <    \n");
    printf("             |_|  |_|\\__,_|_|  \\___/ \\__,_|\\__,_|_| |_|\\__,_/_/\\_\\   \n");
    printf("\n");
    printf("               --------------------------------------------------\n");
    printf("\n");
    printf("                          Demarrage de Marouanux OS...\n");
    printf("\n");
    printf("                   [========================================]\n");

    // lancement des interruptions
    sti();    

    // test interruption 50 : Ok
    // Envoyer une interruption 50
    //__asm__("int $50"::);

    // initier le timer
    init_timer();

    // initialiser la table des processus
    init_processes();
    
    // on ne doit jamais sortir de kernel_start
    while (1) {
        char ch = kgetch();
        if (ch != -1) printf("%c", ch);
        // cette fonction arrete le processeur
        hlt();
    }
}
