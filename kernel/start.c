#include <n7OS/cpu.h>
#include <inttypes.h>
#include <n7OS/processor_structs.h>
#include <n7OS/console.h>
#include <unistd.h>
#include <n7OS/paging.h>
#include <n7OS/irq.h>
#include <n7OS/time.h>

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
    // effacer l'écran et afficher le curseur au début de l'écran
    printf("\f");


    //test de la pagination
    alloc_page_entry(0xA000000, 1, 1); // allouer une page pour l'adresse 0xA000000
    uint32_t *ptr = (uint32_t *) 0xA000000;
    int page_defaut = *ptr;
    page_defaut++;

    // afficher l'état de la mémoire physique
    print_mem();

    // lancement des interruptions
    sti();    

    // initier le timer
    init_timer();
    // test interruption 50 : Ok
    // Envoyer une interruption 50
    __asm__("int $50"::);
    
    // on ne doit jamais sortir de kernel_start
    while (1) {
        // cette fonction arrete le processeur
        hlt();
    }
}
