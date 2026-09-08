#ifndef V4S12_G3_HASH_H
#define V4S12_G3_HASH_H

#include <cstdint>
#include <cstddef>
#include <array>

namespace v4s12 {

using Hash256 = std::array<uint8_t, 32>;

// Firma un bloque binario arbitrario (vértices e índices) con G3-RH512-256
Hash256 compute_g3_hash(const void* data, size_t size);

} // namespace v4s12

#endif