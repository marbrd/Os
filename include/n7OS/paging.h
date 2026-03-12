/**
 * @file paging.h
 * @brief Gestion de la pagination dans le noyau
 */
#ifndef _PAGING_H
#define _PAGING_H

#include <inttypes.h>

/**
 * @brief Description d'une ligne de la table de page
 * 
 */
typedef struct {
    uint8_t P          : 1; // 1 si la page est présente en mémoire, 0 sinon
    uint8_t W          : 1; // 1 si la page est accessible en lecture/écriture, 0 si seulement lecture
    uint8_t U          : 1; // 1 si la page est accessible par l'utilisateur, 0 sinon
    uint8_t RSVD       : 2; // bits réservés
    uint8_t A          : 1; // 1 si la page a été accédée, 0 sinon
    uint8_t D          : 1; // 1 si la page a été modifiée, 0 sinon
    uint8_t RSVD2      : 2; // bits réservés
    uint8_t AVAIL      : 3; // bits disponibles pour le système d'exploitation
    uint32_t frame     : 20; // adresse de la page physique (frame) associée à cette entrée de table de page
} page_table_entry_t;

/**
 * @brief Une entrée dans la table de page peut être manipulée en utilisant
 *        la structure page_table_entry_t ou directement la valeur
 */
typedef union {
    page_table_entry_t page_entry;
    uint32_t value;
} PTE; // PTE = Page Table Entry 

/**
 * @brief Une table de page (PageTable) est un tableau de descripteurs de page
 * 
 */
typedef PTE * PageTable;

/**
 * @brief Description d'une ligne du répertoire de page
 * 
 */
typedef struct {
    uint8_t P          : 1; // 1 si la page est présente en mémoire, 0 sinon
    uint8_t W          : 1; // 1 si la page est accessible en lecture/écriture, 0 si seulement lecture
    uint8_t U          : 1; // 1 si la page est accessible par l'utilisateur, 0 sinon
    uint16_t RSVD       : 9; // bits réservés
    uint32_t frame     : 20; // adresse de la page physique (frame) associée à cette entrée du répertoire de page
} directory_table_entry_t;

/**
 * @brief Une entrée dans la table de page peut être manipulée en utilisant
 *        la structure directory_table_entry_t ou directement la valeur
 */
typedef union {
    directory_table_entry_t page_entry;
    uint32_t value;
} PDE; // PDE = Page Directory Entry 

/**
 * @brief Une table de page (DirTable) est un tableau de descripteurs de page
 * 
 */
typedef PDE * DirTable;

/**
 * @brief Cette fonction initialise le répertoire de page, alloue les pages de table du noyau
 *        et active la pagination
 * 
 */
void initialise_paging();

/**
 * @brief Cette fonction alloue une page de la mémoire physique à une adresse de la mémoire virtuelle
 * 
 * @param address       Adresse de la mémoire virtuelle à mapper
 * @param is_writeable  Si is_writeable == 1, la page est accessible en écriture
 * @param is_kernel     Si is_kernel == 1, la page ne peut être accédée que par le noyau
 * @return PageTable    La table de page modifiée
 */
PageTable alloc_page_entry(uint32_t address, int is_writeable, int is_kernel);
#endif