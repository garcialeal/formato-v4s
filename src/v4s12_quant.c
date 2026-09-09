/*
 * formato-v4s / LIBV4S12 - Deterministic Vector Geometry Engine
 *
 * Copyright (C) 2026 Antonio García Leal (garcialeal)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
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