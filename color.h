#ifndef COLOR_H
#define COLOR_H

#include "general.h"
#include "vec3.h"

#include <iostream>
#include <fstream>


std::ofstream fout("image.ppm");

void write_color(std::ostream &out, vec3 pixel_color, int samples_per_pixel) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();
    
    //check NaN
    if (r != r) r = 0.0;
    if (g != g) g = 0.0;
    if (b != b) b = 0.0;

    //gamma-correct for gamma=2.0.
    auto scale = 1.0 / samples_per_pixel;
    r = sqrt(scale * r);
    g = sqrt(scale * g);
    b = sqrt(scale * b);

    static const interval intensity(0.000, 0.999);
    out << static_cast<int>(256 * intensity.clamp(r)) << ' '
        << static_cast<int>(256 * intensity.clamp(g)) << ' '
        << static_cast<int>(256 * intensity.clamp(b)) << '\n';
}
void write_image(uint8_t *image, int nx, int ny) {
    if (!fout.is_open())
    {
        std::cerr << "FILE NOT OPENED" << std::endl;
    }
   fout << "P3\n" << nx << " " << ny << "\n255\n";
    for (int j = ny-1; j >= 0; j--) {
        for (int i = 0; i < nx; ++i) {
            fout << static_cast<int>(image[3*(j*nx+i)+0]) << ' ';
            fout << static_cast<int>(image[3*(j*nx+i)+1]) << ' ';
            fout << static_cast<int>(image[3*(j*nx+i)+2]) << '\n';
        }
    }
}
#endif
