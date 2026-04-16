#include "flush_mem.h"
#include <stdint.h>

/*
 * flush_mem : met toutes les valeurs du tableau à zéro
 */
void flush_mem(image_type tab[TAB_SIZE]) {
    for (int i = 0; i < TAB_SIZE; i++) {
        tab[i] = 0;
    }
}

