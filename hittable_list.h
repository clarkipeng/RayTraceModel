#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H
    

#include "hittable.h"
#include "general.h"
#include "aabb.h"
#include <vector>


class hittable_list : public hittable{
    public:
        hittable_list(){}
        hittable_list(std::shared_ptr<hittable> object) {add(object);}
        
        void clear() {objects.clear();}
        void add(std::shared_ptr<hittable> object) {
            objects.push_back(object);
            bbox = aabb(bbox, object->bounding_box());
        }

        virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const override{
            hit_record trec;
            bool hit = 0;
            auto closest = ray_t.max;

            for(const auto& object : objects){
                if(object->hit(r,interval(ray_t.min,closest),trec)){
                    hit = 1;
                    closest = trec.t;
                    rec = trec;
                }
            }
            return hit;
        }
        virtual aabb bounding_box() const override { return bbox; }
        virtual double pdf_value(const vec3& o, const vec3& v) const override {
            auto weight = 1.0/objects.size();
            auto sum = 0.0;

            for (const auto& object : objects)
                sum += weight * object->pdf_value(o, v);

            return sum;
        }
        virtual vec3 random(const vec3 &o) const override {
            return objects[random_ll(0, objects.size()-1)]->random(o);
        }

    public:
        std::vector<std::shared_ptr<hittable> > objects;
        aabb bbox;
};

#endif
