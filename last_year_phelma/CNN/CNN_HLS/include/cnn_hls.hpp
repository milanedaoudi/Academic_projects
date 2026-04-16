#include "fixed_var.hpp"

#ifndef USE_pixel_out_t_HLS
#define USE_pixel_out_t_HLS

/**-----------------------------------------------------
  * @brief 3D or 4D to 1D Memory Index Conversion (Row-Major, Channel-Last)
  *
  * Converts a 3D tensor index (h, w, c) into a linear 1D array index
  * using row-major order with channel-last layout.
  *
  * Memory layout:
  *   index = h * (W * C) + w * C + c
  *
  * Where:
  *   - h : row index    -> [0, H-1]
  *   - w : column index -> [0, W-1]
  *   - c : channel      -> [0, C-1]
**/
inline int IDX3(int h, int w, int c, int W, int Cin) {
    return (h * W * Cin) + (w * Cin) + c;
}
inline int IDX4(int kh, int kw, int ci, int co, int K, int Cin, int Cout) {
    return (kh * K * Cin * Cout) + (kw * Cin * Cout) + (ci * Cout) + co;
}



/**-----------------------------------------------------
  * @brief 2D Convolution Layer
  *
  * Performs a 2D convolution on a 3D input tensor with optional
  * zero-padding ("SAME") and configurable stride.
  *
  *
  * @tparam H*W*Cin      Input height * width * Number of input channels
  * @tparam K       Kernel size (K × K)
  * @tparam Cout    Number of output channels
  * @tparam STRIDE  Stride of the convolution
  * @tparam SAME    Enable SAME padding if true, else VALID
  *
  * @param x        Input feature map  [H × W × Cin]
  * @param Wf       Convolution weights [K × K × Cin × Cout]
  * @param b        Bias vector [Cout]
  * @param out      Output feature map
**/
template<int H, int W, int Cin, int K, int Cout, int STRIDE, bool SAME>
void conv2d_hls(
    const pixel_out_t x[H*W*Cin],
    const fixed_t Wf[K*K*Cin*Cout],
    const fixed_t b[Cout],
    pixel_out_t out[((SAME ? H : (H-K+1))/STRIDE) * ((SAME ? W : (W-K+1))/STRIDE) * Cout]
){
    const int PAD = SAME ? K/2 : 0;
    const int Hp = H + 2*PAD;
    const int Wp = W + 2*PAD;

    static pixel_out_t xpad[Hp*Wp*Cin];
    for(int i=0;i<Hp*Wp*Cin;i++) xpad[i] = 0;

    for(int h=0;h<H;h++)
        for(int w=0;w<W;w++)
            for(int c=0;c<Cin;c++)
                xpad[IDX3(h+PAD,w+PAD,c,Wp,Cin)] = x[IDX3(h,w,c,W,Cin)];

    const int Hout = (Hp-K)/STRIDE + 1;
    const int Wout = (Wp-K)/STRIDE + 1;

    for(int h=0;h<Hout;h++){
        for(int w=0;w<Wout;w++){
            for(int co=0;co<Cout;co++){
                // init accumulation in wide acc_t (promote bias)
                acc_t sum = acc_t(b[co]);

                for(int kh=0;kh<K;kh++){
                    for(int kw=0;kw<K;kw++){
                        for(int ci=0;ci<Cin;ci++){
                            // Read inputs in their native types
                            pixel_out_t xv_p = xpad[IDX3(h*STRIDE+kh,w*STRIDE+kw,ci,Wp,Cin)];
                            fixed_t  wf_f = Wf[IDX4(kh,kw,ci,co,K,Cin,Cout)];

                            acc_t prod = xv_p * wf_f;
                            sum += prod;
                        }
                    }
                }

                // Convertir final acc_t sum en pixel_out_t
                out[IDX3(h,w,co,Wout,Cout)] = pixel_out_t(sum);
            }
        }
    }
}



/**-----------------------------------------------------
  * @brief ReLU Activation Function
  *
  * Applies the Rectified Linear Unit activation:
  * out[i] = max(0, x[i])
  *
  * @tparam N   Total number of elements
  *
  * @param x    Input/output feature vector (processed in-place)
**/

template<int N>
void relu_hls(pixel_out_t x[N]) {
    for(int i=0;i<N;i++)
        x[i] = (x[i] > 0) ? x[i] : 0;
}

    
    
/**-----------------------------------------------------
  * @brief Max Pooling Layer
  *
  * Applies 2D max-pooling independently on each channel,
  * with configurable window size, stride, and optional SAME padding.
  *
  * @tparam H*W*C      Input height * width * Number of input channels
  * @tparam SIZE       Pooling window size (SIZE × SIZE)
  * @tparam STRIDE     Pooling stride
  * @tparam SAME       Enable SAME padding if true, else VALID
  *
  * @param x        Input feature map [H × W × C]
  * @param out      Output pooled feature map
**/
template<int H, int W, int C, int SIZE, int STRIDE, bool SAME>
void maxpool_hls(
    const pixel_out_t x[H*W*C],
    pixel_out_t out[((SAME ? H : (H-SIZE+1))/STRIDE) * ((SAME ? W : (W-SIZE+1))/STRIDE) * C]
){
    const int PAD = SAME ? SIZE/2 : 0;
    const int Hp = H + 2*PAD;
    const int Wp = W + 2*PAD;

    static pixel_out_t xpad[Hp*Wp*C];
    for(int i=0;i<Hp*Wp*C;i++)
        xpad[i] = 0;

    for(int h=0;h<H;h++)
        for(int w=0;w<W;w++)
            for(int c=0;c<C;c++)
                xpad[IDX3(h+PAD,w+PAD,c,Wp,C)] = x[IDX3(h,w,c,W,C)];

    const int Hout = (Hp-SIZE)/STRIDE + 1;
    const int Wout = (Wp-SIZE)/STRIDE + 1;

    for(int h=0;h<Hout;h++){
        for(int w=0;w<Wout;w++){
            for(int c=0;c<C;c++){
                pixel_out_t m = xpad[IDX3(h*STRIDE,w*STRIDE,c,Wp,C)];
                for(int i=0;i<SIZE;i++)
                    for(int j=0;j<SIZE;j++){
                        pixel_out_t v = xpad[IDX3(h*STRIDE+i,w*STRIDE+j,c,Wp,C)];
                        m = (m > v)? m : v;
                    }
                out[IDX3(h,w,c,Wout,C)] = m;

            }
        }
    }
}
    


/**-----------------------------------------------------
  * @brief Flatten Layer
  *
  * Converts a 3D feature map into a 1D vector by linear memory copy.
  * No reordering is applied.
  *
  * @tparam H*W*C      Input height * width * Number of input channels
  * @tparam N          Total number of output elements (H × W × C)
  *
  * @param in   Input feature map [H × W × C]
  * @param out  Output vector [N]
**/
template<int H, int W, int C, int N>
void flatten_hls(
    const pixel_out_t in[H*W*C],
    pixel_out_t out[N]
){
    for(int i=0;i<N;i++){
        out[i] = in[i];
    }
}


/**-----------------------------------------------------
  * @brief Fully Connected (Dense) Layer
  *
  * Computes a matrix-vector multiplication followed by bias addition:
  * out[j] = sum_i ( x[i] * W[i,j] ) + b[j]
  *
  * @tparam N   Number of input features
  * @tparam M   Number of output neurons
  *
  * @param x    Input vector [N]
  * @param W    Weight matrix [N × M]
  * @param b    Bias vector [M]
  * @param out  Output vector [M]
**/
template<int N, int M>
void dense_hls(
    const pixel_out_t x[N],
    const fixed_t W[N*M],
    const fixed_t b[M],
    pixel_out_t out[M]
){
    for(int j=0;j<M;j++){
        acc_t s = b[j];
        for(int i=0;i<N;i++)
            s += x[i] * W[i*M + j];
        out[j] = s;
    }
}

#endif
