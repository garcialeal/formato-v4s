/*
 * v4s12_c_api.h - C-API Export Interface for WASM and Plugins
 * Copyright (C) 2026 ANTONIO GARCÍA LEAL
 */

#ifndef V4S12_C_API_H
#define V4S12_C_API_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */
/* DECODER C-API                                                             */
/* ========================================================================= */

typedef struct v4s12_decoder_t v4s12_decoder_t;

v4s12_decoder_t* v4s12_decoder_create(void);
void v4s12_decoder_destroy(v4s12_decoder_t* decoder);

int v4s12_decoder_load_memory(v4s12_decoder_t* decoder, const char* buffer, size_t size);

uint32_t v4s12_decoder_get_geometry_count(const v4s12_decoder_t* decoder);
uint32_t v4s12_decoder_get_node_count(const v4s12_decoder_t* decoder);

int v4s12_decoder_get_geometry_info(const v4s12_decoder_t* decoder, uint32_t index, 
                                   uint32_t* out_vertex_count, uint32_t* out_index_count);

int v4s12_decoder_read_geometry(const v4s12_decoder_t* decoder, uint32_t index,
                                float* out_vertices, uint32_t* out_indices);

int v4s12_decoder_read_node(const v4s12_decoder_t* decoder, uint32_t index,
                            uint32_t* out_geom_id, double out_transform[16], uint32_t* out_color);

/* ========================================================================= */
/* ENCODER C-API                                                             */
/* ========================================================================= */

typedef struct v4s12_encoder_t v4s12_encoder_t;

v4s12_encoder_t* v4s12_encoder_create(void);
void v4s12_encoder_destroy(v4s12_encoder_t* encoder);

uint32_t v4s12_encoder_add_geometry(v4s12_encoder_t* encoder,
                                    const float* vertices, size_t vertex_count,
                                    const float* normals, size_t normal_count,
                                    const uint32_t* indices, size_t index_count);

void v4s12_encoder_add_node(v4s12_encoder_t* encoder, 
                            uint32_t geometry_id, 
                            const double transform[16], 
                            uint32_t layer_color_rgba, 
                            uint32_t node_flags);

size_t v4s12_encoder_encode(v4s12_encoder_t* encoder);

int v4s12_encoder_copy_buffer(v4s12_encoder_t* encoder, uint8_t* out_buffer, size_t buffer_size);

int v4s12_encoder_write_file(v4s12_encoder_t* encoder, const char* filepath);

#ifdef __cplusplus
}
#endif

#endif // V4S12_C_API_H