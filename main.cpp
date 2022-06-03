
#include "model.h"
#include <functional>
#include <iostream>
#include <thread>
#include "general.h"
#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "aarect.h"
#include "bvh.h"
#include "box.h"
#include "constant_medium.h"
#include "pdf.h"
#include "triangle.h"
#include <chrono>
#include <ctime>



void simple_triangle(hittable_list &world, std::shared_ptr<hittable_list> &lights){
    auto green = make_shared<lambertian>(vec3(.12, .45, .15));
    auto red   = make_shared<lambertian>(vec3(.65, .05, .05));
    auto light = make_shared<diffuse_light>(vec3(15, 15, 15));
    
    world.add(make_shared<triangle>(vec3(-1,-1,0), vec3(1,-1,0), vec3(0,2,0), green));
    world.add(make_shared<triangle>(vec3(-2,-2,-1), vec3(2,-2,-1), vec3(0,4,-1), red));
    
    world.add(make_shared<flip_face>(make_shared<xy_rect>(-5,5,-5,5,10, light)));
    lights->add(make_shared<xy_rect>(-5,5,-5,5,10, shared_ptr<material>()));
}
void test(hittable_list &world, std::shared_ptr<hittable_list> &lights){
    
    model m("backpack/backpack.obj");
    world = m.getHittableList();
    auto red   = make_shared<lambertian>(vec3(.65, .05, .05));
    auto white = make_shared<lambertian>(vec3(.73, .73, .73));
    auto green = make_shared<lambertian>(vec3(.12, .45, .15));
    auto light = make_shared<diffuse_light>(vec3(15, 15, 15));
    world.add(make_shared<yz_rect>(0-270, 555-270, 0, 555, 555-270, green));
    world.add(make_shared<yz_rect>(0-270, 555-270, 0, 555, 0-270, red));
    world.add(make_shared<flip_face>(make_shared<xz_rect>(213-270, 343-270, 227, 332, 554-270, light)));
    lights->add(make_shared<xz_rect>(213-270, 343-270, 227, 332, 554-270, shared_ptr<material>()));
    world.add(make_shared<xz_rect>(0-270, 555-270, 0, 555, 0-270, white));
    world.add(make_shared<xz_rect>(0-270, 555-270, 0, 555, 555-270, white));
    world.add(make_shared<xy_rect>(0-270, 555-270, 0-270, 555-270, 555, white));
    
}
void final_scene(hittable_list &world, std::shared_ptr<hittable_list> &lights){
    hittable_list boxes1;
    auto ground = make_shared<lambertian>(vec3(0.48, 0.83, 0.53));

    const int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i*w;
            auto z0 = -1000.0 + j*w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_double(1,101);
            auto z1 = z0 + w;

            boxes1.add(make_shared<box>(vec3(x0,y0,z0), vec3(x1,y1,z1), ground));
        }
    }

    hittable_list objects;

    objects.add(make_shared<bvh_node>(boxes1));

    auto light = make_shared<diffuse_light>(vec3(7, 7, 7));
    objects.add(make_shared<xz_rect>(123, 423, 147, 412, 554, light));
    
    lights->add(make_shared<xz_rect>(123, 423, 147, 412, 554, shared_ptr<material>()));

    auto center1 = vec3(400, 400, 200);
//    auto center2 = center1 + vec3(30,0,0);

    objects.add(make_shared<sphere>(vec3(260, 150, 45), 50, make_shared<dielectric>(1.5)));
    objects.add(make_shared<sphere>(
        vec3(0, 150, 145), 50, make_shared<metal>(vec3(0.8, 0.8, 0.9), 1.0)
    ));

    auto boundary = make_shared<sphere>(vec3(360,150,145), 70, make_shared<dielectric>(1.5));
    objects.add(boundary);
    objects.add(make_shared<constant_medium>(boundary, 0.2, vec3(0.2, 0.4, 0.9)));
    boundary = make_shared<sphere>(vec3(0, 0, 0), 5000, make_shared<dielectric>(1.5));
    objects.add(make_shared<constant_medium>(boundary, .0001, vec3(1,1,1)));

    auto emat = make_shared<lambertian>(make_shared<image_texture>("earthmap.jpg"));
    objects.add(make_shared<sphere>(vec3(400,200,400), 100, emat));
    auto pertext = make_shared<noise_texture>(0.1);
    objects.add(make_shared<sphere>(vec3(220,280,300), 80, make_shared<lambertian>(pertext)));

    hittable_list boxes2;
    auto white = make_shared<lambertian>(vec3(.73, .73, .73));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes2.add(make_shared<sphere>(vec3::random(0,165), 10, white));
    }

    objects.add(make_shared<translate>(
        make_shared<rotate_>(
            make_shared<bvh_node>(boxes2), 0,15,0),
            vec3(-100,270,395)
        )
    );

    world = objects;
}
void cornell_smoke(hittable_list &world, std::shared_ptr<hittable_list> &lights){
    hittable_list objects;

    auto red   = make_shared<lambertian>(vec3(.65, .05, .05));
    auto white = make_shared<lambertian>(vec3(.73, .73, .73));
    auto green = make_shared<lambertian>(vec3(.12, .45, .15));
    auto light = make_shared<diffuse_light>(vec3(15, 15, 15));

    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xz_rect>(213, 343, 227, 332, 554, light));
    lights->add(make_shared<xz_rect>(213, 343, 227, 332, 554, shared_ptr<material>()));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<box>(vec3(130, 0, 65), vec3(295, 165, 230), white));
    objects.add(make_shared<box>(vec3(265, 0, 295), vec3(430, 330, 460), white));

    shared_ptr<hittable> box1 = make_shared<box>(vec3(0, 0, 0), vec3(165, 330, 165), white);
    box1 = make_shared<rotate_>(box1, 0, 15, 0);
    box1 = make_shared<translate>(box1, vec3(265,0,295));
    objects.add(box1);

    shared_ptr<hittable> box2 = make_shared<box>(vec3(0,0,0), vec3(165,165,165), white);
    box2 = make_shared<rotate_>(box2, 0, -18, 0);
    box2 = make_shared<translate>(box2, vec3(130,0,65));
    objects.add(box2);

    objects.add(make_shared<constant_medium>(box1, 0.01, vec3(0,0,0)));
    objects.add(make_shared<constant_medium>(box2, 0.01, vec3(1,1,1)));

    world = objects;
}
void cornell_box(hittable_list &world, std::shared_ptr<hittable_list> &lights){
    hittable_list objects;

    auto red   = make_shared<lambertian>(vec3(.65, .05, .05));
    auto white = make_shared<lambertian>(vec3(.73, .73, .73));
    auto green = make_shared<lambertian>(vec3(.12, .45, .15));
    auto light = make_shared<diffuse_light>(vec3(15, 15, 15));

    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<flip_face>(make_shared<xz_rect>(213, 343, 227, 332, 554, light)));
    lights->add(make_shared<xz_rect>(213, 343, 227, 332, 554, shared_ptr<material>()));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

    shared_ptr<material> aluminum = make_shared<metal>(vec3(0.8, 0.85, 0.88), 0.0);
    shared_ptr<hittable> box1 = make_shared<box>(vec3(0,0,0), vec3(165,330,165), aluminum);
    box1 = make_shared<rotate_>(box1, 0 , 15, 0);
    box1 = make_shared<translate>(box1, vec3(265,0,295));
    objects.add(box1);

    auto glass = make_shared<dielectric>(1.5);
    objects.add(make_shared<sphere>(vec3(190,90,190), 90 , glass));

    world = objects;
}
void simple_light(hittable_list &world, std::shared_ptr<hittable_list> &lights){
    hittable_list objects;

    auto pertext = make_shared<noise_texture>(4);
    objects.add(make_shared<sphere>(vec3(0,-1000,0), 1000, make_shared<lambertian>(pertext)));
    objects.add(make_shared<sphere>(vec3(0,2,0), 2, make_shared<lambertian>(pertext)));

    auto difflight = make_shared<diffuse_light>(vec3(4));
    objects.add(make_shared<xy_rect>(3, 5, 1, 3, -2, difflight));
    lights->add(make_shared<xy_rect>(3, 5, 1, 3, -2, shared_ptr<material>()));

    world = objects;
}
hittable_list earth() {
    auto earth_texture = make_shared<image_texture>("earthmap.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    auto globe = make_shared<sphere>(vec3(0,0,0), 2, earth_surface);
    return hittable_list(globe);
}
hittable_list two_perlin_spheres() {
    hittable_list objects;

    auto pertext = make_shared<noise_texture>(6);
    objects.add(make_shared<sphere>(vec3(0,-1000,0), 1000, make_shared<lambertian>(pertext)));
    objects.add(make_shared<sphere>(vec3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

    return objects;
}
hittable_list two_spheres() {
    hittable_list objects;

    auto checker = make_shared<checkered_texture>(vec3(0.2, 0.3, 0.1), vec3(0.9, 0.9, 0.9));

    objects.add(make_shared<sphere>(vec3(0,-10, 0), 10, make_shared<lambertian>(checker)));
    objects.add(make_shared<sphere>(vec3(0, 10, 0), 10, make_shared<lambertian>(checker)));

    return objects;
}
hittable_list random_scene() {
    hittable_list world;

    // auto ground_material = make_shared<lambertian>(vec3(0.5, 0.5, 0.5));
    // world.add(make_shared<sphere>(vec3(0,-1000,0), 1000, ground_material));

    auto checker = make_shared<checkered_texture>(vec3(0.2, 0.3, 0.1), vec3(0.9, 0.9, 0.9));
    world.add(make_shared<sphere>(vec3(0,-1000,0), 1000, make_shared<lambertian>(checker)));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            vec3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = vec3::random() * vec3::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = vec3::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(vec3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(vec3(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(vec3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(vec3(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(vec3(4, 1, 0), 1.0, material3));

    return world;
}
vec3 ray_color(const ray& r, const vec3& background, const hittable& world,
    shared_ptr<hittable> lights,  int dep) {
    hit_record rec;
    if(dep==0){
        return vec3(0,0,0);
    }
    
    if (!world.hit(r, interval(0.001, inf), rec)) {
        return background;
    }
    scatter_record srec;
    vec3 emitted = rec.mat_ptr->emitted(r, rec, rec.u, rec.v, rec.p);

    if(!rec.mat_ptr->scatter(r, rec, srec))
        return emitted;
    
    if(srec.is_specular){
        return srec.attenuation * ray_color(srec.specular_ray, background, world, lights, dep-1);
    }
    auto light_ptr = make_shared<hittable_pdf>(lights, rec.p);
    mixture_pdf p(light_ptr, srec.pdf_ptr);
    ray scattered(rec.p, p.generate(), r.time());
    double pdfv = p.value(scattered.direction());
    // if(pdfv)cerr<<"ERROR"<<std::endl;
    return emitted + 
           srec.attenuation * rec.mat_ptr->scattering_pdf(r, rec, scattered) 
                  * ray_color(scattered, background, world, lights, dep-1)/ pdfv;
}

vec3 ray_color(const ray& r, const vec3& background, const bvh_node& node,
    shared_ptr<hittable> &lights,  int dep) {
    hit_record rec;
    if(dep==0){
        return vec3(0,0,0);
    }
    
    if (!node.hit(r, interval(0.001, inf), rec)) {
        return background;
    }
    scatter_record srec;
    vec3 emitted = rec.mat_ptr->emitted(r, rec, rec.u, rec.v, rec.p);

    if(!rec.mat_ptr->scatter(r, rec, srec))
        return emitted;
    
    if(srec.is_specular){
        return srec.attenuation * ray_color(srec.specular_ray, background, node, lights, dep-1);
    }
    auto light_ptr = make_shared<hittable_pdf>(lights, rec.p);
    mixture_pdf p(light_ptr, srec.pdf_ptr);
    ray scattered(rec.p, p.generate(), r.time());
    double pdfv = p.value(scattered.direction());
    return emitted + 
           srec.attenuation
            * rec.mat_ptr->scattering_pdf(r, rec, scattered)
            * ray_color(scattered, background, node, lights, dep-1)
            / pdfv;
    
}

void multitreading_render();
void render();

double aspect_ratio;
int nx,ny,ns,mxdepth;
vec3 lookfrom;
vec3 lookat;
vec3 vup;
double dist_to_focus;
double aperture;
double vfov;
vec3 background;
hittable_list world;
camera cam;
bvh_node node;
shared_ptr<hittable_list> lights = make_shared<hittable_list>();
std::vector<uint8_t> image;

int main() {
    // Image
    aspect_ratio = 16.0 / 9.0;
    nx = 300;
    ns = 100;
    mxdepth = 10;
    
    // Camera
    vup = vec3(0,1,0);
    dist_to_focus = 10.0;
    aperture = 0.0;
    vfov= 40.0;
    // World
    background = vec3(0,0,0);
    switch (0) {
        case 1:
            world = random_scene();
            background = vec3(0.70, 0.80, 1.00);
            lookfrom = vec3(13,2,3);
            lookat = vec3(0,0,0);
            vfov = 20.0;
            aperture = 0.0;
            break;
        case 2:
            world = two_spheres();
            background = vec3(0.70, 0.80, 1.00);
            lookfrom = vec3(13,2,3);
            lookat = vec3(0,0,0);
            vfov = 20.0;
            break;
        case 3:
            world = two_perlin_spheres();
            background = vec3(0.70, 0.80, 1.00);
            lookfrom = vec3(13,2,3);
            lookat = vec3(0,0,0);
            vfov = 20.0;
            break;
        case 4:
            world = earth();
            background = vec3(0.70, 0.80, 1.00);
            lookfrom = vec3(13,2,3);
            lookat = vec3(0,0,0);
            vfov = 20.0;
            break;
        case 5:
            simple_light(world, lights);
            ns = 400;
            background = vec3(0);
            lookfrom = vec3(26,3,6);
            lookat = vec3(0,2,0);
            vfov = 20.0;
            break;
        case 6:
            cornell_box(world, lights);
            aspect_ratio = 1.0;
            nx = 200;
            ns = 500;
            background = vec3(0,0,0);
            lookfrom = vec3(278, 278, -800);
            lookat = vec3(278, 278, 0);
            vfov = 40.0;
            break;
        case 7:
            cornell_smoke(world, lights);
            aspect_ratio = 1.0;
            nx = 300;
            ns = 100;
            lookfrom = vec3(278, 278, -800);
            lookat = vec3(278, 278, 0);
            vfov = 40.0;
            break;
        case 8:
            final_scene(world, lights);
            aspect_ratio = 1.0;
            nx = 400;
            ns = 1000;
            background = vec3(0,0,0);
            lookfrom = vec3(478, 278, -600);
            lookat = vec3(278, 278, 0);
            vfov = 40.0;
            break;
        case 9:
            simple_triangle(world, lights);
            aspect_ratio = 1.0;
            nx = 100;
            ns = 300;
            background = vec3(0.5);
            lookfrom = vec3(0, 0, 5);
            lookat = vec3(0,0,0);
            vfov = 80.0;
            break;
        default:
        case 10:
            test(world, lights);
            aspect_ratio = 1.0;
            nx = 200;
            ns = 300;
            background = vec3(0,0,0);
            lookfrom = vec3(0, 0, -80);
            lookat = vec3(0,0,0);
            vfov = 40.0;
            break;
    }
    ny = static_cast<int>(nx / aspect_ratio);

    std::cerr<<"GEN BVH"<<std::endl;
    node = bvh_node(world);
    
    cam = camera(lookfrom, lookat, vup, vfov, aperture, dist_to_focus);
    cam.init(aspect_ratio);
    // render();
    std::cerr<<"START RENDER"<<std::endl;
    auto blockTime = make_pair(std::chrono::high_resolution_clock::now(), true);
    image.resize(3*nx*ny);
    fill(image.begin(),image.end(), 0);
    multitreading_render();
    // render();
    int t =((int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - blockTime.first).count());
    std::cerr << "\nDone: execution time "<<t<<std::endl;
    // std::cerr<< (double)cntnan/total<<std::endl;
}
int done = 0;
void threadrun(int start, int end){
    // std::cerr<<"THREAD RAN"<<std::endl;
    for(int j=start ;j<end ;j++){
        for (int i = 0; i < nx; i++) {
            vec3 col(0.0);
            for (int s=0; s < ns; s++) {
                float u = float(i + random_double()) / float(nx);
                float v = float(j + random_double()) / float(ny);
                ray r = cam.get_ray(u, v);
                col += ray_color(r, background, node, lights, mxdepth);
            }
            col /= float(ns);
            col = vec3( sqrt(col[0]), sqrt(col[1]), sqrt(col[2]) );
            if (col[0] != col[0]) col[0] = 0.0;
            if (col[1] != col[1]) col[1] = 0.0;
            if (col[2] != col[2]) col[2] = 0.0;

            image[3*(j*nx+i)+0] = int(255.999*fmin(1.0,col[0]));
            image[3*(j*nx+i)+1] = int(255.999*fmin(1.0,col[1]));
            image[3*(j*nx+i)+2] = int(255.999*fmin(1.0,col[2]));
        }
        done++;
        std::cerr<<'\r'<< done << " / "<< ny<<std::flush;
    }
    // std::cerr<<"THREAD DONE"<<std::endl;
}
void multitreading_render(){
    const int numthreads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads(numthreads);
    std::cerr<<"NUMTHREADS "<<numthreads<<std::endl;
    for(int t=0;t<numthreads;t++){
        int start = t*ny/numthreads;
        int end = (t+1)*ny/numthreads;
        if(t==numthreads-1)end = ny;
        threads[t] = std::thread(threadrun, start, end);
    }
    for (int t = 0; t < numthreads; ++t) {
        threads[t].join();
    }
    write_image(&image[0], nx, ny);
}
void render(){
    std::cout << "P3\n" << nx << ' ' << ny << "\n255\n";
    for (int j = ny-1; j >= 0; j--) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < nx; i++) {
            vec3 pixel_color(0,0,0);
            for (int s = 0; s < ns; ++s) {
                auto u = (i + random_double()) / (nx-1);
                auto v = (j + random_double()) / (ny-1);
                ray r = cam.get_ray(u, v);
                
                pixel_color += ray_color(r, background, node, lights, mxdepth);
            }
            write_color(std::cout, pixel_color, ns);
        }
    }
}
