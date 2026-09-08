/*
 * v4s12_zigzag.c - Implementation of V4-ZigZag Energy Reordering
 * Copyright (C) 2026 ANTONIO GARCÍA LEAL
 */

#include "v4s12/v4s12_zigzag.h"

const uint8_t V4_ZIGZAG_MAP[16] = {
    0,  1,  4,  5,   /* DC & Primary Klein orbit 'e' */
    2,  3,  6,  7,   /* Klein orbit 'a' */
    8,  9,  12, 13,  /* Klein orbit 'b' */
    10, 11, 14, 15   /* Klein orbit 'ab' (high frequency) */
};

void v4_zigzag_reorder(const v4s12_int_t in_block[16], v4s12_int_t out_vector[16]) {
    for (size_t i = 0; i < 16; ++i) {
        out_vector[i] = in_block[V4_ZIGZAG_MAP[i]];
    }
}