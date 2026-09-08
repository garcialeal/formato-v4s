/*
 * v4s12_quant.h - Geometric Spline & Vector Quantization over S12 Lattice
 * Copyright (C) 2026 ANTONIO GARCÍA LEAL
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