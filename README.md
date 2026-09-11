# v4s Format Specification & V4S12-MATHEMATICAL-CORE (v1.0)
*Deterministic Vector Data Architecture, V4 Spectral Transform & $S_{12}$ Lattice Engine*

**Lead Author:** Antonio García Leal  
**Core / Specification License:** Dual License (GPLv3 / Commercial License)  
**Language Standard:** C99 (External Mathematical Core & Hash) / C++17 (Core Engine, Pipeline & Containers)

---

## 1. Overview

**v4s v1.0** is a high-performance binary format and deterministic geometric processing engine engineered to eliminate IEEE 754 floating-point bottlenecks, topological drift, and memory redundancy in massive 3D CAD, GIS, and BIM datasets.

By combining the **V4 Butterfly Spectral Transform**, **$S_{12}$ Coprime Residue Lattice Quantization**, and the **G3-RH512-256 Spatial Hashing Algorithm**, **v4s** executes core geometric transformations via bitwise integer combinatorics (**0 FLOPs**). This delivers $O(1)$ instanced deduplication, strict bounded quantization error, and direct Zero-Copy memory mapping for WebGPU, WebAssembly, and native CAD engines.

---

## 2. Modular Architecture & Project Structure

The project decouples low-level mathematical primitives into submodules located in `external/`, maintaining a clean separation between high-level container handling and the core algebra.

    v4s-format/
    ├── external/                            # Core submodules (C99)
    │   ├── HASH-G3-RH512-256/              # G3-RH512-256 spatial hashing algorithm
    │   └── V4S12-MATHEMATICAL-CORE/        # S12 quantization, V4 transform & zigzag
    ├── include/v4s12/                      # Public V4S12-MATHEMATICAL-CORE headers (C++17 / C API)
    │   ├── types.h                         # Container structures and binary types
    │   ├── v4s12.h                         # High-level container API
    │   ├── v4s12_c_api.h                   # Interoperability C-API (WASM / C# / Python)
    │   ├── mesh_optimizer.h                # Topological reordering algorithms
    │   └── spectral_pipeline.h             # Spectral transform pipeline orchestration
    ├── src/                                # Engine implementation
    │   ├── encoder.cpp / decoder.cpp       # .v4s binary serializer/deserializer
    │   ├── mesh_optimizer.cpp              # Vertex cache & topological optimization
    │   ├── spectral_pipeline.cpp           # Quantization & transform pipeline wrappers
    │   ├── v4s12_c_api.cpp                 # C-API wrapper implementation
    │   └── wasm_export.cpp                 # WebAssembly Emscripten entry points
    ├── cli/                                # Command line tools (v4s_info)
    ├── tools/                              # Benchmark generators (generate_benchmarks)
    └── test/                               # Verification suite (test_v2)

---

## 3. Processing Pipeline & Technical Features

The **v4s** pipeline executes a 5-stage processing sequence:

    [Float32 / Double CAD Geometry]
                   │
                   ▼
     [1. G3 Spatial Hashing] ──────────> O(1) Instanced deduplication
                   │
                   ▼
     [2. S12 Lattice Quantization] ────> Bounded C1 continuous coordinate discretization
                   │
                   ▼
     [3. V4 Spectral Transform] ───────> Integer bitwise combinatorics (0 FLOPs)
                   │
                   ▼
     [4. Entropic Reordering] ─────────> Energy grouping via Klein orbits & Zigzag
                   │
                   ▼
     [5. v4s Binary Packing] ──────────> Memory-aligned ZSTD stream serialization

### 3.1 G3 Spatial Hashing Engine
* **Non-Cryptographic Fast Hashing:** Powered by `g3-rh512-256` in pure C99 for instant $O(1)$ mesh hash generation.
* **Structural Instancing:** Detects duplicate meshes before geometry processing, eliminating storage overhead in massive CAD assemblies.

### 3.2 $S_{12}$ Lattice Quantization
* **Strict Error Bounding:** Replaces float32 representations with discrete lattice coordinates, guaranteeing $C^1$ tangential continuity along shared boundaries.
* **Verified Precision:** Reversibility tests confirm a maximum quantization error bounded at $\Delta_{max} \approx 0.0004884$ units (sub-millimeter accuracy for CAD/GIS applications).

### 3.3 V4 Butterfly Spectral Transform (0 FLOPs)
* **Integer Combinatorics:** Eliminates floating-point matrix multiplication in favor of shift-and-add register-level operations (`>>`).
* **Cross-Platform Determinism:** Prevents rendering discrepancies across heterogeneous web/mobile clients by avoiding IEEE 754 rounding differences.

---

## 4. Empirical Benchmarks & Empirical Validation

The pipeline performance has been evaluated on dense meshes and instanced CAD assemblies.

### 4.1 Benchmark Metrics Summary

| Dataset / Benchmark File | Input Description | Raw Memory Size | `.v4s` Compressed Size | Efficiency / Reduction |
| :--- | :--- | :--- | :--- | :--- |
| **`dense_sphere_180k.v4s`** | 89,110 vertices, 536,406 indices (180k triangles) | ~3.22 MB | **1.3 MB** | **~60% Reduction** (~14.5 B/vertex incl. topology) |
| **`cad_instanced_10k.v4s`** | 10,000 scene nodes / instances (1 base mesh) | ~3.20 MB | **1.4 MB** | **~140 Bytes/Node** (High-density instancing) |

### 4.2 Reversibility & Round-Trip Validation (`test_v2`)
* **Pipeline Integrity:** Tested via `./build/test_v2` over full `S12 -> V4 -> ZSTD -> De-ZSTD -> Inv-V4 -> De-S12` round-trip.
* **Reconstruction Accuracy:**
  * Original Coordinate: `X = 1.000000`
  * Restored Coordinate: `X = 0.999512`
  * Maximum Observed Error ($\Delta$): `0.0004884` units.
* **Verification Status:** `SUCCESS` (Topological reversibility and bounded precision fully validated).

---

## 5. Binary Format Specification (.v4s v1.0)

A **.v4s** file consists of three memory-aligned contiguous blocks:

    +-------------------------------------------------------+
    | Header (64 bytes, 8-byte aligned)                     |
    +-------------------------------------------------------+
    | Scene Nodes Table (num_nodes * sizeof(SceneNode))     |
    +-------------------------------------------------------+
    | Geometry Blocks (Hash + AABB + ZSTD Payload Stream)   |
    +-------------------------------------------------------+

### 5.1 Header Structure (`Header`)

| Field | Type | Size | Description |
| :--- | :--- | :--- | :--- |
| `magic` | `char[4]` | 4 bytes | Identifies format (`'V'`, `'4'`, `'S'`, `'2'`) |
| `version` | `uint16_t` | 2 bytes | Standard version (`100` = v1.0) |
| `flags` | `uint16_t` | 2 bytes | Global format bitmask |
| `global_aabb_min` | `double[3]` | 24 bytes | Minimum bounding box ($X, Y, Z$) |
| `global_aabb_max` | `double[3]` | 24 bytes | Maximum bounding box ($X, Y, Z$) |
| `num_geometries` | `uint32_t` | 4 bytes | Count of unique deduplicated geometry blocks |
| `num_nodes` | `uint32_t` | 4 bytes | Count of scene instances / node transformations |
| `offset_hashtable`| `uint64_t` | 8 bytes | Byte offset to scene nodes block |
| `offset_geometries`| `uint64_t`| 8 bytes | Byte offset to payload stream |

### 5.2 Format Bitmask Flags (`FormatFlags`)
* `FLAG_NONE` (`0x0000`): Base configuration.
* `FLAG_HAS_NORMALS` (`0x0001`): Encoded normal attributes.
* `FLAG_LAYER_COLORS` (`0x0002`): Per-entity packed 32-bit RGBA colors.
* `FLAG_ZSTD_COMPRESSED` (`0x0004`): Payload compressed via ZSTD algorithm.
* `FLAG_INDEX_16BIT` (`0x0008`): 16-bit index buffers for optimized small geometries.

---

## 6. Native C-API (`v4s12_c_api.h`)

An opaque pointer interface (`v4s12_decoder_t`) exposes the decoder to WebAssembly, C#, Python, Rust, and ObjectARX/C++ plugins:

    #include "v4s12/v4s12_c_api.h"

    // Lifecycle management
    v4s12_decoder_t* v4s12_decoder_create(void);
    void v4s12_decoder_destroy(v4s12_decoder_t* decoder);

    // Zero-copy memory decoding
    int v4s12_decoder_load_memory(v4s12_decoder_t* decoder, const char* buffer, size_t size);

    // Scene inspection
    uint32_t v4s12_decoder_get_geometry_count(const v4s12_decoder_t* decoder);
    uint32_t v4s12_decoder_get_node_count(const v4s12_decoder_t* decoder);

    // Geometry extraction
    int v4s12_decoder_get_geometry_info(const v4s12_decoder_t* decoder, uint32_t index, 
                                       uint32_t* out_vertex_count, uint32_t* out_index_count);
    int v4s12_decoder_read_geometry(const v4s12_decoder_t* decoder, uint32_t index,
                                    float* out_vertices, uint32_t* out_indices);

    // Instance transform extraction
    int v4s12_decoder_read_node(const v4s12_decoder_t* decoder, uint32_t index,
                                uint32_t* out_geom_id, double out_transform[16], uint32_t* out_color);

---

## 7. Building, Testing & Compilation

**CMake 3.15+** is used for cross-platform builds.

### 7.1 Clone with Submodules
Ensure submodules in `external/` are fetched upon cloning:

    git clone --recursive https://github.com/garcialeal/V4S-FORMAT.git
    # Or inside an existing workspace:
    git submodule update --init --recursive

### 7.2 Native Build & Testing

    # Configure and build static library and tools
    cmake -B build
    cmake --build build

    # Run unit tests (Precision and reversibility validation)
    ./build/test_v2

    # Run benchmarks (Generates dense sphere and CAD instanced benchmarks)
    ./build/generate_benchmarks

    # Inspect generated binary file metadata
    ./build/v4s_info benchmarks/dense_sphere_180k.v4s

### 7.3 WebAssembly Compilation (Emscripten)
Generates `v4s12_decoder.js` and `v4s12_decoder.wasm` for WebGL/WebGPU web rendering contexts:

    mkdir build-wasm && cd build-wasm
    emcmake cmake ..
    emmake make

---

## 8. Licensing Model

* **API Specification & Open V4S12-MATHEMATICAL-CORE (GPLv3):** Free for open-source tools, academic research, and public inspection.
* **Commercial V4S12-MATHEMATICAL-CORE License:** Proprietary license for commercial CAD/GIS plugins, enterprise pipelines, and embedded engines without GPLv3 copyleft obligations.