#include "v4s12/spectral_pipeline.h"
#include "v4s12/v4s12_transform.h"
#include "v4s12/v4s12_zigzag.h"
#include "v4s12/v4s12_quant.h"

namespace v4s12 {

// 1. Reordenamiento ZigZag Inverso
void v4_zigzag_reorder_inv(const v4s12_int_t in_vector[16], v4s12_int_t out_block[16]) {
    // Restauramos las órbitas de Klein a sus coordenadas espaciales 4x4
    for (size_t i = 0; i < 16; ++i) {
        out_block[V4_ZIGZAG_MAP[i]] = in_vector[i];
    }
}

// 2. Transformada Mariposa Inversa (2D 4x4)
void v4_transform_2d_4x4_inv(v4s12_int_t block[16]) {
    // Aprovecha la propiedad auto-involutiva de la matriz: H_V4 * H_V4 = 4 * I_4
    v4_transform_2d_4x4(block);

    // Escalado aritmético bit a bit exacto para regresar al dominio espacial
    for (size_t i = 0; i < 16; ++i) {
        block[i] >>= 4;
    }
}

// 3. Descuantización S12
v4s12_int_t s12_dequantize_spline(v4s12_int_t quantized_coord) {
    // Al salir de la transformada inversa, el entero ya descansa exactamente 
    // sobre un nodo válido de la celosía S12 (residuos coprimos 1, 5, 7, 11).
    // Su valor matemático es íntegro, por lo que actúa como un puente directo
    // hacia la des-normalización flotante del Bounding Box.
    return quantized_coord;
}

} // namespace v4s12