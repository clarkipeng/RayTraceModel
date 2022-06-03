#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "general.h"
#include "hittable.h"

class triangle : public hittable{
    public:
        triangle(vec3 _a, vec3 _b, vec3 _c, std::shared_ptr<material> mat) : a(_a), b(_b), c(_c), mp(mat) {
            //assume counterclockwise;
            area = cross(b-a, c-a).length();
            normal = unit_vector(cross(b-a, c-a));
        }
        void setuv(vec3 _uv1, vec3 _uv2, vec3 _uv3) {
            auv=_uv1;
            buv=_uv2;
            cuv=_uv3;
        }
        bool hit(const ray& r, interval ray_t, hit_record &rec) const override{
            //Möller–Trumbore intersection algorithm
            const float EPSILON= 0.0000001;
            vec3 e1,e2,pvec,tvec,qvec;
            double det,inv_det,u,v;
            e1 = b-a;
            e2 = c-a;
            pvec = cross(r.direction(),e2);
            det = dot(e1, pvec);
            inv_det = 1 / det;
            if(fabs(det) < EPSILON) return 0;
            tvec = r.origin()-a;
            u = inv_det * dot(tvec, pvec);
            if(u < 0.0 || u > 1) return 0;
            qvec = cross(tvec, e1);
            v = inv_det * dot(r.direction(), qvec);
            if(v < 0.0 || v+u > 1) return 0;
            
            double t = inv_det * dot(e2,qvec);
            
            if(!ray_t.contains(t))return 0;
            
            rec.set_face_normal(r, normal);
            rec.t = t;
            rec.p = r.at(t);
            rec.mat_ptr = mp;
            rec.u = u;
            rec.v = v;
            triangle_uv(rec.p, rec.u, rec.v);
            return 1;
        }
        aabb bounding_box() const override {
            interval x(fmin(fmin(a[0],b[0]),c[0]),fmax(fmax(a[0],b[0]),c[0]));
            interval y(fmin(fmin(a[1],b[1]),c[1]),fmax(fmax(a[1],b[1]),c[1]));
            interval z(fmin(fmin(a[2],b[2]),c[2]),fmax(fmax(a[2],b[2]),c[2]));
            return aabb(x,y,z);
        }
        virtual double pdf_value(const vec3& origin, const vec3& v) const override {
            hit_record rec;
            if (!this->hit(ray(origin, v), interval(0.001, inf), rec))
                return 0;

            auto distance_squared = rec.t * rec.t * v.length_squared();
            auto cosine = fabs(dot(v, rec.normal) / v.length());

            return distance_squared / (cosine * area);
        }

        virtual vec3 random(const vec3& origin) const override {
            double r1 = random_double();
            double r2 = random_double()*r1;
            auto random_point = a+ r1*(b-a) + r2*(c-b);
            return random_point - origin;
        }
    public:
        vec3 a,b,c;
        vec3 auv = vec3(0,0,-1),buv = vec3(0,1,-1),cuv = vec3(1,0,-1);
        // double sa,sb,sc; //side opposide to pts.
        double area;
        vec3 normal;
        std::shared_ptr<material> mp;
    private:
        void triangle_uv(const vec3& p, double& u, double& v) const {
            vec3 d(u*auv+ v*buv+ (1-u-v)*cuv);
            u = d[0];
            v = d[1];
        }
};

#endif
