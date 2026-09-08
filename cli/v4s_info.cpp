#include "v4s12/v4s12.h"
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <archivo.v4s>\n";
        return 1;
    }

    v4s12::Decoder decoder;
    if (!decoder.read_file(argv[1])) {
        std::cerr << "Error: No se pudo decodificar el archivo o no es un formato v4s12 válido.\n";
        return 1;
    }

    const auto& geoms = decoder.get_geometries();
    const auto& nodes = decoder.get_nodes();

    std::cout << "--- V4S12 File Info ---\n";
    std::cout << "Geometrías : " << geoms.size() << "\n";
    std::cout << "Nodos      : " << nodes.size() << "\n";

    size_t total_verts = 0;
    for (size_t i = 0; i < geoms.size(); ++i) {
        total_verts += geoms[i].original_vertex_count;
        std::cout << "  Geom [" << i << "] Vértices: " << geoms[i].original_vertex_count 
                  << " | Índices: " << geoms[i].original_index_count << "\n";
    }
    
    std::cout << "Total Vértices Originales: " << total_verts << "\n";
    return 0;
}