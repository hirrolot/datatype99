#ifndef MIRWA_AUX_H
#define MIRWA_AUX_H

#define MIRWA_NON_NULL static 1

/**
 * Constructs a pointer to an unnamed object in-place.
 *
 * @see https://en.cppreference.com/w/c/language/compound_literal
 */
#define MIRWA_OBJ(T, expr) ((T *)(T[]){expr})

#endif // MIRWA_AUX_H
