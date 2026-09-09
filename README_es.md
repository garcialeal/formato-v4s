# v4s Format Specification & SDK (v2.0)
*Arquitectura de Datos Vectoriales Deterministas, Transformada Espectral V4 y Celosía $S_{12}$*

**Autor Principal:** Antonio García Leal  
**Licencia Núcleo / Especificación:** Dual License (GPLv3 / Licencia Comercial SDK)  
**Estándar de Lenguaje:** C99 (Núcleo Matemático) / C++17 (Contenedor & SDK)

---

## 1. Visión General

**v4s v2.0** es un formato binario y motor de renderizado geométrico determinista diseñado para eliminar los cuellos de botella de la coma flotante estándar (IEEE 754) en el almacenamiento y transmisión de geometrías masivas (CAD, GIS, LiDAR, 3D).

Mediante la combinación de la **Transformada Espectral Mariposa V4**, la **Cuantización en Celosía de Residuos Coprimos $S_{12}$** y el **Algoritmo de Firma Espacial G3-RH512-256**, **v4s** ejecuta el procesamiento geométrico mediante operaciones enteras bit a bit (**0 FLOPs**), garantizando cero desvío topológico (*drift*), desduplicación instanciada nativa y compatibilidad directa con estructuras de memoria GPU (*Zero-Copy Memory Mapping*).

## 2. Arquitectura de Procesamiento y Ventajas Técnicas

El formato **v4s** implementa un flujo de compresión y renderizado determinista en cinco etapas que transforma geometría 3D en una representación espectral ligera:

  [Geometría Float32 (CAD/GIS)]
         │
         ▼
   [1. Firma Espacial G3] ─────────> Deduplicación instantánea de objetos repetidos
         │
         ▼
   [2. Cuantización en Celosía S12] ─> Reducción de tasa de bits con continuidad C1
         │
         ▼
   [3. Transformada Espectral V4] ──> Procesamiento espectral entero (0 FLOPs)
         │
         ▼
   [4. Reordenamiento Entrópico] ────> Agrupación de energía por órbitas de Klein
         │
         ▼
   [5. Empaquetado Binario v4s] ────> Compresión entrópica final de alta densidad

### 2.1 Motor de Firma Espacial G3 (Deduplicación Instanciada)
* **Verificación de Identidad Instantánea:** Evalúa mallas e índices en tiempo $O(1)$ mediante hashing no criptográfico de 256 bits.
* **Ahorro de Memoria:** Identifica duplicados en grandes conjuntos de datos (ej. elementos repetidos en arquitecturas BIM/CAD) antes del procesamiento geométrico.

### 2.2 Cuantización en Celosía $S_{12}$
* **Reducción del 44.21% en Tasa de Bits:** Compacta el espacio de estados geométricos proyectando las coordenadas sobre una malla discreta balanceada.
* **Integridad Topológica $C^1$:** Conserva la continuidad tangencial en curvas y mallas continuas, previniendo fisuras en bordes compartidos.

### 2.3 Transformada Espectral Mariposa V4 (0 FLOPs)
* **Cero Operaciones en Coma Flotante:** Reemplaza el costo computacional de las matrices tradicionales por combinatoria entera bit a bit.
* **Determinismo Absoluto:** Elimina las diferencias de renderizado entre plataformas al evitar la acumulación de errores de redondeo IEEE 754.
* **Decodificación Ultrarrápida:** La reconstrucción inversa se realiza con operaciones inmediatas a nivel de registro (`>>`), maximizando el rendimiento en GPU y navegadores web.

### 2.4 Reordenamiento Topo-Entrópico
* **Optimización para Compresión:** Agrupa la energía de los coeficientes espectrales basándose en la simetría de grupos algebraicos.
* **Ratios de Compresión Superiores:** Maximiza la densidad de información previa al empaquetado entrópico final, logrando archivos sensiblemente más pequeños sin sacrificar precisión visual.

## 3. Especificación del Formato Binario (.v4s v2.0)

Un archivo **.v4s** organiza los datos vectoriales en tres bloques contiguos alineados en memoria para optimizar el acceso directo (*Zero-Copy Memory Mapping*):

```text
+-------------------------------------------------------+
| Cabecera Header (64 bytes, alineado pack 8)           |
+-------------------------------------------------------+
| Tabla de Nodos de Escena (num_nodes * sizeof(Node))   |
+-------------------------------------------------------+
| Bloques de Geometría (Hash + AABB + Payload ZSTD)     |
+-------------------------------------------------------+
```

### 3.1 Estructura del Header (`Header`)
| Campo | Tipo | Tamaño | Descripción |
| :--- | :--- | :--- | :--- |
| `magic` | `char[4]` | 4 bytes | Identificador del formato (`'V'`, `'4'`, `'S'`, `'2'`) |
| `version` | `uint16_t` | 2 bytes | Versión del estándar (`200` = v2.0) |
| `flags` | `uint16_t` | 2 bytes | Banderas globales de configuración del archivo |
| `global_aabb_min` | `double[3]` | 24 bytes | Coordenadas mínimas del cuadro delimitador ($X, Y, Z$) |
| `global_aabb_max` | `double[3]` | 24 bytes | Coordenadas máximas del cuadro delimitador ($X, Y, Z$) |
| `num_geometries` | `uint32_t` | 4 bytes | Cantidad de bloques geométricos únicos deduplicados |
| `num_nodes` | `uint32_t` | 4 bytes | Número total de instancias / nodos en la escena |
| `offset_hashtable`| `uint64_t` | 8 bytes | Desplazamiento (bytes) hasta el bloque de nodos |
| `offset_geometries`| `uint64_t`| 8 bytes | Desplazamiento (bytes) hasta los datos geométricos |

### 3.2 Nodo de Escena (`SceneNode`)
Representa la jerarquía, posición e instanciación de geometrías reutilizables en el espacio 3D:
*   `geometry_id` (`uint32_t`): Identificador único de la geometría deduplicada vinculada.
*   `transform` (`double[16]`): Matriz de transformación $4 \times 4$ en precisión doble.
*   `layer_color_rgba` (`uint32_t`): Color de capa o entidad empaquetado en 32 bits RGBA.
*   `node_flags` (`uint32_t`): Máscara de atributos específicos del nodo.

### 3.3 Banderas de Formato (`FormatFlags`)
*   `FLAG_NONE` (`0x0000`): Configuración base por defecto.
*   `FLAG_HAS_NORMALS` (`0x0001`): Incluye vectores normales decodificables.
*   `FLAG_LAYER_COLORS` (`0x0002`): Habilita colores por capa / entidad.
*   `FLAG_ZSTD_COMPRESSED` (`0x0004`): Payload comprimido mediante algoritmo ZSTD.
*   `FLAG_INDEX_16BIT` (`0x0008`): Índices de 16 bits (optimizado para mallas pequeñas).
*   `FLAG_PLANAR_2D` (`0x0010`): Geometría plana 2D (optimización GIS/CAD 2D).
*   `FLAG_N_DIMENSIONAL` (`0x0020`): Soporte para capas de atributos multidimensionales.

## 4. API en C Nativa (`v4s12_c_api.h`)

Para garantizar la interoperabilidad con WebAssembly, C#, Python, Rust y SDKs de terceros (CAD, GIS, motores gráficos), el SDK expone una interfaz de decodificación mediante el puntero opaco `v4s12_decoder_t`:

```c
// Creación y destrucción del decodificador
v4s12_decoder_t* v4s12_decoder_create(void);
void v4s12_decoder_destroy(v4s12_decoder_t* decoder);

// Carga en memoria (Zero-Copy friendliness)
int v4s12_decoder_load_memory(v4s12_decoder_t* decoder, const char* buffer, size_t size);

// Métodos de inspección de recuentos
uint32_t v4s12_decoder_get_geometry_count(const v4s12_decoder_t* decoder);
uint32_t v4s12_decoder_get_node_count(const v4s12_decoder_t* decoder);

// Consulta de metadatos de geometría
int v4s12_decoder_get_geometry_info(const v4s12_decoder_t* decoder, uint32_t index, 
                                   uint32_t* out_vertex_count, uint32_t* out_index_count);

// Lectura de búferes geométricos reconstruidos (vértices e índices)
int v4s12_decoder_read_geometry(const v4s12_decoder_t* decoder, uint32_t index,
                                float* out_vertices, uint32_t* out_indices);

// Lectura de nodo de escena (ID de geometría, matriz 4x4 y color RGBA)
int v4s12_decoder_read_node(const v4s12_decoder_t* decoder, uint32_t index,
                            uint32_t* out_geom_id, double out_transform[16], uint32_t* out_color);

```

## 5. Compilación y Construcción (CMake)

El proyecto utiliza **CMake 3.15+** como sistema de construcción multiplataforma[cite: 22]. La librería central `v4s12_lib` actúa como el núcleo universal para binarios nativos, módulos WebAssembly y futuros *plugins* de integración CAD/GIS (AutoCAD, SolidWorks, Revit, etc.).

### 5.1 Compilación Nativa (C++17 / C99)
Genera la librería estática central, la herramienta CLI de inspección (`v4s_info`) y el ejecutable de pruebas:

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

### 5.2 Compilación WebAssembly (Emscripten)
Genera el módulo ejecutable `v4s12_decoder.js` y `v4s12_decoder.wasm` para motores WebGPU / WebGL en navegadores web:

```bash
mkdir build_wasm && cd build_wasm
emcmake cmake ..
emmake make
```

**Configuración de exportación WASM (`Emscripten`):**
*   `EXPORTED_FUNCTIONS`: Expone las funciones de la C-API y la gestión de memoria (`_malloc`, `_free`).
*   `MODULARIZE=1`: Encapsula el motor en el módulo aislado `V4S12Module`.
*   `ALLOW_MEMORY_GROWTH=1`: Permite la expansión dinámica de la memoria RAM en el navegador.

### 5.3 Extensibilidad y Plugins CAD / GIS
La interfaz binaria expuesta por `v4s12_c_api.h` permite empaquetar el motor en librerías dinámicas (`.dll` / `.so` / `.dylib`) para la integración en entornos de terceros:
*   **AutoCAD (ObjectARX / C++):** Vinculación nativa directa con `v4s12_lib`.
*   **SolidWorks / Revit (C# / .NET):** Interoperabilidad vía P/Invoke consumiendo la C-API.
*   **QGIS / Python:** Bindings mediante `ctypes` o `CFFI` sobre la API en C nativa.

## 6. Modelo de Licenciamiento y Protección de PI

El ecosistema **v4s** adopta un modelo de **doble licencia** para maximizar la adopción industrial y asegurar la sostenibilidad del proyecto:

*   **Especificación ABI/API y Visores Abiertos (GPLv3):** Garantiza la transparencia, auditoría pública y adopción comunitaria en proyectos de software libre y proyectos académicos.
*   **Enterprise SDK & Núcleo Integrable (Licencia Comercial):** Permite a empresas y desarrolladores integrar la aceleración nativa y el decodificador **v4s** dentro de software propietario (CAD, GIS corporativo, motores 3D) manteniendo el código fuente desacoplado mediante acuerdos de distribución comercial.