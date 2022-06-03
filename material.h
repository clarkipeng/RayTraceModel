#ifndef MATERIAL_H
#define MATERIAL_H


#include "general.h"
#include "texture.h"
#include "pdf.h"


struct scatter_record{
    ray specular_ray;
    bool is_specular;
    vec3 attenuation;
    shared_ptr<pdf> pdf_ptr;
};


class material {
    public:
        virtual vec3 emitted(const ray& r_in, const hit_record& rec, double u, double v, const vec3& p)
        const{
            return vec3(0.0);
        }
        virtual bool scatter(
            const ray& r_in, const hit_record& rec, scatter_record& srec
        ) const{
            return 0;
        }
        virtual double scattering_pdf(
            const ray& r_in, const hit_record& rec, const ray& scattered
        ) const { return 0.0; }
};

class lambertian : public material {
    public:
        lambertian(const vec3& c) : albedo(make_shared<solid_color>(c)) {}
        lambertian(shared_ptr<texture> c) : albedo(c) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, scatter_record& srec
        ) const override {
            srec.is_specular = 0;
            srec.attenuation = albedo->value(rec.u, rec.v, rec.p);
            srec.pdf_ptr = make_shared<cos_pdf>(rec.normal);
            return 1;
        }
        virtual double scattering_pdf(
            const ray& r_in, const hit_record& rec, const ray& scattered
        ) const override {
            auto cos = dot(rec.normal, unit_vector(scattered.direction()));
            return cos < 0 ? 0 : cos/pi;
        }

    public:
        shared_ptr<texture> albedo;
};
class metal : public material{
    public:
        metal(const vec3 c, double f=0.0): albedo(c), fuzz(fmin(1.0,f)){}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, scatter_record& srec
        ) const override {
            vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
            srec.is_specular =1;
            srec.specular_ray = ray(rec.p, reflected + fuzz*random_in_unit_sphere(), r_in.time());
            srec.attenuation = albedo;
            srec.pdf_ptr = nullptr;
            return 1;
        }

    public:
        vec3 albedo;
        double fuzz;
};
class dielectric :public material{
    public:
        dielectric(double n1) : n(n1){}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, scatter_record& srec
        ) const override {
            srec.is_specular = 1;
            srec.pdf_ptr = nullptr;
            srec.attenuation = vec3(1.0, 1.0, 1.0);
            double ratio = rec.front_face ? (1.0/n) : n;
            vec3 unit_direction = unit_vector(r_in.direction());

            double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
            double sin_theta = sqrt(1.0 - cos_theta*cos_theta);
            vec3 result;
            
            bool cant_refract = sin_theta * ratio >1.0;
            if(cant_refract||reflectance(cos_theta, ratio)>random_double())
                result = reflect(unit_direction, rec.normal);
            else 
                result = refract(unit_direction, rec.normal, ratio);

            srec.specular_ray = ray(rec.p, result, r_in.time());
            return true;
        }
    public:
        double n;
    private:
        static double reflectance(double cosine, double ref_idx) {
            // Use Schlick's approximation for reflectance.
            auto r0 = (1-ref_idx) / (1+ref_idx);
            r0 = r0*r0;
            return r0 + (1-r0)*pow((1 - cosine),5);
        }
};
class diffuse_light : public material{
    public:
        diffuse_light(shared_ptr<texture> a) : emit(a){}
        diffuse_light(vec3 c) : emit(make_shared<solid_color>(c)){}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, scatter_record& srec
        ) const override {
            return 0;
        }
        virtual vec3 emitted(const ray& r_in, const hit_record& rec, double u, double v, const vec3& p)
        const override {
            if (rec.front_face)
                return emit->value(u, v, p);
            return vec3(0,0,0);
        }


    public:
        shared_ptr<texture> emit;
};
class isotropic : public material {
    public:
        isotropic(vec3 c) : albedo(make_shared<solid_color>(c)) {}
        isotropic(shared_ptr<texture> a) : albedo(a) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, scatter_record& srec
        ) const override {
            srec.specular_ray = ray(rec.p, random_in_unit_sphere(), r_in.time());
            srec.attenuation = albedo->value(rec.u, rec.v, rec.p);
            return 1;
        }

    public:
        shared_ptr<texture> albedo;
};


#endif