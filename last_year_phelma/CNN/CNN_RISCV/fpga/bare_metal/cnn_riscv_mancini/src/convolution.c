#include <stdint.h>
#include "convolution.h"


#include "types.h"

// Convolution 3x3 simple avec ReLU
void convolution(coef_type matrice[9], 
                 image_type image_in[TAB_SIZE], 
                 image_type image_out[TAB_SIZE], 
                 int size, int base_in, int base_out) 
{
    for (int y = 1; y < size-1; y++) {
        for (int x = 1; x < size-1; x++) {
            float sum = 0.0f;
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    int idx_in = (y + ky) * size + (x + kx) + base_in;
                    int idx_kernel = (ky + 1) * 3 + (kx + 1);
                    sum += image_in[idx_in] * matrice[idx_kernel];
                }
            }
            // ReLU
            if (sum < 0.0f) sum = 0.0f;
            int idx_out = y * size + x + base_out;
            image_out[idx_out] = sum;
        }
    }
}

// multi_convolution : applique la convolution 3x3 sur tous les canaux d'entrée
void multi_convolution(coef_type coeffs[NB_COEFFS], coef_type biais[NB_BIAIS], 
                       image_type image_in[TAB_SIZE], image_type image_out[TAB_SIZE], 
                       int base_coeffs, int base_biais, 
                       int ncan_in, int ncan_out, int size)
{
    int channel_size = size * size;
    for (int out_c = 0; out_c < ncan_out; out_c++) {
        int base_out = out_c * channel_size;
        for (int in_c = 0; in_c < ncan_in; in_c++) {
            int base_in = in_c * channel_size;
            int coeff_base = base_coeffs + out_c * 9 * ncan_in + in_c * 9; // chaque sortie a 9*input
            convolution(&coeffs[coeff_base], &image_in[base_in], &image_out[base_out], size, 0, 0);
        }
        // Ajouter le biais + ReLU
        int idx_bias = base_biais + out_c;
        for (int i = 0; i < channel_size; i++) {
            image_out[base_out + i] += biais[idx_bias];
            if (image_out[base_out + i] < 0) image_out[base_out + i] = 0; // ReLU
        }
    }
}

