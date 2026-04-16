#ifndef COEFFS_HPP
#define COEFFS_HPP

#include "fixed_var.hpp"  // Définition de fixed_t et types CNN

/**-----------------------------------------------------
 * @brief CNN Layer Dimensions (Compile-Time Constants)
 *
 * Naming convention:
 *  - Bx_SIZE : Bias vector size of layer x
 *  - Wx_SIZE : Weight tensor size of layer x
 *
 * Layer configuration:
 *  - Conv1 : 3 × 3 × 3   -> 64 feature maps
 *  - Conv2 : 3 × 3 × 64  -> 32 feature maps
 *  - Conv3 : 3 × 3 × 32  -> 20 feature maps
 *  - FC    : 180         -> 10 outputs
**/
#define B1_SIZE   64
#define W1_SIZE   (3*3*3*64)

#define B2_SIZE   32
#define W2_SIZE   (3*3*64*32)

#define B3_SIZE   20
#define W3_SIZE   (3*3*32*20)

#define BFC_SIZE  10
#define WFC_SIZE  (180*10)

/**-----------------------------------------------------
 * @brief CNN Coefficients (ROM)
 *
 * Declaration only; definition in coeffs.cpp
**/
extern const fixed_t b1[B1_SIZE];
extern const fixed_t W1[W1_SIZE];

extern const fixed_t b2[B2_SIZE];
extern const fixed_t W2[W2_SIZE];

extern const fixed_t b3[B3_SIZE];
extern const fixed_t W3[W3_SIZE];

// Fully Connected layer
extern const fixed_t bfc[BFC_SIZE];
extern const fixed_t Wfc[WFC_SIZE];

#endif // COEFFS_HPP

