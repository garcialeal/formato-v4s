/*
 * v4s12_quant.c - Implementation of S12 Lattice Quantization
 * Copyright (C) 2026 ANTONIO GARCÍA LEAL
 */

#include "v4s12/v4s12_quant.h"

const uint8_t S12_RESIDUES[4] = {1, 5, 7, 11};

v4s12_int_t s12_quantize_spline(v4s12_int_t coord) {
    v4s12_int_t base_block = coord / 12;
    v4s12_int_t rem = coord % 12;

    /* Handle negative modulo in standard C integer arithmetic */
    if (rem < 0) {
        rem += 12;
        base_block -= 1;
    }

    /* Find nearest coprime residue in S12 = {1, 5, 7, 11} */
    v4s12_int_t best_s = S12_RESIDUES[0];
    v4s12_int_t min_dist = (rem - best_s < 0) ? (best_s - rem) : (rem - best_s);

    for (size_t i = 1; i < 4; ++i) {
        v4s12_int_t s = S12_RESIDUES[i];
        v4s12_int_t dist = (rem - s < 0) ? (s - rem) : (rem - s);
        if (dist < min_dist) {
            min_dist = dist;
            best_s = s;
        }
    }

    return (base_block * 12) + best_s;
}