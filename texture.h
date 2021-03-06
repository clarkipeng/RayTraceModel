#ifndef TEXTURE_H
#define TEXTURE_H

#include "general.h"


class texture{
    public:
        virtual vec3 value(double u, double v, const vec3& p) const = 0;
};
class solid_color : public texture{
    public: 
        solid_color(){}
        solid_color(vec3 col): color_value(col){}

        solid_color(double red, double green, double blue)
          : solid_color(vec3(red,green,blue)) {}
        
        virtual vec3 value(double u, double v, const vec3& p) const override{
            return color_value;
        }

    public:
        vec3 color_value;
};
class checkered_texture: public texture{
    public:
        checkered_texture(){}
        checkered_texture(shared_ptr<texture> _even, shared_ptr<texture> _odd)
            : even(_even), odd(_odd) {}
        checkered_texture(vec3 _even, vec3 _odd)
            : even(make_shared<solid_color>(_even)), odd(make_shared<solid_color>(_odd)) {}
            
        virtual vec3 value(double u, double v, const vec3& p) const override {
            auto sines = sin(10*p.x())*sin(10*p.y())*sin(10*p.z());
            if (sines < 0)
                return odd->value(u, v, p);
            else
                return even->value(u, v, p);
        }

    public:
        shared_ptr<texture> odd, even;
};
#include "perlin.h"
class noise_texture: public texture{
    public:
        noise_texture(){}
        noise_texture(double s): scale(s){}

        virtual vec3 value(double u, double v, const vec3& p) const override{
            //return vec3(1,1,1) * 0.5 * (1.0 + noise.noise(scale * p));
            //return vec3(1.0) * noise.turb(scale * p);
            return vec3(1,1,1) * 0.5 * (1 + sin(scale*p.z() + 10*noise.turb(p)));
        }
    public:
        perlin noise;
        double scale;
};
#include "general_stb_image.h"

class image_texture : public texture {
    public:
        const static int bytes_per_pixel = 3;

        image_texture()
          : data(nullptr), width(0), height(0), bytes_per_scanline(0) {}

        image_texture(const char* filename) {
            
            auto components_per_pixel = bytes_per_pixel;

            data = stbi_load(
                filename, &width, &height, &components_per_pixel, components_per_pixel);

            if (!data) {
                std::cerr << "ERROR: Could not load texture image file '" << filename << "'.\n";
                width = height = 0;
            }
            bytes_per_scanline = bytes_per_pixel * width;
        }

        ~image_texture() {
            STBI_FREE(data);
        }

        virtual vec3 value(double u, double v, const vec3& p) const override {
            // If we have no texture data, then return solid cyan as a debugging aid.
            if (data == nullptr)
                return vec3(0,1,1);
            
            // Clamp input texture coordinates to [0,1] x [1,0]
            u = clamp(u, 0.0, 1.0);
            v = 1.0 - clamp(v, 0.0, 1.0);  // Flip V to image coordinates

            auto i = static_cast<int>(u * width);
            auto j = static_cast<int>(v * height);

            // Clamp integer mapping, since actual coordinates should be less than 1.0
            if (i >= width)  i = width-1;
            if (j >= height) j = height-1;

            const auto color_scale = 1.0 / 255.0;
            auto pixel = data + j*bytes_per_scanline + i*bytes_per_pixel;

            return color_scale* vec3(pixel[0], pixel[1], pixel[2]);
        }

    private:
        unsigned char *data;
        int width, height;
        int bytes_per_scanline;
};


#endif