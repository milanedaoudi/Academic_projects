#include <string>
#include <iostream>
#include <iomanip>

#include "ac_fixed.h"

#ifndef FIXED_VAR_HPP
#define FIXED_VAR_HPP

// Types used in the network
#define DATA_WIDTH 16
#define ACC_WIDTH 33
#define PIXEL_OUT_WIDTH 16  // Output pixel width (fixed point)

#define IMG_SIZE_0 32
#define IMG_SIZE_1 32
#define IMG_CHANNEL 3
#define IMG_SIZE IMG_SIZE_0*IMG_SIZE_1*IMG_CHANNEL

#define IMG_CROP_SIZE_0 24
#define IMG_CROP_SIZE_1 24
#define IMG_CROP_CHANNEL 3
#define IMG_CROP_SIZE (IMG_CROP_SIZE_0*IMG_CROP_SIZE_1*IMG_CROP_CHANNEL)



typedef ac_fixed<ACC_WIDTH, 16, true, AC_RND_INF, AC_SAT> acc_t;
typedef ac_fixed<DATA_WIDTH, 9, true, AC_RND_INF, AC_SAT> fixed_t;
typedef ac_fixed<PIXEL_OUT_WIDTH, 9, true, AC_RND_INF, AC_SAT> pixel_out_t;



/*
typedef float acc_t;
typedef float fixed_t;
typedef float pixel_out_t; 
*/
#endif

