#include <n7OS/cpu.h>
#include <inttypes.h>
#include <n7OS/processor_structs.h>
#include <n7OS/console.h>
#include <unistd.h>

void kernel_start(void)
{
    init_console();

    // effacer l'écran et afficher le curseur au début de l'écran
    printf("\f");
    
        // initialiser la pagination
    initialise_paging();
    alloc_page_entry(0xA000000, 1, 1); // allouer une page pour l'adresse 0xA000000
    uint32_t *ptr = (uint32_t *) 0xA000000;
    int page_defaut = *ptr;
    page_defaut++;

    // lancement des interruptions
    sti();

    // on ne doit jamais sortir de kernel_start
    while (1) {
        // cette fonction arrete le processeur
        hlt();
    }
}
