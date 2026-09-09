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

#ifndef V4S12_S12_QUANT_H
#define V4S12_S12_QUANT_H

#include "v4s12/v4s12.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Base-12 coprime residue table S12 = {1, 5, 7, 11} (mod 12) */
extern const uint8_t S12_RESIDUES[4];

/**
 * @brief Geometric projection of spline coordinates onto the S12 grid.
 * Reduces state space from 12 to 4 states (44.21% bit-rate savings).
 * @param coord Original integer coordinate.
 * @return Projected coordinate matching closest S12 node (preserves C1 continuity).
 */
v4s12_int_t s12_quantize_spline(v4s12_int_t coord);

#ifdef __cplusplus
}
#endif

#endif /* V4S12_S12_QUANT_H */