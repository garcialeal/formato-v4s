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

#ifndef V4S12_V4_ZIGZAG_H
#define V4S12_V4_ZIGZAG_H

#include "v4s12/v4s12.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Klein orbit energy mapping array */
extern const uint8_t V4_ZIGZAG_MAP[16];

/**
 * @brief Entropic V4-ZigZag reordering to maximize EOB truncation.
 * @param in_block Quantized 4x4 matrix (16 elements).
 * @param out_vector Output array sorted by Klein orbit variance (16 elements).
 */
void v4_zigzag_reorder(const v4s12_int_t in_block[16], v4s12_int_t out_vector[16]);

#ifdef __cplusplus
}
#endif

#endif /* V4S12_V4_ZIGZAG_H */