#ifndef PDF_H
#define PDF_H
// probability density function

#include "general.h"

#include "onb.h"


inline vec3 random_cos_direction() {
    //pdf = cos(theta)/pi
    auto r1 = random_double();
    auto r2 = random_double();
    auto z = sqrt(1-r2);

    auto phi = 2*pi*r1;
    auto x = cos(phi)*sqrt(r2);
    auto y = sin(phi)*sqrt(r2);

    return vec3(x, y, z);
}
inline vec3 random_to_sphere(double radius, double distance_squared) {
    auto r1 = random_double();
    auto r2 = random_double();
    auto z = 1 + r2*(sqrt(1-radius*radius/distance_squared) - 1);

    auto phi = 2*pi*r1;
    auto x = cos(phi)*sqrt(1-z*z);
    auto y = sin(phi)*sqrt(1-z*z);

    return vec3(x, y, z);
}
class pdf{
    public:
        virtual ~pdf() {}

        virtual double value(const vec3& direction)const = 0;

        virtual vec3 generate() const = 0;
};

class cos_pdf : public pdf{
    public:
        cos_pdf(const vec3& normal) { uvw.build(normal); }

        virtual double value(const vec3& direction) const override{
            auto cos = dot(unit_vector(direction), uvw.w());
            return (cos <= 0) ? 0 : cos/pi;
        }
        virtual vec3 generate() const override {
            return uvw.get(random_cos_direction());
        }
    public:
        onb uvw;
};

class hittable_pdf : public pdf {
    public:
    
        hittable_pdf(shared_ptr<hittable> p, const vec3& origin) : ptr(p), o(origin) {}

        virtual double value(const vec3& direction) const override {
            return ptr->pdf_value(o, direction);
        }

        virtual vec3 generate() const override {
            return ptr->random(o);
        }

    public:
        vec3 o;
        shared_ptr<hittable> ptr;
};

class mixture_pdf : public pdf{
    public: 
        mixture_pdf(shared_ptr<pdf> p0, shared_ptr<pdf> p1){
            p[0]=p0;
            p[1]=p1;
        }

        virtual double value(const vec3& direction) const override{
            return 0.5* p[0]->value(direction) + 0.5* p[1]->value(direction);
        }
        
        virtual vec3 generate() const override{
            if(random_double() <0.5)return p[0]->generate();
            else return p[1]->generate();
        }
    public:
        shared_ptr<pdf> p[2];
};

#endif