/*
 * v4s12_transform.c - Implementation of V4 Butterfly Transforms
 * Copyright (C) 2026 ANTONIO GARCÍA LEAL
 */

#include "v4s12/v4s12_transform.h"

void v4_transform_2d_4x4(v4s12_int_t block[16]) {
    /* Phase 1: Separable row transform */
    for (size_t i = 0; i < 16; i += 4) {
        v4_transform_1d(&block[i], &block[i + 1], &block[i + 2], &block[i + 3]);
    }

    /* Phase 2: Separable column transform */
    for (size_t i = 0; i < 4; ++i) {
        v4_transform_1d(&block[i], &block[i + 4], &block[i + 8], &block[i + 12]);
    }
}

void v4_inverse_2d_4x4(v4s12_int_t block[16]) {
    /* Re-apply forward matrix due to self-involutive property H_V4 * H_V4 = 4 * I_4 */
    v4_transform_2d_4x4(block);

    /* Exact bit-for-bit scaling via arithmetic right shift (>> 4) */
    for (size_t i = 0; i < 16; ++i) {
        block[i] >>= 4;
    }
}

void v4_kernel_3d_soa(v4s12_int_t block_soa[64]) {
    /* Phase 1: X-axis transform (16 lines of 4 elements) */
    for (size_t i = 0; i < 64; i += 4) {
        v4_transform_1d(&block_soa[i], &block_soa[i + 1], 
                        &block_soa[i + 2], &block_soa[i + 3]);
    }

    /* Phase 2: Y-axis transform (16 columns of 4 elements) */
    for (size_t x = 0; x < 4; ++x) {
        for (size_t slice = 0; slice < 4; ++slice) {
            size_t base = (slice * 16) + x;
            v4_transform_1d(&block_soa[base], &block_soa[base + 4], 
                            &block_soa[base + 8], &block_soa[base + 12]);
        }
    }

    /* Phase 3: Z-axis transform (16 depth vectors of 4 elements) */
    for (size_t i = 0; i < 16; ++i) {
        v4_transform_1d(&block_soa[i], &block_soa[i + 16], 
                        &block_soa[i + 32], &block_soa[i + 48]);
    }
}