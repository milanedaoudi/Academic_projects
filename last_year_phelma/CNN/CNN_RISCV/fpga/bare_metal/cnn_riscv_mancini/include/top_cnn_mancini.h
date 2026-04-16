#ifndef TOP_H
#define TOP_H

#include "types.h"

void top_cnn_mancini(coef_type tab_coeffs[NB_COEFFS], coef_type tab_biais[NB_BIAIS],  led_type cifar_class[1], image_type image_in[CONV_SIZE_1*CONV_SIZE_1*3], image_type cifar_probabilities[NCAN_OUT_5]);

extern void convolution(coef_type matrice[9], image_type image_in[], image_type image_out[], int size, int base_in, int base_out);
extern void maxpool(image_type image_in[], image_type image_out[], int size, int base_in, int base_out);
extern void multi_maxpool(image_type image_in[], image_type image_out[], int ncan, int size);
extern void perceptron(coef_type coeffs[], coef_type biais[], image_type image_in[], image_type image_out[]);
extern void argmax(image_type image_in[], led_type cifar_class[1]);
extern void reshape(image_type image_in[], image_type image_out[], int size, int ncan);
#endif
