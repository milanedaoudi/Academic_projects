#include "reshape.h"

void reshape(image_type image_in[], image_type image_out[], int size, int ncan){
    int idx_out = 0;
    int channel_size = size * size;

    for (int ch = 0; ch < ncan; ch++){
        int base_in = ch * channel_size;
        for (int i = 0; i < channel_size; i++){
            image_out[idx_out++] = image_in[base_in + i];
        }
    }
}
