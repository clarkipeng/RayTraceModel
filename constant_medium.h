#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H

#include "general.h"
#include "hittable.h"
#include "material.h"
#include "texture.h"

class constant_medium : public hittable {
    public:
        constant_medium(shared_ptr<hittable> b, double d, shared_ptr<texture> a): boundary(b),
              neg_inv_density(-1/d),
              phase_function(make_shared<isotropic>(a))
            {}
        constant_medium(shared_ptr<hittable> b, double d, vec3 c): boundary(b),
              neg_inv_density(-1/d),
              phase_function(make_shared<isotropic>(c))
            {}

        virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const override{
            const bool debug = 0;
            const bool debugi = debug&&random_double() < 0.00001;

            hit_record rec1, rec2;

            if (!boundary->hit(r, interval::universe, rec1)) return 0;
            if (!boundary->hit(r, interval(rec1.t+0.0001, inf), rec2))return 0;

            if (debugi) std::cerr << "\nt_min=" << rec1.t << ", t_max=" << rec2.t << '\n';

            rec1.t = fmax(rec1.t,ray_t.min);
            rec2.t = fmin(rec2.t,ray_t.max);
            if(rec1.t>=rec2.t)return 0;
            rec1.t = fmax(0, rec1.t);
            const auto rlength = r.direction().length();
            const auto dbound = (rec2.t - rec1.t) * rlength;
            const auto dthresh = neg_inv_density * log(random_double());
            if(dbound<dthresh)return 0;
            rec.t = rec1.t + dthresh / rlength;
            rec.p = r.at(rec.t);
            if (debugi) {
                std::cerr << "dbound = " <<  dbound << '\n'
                        << "rec.t = " <<  rec.t << '\n'
                        << "rec.p = " <<  rec.p << '\n';
            }
            rec.normal = vec3(1,0,0);// arbitrary
            rec.front_face = true;
            rec.mat_ptr = phase_function;
            return 1;
        }

        virtual aabb bounding_box() const override{
            return boundary->bounding_box();
        }

    public:
        shared_ptr<hittable> boundary;
        shared_ptr<material> phase_function;
        double neg_inv_density;
};


#endif