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

#ifndef V4S12_V4_TRANSFORM_H
#define V4S12_V4_TRANSFORM_H

#include "v4s12/v4s12.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Fundamental 1D V4 butterfly transform (4 elements).
 * Complexity: O(1), 0 multiplications, integer additions/subtractions only.
 */
static inline void v4_transform_1d(v4s12_int_t *a, v4s12_int_t *b, v4s12_int_t *c, v4s12_int_t *d) {
    const v4s12_int_t s0 = *a + *b;
    const v4s12_int_t d0 = *a - *b;
    const v4s12_int_t s1 = *c + *d;
    const v4s12_int_t d1 = *c - *d;

    *a = s0 + s1;
    *b = d0 + d1;
    *c = s0 - s1;
    *d = d0 - d1;
}

/**
 * @brief Full 2D spectral transform over a 4x4 integer matrix.
 * @param block Contiguous array of 16 elements (4x4 matrix).
 */
void v4_transform_2d_4x4(v4s12_int_t block[16]);

/**
 * @brief Exact 2D reconstruction (Inverse Transform) via arithmetic bit-shift.
 * Guarantees MSE = 0.0000 bit-for-bit lossless accuracy.
 * @param block Spectral matrix of 16 elements (4x4 matrix).
 */
void v4_inverse_2d_4x4(v4s12_int_t block[16]);

/**
 * @brief 3D kernel (V4 x V4 x V4) over a 64-voxel SoA block (4x4x4).
 * Optimized for low-latency 3D LiDAR point cloud processing.
 * @param block_soa Contiguous array of 64 elements (aligned to cache line).
 */
void v4_kernel_3d_soa(v4s12_int_t block_soa[64]);

#ifdef __cplusplus
}
#endif

#endif /* V4S12_V4_TRANSFORM_H */