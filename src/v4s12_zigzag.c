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