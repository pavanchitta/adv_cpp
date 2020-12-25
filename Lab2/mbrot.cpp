#include "mbrot.h"

MandelbrotPointInfo compute_mandelbrot(d_complex c, int max_iters,
                                       bool collect_points) {
    auto info = MandelbrotPointInfo{};
    info.max_iters = max_iters;
    info.num_iters = max_iters;
    d_complex z_0{0.0, 0.0};
    d_complex z_n = z_0;
    for (int i = 0; i < max_iters; i++) {
       z_n = z_n*z_n + c; 
       if (collect_points) {
           info.points_in_path.emplace_back(z_n);
       }
       if (std::norm(z_n) > 2) {
           info.escaped = true;
           info.num_iters = i;
           break;
       }
    }
    return info;
}
