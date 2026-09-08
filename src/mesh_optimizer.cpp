#include "v4s12/mesh_optimizer.h"
#include "v4s12/g3_hash.h"
#include <unordered_map>
#include <tuple>
#include <cmath>
#include <cstring>

namespace v4s12 {

// Envoltorio para inyectar G3-RH512-256 como motor de hash de vértices
struct G3SpatialHash {
    size_t operator()(const std::tuple<float, float, float>& vertex) const {
        // Empaquetamos las coordenadas truncadas en un array contiguo
        float data[3] = { std::get<0>(vertex), std::get<1>(vertex), std::get<2>(vertex) };
        
        // Calculamos la firma de 256 bits con el algoritmo G3 nativo
        Hash256 full_hash = compute_g3_hash(data, sizeof(data));
        
        // Extraemos los primeros 64 bits (size_t) para el mapeo rápido en RAM
        size_t quick_hash;
        std::memcpy(&quick_hash, full_hash.data(), sizeof(size_t));
        return quick_hash;
    }
};

void optimize_geometry(
    const std::vector<float>& in_vertices,
    const std::vector<uint32_t>& in_indices,
    std::vector<float>& out_vertices,
    std::vector<uint32_t>& out_indices) 
{
    out_vertices.clear();
    out_indices.clear();
    out_indices.reserve(in_indices.size());

    // Usamos el hash G3 en lugar del estándar de la librería C++
    std::unordered_map<std::tuple<float, float, float>, uint32_t, G3SpatialHash> vertex_map;
    uint32_t current_new_index = 0;

    for (size_t i = 0; i < in_indices.size(); ++i) {
        uint32_t original_idx = in_indices[i];
        size_t v_offset = original_idx * 3;

        // Truncamiento milimétrico para absorber derivas de AutoCAD antes del Hash
        auto v_tuple = std::make_tuple(
            std::round(in_vertices[v_offset] * 1e5f) / 1e5f,
            std::round(in_vertices[v_offset + 1] * 1e5f) / 1e5f,
            std::round(in_vertices[v_offset + 2] * 1e5f) / 1e5f
        );

        auto it = vertex_map.find(v_tuple);
        if (it == vertex_map.end()) {
            vertex_map[v_tuple] = current_new_index;
            out_vertices.push_back(in_vertices[v_offset]);
            out_vertices.push_back(in_vertices[v_offset + 1]);
            out_vertices.push_back(in_vertices[v_offset + 2]);
            
            out_indices.push_back(current_new_index);
            current_new_index++;
        } else {
            out_indices.push_back(it->second);
        }
    }
}

} // namespace v4s12