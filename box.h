#ifndef BOX_H
#define BOX_H

#include "general.h"
#include "aarect.h"
#include "hittable_list.h"

class box : public hittable  {
    public:
        box() {}
        box(const vec3& p0, const vec3& p1, shared_ptr<material> mat){
            box_min = p0;
            box_max = p1;
            mp = mat;
            sides.add(make_shared<xy_rect>(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), mat));
            sides.add(make_shared<xy_rect>(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), mat));

            sides.add(make_shared<xz_rect>(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), mat));
            sides.add(make_shared<xz_rect>(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), mat));

            sides.add(make_shared<yz_rect>(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), mat));
            sides.add(make_shared<yz_rect>(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), mat));
        }

        virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const override{
            return sides.hit(r, ray_t, rec);
        }
        virtual aabb bounding_box() const override {
            return aabb(interval(box_min.x(),box_max.x()),interval(box_min.y(),box_max.y()),interval(box_min.z(),box_max.z()));
        }
    public:
        vec3 box_min;
        vec3 box_max;
        shared_ptr<material> mp;
        hittable_list sides;
};

#endif