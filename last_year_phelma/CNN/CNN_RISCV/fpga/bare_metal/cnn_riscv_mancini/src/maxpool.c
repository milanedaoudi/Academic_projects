#include "maxpool.h"

// Maxpool sur 2x2, ReLU déjà implicite (max >=0)
void maxpool(image_type image_in[TAB_SIZE], 
             image_type image_out[TAB_SIZE], 
             int size, int base_in, int base_out) 
{
    int new_size = size / 2;
    for (int y = 0; y < new_size; y++) {
        for (int x = 0; x < new_size; x++) {
            int idx0 = (2*y)*size + (2*x) + base_in;
            int idx1 = (2*y)*size + (2*x+1) + base_in;
            int idx2 = (2*y+1)*size + (2*x) + base_in;
            int idx3 = (2*y+1)*size + (2*x+1) + base_in;

            float max_val = image_in[idx0];
            if (image_in[idx1] > max_val) max_val = image_in[idx1];
            if (image_in[idx2] > max_val) max_val = image_in[idx2];
            if (image_in[idx3] > max_val) max_val = image_in[idx3];

            image_out[y * new_size + x + base_out] = max_val;
        }
    }
}

// Appliquer le maxpool sur plusieurs canaux
void multi_maxpool(image_type image_in[TAB_SIZE], 
                   image_type image_out[TAB_SIZE], 
                   int ncan, int size) 
{
    int channel_size = size * size;
    int out_channel_size = channel_size / 4; // maxpool 2x2
    for (int c = 0; c < ncan; c++) {
        int base_in = c * channel_size;
        int base_out = c * out_channel_size;
        maxpool(image_in, image_out, size, base_in, base_out);
    }
}

