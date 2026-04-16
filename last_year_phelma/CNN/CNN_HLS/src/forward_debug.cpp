#include "forward_debug.hpp"

// -------------------------------------------------
// ROM image selection
// -------------------------------------------------
inline const pixel_out_t* select_image(int img_sel) {
    switch (img_sel) {
        case 0: return img_0;
        case 1: return img_1;
        case 2: return img_2;
        default: return img_0;
    }
}

void forward_debug_hls(
    const int img_sel,
    int &pred_out,
    bool &pred_valid
){
    // Default: not valid
    pred_valid = false;

    // -------------------------------------------------
    // Image Selection
    // -------------------------------------------------
    const pixel_out_t* input = select_image(img_sel);

    // -------------------------------------------------
    // Static HLS buffers
    // -------------------------------------------------
    static pixel_out_t c1[24*24*64];
    static pixel_out_t p1[12*12*64];

    static pixel_out_t c2[12*12*32];
    static pixel_out_t p2[6*6*32];

    static pixel_out_t c3[6*6*20];
    static pixel_out_t p3[3*3*20];

    static pixel_out_t flat[180];
    pixel_out_t probs_out[10];

    // -------------------------------------------------
    // CNN
    // -------------------------------------------------
    conv2d_hls<24,24,3,3,64,1,true>(input, W1, b1, c1);
    relu_hls<24*24*64>(c1);
    maxpool_hls<24,24,64,3,2,true>(c1, p1);

    conv2d_hls<12,12,64,3,32,1,true>(p1, W2, b2, c2);
    relu_hls<12*12*32>(c2);
    maxpool_hls<12,12,32,3,2,true>(c2, p2);

    conv2d_hls<6,6,32,3,20,1,true>(p2, W3, b3, c3);
    relu_hls<6*6*20>(c3);
    maxpool_hls<6,6,20,3,2,true>(c3, p3);

    flatten_hls<3,3,20,180>(p3, flat);
    dense_hls<180,10>(flat, Wfc, bfc, probs_out);

    // -------------------------------------------------
    // Argmax
    // -------------------------------------------------
    pixel_out_t best = probs_out[0];
    int pred = 0;

    for (int k = 1; k < 10; ++k) {
        if (probs_out[k] > best) {
            best = probs_out[k];
            pred = k;
        }
    }

    // -------------------------------------------------
    // Outputs
    // -------------------------------------------------
    pred_out   = pred;
    pred_valid = true;  
}
