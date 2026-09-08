/*
 * spectral_pipeline.h - Spectral Pipeline Interface - VERSION 2.0
 * Copyright (C) 2026 ANTONIO GARCÍA LEAL
 */

#ifndef V4S12_SPECTRAL_PIPELINE_H
#define V4S12_SPECTRAL_PIPELINE_H

#include "v4s12/types.h"

#ifdef __cplusplus
extern "C" {
#endif

void v4_transform_2d_4x4(v4s12_int_t block[16]);
void v4_zigzag_reorder(const v4s12_int_t in_block[16], v4s12_int_t out_vector[16]);
v4s12_int_t s12_quantize_spline(v4s12_int_t coord);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
namespace v4s12 {

// Prototipos de las funciones inversas (Decodificador)
void v4_zigzag_reorder_inv(const v4s12_int_t in_vector[16], v4s12_int_t out_block[16]);
void v4_transform_2d_4x4_inv(v4s12_int_t block[16]);
v4s12_int_t s12_dequantize_spline(v4s12_int_t quantized_coord);

} // namespace v4s12
#endif

#endif // V4S12_SPECTRAL_PIPELINE_H