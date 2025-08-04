#include "rtWeekend.h"
#include "scene_info.h"
#include "interval.h"
#include "aabb.h"
#include "hittable.h"
#include "hittable_list.h"
#include "bvh.h"
#include "sphere.h"
#include "triangle.h"
#include "polygon_mesh.h"
#include "image_opener.h"
#include "camera.h"
#include "material.h"

void scene1(hittable_list& world, camera& cam) {
    // 물체에 사용할 머티리얼
    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_left = make_shared<dielectric>(1.50);
    auto material_bubble = make_shared<dielectric>(1.00 / 1.50);
    auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 0.2);

    world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(make_shared<sphere>(point3(0.0, 0.0, 0.8), 0.5, material_center));
    world.add(make_shared<sphere>(point3(-1.0, 0.0, 0.8), 0.3, material_left));
    world.add(make_shared<sphere>(point3(-1.0, 0.0, 0.8), 0.5, material_bubble));
    world.add(make_shared<sphere>(point3(1.0, 0.0, 0.8), 0.5, material_right));
}
// 카메라 테스트 1
void scene2(hittable_list& world, camera& cam) {
    auto R = std::cos(pi / 4);
    
    auto material_left = make_shared<lambertian>(color(0, 0, 1));
    auto material_right = make_shared<lambertian>(color(1, 0, 0));

    world.add(make_shared<sphere>(point3(-R, 0, -1), R, material_left));
    world.add(make_shared<sphere>(point3(R, 0, -1), R, material_right));
}

// 삼각형 테스트
void scene3(hittable_list& world, camera& cam) {
    auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_left = make_shared<metal>(color(0.3, 0.6, 0.8), 1.0);
    auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 0.1);
    
    world.add(make_shared<triangle>(
	point3(-0.5, 0.1, 1.0),
	point3(0.5, 0.1, 1.0),
	point3(0.0, 0.85, 1.0),
	material_center
    ));

    world.add(make_shared<triangle>(
	point3(-1.5, 0.1, 1.1),
	point3(-0.5, 0.1, 1.0),
	point3(-1.0, 0.85, 1.0),
	material_left
    ));

    world.add(make_shared<triangle>(
	point3(0.5, 0.1, 1.0),
	point3(1.5, 0.1, 1.1),
	point3(1.0, 0.85, 1.0),
	material_right
    ));

    cam.lookfrom = point3(0, 0, 2); // scene 3
    cam.lookat = point3(0, 0, 0);
}

// 폴리곤 메시 테스트
void scene4(hittable_list& world, camera& cam) {
    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_lambertian = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_metal1 = make_shared<metal>(color(0.3, 0.6, 0.8), 1.0);
    auto material_metal2 = make_shared<metal>(color(0.8, 0.6, 0.2), 0.4);
    auto material_dielectric = make_shared<dielectric>(1.50);

    world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));

    std::string teapot_path = "C:/Users/kkj48/Desktop/Projects/RayTracingInOneWeekend/res/teapot.obj";
    auto obj1 = make_shared<polygon_mesh>(
	teapot_path,
	material_lambertian,
	world,
	point3(0, 0, 0),
	vec3(1, 1, 1)
    );

    auto obj2 = make_shared<polygon_mesh>(
	teapot_path,
	material_dielectric,
	world,
	point3(-6, 0, 0),
	vec3(1, 1, 1)
    );

    std::string bunny_path = "C:/Users/kkj48/Desktop/Projects/RayTracingInOneWeekend/res/stanford-bunny.obj";
    auto obj3 = make_shared<polygon_mesh>(
	bunny_path,
	material_metal2,
	world,
	point3(6, 0, 1),
	vec3(20, 20, 20)
    );

    cam.lookfrom = point3(0, 4, 6);
    cam.lookat = point3(0, 0, 0);
}

// Triangle 개수에 따른 렌더 시간 테스트
void scene5(hittable_list& world, camera& cam) {
    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_lambertian = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_metal = make_shared<metal>(color(0.8, 0.6, 0.2), 0.1);

    std::string bunny_path = "C:/Users/kkj48/Desktop/Projects/RayTracingInOneWeekend/res/stanford-bunny.obj";
    std::string bunny_path_08 = "C:/Users/kkj48/Desktop/Projects/RayTracingInOneWeekend/res/stanford-bunny-08.obj";
    std::string bunny_path_06 = "C:/Users/kkj48/Desktop/Projects/RayTracingInOneWeekend/res/stanford-bunny-06.obj";
    std::string bunny_path_04 = "C:/Users/kkj48/Desktop/Projects/RayTracingInOneWeekend/res/stanford-bunny-04.obj";
    std::string bunny_path_02 = "C:/Users/kkj48/Desktop/Projects/RayTracingInOneWeekend/res/stanford-bunny-02.obj";
    std::string bunny_path_01 = "C:/Users/kkj48/Desktop/Projects/RayTracingInOneWeekend/res/stanford-bunny-01.obj";

    //world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
    auto bunny_test = make_shared<polygon_mesh>(
	bunny_path, 
	material_metal,
	world,
	point3(0, 2, 0),
	vec3(20, 20, 20)
    );

    cam.lookfrom = point3(0, 5, 3); // scene 5
    cam.lookat = point3(0, 4, 0);
}

void scene6(hittable_list& world, camera& cam) {
    auto material_lambertian = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_metal = make_shared<metal>(color(0.8, 0.6, 0.2), 0.1);
    std::string vase_path = "C:/Users/kkj48/Desktop/Projects/RayTracingInOneWeekend/res/vase.obj";
    auto vase = make_shared<polygon_mesh>(
	vase_path,
	material_metal,
	world,
	point3(0, 4, 0),
	vec3(0.1, 0.1, 0.1)
    );
}

void cornell_box(hittable_list& world, camera& cam) {
    std::string left_path = "C:/Users/kkj48/Desktop/Projects/RayTracingInOneWeekend/res/cornell_box/left.obj";
    std::string right_path = "C:/Users/kkj48/Desktop/Projects/RayTracingInOneWeekend/res/cornell_box/right.obj";
    std::string back_path = "C:/Users/kkj48/Desktop/Projects/RayTracingInOneWeekend/res/cornell_box/back.obj";
    std::string floor_path = "C:/Users/kkj48/Desktop/Projects/RayTracingInOneWeekend/res/cornell_box/floor.obj";
    std::string ceil_path = "C:/Users/kkj48/Desktop/Projects/RayTracingInOneWeekend/res/cornell_box/ceil.obj";

    auto mat_red = make_shared<lambertian>(color(1.0, 0.0, 0.0));
    auto mat_green = make_shared<lambertian>(color(0.0, 1.0, 0.0));
    auto mat_blue = make_shared<lambertian>(color(0.0, 0.0, 1.0));
    auto mat_white = make_shared<lambertian>(color(1.0, 1.0, 1.0));

    auto left = make_shared<polygon_mesh>(
	left_path,
	mat_red,
	world,
	point3(0, 0, 0),
	vec3(0.01, 0.01, 0.01)
    );

    auto right = make_shared<polygon_mesh>(
	right_path,
	mat_green,
	world,
	point3(0, 0, 0),
	vec3(0.01, 0.01, 0.01)
    );

    auto ceil = make_shared<polygon_mesh>(
	ceil_path,
	mat_white,
	world,
	point3(0, 0, 0),
	vec3(0.01, 0.01, 0.01)
    );

    auto back = make_shared<polygon_mesh>(
	back_path,
	mat_white,
	world,
	point3(0, 0, 0),
	vec3(0.01, 0.01, 0.01)
    );

    auto floor = make_shared<polygon_mesh>(
	floor_path,
	mat_white,
	world,
	point3(0, 0, 0),
	vec3(0.01, 0.01, 0.01)
    );

    std::string bunny_path = "C:/Users/kkj48/Desktop/Projects/RayTracingInOneWeekend/res/stanford-bunny.obj";
    std::string teapot_path = "C:/Users/kkj48/Desktop/Projects/RayTracingInOneWeekend/res/teapot.obj";

    auto material_lambertian = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_metal1 = make_shared<metal>(color(0.3, 0.6, 0.8), 1.0);
    auto material_metal2 = make_shared<metal>(color(0.8, 0.6, 0.2), 0.4);
    auto material_dielectric = make_shared<dielectric>(1.50);

    auto obj1 = make_shared<polygon_mesh>(
	teapot_path,
	material_lambertian,
	world,
	point3(0, -2, 0),
	vec3(0.3, 0.3, 0.3)
    );

    auto obj2 = make_shared<polygon_mesh>(
	teapot_path,
	material_dielectric,
	world,
	point3(-1, -1, 0),
	vec3(0.3, 0.3, 0.3)
    );

    auto obj3 = make_shared<polygon_mesh>(
	bunny_path,
	material_metal2,
	world,
	point3(1, -1, 0),
	vec3(10, 10, 10)
    );

    auto sphere_center1 = point3(-1, 0.5, 0);
    auto sphere_center2 = sphere_center1 + vec3(random_double(0, 0.5), random_double(0, 1), 0);
    world.add(make_shared<sphere>(sphere_center1, sphere_center2, 
	0.2, material_lambertian));

    cam.lookfrom = point3(0, 0, 3);
    cam.lookat = point3(0, 0, 0);

    cam.defocus_angle = 0; // disable DOF
}

int main() {
    // 카메라
    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 2048;
    cam.samples_per_pixel = 100;
    cam.max_depth = 10;

    cam.vfov = 70;
    cam.lookfrom = point3(0, 5, 20); // scene 5
    cam.lookat = point3(0, 4, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 10.0;
    cam.focus_dist = 3;

    // 월드
    hittable_list world; // 모든 hittable한 오브젝트를 저장

    // 불러올 씬
    cornell_box(world, cam);

    // BVH
    world = hittable_list(make_shared<bvh_node>(world));

    cam.render(world); // hittable_list에 있는 모든 물체에 대해 렌더링

    std::clog << "\nRENDER INFO\n";
    std::clog << "Vertices: " << scene_info::vertices << "\n";
    std::clog << "Faces: " << scene_info::faces << "\n";
    std::clog << "Aspect Ratio: " << cam.aspect_ratio << "\n";
    std::clog << "Image Width: " << cam.image_width << "\n";
    std::clog << "Samples Per Pixel: " << cam.samples_per_pixel << "\n";
    std::clog << "Ray Max Depth: " << cam.max_depth << "\n";
}