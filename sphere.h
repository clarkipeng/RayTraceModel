#ifndef SPHERE_H
#define SPHERE_H

#include "general.h"
#include "hittable.h"

class sphere : public hittable {
    public:
        sphere() {}

        sphere(vec3 cen, double r, shared_ptr<material> m): center(cen), radius(r), mp(m) {
            const auto rvec = vec3(radius);
            bbox = aabb(center -rvec, center + rvec);
        };
        bool hit(const ray& r, interval ray_t, hit_record &rec) const override{
            vec3 oc = r.origin()- center;
            auto a = dot(r.direction(), r.direction());
            auto hb = dot(oc, r.direction());
            auto c = dot(oc, oc) - radius*radius;

            auto discriminant = hb*hb - a*c;
            if(discriminant<0)return 0;

            auto root = (-hb - sqrt(discriminant)) / a;
            if (!ray_t.contains(root)) {
                root = -root - 2*hb/a;
                if (!ray_t.contains(root))
                    return 0;
            }
            rec.t = root;
            rec.p = r.at(root);
            vec3 onormal = (rec.p - center) / radius;
            rec.set_face_normal(r, onormal);
            sphere_uv(onormal, rec.u, rec.v);
            rec.mat_ptr = mp;
            return 1;
        }
        aabb bounding_box() const override { return bbox; }
        virtual double pdf_value(const vec3& o, const vec3& v) const override {
            hit_record rec;
            if (!this->hit(ray(o, v), interval(0.001, inf), rec))
                return 0;

            auto cos_theta_max = sqrt(1 - radius*radius/(center-o).length_squared());
            auto solid_angle = 2*pi*(1-cos_theta_max);

            return  1 / solid_angle;
        }
        virtual vec3 random(const vec3& o) const override{
            vec3 direction = center - o;
            auto distance_squared = direction.length_squared();
            onb uvw;
            uvw.build(direction);
            return uvw.get(random_to_sphere(radius, distance_squared));
        }

    public:
        vec3 center;
        double radius;
        shared_ptr<material> mp;
        aabb bbox;
    private:
        static void sphere_uv(const vec3& p, double& u, double& v) {
            // p: pt on sphere at origin
            // u: returned value [0,1] of angle around the Y axis from X=-1.x
            // v: returned value [0,1] of angle from Y=-1 to Y=+1.
            //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
            //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
            //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

            auto theta = acos(-p.y());
            auto phi = atan2(-p.z(), p.x()) + pi;

            u = phi / (2*pi);
            v = theta / pi;
        }
};



#endif