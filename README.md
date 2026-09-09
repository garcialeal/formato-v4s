# v4s Format Specification & SDK (v2.0)
*Deterministic Vector Data Architecture, V4 Spectral Transform & $S_{12}$ Lattice*

**Lead Author:** Antonio García Leal  
**Core / Specification License:** Dual License (GPLv3 / Commercial SDK License)  
**Language Standard:** C99 (Mathematical Core) / C++17 (Wrapper & SDK)

---

## 1. Overview

**v4s v2.0** is a binary format and deterministic geometric rendering engine designed to eliminate standard floating-point (IEEE 754) bottlenecks in the storage and transmission of massive 3D geometries (CAD, GIS, LiDAR).

Combining the **V4 Butterfly Spectral Transform**, **$S_{12}$ Coprime Residue Lattice Quantization**, and the **G3-RH512-256 Spatial Hashing Algorithm**, **v4s** executes geometric processing via bitwise integer operations (**0 FLOPs**). This guarantees zero topological drift, native instanced deduplication, and direct compatibility with GPU memory layouts (*Zero-Copy Memory Mapping*).

## 2. Processing Pipeline & Technical Features

The **v4s** format implements a 5-stage compression and rendering pipeline:

```text
  [Float32 Geometry (CAD/GIS)]
         │
         ▼
   [1. G3 Spatial Hashing] ─────────> Instant O(1) object deduplication
         │
         ▼
   [2. S12 Lattice Quantization] ───> Bitrate reduction with C1 continuity
         │
         ▼
   [3. V4 Spectral Transform] ──────> Pure integer processing (0 FLOPs)
         │
         ▼
   [4. Entropic Reordering] ────────> Energy grouping via Klein orbits
         │
         ▼
   [5. v4s Binary Packing] ─────────> High-density ZSTD compression
```

### 2.1 G3 Spatial Hashing Engine (Instanced Deduplication)
* **Instant Verification:** Evaluates meshes and indices in $O(1)$ time using a 256-bit non-cryptographic hash algorithm.
* **Memory Reduction:** Detects duplicates in large datasets (e.g., repeated structural elements in BIM/CAD) prior to geometric processing.

### 2.2 $S_{12}$ Lattice Quantization
* **44.21% Bitrate Reduction:** Compacts geometric state space by projecting coordinates onto a balanced discrete lattice.
* **$C^1$ Topological Integrity:** Preserves tangential continuity across curves and continuous meshes, preventing cracks on shared edges.

### 2.3 V4 Butterfly Spectral Transform (0 FLOPs)
* **Zero Floating-Point Operations:** Replaces costly traditional transformation matrices with bitwise integer combinatorics.
* **Absolute Determinism:** Eliminates cross-platform rendering discrepancies by avoiding IEEE 754 rounding drift.
* **Ultra-Fast Decoding:** Inverse reconstruction uses immediate register-level operations (`>>`), maximizing throughput on GPUs and WebAssembly.

### 2.4 Topo-Entropic Reordering
* **Compression Optimization:** Groups spectral coefficient energy based on algebraic group symmetry.
* **Superior Compression Ratios:** Maximizes information density prior to final entropic packing, achieving significantly smaller file sizes without sacrificing visual precision.

## 3. Binary Format Specification (.v4s v2.0)

A **.v4s** file organizes vector data into three contiguous, memory-aligned blocks to optimize direct memory access (*Zero-Copy Memory Mapping*):

```text
+-------------------------------------------------------+
| Header (64 bytes, aligned pack 8)                     |
+-------------------------------------------------------+
| Scene Nodes Table (num_nodes * sizeof(Node))          |
+-------------------------------------------------------+
| Geometry Blocks (Hash + AABB + ZSTD Payload)          |
+-------------------------------------------------------+
```

### 3.1 Header Structure (`Header`)
| Field | Type | Size | Description |
| :--- | :--- | :--- | :--- |
| `magic` | `char[4]` | 4 bytes | Format identifier (`'V'`, `'4'`, `'S'`, `'2'`) |
| `version` | `uint16_t` | 2 bytes | Standard version (`200` = v2.0) |
| `flags` | `uint16_t` | 2 bytes | Global configuration flags |
| `global_aabb_min` | `double[3]` | 24 bytes | Minimum bounding box coordinates ($X, Y, Z$) |
| `global_aabb_max` | `double[3]` | 24 bytes | Maximum bounding box coordinates ($X, Y, Z$) |
| `num_geometries` | `uint32_t` | 4 bytes | Count of unique deduplicated geometry blocks |
| `num_nodes` | `uint32_t` | 4 bytes | Total number of instances / scene nodes |
| `offset_hashtable`| `uint64_t` | 8 bytes | Byte offset to scene nodes block |
| `offset_geometries`| `uint64_t`| 8 bytes | Byte offset to geometry data |

### 3.2 Scene Node (`SceneNode`)
Represents the hierarchy, positioning, and instancing of reusable geometries in 3D space:
* `geometry_id` (`uint32_t`): Unique ID of the linked deduplicated geometry block.
* `transform` (`double[16]`): $4 \times 4$ transformation matrix in double precision.
* `layer_color_rgba` (`uint32_t`): Layer or entity color packed in 32-bit RGBA.
* `node_flags` (`uint32_t`): Node-specific attribute bitmask.

### 3.3 Format Flags (`FormatFlags`)
* `FLAG_NONE` (`0x0000`): Default base configuration.
* `FLAG_HAS_NORMALS` (`0x0001`): Includes decodable normal vectors.
* `FLAG_LAYER_COLORS` (`0x0002`): Enables per-layer / per-entity colors.
* `FLAG_ZSTD_COMPRESSED` (`0x0004`): Payload compressed via ZSTD algorithm.
* `FLAG_INDEX_16BIT` (`0x0008`): 16-bit indices (optimized for smaller meshes).
* `FLAG_PLANAR_2D` (`0x0010`): 2D planar geometry (2D GIS/CAD optimization).
* `FLAG_N_DIMENSIONAL` (`0x0020`): Support for N-dimensional attribute layers.

## 4. Native C-API (`v4s12_c_api.h`)

To ensure interoperability with WebAssembly, C#, Python, Rust, and third-party CAD/GIS SDKs, the engine exposes an opaque pointer interface (`v4s12_decoder_t`):

```c
// Decoder creation and teardown
v4s12_decoder_t* v4s12_decoder_create(void);
void v4s12_decoder_destroy(v4s12_decoder_t* decoder);

// In-memory loading (Zero-Copy friendly)
int v4s12_decoder_load_memory(v4s12_decoder_t* decoder, const char* buffer, size_t size);

// Inspection and count queries
uint32_t v4s12_decoder_get_geometry_count(const v4s12_decoder_t* decoder);
uint32_t v4s12_decoder_get_node_count(const v4s12_decoder_t* decoder);

// Geometry metadata inspection
int v4s12_decoder_get_geometry_info(const v4s12_decoder_t* decoder, uint32_t index, 
                                   uint32_t* out_vertex_count, uint32_t* out_index_count);

// Buffer reading (reconstructed vertices and indices)
int v4s12_decoder_read_geometry(const v4s12_decoder_t* decoder, uint32_t index,
                                float* out_vertices, uint32_t* out_indices);

// Scene node reading (geometry ID, 4x4 matrix, RGBA color)
int v4s12_decoder_read_node(const v4s12_decoder_t* decoder, uint32_t index,
                            uint32_t* out_geom_id, double out_transform[16], uint32_t* out_color);
```

## 5. Building & Compilation (CMake)

The project uses **CMake 3.15+** as its cross-platform build system. The core library `v4s12_lib` acts as the universal engine for native binaries, WebAssembly modules, and third-party CAD/GIS integration plugins (AutoCAD, SolidWorks, Revit, etc.).

### 5.1 Native Build (C++17 / C99)
Generates the core static library, CLI inspection tool (`v4s_info`), and test suite executable:

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

### 5.2 WebAssembly Build (Emscripten)
Generates executable modules `v4s12_decoder.js` and `v4s12_decoder.wasm` for WebGPU / WebGL browser engines:

```bash
mkdir build_wasm && cd build_wasm
emcmake cmake ..
emmake make
```

**Emscripten Export Configuration:**
* `EXPORTED_FUNCTIONS`: Exposes C-API functions and memory management (`_malloc`, `_free`).
* `MODULARIZE=1`: Encapsulates the engine inside the `V4S12DecoderModule` namespace.
* `ALLOW_MEMORY_GROWTH=1`: Enables dynamic browser memory expansion.

### 5.3 Extensibility & CAD / GIS Plugins
The binary interface exposed by `v4s12_c_api.h` allows wrapping the engine into dynamic libraries (`.dll` / `.so` / `.dylib`) for third-party embedding:
* **AutoCAD (ObjectARX / C++):** Direct native linkage with `v4s12_lib`.
* **SolidWorks / Revit (C# / .NET):** P/Invoke interop consuming the native C-API.
* **QGIS / Python:** Bindings via `ctypes` or `CFFI` over the C-API.

## 6. Licensing Model & IP Protection

The **v4s** ecosystem adopts a **dual-license model** to maximize industrial adoption while ensuring project sustainability:

* **ABI/API Specification & Open Viewers (GPLv3):** Guarantees transparency, public auditing, and community adoption in open-source and academic projects.
* **Enterprise SDK & Core Integration (Commercial License):** Enables enterprise teams to embed native acceleration and the **v4s** decoder inside proprietary software (CAD, corporate GIS, 3D engines) while keeping source code decoupled through commercial distribution agreements.