#include "bbrot.h"
#include <random>
#include <iostream>
#include <stdlib.h>

double normalize(double min, double max, double value) {
    if (max < min) {
        cerr << "Max must be >= min" << endl;
        return -1;
    }
    return (value - min) / (max - min);
}
void update_image(Image &image, const MandelbrotPointInfo &info) {
    for (const auto &point : info.points_in_path) {
        if (real(point) < -2 || real(point) > 1 || abs(imag(point)) > 1.5) {
            continue;
        }
        
        int x = (int) (normalize(-2, 1, real(point)) * (image.getWidth() - 1));
        int y = (int) (normalize(-1.5, 1.5, imag(point)) * (image.getHeight() - 1));
        image.incValue(x, image.getHeight() - y - 1);
    }
}
void output_image_to_pgm(const Image &image, ostream &os) {

    // Header 
    int max_value = image.getMaxValue();
    os << "P2 " << image.getWidth() << " " << image.getHeight() << " 255" << endl;
    for (int y = 0; y < image.getHeight(); y++) {
        for (int x = 0; x < image.getWidth(); x++) {
            os << int(normalize(0, max_value, image.getValue(x, y)) * 255) << " ";
        }
        os << endl;
    }
}


int main(int argc, char **argv) {
    if (argc != 4) {
        cerr << "Invalid arguments" << endl;
        return 1;
    }
    int size = atoi(argv[1]);
    int num_start_points = atoi(argv[2]);
    int max_iters = atoi(argv[3]);
    Image image = Image(size, size);
    // Seed with a real random value, if available
    std::random_device r;
 
    // Choose a random mean between 1 and 6
    std::default_random_engine e1(r());
    std::uniform_real_distribution<double> real_uniform_dist(-2, 1);
    std::uniform_real_distribution<double> complex_uniform_dist(-1.5, 1.5);
    // Main Buddhabrot loop
    for (int i = 0; i < num_start_points; i++) {
        auto point = d_complex{real_uniform_dist(e1), complex_uniform_dist(e1)}; 
        auto info = compute_mandelbrot(point,max_iters, true); 
        if (info.escaped) {
            update_image(image, info);            
        }
    }
    output_image_to_pgm(image, std::cout);
    return 0;
}
