#include <emscripten/bind.h>
#include "v4s12/v4s12.h"

using namespace emscripten;
using namespace v4s12;

// Exposición directa de la memoria de C++ a TypedArrays de JavaScript
val get_geometry_vertices(const Decoder& decoder, uint32_t index) {
    const auto& geoms = decoder.get_geometries();
    if (index >= geoms.size()) return val::null();
    return val(typed_memory_view(geoms[index].vertices.size(), geoms[index].vertices.data()));
}

val get_geometry_indices(const Decoder& decoder, uint32_t index) {
    const auto& geoms = decoder.get_geometries();
    if (index >= geoms.size()) return val::null();
    return val(typed_memory_view(geoms[index].indices.size(), geoms[index].indices.data()));
}

EMSCRIPTEN_BINDINGS(v4s12_wasm) {
    class_<Decoder>("Decoder")
        .constructor<>()
        // JavaScript pasa el binario del archivo como un string/Uint8Array
        .function("read_memory", optional_override([](Decoder& self, const std::string& buffer) {
            return self.read_memory(buffer.data(), buffer.size());
        }))
        .function("get_geometry_vertices", &get_geometry_vertices)
        .function("get_geometry_indices", &get_geometry_indices);
}