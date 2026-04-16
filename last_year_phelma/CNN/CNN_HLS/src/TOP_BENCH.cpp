#include <iostream>
#include "mc_scverify.h"
#include "forward_debug.hpp"

#ifndef CCS_MAIN
#define CCS_MAIN int main
#define CCS_DESIGN(d) d
#define CCS_RETURN(a) return a
#endif

CCS_MAIN(int argc, char **argv)
{
    int labels_all[] = { 6, 9, 9, 4, 1, 1, 2, 7, 8, 3 };
    const int N = 1;

    int correct = 0;

    for (int i = 0; i < N; ++i) {

        int  pred  = -1;
        bool valid = false;

        // Launch the CNN
        CCS_DESIGN(forward_debug_hls)(
            i % 3,   // img_sel
            pred,
            valid
        );

        // ? WAIT FOR HARDWARE COMPLETION
        while (!valid) {
            wait();   // advances one HLS cycle
        }

        if (pred == labels_all[i])
            correct++;

        std::cout << "Image " << i
                  << " | True = " << labels_all[i]
                  << " | Pred = " << pred
                  << std::endl;
    }

    float accuracy = float(correct) / float(N) * 100.f;

    std::cout << "\n============================\n";
    std::cout << "Accuracy on " << N << " images = "
              << accuracy << "%\n";
    std::cout << "============================\n";

    CCS_RETURN(0);
}
