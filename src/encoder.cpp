#include "v4s12/v4s12.h"
#include "v4s12/types.h"
#include "v4s12/mesh_optimizer.h"
#include <fstream>
#include <zstd.h>
#include <iostream>
#include <algorithm>
#include <cstring>
#include <limits>
#include <cmath>

namespace v4s12 {

void Encoder::calculate_aabb(const std::vector<float>& vertices, double min[3], double max[3]) {
    for (int i = 0; i < 3; ++i) {
        min[i] = std::numeric_limits<double>::max();
        max[i] = std::numeric_limits<double>::lowest();
    }
    for (size_t i = 0; i < vertices.size(); i += 3) {
        if (vertices[i] < min[0]) min[0] = vertices[i];
        if (vertices[i] > max[0]) max[0] = vertices[i];
        if (vertices[i+1] < min[1]) min[1] = vertices[i+1];
        if (vertices[i+1] > max[1]) max[1] = vertices[i+1];
        if (vertices[i+2] < min[2]) min[2] = vertices[i+2];
        if (vertices[i+2] > max[2]) max[2] = vertices[i+2];
    }
}

uint32_t Encoder::add_geometry(const std::vector<float>& vertices,
                               const std::vector<float>& normals,
                               const std::vector<uint32_t>& indices) {
                               
    // 1. Deduplicación mediante G3-RH512-256
    Hash256 current_hash = compute_g3_hash(vertices.data(), vertices.size() * sizeof(float));
    for (size_t i = 0; i < geometries_.size(); ++i) {
        if (geometries_[i].hash == current_hash) return static_cast<uint32_t>(i);
    }

    GeometryData geom;
    geom.hash = current_hash;
    
    // 2. Limpieza y Soldadura de Vértices (Mesh Optimizer)
    std::vector<float> opt_vertices;
    std::vector<uint32_t> opt_indices;
    optimize_geometry(vertices, indices, opt_vertices, opt_indices);
    
    geom.original_vertex_count = opt_vertices.size() / 3;
    geom.original_index_count = opt_indices.size();

    // 3. Extracción de Caja Delimitadora Local sobre geometría optimizada
    calculate_aabb(opt_vertices, geom.local_aabb_min, geom.local_aabb_max);

    // 4. Cuantización S12 y Preparación de Bloques
    std::vector<v4s12_int_t> quantized_data;
    quantized_data.reserve(opt_vertices.size());
    
    double range[3] = {
        std::max(geom.local_aabb_max[0] - geom.local_aabb_min[0], 1e-5),
        std::max(geom.local_aabb_max[1] - geom.local_aabb_min[1], 1e-5),
        std::max(geom.local_aabb_max[2] - geom.local_aabb_min[2], 1e-5)
    };

    for (size_t i = 0; i < opt_vertices.size(); i += 3) {
        for (int j = 0; j < 3; ++j) {
            // Normalización a espacio [0, 4095] (12-bits)
            double normalized = (opt_vertices[i+j] - geom.local_aabb_min[j]) / range[j];
            v4s12_int_t coord = static_cast<v4s12_int_t>(std::round(normalized * 4095.0));
            // Proyección sobre la malla residual S12
            quantized_data.push_back(s12_quantize_spline(coord));
        }
    }

    // Padding para asegurar múltiplos de 16 (requerido por v4_transform_2d_4x4)
    while (quantized_data.size() % 16 != 0) {
        quantized_data.push_back(0); 
    }

    // 5. Transformada en Mariposa V4 y Reordenamiento ZigZag
    std::vector<v4s12_int_t> spectral_payload(quantized_data.size());
    for (size_t i = 0; i < quantized_data.size(); i += 16) {
        v4s12_int_t block[16];
        std::memcpy(block, &quantized_data[i], 16 * sizeof(v4s12_int_t));
        
        v4_transform_2d_4x4(block);
        
        v4s12_int_t zigzag_block[16];
        v4_zigzag_reorder(block, zigzag_block);
        
        std::memcpy(&spectral_payload[i], zigzag_block, 16 * sizeof(v4s12_int_t));
    }

    // 6. Ensamblaje del Payload Binario
    std::vector<uint8_t> raw_payload;
    size_t payload_bytes = spectral_payload.size() * sizeof(v4s12_int_t) + opt_indices.size() * sizeof(uint32_t);
    raw_payload.resize(payload_bytes);
    
    size_t spectral_bytes = spectral_payload.size() * sizeof(v4s12_int_t);
    std::memcpy(raw_payload.data(), spectral_payload.data(), spectral_bytes);
    std::memcpy(raw_payload.data() + spectral_bytes, opt_indices.data(), opt_indices.size() * sizeof(uint32_t));

    // 7. Compresión Entrópica ZSTD
    size_t max_compressed = ZSTD_compressBound(raw_payload.size());
    geom.compressed_payload.resize(max_compressed);
    size_t c_size = ZSTD_compress(geom.compressed_payload.data(), max_compressed,
                                  raw_payload.data(), raw_payload.size(), 3);
    geom.compressed_payload.resize(c_size);

    geometries_.push_back(std::move(geom));
    return static_cast<uint32_t>(geometries_.size() - 1);
}

void Encoder::add_node(uint32_t geometry_id, const double transform[16], uint32_t layer_color_rgba, uint32_t node_flags) {
    SceneNode node;
    node.geometry_id = geometry_id;
    std::copy(transform, transform + 16, node.transform.begin());
    node.layer_color_rgba = layer_color_rgba;
    node.node_flags = node_flags;
    nodes_.push_back(node);
}

bool Encoder::write_memory(std::vector<uint8_t>& out_buffer) const {
    Header hdr;
    hdr.num_geometries = static_cast<uint32_t>(geometries_.size());
    hdr.num_nodes = static_cast<uint32_t>(nodes_.size());
    hdr.flags = FLAG_ZSTD_COMPRESSED | FLAG_LAYER_COLORS;
    
    hdr.offset_hashtable = sizeof(Header); 
    hdr.offset_geometries = hdr.offset_hashtable + (nodes_.size() * sizeof(SceneNode));

    size_t total_size = sizeof(Header) + (nodes_.size() * sizeof(SceneNode));
    for (const auto& g : geometries_) {
        total_size += 32 + (3 * sizeof(double)) * 2 + (sizeof(uint32_t) * 3) + g.compressed_payload.size();
    }

    out_buffer.clear();
    out_buffer.reserve(total_size);

    auto append_bytes = [&out_buffer](const void* src, size_t size) {
        const uint8_t* p = reinterpret_cast<const uint8_t*>(src);
        out_buffer.insert(out_buffer.end(), p, p + size);
    };

    append_bytes(&hdr, sizeof(Header));
    if (!nodes_.empty()) {
        append_bytes(nodes_.data(), nodes_.size() * sizeof(SceneNode));
    }

    for (const auto& g : geometries_) {
        append_bytes(g.hash.data(), 32);
        append_bytes(g.local_aabb_min, 3 * sizeof(double));
        append_bytes(g.local_aabb_max, 3 * sizeof(double));
        append_bytes(&g.original_vertex_count, sizeof(uint32_t));
        append_bytes(&g.original_index_count, sizeof(uint32_t));
        
        uint32_t payload_size = static_cast<uint32_t>(g.compressed_payload.size());
        append_bytes(&payload_size, sizeof(uint32_t));
        append_bytes(g.compressed_payload.data(), payload_size);
    }

    return true;
}

bool Encoder::write_file(const std::string& filepath) {
    std::vector<uint8_t> buffer;
    if (!write_memory(buffer)) return false;

    std::ofstream out(filepath, std::ios::binary);
    if (!out.is_open()) return false;

    out.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
    return out.good();
}

} // namespace v4s12