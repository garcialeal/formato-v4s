/*
 * formato-v4s / LIBV4S12 - Deterministic Vector Geometry Engine
 *
 * Copyright (C) 2026 Antonio García Leal (garcialeal)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
 
#include "v4s12/v4s12.h"
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file.v4s>\n";
        return 1;
    }

    v4s12::Decoder decoder;
    if (!decoder.read_file(argv[1])) {
        std::cerr << "Error: Failed to decode file or invalid v4s12 format.\n";
        return 1;
    }

    const auto& geoms = decoder.get_geometries();
    const auto& nodes = decoder.get_nodes();

    std::cout << "--- V4S12 File Info ---\n";
    std::cout << "Geometries : " << geoms.size() << "\n";
    std::cout << "Nodes      : " << nodes.size() << "\n";

    size_t total_verts = 0;
    for (size_t i = 0; i < geoms.size(); ++i) {
        total_verts += geoms[i].original_vertex_count;
        std::cout << "  Geom [" << i << "] Vertices: " << geoms[i].original_vertex_count 
                  << " | Indices: " << geoms[i].original_index_count << "\n";
    }
    
    std::cout << "Total Original Vertices: " << total_verts << "\n";
    return 0;
}