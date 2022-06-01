#ifndef ONB_H
#define ONB_H
//orthonormal basis
#include "general.h"

class onb{
    public:
        onb() {}
        
        inline vec3 operator[](int i) const {return axis[i];}

        vec3 u() const {return axis[0];}
        vec3 v() const {return axis[1];}
        vec3 w() const {return axis[2];}

        vec3 get(const vec3& a) const {
            return a.x()*u() + a.y()*v() + a.z()*w();
        }
        vec3 get(double a, double b, double c) const {
            return a*u() + b*v() + c*w();
        }

        void build(const vec3& normal){
            axis[2] = unit_vector(normal);
            vec3 notnormal = (fabs(w().x())>0.9)? vec3(0,1,0) : vec3(1,0,0);
            axis[1] = unit_vector(cross(normal, notnormal));
            axis[0] = cross(w(), v());
        }
    public:
        vec3 axis[3];
};

#endif