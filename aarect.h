#ifndef AARECT_H
#define AARECT_H

#include "general.h"
#include "hittable.h"

class xy_rect: public hittable{
    public:
        xy_rect() {}

        xy_rect(double _x0, double _x1, double _y0, double _y1, double _z, 
            std::shared_ptr<material> mat)
            : x0(_x0), x1(_x1), y0(_y0), y1(_y1), z(_z), mp(mat) {
                if(x0>x1)std::swap(x0,x1);
                if(y0>y1)std::swap(y0,y1);
            }

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override{
            auto t = (z-r.origin().z()) / r.direction().z();
            if (!ray_t.contains(t))
                return 0;
            auto x = r.origin().x() + t*r.direction().x();
            auto y = r.origin().y() + t*r.direction().y();
            if (x < x0 || x > x1 || y < y0 || y > y1)
                return 0;
            rec.u = (x-x0)/(x1-x0);
            rec.v = (y-y0)/(y1-y0);
            rec.t = t;
            auto onormal = vec3(0, 0, 1);
            rec.set_face_normal(r, onormal);
            rec.mat_ptr = mp;
            rec.p = r.at(t);
            return 1;
        }
        aabb bounding_box() const override {
            return aabb(vec3(x0,y0, z-0.0001), vec3(x1, y1, z+0.0001));
        }
        virtual double pdf_value(const vec3& origin, const vec3& v) const override {
            hit_record rec;
            if (!this->hit(ray(origin, v), interval(0.001, inf), rec))
                return 0;

            auto area = (x1-x0)*(y1-y0);
            auto distance_squared = rec.t * rec.t * v.length_squared();
            auto cos = fabs(dot(v, rec.normal) / v.length());

            return distance_squared / (cos * area);
        }

        virtual vec3 random(const vec3& origin) const override {
            auto random_point = vec3(random_double(x0,x1), random_double(y0,y1), z);
            return random_point - origin;
        }
    public:
        double x0,x1,y0,y1,z;
        std::shared_ptr<material> mp;
};
class xz_rect: public hittable{
    public:
        xz_rect() {}

        xz_rect(double _x0, double _x1, double _z0, double _z1, double _y, 
            std::shared_ptr<material> mat)
            : x0(_x0), x1(_x1), z0(_z0), z1(_z1), y(_y), mp(mat) {
                if(x0>x1)std::swap(x0,x1);
                if(z0>z1)std::swap(z0,z1);
            }

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override{
            auto t = (y-r.origin().y()) / r.direction().y();
            if (!ray_t.contains(t))
                return 0;
            auto x = r.origin().x() + t*r.direction().x();
            auto z = r.origin().z() + t*r.direction().z();
            if (x < x0 || x > x1 || z < z0 || z > z1)
                return 0;
            rec.u = (x-x0)/(x1-x0);
            rec.v = (z-z0)/(z1-z0);
            rec.t = t;
            auto outward_normal = vec3(0, 1, 0);
            rec.set_face_normal(r, outward_normal);
            rec.mat_ptr = mp;
            rec.p = r.at(t);
            return 1;
        }
        aabb bounding_box() const override {
            return aabb(vec3(x0,y-0.0001,z0), vec3(x1, y+0.0001, z1));
        }
        virtual double pdf_value(const vec3& origin, const vec3& v) const override {
            hit_record rec;
            if (!this->hit(ray(origin, v), interval(0.001, inf), rec))
                return 0;

            auto area = (x1-x0)*(z1-z0);
            auto distance_squared = rec.t * rec.t * v.length_squared();
            auto cos = fabs(dot(v, rec.normal) / v.length());

            return distance_squared / (cos * area);
        }

        virtual vec3 random(const vec3& origin) const override {
            auto random_point = vec3(random_double(x0,x1), y, random_double(z0,z1));
            return random_point - origin;
        }
    public:
        double x0,x1,z0,z1,y;
        std::shared_ptr<material> mp;
};
class yz_rect: public hittable{
    public:
        yz_rect() {}

        yz_rect(double _y0, double _y1, double _z0, double _z1, double _x,
            std::shared_ptr<material> mat)
            : y0(_y0), y1(_y1), z0(_z0), z1(_z1), x(_x), mp(mat)
            {
                if(y0>y1)std::swap(y0,y1);
                if(z0>z1)std::swap(z0,z1);
            }

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override{
            auto t = (x-r.origin().x()) / r.direction().x();
            if (!ray_t.contains(t))
                return 0;
            auto y = r.origin().y() + t*r.direction().y();
            auto z = r.origin().z() + t*r.direction().z();
            if (y < y0 || y > y1 || z < z0 || z > z1)
                return 0;
            rec.u = (y-y0)/(y1-y0);
            rec.v = (z-z0)/(z1-z0);
            rec.t = t;
            auto outward_normal = vec3(1, 0, 0);
            rec.set_face_normal(r, outward_normal);
            rec.mat_ptr = mp;
            rec.p = r.at(t);
            return 1;
        }
        aabb bounding_box() const override {
            return aabb(vec3(x-0.0001, y0, z0), vec3(x+0.0001, y1, z1));
        }
        virtual double pdf_value(const vec3& origin, const vec3& v) const override {
            hit_record rec;
            if (!this->hit(ray(origin, v), interval(0.001, inf), rec))
                return 0;

            auto area = (z1-z0)*(y1-y0);
            auto distance_squared = rec.t * rec.t * v.length_squared();
            auto cosine = fabs(dot(v, rec.normal) / v.length());

            return distance_squared / (cosine * area);
        }

        virtual vec3 random(const vec3& origin) const override {
            auto random_point = vec3(x, random_double(y0,y1), random_double(z0,z1));
            return random_point - origin;
        }
    public:
        double y0,y1,z0,z1,x;
        std::shared_ptr<material> mp;
};

#endif
