#define _CRT_SECURE_NO_WARNINGS
#include "RTWeekend.h"
#include "TransformationMatrix.h"
#include "SceneInfo.h"
#include "Interval.h"
#include "AABB.h"
#include "Hittable.h"
#include "HittableList.h"
#include "BVH.h"
#include "Sphere.h"
#include "Triangle.h"
#include "PolygonMesh.h"
#include "Quad.h"
#include "Transform.h"
#include "ImageOpener.h"
#include "Camera.h"
#include "Material.h"
#include "Texture.h"
#include "ConstantMedium.h"

void CornellBox(HittableList& world, Camera& cam) {
    auto matRed = make_shared<Lambertian>(Color(1.0, 0.0, 0.0));
    auto matGreen = make_shared<Lambertian>(Color(0.0, 1.0, 0.0));
    auto matBlue = make_shared<Lambertian>(Color(0.0, 0.0, 1.0));
    auto matWhite = make_shared<Lambertian>(Color(1.0, 1.0, 1.0));
    auto matLight = make_shared<DiffuseLight>(Color(20, 20, 20));

    auto left = make_shared<Transform>(
	make_shared<Quad>(
	    Point3(-2, -2, 2),
	    Vec3(0, 0, -4),
	    Vec3(0, 4, 0),
	    matRed
	)
    );
    world.Add(left);

    auto right = make_shared<Transform>(
	make_shared<Quad>(
	    Point3(2, -2, 2),
	    Vec3(0, 0, -4),
	    Vec3(0, 4, 0),
	    matGreen
	)
    );
    world.Add(right);

    auto floor = make_shared<Transform>(
	make_shared<Quad>(
	    Point3(-2, -2, 2),
	    Vec3(4, 0, 0),
	    Vec3(0, 0, -4),
	    matWhite
	)
    );
    world.Add(floor);

    auto ceil = make_shared<Transform>(
	make_shared<Quad>(
	    Point3(-2, 2, 2),
	    Vec3(4, 0, 0),
	    Vec3(0, 0, -4),
	    matWhite
	)
    );
    world.Add(ceil);

    auto back = make_shared<Transform>(
	make_shared<Quad>(
	    Point3(-2, -2, -2),
	    Vec3(4, 0, 0),
	    Vec3(0, 4, 0),
	    matWhite
	)
    );
    world.Add(back);

    auto emit = make_shared<Transform>(
	make_shared<Quad>(
	    Point3(-0.5, 1.99, -.25),
	    Vec3(1.0, 0, 0),
	    Vec3(0, 0, -1.0),
	    matLight
	),
	Point3(0, 0, 1)
    );
    world.Add(emit);

    cam.lookfrom = Point3(0, 0, 5);
    cam.lookat = Point3(0, 0, 0);

    cam.defocusAngle = 0; // disable DOF
}

//void scene1(HittableList& world, Camera& cam) {
//    // 물체에 사용할 머티리얼
//    auto material_ground = make_shared<Lambertian>(Color(0.8, 0.8, 0.0));
//    auto material_center = make_shared<Lambertian>(Color(0.1, 0.2, 0.5));
//    auto material_left = make_shared<Dielectric>(1.50);
//    auto material_bubble = make_shared<Dielectric>(1.00 / 1.50);
//    auto material_right = make_shared<Metal>(Color(0.8, 0.6, 0.2), 0.2);
//
//    world.Add(make_shared<Sphere>(Point3(0.0, -100.5, -1.0), 100.0, material_ground));
//    world.Add(make_shared<Sphere>(Point3(0.0, 0.0, 0.8), 0.5, material_center));
//    world.Add(make_shared<Sphere>(Point3(-1.0, 0.0, 0.8), 0.3, material_left));
//    world.Add(make_shared<Sphere>(Point3(-1.0, 0.0, 0.8), 0.5, material_bubble));
//    world.Add(make_shared<Sphere>(Point3(1.0, 0.0, 0.8), 0.5, material_right));
//
//    cam.background = Color(0.70, 0.80, 1.00);
//}
//// 카메라 테스트 1
//void scene2(HittableList& world, Camera& cam) {
//    auto R = std::cos(pi / 4);
//    
//    auto material_left = make_shared<Lambertian>(Color(0, 0, 1));
//    auto material_right = make_shared<Lambertian>(Color(1, 0, 0));
//
//    world.Add(make_shared<Sphere>(Point3(-R, 0, -1), R, material_left));
//    world.Add(make_shared<Sphere>(Point3(R, 0, -1), R, material_right));
//
//    cam.background = Color(0.70, 0.80, 1.00);
//}
//
//// 삼각형 테스트
//void scene3(HittableList& world, Camera& cam) {
//    auto material_center = make_shared<Lambertian>(Color(0.1, 0.2, 0.5));
//    auto material_left = make_shared<Metal>(Color(0.3, 0.6, 0.8), 1.0);
//    auto material_right = make_shared<Metal>(Color(0.8, 0.6, 0.2), 0.1);
//    
//    world.Add(make_shared<Triangle>(
//	Point3(-0.5, 0.1, 1.0),
//	Point3(0.5, 0.1, 1.0),
//	Point3(0.0, 0.85, 1.0),
//	material_center
//    ));
//
//    world.Add(make_shared<Triangle>(
//	Point3(-1.5, 0.1, 1.1),
//	Point3(-0.5, 0.1, 1.0),
//	Point3(-1.0, 0.85, 1.0),
//	material_left
//    ));
//
//    world.Add(make_shared<Triangle>(
//	Point3(0.5, 0.1, 1.0),
//	Point3(1.5, 0.1, 1.1),
//	Point3(1.0, 0.85, 1.0),
//	material_right
//    ));
//
//    cam.lookfrom = Point3(0, 0, 2); // scene 3
//    cam.lookat = Point3(0, 0, 0);
//    cam.background = Color(0.70, 0.80, 1.00);
//}
//
//// 폴리곤 메시 테스트
//void scene4(HittableList& world, Camera& cam) {
//    auto material_ground = make_shared<Lambertian>(Color(0.8, 0.8, 0.0));
//    auto materialLambertian = make_shared<Lambertian>(Color(0.1, 0.2, 0.5));
//    auto materialMetal1 = make_shared<Metal>(Color(0.3, 0.6, 0.8), 1.0);
//    auto materialMetal2 = make_shared<Metal>(Color(0.8, 0.6, 0.2), 0.4);
//    auto materialDielectric = make_shared<Dielectric>(1.50);
//
//    world.Add(make_shared<Sphere>(Point3(0.0, -100.5, -1.0), 100.0, material_ground));
//
//    std::string teapotPath = "../res/teapot.obj";
//    auto obj1 = make_shared<PolygonMesh>(
//	teapotPath,
//	materialLambertian,
//	world,
//	Point3(0, 0, 0),
//	Vec3(1, 1, 1)
//    );
//    world.Add(obj1);
//
//    auto obj2 = make_shared<PolygonMesh>(
//	teapotPath,
//	materialDielectric,
//	world,
//	Point3(-6, 0, 0),
//	Vec3(1, 1, 1)
//    );
//    world.Add(obj2);
//
//    std::string bunnyPath = "../res/stanford-bunny.obj";
//    auto obj3 = make_shared<PolygonMesh>(
//	bunnyPath,
//	materialMetal2,
//	world,
//	Point3(6, 0, 1),
//	Vec3(20, 20, 20)
//    );
//    world.Add(obj3);
//
//    cam.lookfrom = Point3(0, 4, 6);
//    cam.lookat = Point3(0, 0, 0);
//    cam.background = Color(0.70, 0.80, 1.00);
//
//    cam.defocusAngle = 0; // disable DOF
//}
//
//// Triangle 개수에 따른 렌더 시간 테스트
//void scene5(HittableList& world, Camera& cam) {
//    auto material_ground = make_shared<Lambertian>(Color(0.8, 0.8, 0.0));
//    auto materialLambertian = make_shared<Lambertian>(Color(0.1, 0.2, 0.5));
//    auto material_metal = make_shared<Metal>(Color(0.8, 0.6, 0.2), 0.1);
//
//    std::string bunnyPath = "../res/stanford-bunny.obj";
//    std::string bunny_path_08 = "../res/stanford-bunny-08.obj";
//    std::string bunny_path_06 = "../res/stanford-bunny-06.obj";
//    std::string bunny_path_04 = "../res/stanford-bunny-04.obj";
//    std::string bunny_path_02 = "../res/stanford-bunny-02.obj";
//    std::string bunny_path_01 = "../res/stanford-bunny-01.obj";
//
//    //world.Add(make_shared<Sphere>(Point3(0.0, -100.5, -1.0), 100.0, material_ground));
//    auto bunny_test = make_shared<PolygonMesh>(bunnyPath, material_metal);
//
//    cam.lookfrom = Point3(0, 5, 3); // scene 5
//    cam.lookat = Point3(0, 4, 0);
//    cam.background = Color(0.70, 0.80, 1.00);
//}
//
//void scene6(HittableList& world, Camera& cam) {
//    auto materialLambertian = make_shared<Lambertian>(Color(0.1, 0.2, 0.5));
//    auto material_metal = make_shared<Metal>(Color(0.8, 0.6, 0.2), 0.1);
//    std::string vase_path = "../res/vase.obj";
//    auto vase = make_shared<PolygonMesh>(
//	vase_path,
//	material_metal,
//	world,
//	Point3(0, 4, 0),
//	Vec3(0.1, 0.1, 0.1)
//    );
//    cam.background = Color(0.70, 0.80, 1.00);
//}
//
//// Quads
//void scene7(HittableList& world, Camera& cam) {
//    // Materials
//    auto left_red = make_shared<Lambertian>(Color(1.0, 0.2, 0.2));
//    auto back_green = make_shared<Lambertian>(Color(0.2, 1.0, 0.2));
//    auto right_blue = make_shared<Lambertian>(Color(0.2, 0.2, 1.0));
//    auto upper_orange = make_shared<Lambertian>(Color(1.0, 0.5, 0.0));
//    auto lower_teal = make_shared<Lambertian>(Color(0.2, 0.8, 0.8));
//
//    // Quads
//    world.Add(make_shared<Quad>(Point3(-3, -2, 5), Vec3(0, 0, -4), Vec3(0, 4, 0), left_red));
//    world.Add(make_shared<Quad>(Point3(-2, -2, 0), Vec3(4, 0, 0), Vec3(0, 4, 0), back_green));
//    world.Add(make_shared<Quad>(Point3(3, -2, 1), Vec3(0, 0, 4), Vec3(0, 4, 0), right_blue));
//    world.Add(make_shared<Quad>(Point3(-2, 3, 1), Vec3(4, 0, 0), Vec3(0, 0, 4), upper_orange));
//    world.Add(make_shared<Quad>(Point3(-2, -3, 5), Vec3(4, 0, 0), Vec3(0, 0, -4), lower_teal));
//
//    cam.vfov = 80;
//    cam.lookfrom = Point3(0, 0, 9);
//    cam.lookat = Point3(0, 0, 0);
//    cam.background = Color(0.70, 0.80, 1.00);
//
//    cam.defocusAngle = 0;
//}

void Scene8(HittableList& world, Camera& cam) {
    CornellBox(world, cam);

    std::string bunnyPath = "../res/stanford-bunny.obj";
    std::string teapotPath = "../res/teapot.obj";

    auto materialLambertian = make_shared<Lambertian>(Color(0.1, 0.2, 0.5));
    auto materialMetal1 = make_shared<Metal>(Color(0.3, 0.6, 0.8), 1.0);
    auto materialMetal2 = make_shared<Metal>(Color(0.8, 0.6, 0.2), 0.4);
    auto materialDielectric = make_shared<Dielectric>(1.50);

    auto obj1 = make_shared<Transform>(
	    make_shared<PolygonMesh>(teapotPath, materialLambertian),
	    Point3(0, -2, 0),
	    Vec3(0, 0, 0),
	    Vec3(0.3, 0.3, 0.3)
    );
    world.Add(obj1);

    auto obj2 = make_shared<Transform>(
	    make_shared<PolygonMesh>(teapotPath, materialDielectric),
	    Point3(-1, -1, 0),
	    Vec3(0, 0, 0),
	    Vec3(0.3, 0.3, 0.3)
    );
    world.Add(obj2);

    auto obj3 = make_shared<Transform>(
	    make_shared<PolygonMesh>(bunnyPath, materialMetal2),
	    Point3(1, -1, 0),
	    Vec3(0, 0, 0),
	    Vec3(10, 10, 10)
    );
    world.Add(obj3);

    auto sphereCenter1 = Point3(-1, 0.5, 0);
    auto sphereCenter2 = sphereCenter1 + Vec3(GetRandomDouble(0, 0.5), GetRandomDouble(0, 1), 0);
    auto movingSphere = make_shared<Transform>(
	    make_shared<Sphere>(sphereCenter1, sphereCenter2, 0.2, materialLambertian)
    );
    world.Add(movingSphere);

    auto earthTexture = make_shared<ImageTexture>("earthmap.jpg");
    auto earthMaterial = make_shared<Lambertian>(earthTexture);
    auto earthSphere = make_shared<Transform>(
	make_shared<Sphere>(0.4, earthMaterial),
	    Point3(0.6, -0.5, 0.7)
    );
    world.Add(earthSphere);

    cam.vfov = 50;
}

void Scene9(HittableList& world, Camera& cam) {
    CornellBox(world, cam);

    std::string bunnyPath = "../res/stanford-bunny.obj";
    std::string teapotPath = "../res/teapot.obj";

    auto matWhite = make_shared<Lambertian>(Color(1.0, 1.0, 1.0));
    auto materialMetal1 = make_shared<Metal>(Color(0.3, 0.6, 0.8), 1.0);
    auto materialMetal2 = make_shared<Metal>(Color(0.8, 0.6, 0.2), 0.4);
    auto materialDielectric = make_shared<Dielectric>(1.50);

    auto box1 = make_shared<Transform>(
	    MakeBox(Point3(-0.9, 0.7, -2), Point3(0, -2, -1), matWhite),
	    Point3(0, 0, 0)
    );
    world.Add(box1);

    auto box2 = make_shared<Transform>(
	    MakeBox(Point3(-0.1, -2, 0), Point3(0.9, -1, 1), matWhite),
	    Point3(0, 0, 0),
	    Vec3(0, 30, 0),
	    Vec3(1, 1, 1)
    );
    world.Add(box2);

    auto box3 = make_shared<Transform>(
	    make_shared<ConstantMedium>(
	        MakeBox(Point3(-0.1, -2, 0), Point3(0.9, -1, 1), matWhite),
	        0.1,
	        Color(0, 0, 0)
	    ),
	    Point3(0, 0, 0),
	    Vec3(0, 0, 0),
	    Vec3(1, 1, 1)
    );
    world.Add(box3);

    auto sphere1 = make_shared<Transform>(
	    make_shared<Sphere>(Point3(0, 0, 0), 0.5, materialDielectric),
	    Point3(-1, -1.5, 1)
    );
    world.Add(sphere1);

    auto obj3 = make_shared<Transform>(
	    make_shared<PolygonMesh>(bunnyPath, materialMetal2),
	    Point3(0.7, -1.2, 0),
	    Vec3(0, -15, 0),
	    Vec3(4, 4, 4)
    );
    world.Add(obj3);
}

void Microstructure(HittableList& world, Camera& cam) {
    cam.background = Color(0.16, 0.21, 0.66);
    cam.vfov = 40;
    cam.lookfrom = Point3(-2, 2, 2);

    std::string tile111Path = "../res/microstructure/Tile1x1x1.obj";
    std::string tile222Path = "../res/microstructure/Tile2x2x2.obj";
    std::string tile444Path = "../res/microstructure/Tile4x4x4.obj";

    auto matRed = make_shared<Lambertian>(Color(1.0, 0.0, 0.0));
    auto materialMetal1 = make_shared<Metal>(Color(0.3, 0.6, 0.8), 1.0);
    auto materialMetal2 = make_shared<Metal>(Color(0.8, 0.6, 0.2), 0.4);
    auto materialDielectric = make_shared<Dielectric>(1.50);
    //auto mat_light = make_shared<DiffuseLight>(Color(20, 20, 20));

    auto tile111 = make_shared<Transform>(
        make_shared<PolygonMesh>(tile111Path, materialMetal1),
        Point3(-1, 0, 0)
    );
    //world.Add(tile111);

    auto tile222 = make_shared<Transform>(
        make_shared<PolygonMesh>(tile222Path, materialMetal1),
        Point3(-1, 0, 0)
    );
    //world.Add(tile222);

    auto tile444 = make_shared<Transform>(
        make_shared<PolygonMesh>(tile444Path, materialMetal1),
        Point3(-1, 0, 0)
    );
    world.Add(tile444);

    //auto emit = make_shared<Transform>(
    //    make_shared<Quad>(
    //        Point3(-0.5, 1.99, -.25),
    //        Vec3(1.0, 0, 0),
    //        Vec3(0, 0, -1.0),
    //        mat_light
    //    ),
    //    Point3(-1, 2, 0)
    //);
    //world.Add(emit);
}

int main() {
    // 카메라
    Camera cam;
    cam.aspectRatio = 1.0;
    cam.imageWidth = 2048;
    cam.samplesPerPixel = 10;
    cam.maxDepth = 10;
    cam.background = Color(0, 0, 0);

    cam.vfov = 70;
    cam.lookfrom = Point3(0, 0, 5);
    cam.lookat = Point3(0, 0, 0);
    cam.vup = Vec3(0, 1, 0);

    cam.defocusAngle = 0.0;
    cam.focusDist = 3;

    // 월드
    HittableList world; // 모든 hittable한 오브젝트를 저장

    // 불러올 씬
    Microstructure(world, cam);

    // 월드 공간 BVH
    world = HittableList(make_shared<BVHNode>(world));

    cam.Render(world); // hittable_list에 있는 모든 물체에 대해 렌더링

    std::clog << "\nRENDER INFO\n";
    std::clog << "Vertices: " << SceneInfo::vertices << "\n";
    std::clog << "Faces: " << SceneInfo::faces << "\n";
    std::clog << "Aspect Ratio: " << cam.aspectRatio << "\n";
    std::clog << "Image Width: " << cam.imageWidth << "\n";
    std::clog << "Samples Per Pixel: " << cam.samplesPerPixel << "\n";
    std::clog << "Ray Max Depth: " << cam.maxDepth << "\n";
}