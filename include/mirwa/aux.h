#ifndef MIRWA_AUX_H
#define MIRWA_AUX_H

#define MIRWA_NON_NULL static 1

/**
 * Constructs a pointer to an unnamed object in-place.
 *
 * @see https://en.cppreference.com/w/c/language/compound_literal
 */
#define MIRWA_OBJ(T, expr) ((T *)(T[]){expr})

#define MIRWA_TYPEDEF_STRUCT(T) typedef struct T T

#define MIRWA_SEMICOLON_IN_BLOCK()                                                                 \
    do {                                                                                           \
    } while (0)

#endif // MIRWA_AUX_H
