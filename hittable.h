#ifndef HITTABLE_H
#define HITTABLE_H

#include "general.h"
#include "aabb.h"

class material;

struct hit_record {
    vec3 p;
    vec3 normal;
    double t;
    double u, v; //texcoord
    bool front_face;
    shared_ptr<material> mat_ptr;
    inline void set_face_normal(const ray& r, const vec3& outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal :-outward_normal;
    }

};

class hittable {
    public:
        virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
        virtual aabb bounding_box() const = 0;
        virtual double pdf_value(const vec3& o, const vec3& v) const {
            return 0.0;
        }
        virtual vec3 random(const vec3& o) const {
            return vec3(1,0,0);
        }
};
class translate : public hittable {
    public:
        translate(shared_ptr<hittable> p, const vec3& displacement)
            : ptr(p), offset(displacement) {}

        virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const override{
            ray r1 = ray(r.origin() - offset, r.direction(), r.time());
            if (!ptr->hit(r1, ray_t, rec))return 0;
            rec.p += offset;
            rec.set_face_normal(r1, rec.normal);
            return 1;
        }
        virtual aabb bounding_box() const override{
            return ptr->bounding_box()+offset;
        }

    public:
        shared_ptr<hittable> ptr;
        vec3 offset;
};
class rotate_ : public hittable {
    public:
        rotate_(shared_ptr<hittable> p, double ax, double ay, double az) : ptr(p){
            ax = radians(ax); ay =radians(ay); az = radians(az);
            sinx = sin(ax);siny = sin(ay);sinz = sin(az);
            cosx = cos(ax);cosy = cos(ay);cosz = cos(az);
            bbox = ptr->bounding_box();
            vec3 min(inf), max(-inf);
             for (int i = 0; i < 2; i++) {
                for (int j = 0; j < 2; j++) {
                    for (int k = 0; k < 2; k++) {
                        auto x = i*bbox.x.max + (1-i)*bbox.x.min;
                        auto y = j*bbox.y.max + (1-j)*bbox.y.min;
                        auto z = k*bbox.z.max + (1-k)*bbox.z.min;

                        auto tester = transform(x,y,z);

                        for (int c = 0; c < 3; c++) {
                            min[c] = fmin(min[c], tester[c]);
                            max[c] = fmax(max[c], tester[c]);
                        }
                    }
                }
            }
            bbox = aabb(min, max);
        }
        virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const override{
            auto origin = r.origin();
            auto direction = r.direction();
            origin = invtransform(origin);
            direction = invtransform(direction);

            ray rotated_r(origin, direction, r.time());

            if (!ptr->hit(rotated_r, ray_t, rec))
                return false;

            auto p = rec.p;
            auto normal = rec.normal;

            p = transform(p);
            normal = transform(normal);

            rec.p = p;
            rec.set_face_normal(rotated_r, normal);

            return true;
        }

        virtual aabb bounding_box() const override{
            return bbox;
        }

    public:
        shared_ptr<hittable> ptr;
        double sinx,cosx,siny,cosy,sinz,cosz;
        aabb bbox;
    private:
        vec3 transform(double x, double y, double z) const{
            double x1 =  x;
            double y1 =  cosx*y + sinx*z;
            double z1 = -sinx*y + cosx*z;

            x =  cosy*x1 + siny*z1;
            y =  y1;
            z = -siny*x1 + cosy*z1;

            x1 =  cosz*x + sinz*y;
            y1 = -sinz*x + cosz*y;
            z1 =  z;

            return vec3(x1,y1,z1);
        }
        vec3 transform(vec3 &v) const{
            return transform(v.x(),v.y(),v.z());
        }
        vec3 invtransform(double x, double y, double z) const{
            double x1 =  x;
            double y1 =  cosx*y - sinx*z;
            double z1 =  sinx*y + cosx*z;

            x =  cosy*x1 - siny*z1;
            y =  y1;
            z =  siny*x1 + cosy*z1;

            x1 =  cosz*x - sinz*y;
            y1 =  sinz*x + cosz*y;
            z1 =  z;

            return vec3(x1,y1,z1);
        }
        vec3 invtransform(vec3 &v) const{
            return invtransform(v.x(),v.y(),v.z());
        }
};
class flip_face : public hittable {
    public:
        flip_face(shared_ptr<hittable> p) : ptr(p) {}
        virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const override{
            if (!ptr->hit(r, ray_t, rec))
                return false;

            rec.front_face = !rec.front_face;
            return true;
        }
        virtual aabb bounding_box() const override {
            return ptr->bounding_box();
        }

    public:
        shared_ptr<hittable> ptr;
};


#endif
