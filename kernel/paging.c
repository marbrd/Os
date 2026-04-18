#include <n7OS/paging.h>
#include <stddef.h> // nécessaire pour NULL
#include <n7OS/mem.h>
#include <n7OS/kheap.h>
#include <n7OS/processor_structs.h>

DirTable repertoire_page; // répertoire de pages du noyau

void initialise_paging() {
    init_mem(); // initialiser le gestionnaire de mémoire physique
    repertoire_page = (DirTable) kmalloc_a(PAGE_SIZE); // allouer une page pour le répertoire de pages
    for (int i=0; i<1024; i++) { // initialiser les entrées du répertoire de pages
        repertoire_page[i].page_entry.P = 1; // marquer les pages comme présentes
        repertoire_page[i].page_entry.W = 1; // définir les permissions d'écriture
        repertoire_page[i].page_entry.U = 0; // définir les permissions d'accès utilisateur
        repertoire_page[i].page_entry.frame = kmalloc_a(PAGE_SIZE) >> 12; // adresse physique de la page
        alloc_page_entry(i*PAGE_SIZE, 1, 1); 
    }

    // charger l'adresse du répertoire de pages dans le registre CR3
    __asm__ __volatile__("mov %0, %%cr3":: "r" ((uint32_t) repertoire_page));
    // activer la pagination en définissant le bit PG dans le registre CR0
    uint32_t cr0;
    __asm__ __volatile__("mov %%cr0, %0" : "=r" (cr0));
    cr0 |= 0x80000000; // définir le bit PG
    __asm__ __volatile__("mov %0, %%cr0":: "r" (cr0));  

    // activer la pagination en chargeant l'adresse du répertoire de pages dans le registre CR3
    setup_base((uint32_t)repertoire_page);
}

PageTable alloc_page_entry(uint32_t address, int is_writeable, int is_kernel) {
    PageTable pgtab = (PTE*)(repertoire_page[address >> 22].page_entry.frame << 12); // adresse physique de la table de page
    PTE* page = &pgtab[(address >> 12) & 0x3FF]; // entrée de la table de page correspondant à l'adresse
    page->page_entry.P = 1; // marquer la page comme présente
    page->page_entry.W = is_writeable; // définir les permissions d'écriture
    page->page_entry.U = !is_kernel; // définir les permissions d'accès utilisateur
    page->page_entry.frame = findfreePage() >> 12; // adresse physique de la page à allouer
    return pgtab;
}
