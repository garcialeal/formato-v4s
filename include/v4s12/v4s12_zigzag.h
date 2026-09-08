/*
 * v4s12_zigzag.h - Topo-entropic V4-ZigZag Permutation Mapping
 * Copyright (C) 2026 ANTONIO GARCÍA LEAL
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