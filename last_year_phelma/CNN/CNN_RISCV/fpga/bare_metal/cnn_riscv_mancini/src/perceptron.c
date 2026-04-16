#include "perceptron.h"

/* simple exp approximation */
float expf(float x)
{
    float r = 1.0f + x;
    float p = x * x * 0.5f;
    return r + p;
}

void perceptron(coef_type coeffs[], coef_type biais[], 
                image_type image_in[NCAN_IN_5], 
                image_type image_out[NCAN_OUT_5])
{
    float max_val = -1e30f;

    // Calcul du perceptron
    for (int j = 0; j < NCAN_OUT_5; j++) {
        float sum = 0.0f;
        for (int i = 0; i < NCAN_IN_5; i++) {
            sum += image_in[i] * coeffs[j*NCAN_IN_5 + i];
        }
        sum += biais[j];
        image_out[j] = (sum < 0.0f) ? 0.0f : sum; // ReLU
        if (image_out[j] > max_val) max_val = image_out[j];
    }

    // Softmax pour transformer en probabilités
    float total = 0.0f;
    for (int j = 0; j < NCAN_OUT_5; j++) {
        image_out[j] = expf(image_out[j] - max_val); // stabilité numérique
        total += image_out[j];
    }
    for (int j = 0; j < NCAN_OUT_5; j++)
        image_out[j] /= total;
}
