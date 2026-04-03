#include <n7OS/mem.h>

uint32_t free_page_bitmap_table[NBR_PAGES/32];

/**
 * @brief Marque la page allouée
 * 
 * Lorsque la page a été choisie, cette fonction permet de la marquer allouée
 * 
 * @param addr Adresse de la page à allouer
 */
void setPage(uint32_t addr) {
    uint32_t page_num = addr / PAGE_SIZE;
    uint32_t table_index = page_num / 32;
    uint32_t bit_index = page_num % 32;
    free_page_bitmap_table[table_index] |= (1 << bit_index);
}

/**
 * @brief Désalloue la page
 * 
 * Libère la page allouée.
 * 
 * @param addr Adresse de la page à libérer
 */
void clearPage(uint32_t addr) {
    uint32_t page_num = addr / PAGE_SIZE;
    uint32_t table_index = page_num / 32;
    uint32_t bit_index = page_num % 32;
    free_page_bitmap_table[table_index] &= ~(1 << bit_index);
}

/**
 * @brief Fourni la première page libre de la mémoire physique tout en l'allouant
 * 
 * @return uint32_t Adresse de la page sélectionnée
 */
uint32_t findfreePage() {
    uint32_t adresse= 0x0;
    for (int i=0; i < (NBR_PAGES/32); i++) {
        for (int j=0; j<32; j++) {
            if (!(free_page_bitmap_table[i] & (1 << j))) {
                adresse = (i*32+j)*PAGE_SIZE;
                setPage(adresse); // marquer la page comme allouée
                return adresse; // adresse de la page libre trouvée
            }
        }
    }
    return 0xFFFFFFFF; // aucune page libre trouvée
}

/**
 * @brief Initialise le gestionnaire de mémoire physique
 * 
 */
void init_mem() {
    memset(free_page_bitmap_table, 0, sizeof(free_page_bitmap_table)); // toutes les pages sont libres au début
}

/**
 * @brief Affiche l'état de la mémoire physique
 * 
 */
void print_mem() {
    printf("Taille total de la memoire : 16Mo\n");
    printf("Nombre total des pages : %d\n", NBR_PAGES);
    // Calcul de la mémoire libre
    int nbr_libre = 0;
    for (int i=0; i < (NBR_PAGES/32); i++) {
        for (int j=0; j<32; j++) {
            if (!(free_page_bitmap_table[i] & (1 << j))) {
                nbr_libre++;
            }
        }
    }
    printf("Taille de la memoire libre en Ko : %d, en Mo : %d\n",nbr_libre*4,(nbr_libre*4)/1000);
    printf("Nombre des pages libres : %d\n", nbr_libre);
}