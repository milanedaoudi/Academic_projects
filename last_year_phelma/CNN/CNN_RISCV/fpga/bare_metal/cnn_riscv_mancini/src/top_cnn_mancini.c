#include "top_cnn_mancini.h"

// Définition des tailles
#define CNN_SIZE 24       // image resize
#define N_CHANNELS 3      // nombre de canaux après convolution
#define FC_INPUT_SIZE 576 // CNN_SIZE*CNN_SIZE (après 1 canal par exemple)
#define NB_CLASSES 10
/*
void print_image(image_type buffer[TAB_SIZE], int size, int base) {
    printf("Image (taille %dx%d, base %d):\n", size, size, base);
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            int idx = y * size + x + base;
            printf("%6.2f ", buffer[idx]);
        }
        printf("\n");
    }
    printf("\n");
}
*/
// Fonctions externes
/*
extern void convolution(coef_type matrice[9], image_type image_in[], image_type image_out[], int size, int base_in, int base_out);
extern void maxpool(image_type image_in[], image_type image_out[], int size, int base_in, int base_out);
extern void multi_maxpool(image_type image_in[], image_type image_out[], int ncan, int size);
extern void perceptron(coef_type coeffs[], coef_type biais[], image_type image_in[], image_type image_out[]);
extern void argmax(image_type image_in[], led_type cifar_class[1]);
extern void reshape(image_type image_in[], image_type image_out[], int size, int ncan);*/

void top_cnn_mancini(coef_type tab_coeffs[NB_COEFFS],
                     coef_type tab_biais[NB_BIAIS],
                     led_type cifar_class[1],
                     image_type image_in[CONV_SIZE_1*CONV_SIZE_1*3],
                     image_type cifar_probabilities[NCAN_OUT_5])
{
    static image_type buffer1[TAB_SIZE];
    static image_type buffer2[TAB_SIZE];

    // 1. Convolution 1
    multi_convolution(tab_coeffs, tab_biais, image_in, buffer1, 
                      BASE_COEFFS_1, BASE_BIAIS_1, 
                      NCAN_IN_1, NCAN_OUT_1, CONV_SIZE_1);
    //print_image(buffer1, CONV_SIZE_1, NCAN_OUT_1);

    // 2. Maxpool 1
    multi_maxpool(buffer1, buffer2, NCAN_OUT_1, CONV_SIZE_1);
    //print_image(buffer2, CONV_SIZE_1 / 2, NCAN_OUT_1);

    // 3. Convolution 2
    multi_convolution(tab_coeffs, tab_biais, buffer2, buffer1, 
                      BASE_COEFFS_2, BASE_BIAIS_2, 
                      NCAN_IN_2, NCAN_OUT_2, CONV_SIZE_2);
    //print_image(buffer1, CONV_SIZE_2, NCAN_OUT_2);

    // 4. Maxpool 2
    multi_maxpool(buffer1, buffer2, NCAN_OUT_2, CONV_SIZE_2);
    //print_image(buffer2, CONV_SIZE_2 / 2, NCAN_OUT_2);

    // 5. Convolution 3
    multi_convolution(tab_coeffs, tab_biais, buffer2, buffer1, 
                      BASE_COEFFS_3, BASE_BIAIS_3, 
                      NCAN_IN_3, NCAN_OUT_3, CONV_SIZE_3);
    //print_image(buffer1, CONV_SIZE_3, NCAN_OUT_3);

    // 6. Reshape pour perceptron
    reshape(buffer1, buffer2, CONV_SIZE_3, NCAN_IN_4);

    // 7. Perceptron final
    perceptron(tab_coeffs, tab_biais, buffer2, cifar_probabilities);

    // 8. Classe prédite
}


