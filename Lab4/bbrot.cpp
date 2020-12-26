#include "bbrot.h"
#include "cbqueue.h"
#include <thread>
#include <random>
#include <iostream>
#include <stdlib.h>
#include <getopt.h>
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

void generate_bbrot_trajectories(int num_points, int max_iters, ConcurrentBoundedQueue<SP_MandelbrotPointInfo> &queue) {
    
    // Seed with a real random value, if available
    std::random_device r;
 
    // Choose a random mean between 1 and 6
    std::default_random_engine e1(r());
    std::uniform_real_distribution<double> real_uniform_dist(-2, 1);
    std::uniform_real_distribution<double> complex_uniform_dist(-1.5, 1.5);
    for (int i = 0; i < num_points; i++) {
        auto point = d_complex{real_uniform_dist(e1), complex_uniform_dist(e1)}; 
        auto info = compute_mandelbrot(point, max_iters, true);
        if (info->escaped) {
            // Add to the queue
            queue.put(info);
        }
    }
    // Put done value by inserting a nullptr
    queue.put(std::shared_ptr<MandelbrotPointInfo>(nullptr));
}

int main(int argc, char **argv) {
    if (argc != 5) {
        cerr << "Invalid arguments" << endl;
        return 1;
    }
    char *str_end;
    int size = strtol(argv[1], &str_end, /* base */ 10);
    if (*str_end != 0) {
        cerr << "Invalid argument" << endl;
        return 1;
    }
    int num_start_points = strtol(argv[2], &str_end, 10); 
    if (*str_end != 0) {
        cerr << "Invalid argument" << endl;
        return 1;
    }
    int max_iters = strtol(argv[3], &str_end, 10);
    if (*str_end != 0) {
        cerr << "Invalid argument" << endl;
        return 1;
    }
    int N = strtol(argv[4], &str_end, 10);
    if (*str_end != 0) {
        cerr << "Invalid argument" << endl;
        return 1;
    }
    cerr << "Image size: " << size << endl;
    cerr << "Number of starting points: " << num_start_points << endl;
    cerr << "Max number of iterations: " << max_iters << endl;
    cerr << "Number of threads: " << N << endl;
    Image image = Image(size, size);
    int points_per_thread = num_start_points / N + 1;
    std::vector<std::thread> threads;
    ConcurrentBoundedQueue<SP_MandelbrotPointInfo> queue{100};
    for (int i = 0; i < N; i++) {
        // Initialize each thread
        std::thread t(generate_bbrot_trajectories, points_per_thread, max_iters, std::ref(queue));
        threads.push_back(std::move(t));
    }
    int num_finished = 0;
    while (num_finished < N) {
        auto info_ptr = queue.get();
        if (info_ptr == nullptr) {
            num_finished++;
            continue;
        }
        update_image(image, *info_ptr);
    }
    cerr << "Joining threads" << endl;
    // Join all threads
    for (auto& t : threads) {
        t.join();
    }
    output_image_to_pgm(image, std::cout);
    return 0;
}
