#ifndef CAMERA_H
#define CAMERA_H

#include "general.h"
#include "vec3.h"
#include "ray.h"

class camera{
    public:
        camera(){}
        camera(vec3 lf, vec3 la,vec3 up, double fov,double dia,double dist)
        : vfov(fov), aperture(dia), focus_dist(dist), lookfrom(lf),lookat(la),lookup(up){}
        void init(double aspect_ratio = 1.0) {
            auto theta = radians(vfov);
            auto h = tan(theta/2);
            auto viewport_height = 2.0 * h;
            auto viewport_width = aspect_ratio * viewport_height;

            w = unit_vector(lookfrom - lookat);
            u = unit_vector(cross(lookup, w));
            v = cross(w, u);

            origin = lookfrom;
            horiz = focus_dist * viewport_width * u;
            vert = focus_dist * viewport_height * v;
            llc = origin - horiz/2 - vert/2 - focus_dist*w;

            lens_radius = aperture / 2;
        }

        ray get_ray(double s, double t) const {
            vec3 rd = lens_radius * random_in_unit_disk();
            vec3 offset = u * rd.x() + v * rd.y();
            const auto ray_time = random_double(0.0, 1.0);

            return ray(
                origin + offset,
                llc + s*horiz + t*vert - origin - offset,
                ray_time
            );
        }
    public:
        double vfov       = 40;
        double aperture   = 0;
        double focus_dist = 10;
        vec3 lookfrom = vec3(0,0,-1);
        vec3 lookat   = vec3(0,0,0);
        vec3 lookup   = vec3(0,1,0);

    private:
        vec3 origin, llc, horiz, vert;
        vec3 u, v, w;
        double lens_radius;
};

#endif