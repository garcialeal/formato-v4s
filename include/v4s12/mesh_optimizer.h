#ifndef V4S12_MESH_OPTIMIZER_H
#define V4S12_MESH_OPTIMIZER_H

#include <vector>
#include <cstdint>

namespace v4s12 {

// Toma los datos brutos exportados de AutoCAD y devuelve una malla indexada y limpia
void optimize_geometry(
    const std::vector<float>& in_vertices,
    const std::vector<uint32_t>& in_indices,
    std::vector<float>& out_vertices,
    std::vector<uint32_t>& out_indices
);

} // namespace v4s12

#endif // V4S12_MESH_OPTIMIZER_H