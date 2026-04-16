#include "types.h"
#include "argmax.h"

void argmax(image_type image_in[], led_type cifar_class[1]) {
    float max_val = image_in[0];
    int max_idx = 0;

    for (int i = 1; i < NCAN_OUT_5; i++) {
        if (image_in[i] > max_val) {
            max_val = image_in[i];
            max_idx = i;
        }
    }

    cifar_class[0] = max_idx;
}

