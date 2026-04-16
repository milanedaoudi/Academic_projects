#ifndef FORWARD_DEBUG_HPP
#define FORWARD_DEBUG_HPP

#include "fixed_var.hpp"
#include "cnn_hls.hpp"
#include "coeff.hpp"
#include "images.hpp"

/**-----------------------------------------------------
  * @brief Executes the full CNN forward inference
  *
  * @param img_sel    Index of the input image to use (0,1,2,...)
  * @param probs_out  Output class probabilities (10 classes), fixed-point format
  * @param pred_out   Predicted class
**/
void forward_debug_hls(
    const int img_sel,
    int &pred_out,
    bool &pred_valid
);


#endif // FORWARD_DEBUG_HPP

