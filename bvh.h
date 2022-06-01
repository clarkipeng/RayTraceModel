#ifndef BVH_H
#define BVH_H
//bounding volume heirarchy

#include "general.h"
#include "hittable.h"
#include "hittable_list.h"
#include <algorithm>

class bvh_node : public hittable{
    public:
        bvh_node(){}
        
        bvh_node(const hittable_list &list) :
            bvh_node(list.objects, 0, list.objects.size()) {}
    
        bvh_node(const std::vector<std::shared_ptr<hittable>>& src_objects, size_t s, size_t e) {
            std::vector<std::shared_ptr<hittable>> objects = src_objects;
            auto size = e-s;
            
            if(size ==0){
                zero=1;
                return;
            }else if(size ==1){
                bbox = objects[0]->bounding_box();
                return;
            }
            gen_node(&objects[0], s, e);
        }
        bvh_node(std::shared_ptr<hittable> *objects, size_t s, size_t e) {
            gen_node(&objects[0], s, e);
        }
        
        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            if(zero)return false;
            if (!bbox.hit(r, ray_t))
                return false;

            bool hit_left = left->hit(r, ray_t, rec);
            bool hit_right = right->hit(r, interval(ray_t.min, hit_left ? rec.t : ray_t.max), rec);
            return hit_left || hit_right;
        }
        aabb bounding_box() const override { return bbox; }
    
        void gen_node(std::shared_ptr<hittable> *objects, size_t s, size_t e){
            int axis = random_int(0,2);
            auto comparator = (axis == 0) ? box_x_compare
                            : (axis == 1) ? box_y_compare
                                        : box_z_compare;

            size_t object_span = e - s;
            if (object_span == 3) {
                left = std::make_shared<bvh_node>(objects, s, s+2);
                right = objects[s+2];
            } else if (object_span == 2) {
                left = objects[s];
                right = objects[s+1];
            } else {
                std::sort(objects+ s, objects+ e, comparator);
                auto m = s + object_span/2;
                left= std::make_shared<bvh_node>(objects, s, m);
                right= std::make_shared<bvh_node>(objects, m, e);
            }
            bbox = aabb(left->bounding_box(), right->bounding_box());
        }
    public:
        std::shared_ptr<hittable> left;
        std::shared_ptr<hittable> right;
        aabb bbox;
        bool zero=0;
            
    private:
        static bool box_compare(const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b, int axis) {
            aabb box_a;
            aabb box_b;
            return a->bounding_box().axis(axis).min < b->bounding_box().axis(axis).min;
        }
        static bool box_x_compare (const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b) {
            return box_compare(a, b, 0);
        }
        static bool box_y_compare (const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b) {
            return box_compare(a, b, 1);
        }
        static bool box_z_compare (const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b) {
            return box_compare(a, b, 2);
        }
};

#endif
