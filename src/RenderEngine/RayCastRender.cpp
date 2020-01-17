#include "RayCastRender.h"


#include <iostream>

#include <time.h>


using namespace CRAB;

//RayCast::RayCast(){}

RayCast::RayCast(const CRAB::Camera &cam) : resolution(cam.resolution){

	accumulateBuffer = new(nothrow) CRAB::Vector4Df[resolution.x * resolution.y * sizeof(CRAB::Vector4Df)];

	for (unsigned y = 0, s = (unsigned)(resolution.x * resolution.y); y < s; y ++) {
		accumulateBuffer[y] = CRAB::Vector4Df{ 0.5f, 0.5f, 0.5f, 0.5f };
	}
}

RayCast::~RayCast() {
	delete[]accumulateBuffer;
}

inline Vector4Df ray_cast3D(register const Ray &ray, const std::vector<Geometry*> &objects, const std::vector<Light *> &lights, bool print, const CRAB::Camera &cam)
{
	Collision closest_collision{ INFINITY, nullptr };
	
	for (Geometry* obj : objects) {
		const CRAB::Collision col = obj->CollideClosest(ray);//Collision
		const float o_dist = col.distance;
		if (o_dist < closest_collision.distance && o_dist > cam.n) {
			closest_collision = col;
		}
	}

	Vector4Df accucolor = Vector4Df{ 1.0f, 0.62f, 0.7f, 0.0f };
	if (closest_collision.geometry) {
		accucolor = Vector4Df{ 0.0f, 0.0f, 0.0f, 0.0f };
		//accucolor = closest_collision.geometry->getNormal(closest_collision.pint);
		for (Light * light : lights)
		{
			accucolor += light->Illumination(closest_collision.geometry->material, closest_collision.normal.normalized(), ray.direction * (-1.0f), ray.origin + ray.direction * closest_collision.distance);
		}
	}
	return accucolor;
}

//will be removed after
Geometry* RayCast::RayPick(const CRAB::Camera &cam, std::vector<Geometry *> &objects, int x, int y) const  {
	const Vector4Df base = (cam.view - cam.position).normalized();
	const Vector4Df up = cam.up * (cam.dimensions.y / cam.resolution.y);
	const Vector4Df left = cross(cam.up, base) * (cam.dimensions.x / cam.resolution.x);
	
	const int width = (int)cam.resolution.x;
	const int height = (int)cam.resolution.y;
	
	bool print = false;
	
	const Vector4Df posi_pix_0_0 = base * cam.n + up * (height*(0.5f) - 0.5f) + left * (width*(0.5f) - 0.5f);
	Vector4Df direction = posi_pix_0_0 + up * (-y)+left * (-x);
	direction.normalize();
	
	std::cout << "*** Testando *** x:" << x << " y:" << y << "\n";
	
	Ray ray{ cam.position, direction };
	
	Geometry *colidiu = nullptr;
	CRAB::Collision *col = nullptr;
	float dist = INFINITY;
	
	for (Geometry *obj : objects) {
		col = &obj->CollideClosest(ray);//Collision
		const float o_dist = col->distance;
		if (o_dist < dist && o_dist > cam.n) {
			dist = o_dist;
			colidiu = col->geometry;
		}
		//RayCollisionList cols = obj.CollideAll(ray);
		
	}
	
	
	return colidiu;

}

CRAB::Vector4Df* RayCast::Render(const CRAB::Camera &cam, const std::vector<Geometry*> &objects, std::vector<Light*> &lights) const  {

	clock_t t;
	t = clock();

	const Vector4Df base = (cam.view - cam.position).normalized();
	const Vector4Df up = cam.up * (cam.dimensions.y / cam.resolution.y);
	const Vector4Df left = cross(cam.up, base) * (cam.dimensions.x / cam.resolution.x);


	//Vector4Df *pix = accumulatebuffer;
	const int width = (int)cam.resolution.x;
	const int height = (int)cam.resolution.y;

	bool print = false;
	
	const Vector4Df posi_pix_0_0 = base * cam.n + up * (height*(-0.5f) + 0.5f) + left * (width*(0.5f) - 0.5f);

	#pragma omp parallel for num_threads(16) schedule(dynamic)
	for (int y = 0; y < height; y+=2) {
		for (int x = 0; x < width; x+=2) {
			Vector4Df direction = posi_pix_0_0 + up * (y) + left * (-x);
			direction.normalize();
			
			accumulateBuffer[y*width + x] = ray_cast3D(Ray { cam.position, direction }, objects, lights, print, cam);
			accumulateBuffer[y * width + x + 1] = accumulateBuffer[y * width + x];
			accumulateBuffer[(y+1) * width + x] = accumulateBuffer[y * width + x];
			accumulateBuffer[(y+1) * width + x + 1] = accumulateBuffer[y * width + x];

		}
	}
	
	t = clock() - t;	
	//std::cout << "levou " << t << " clocks ou " << ((float)t) / CLOCKS_PER_SEC << " segundos ou " << 1.0f/(((float)t) / CLOCKS_PER_SEC) << " fps\n";
	
	return accumulateBuffer;
}


CRAB::Vector4Df* RayCast::Area_Volume(const CRAB::Camera& cam, const std::vector<Geometry*>& objects) const  {

	const Vector4Df base = (cam.view - cam.position).normalized();
	const Vector4Df up = cam.up * (cam.dimensions.y / cam.resolution.y);
	const Vector4Df left = cross(cam.up, base) * (cam.dimensions.x / cam.resolution.x);


	//Vector4Df *pix = accumulatebuffer;
	const int width = (int)cam.resolution.x;
	const int height = (int)cam.resolution.y;

	bool print = false;

	const Vector4Df posi_pix_0_0 = base * cam.n + up * (height * (-0.5f) + 0.5f) + left * (width * (0.5f) - 0.5f);
	
	float area_pix = (cam.dimensions.y / cam.resolution.y) * (cam.dimensions.x / cam.resolution.x);
	float area = 0;
	float vol = 0;
//#pragma omp parallel for num_threads(16) schedule(guided)
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			Vector4Df posi = posi_pix_0_0 + up * (y)+left * (-x) + cam.position;
			
			for (Geometry* obj : objects) {
				const CRAB::RayCollisionList col = obj->Collide(CRAB::Ray{posi,base});//Collision
				int i = 0, s = col.collisions_count;
				float d = 0;
				if (s) {
					if (!col.first_state) {
						i = 1;
						d = col.collisions[0].distance;
						area += area_pix;
					}
					while (i < s) {
						area += area_pix;
						vol += area_pix * (col.collisions[i].distance - d);
						i ++;
						if (i < s) {
							area += area_pix;
							d = col.collisions[i].distance;
						}
						i++;
					}
				}
			}

		}
	}

	std::cout << "area: " << area << ";   volume: " << vol << ";\n";

	return accumulateBuffer;
}

CRAB::Vector4Df* RayCast::Render2D(const CRAB::Camera& cam, const std::vector<Geometry*>& objects) const {

	clock_t t;
	t = clock();
	
	const float size = (cam.view - cam.position).length();
	
	//Vector4Df *pix = accumulatebuffer;
	const int width = (int)cam.resolution.x;
	const int height = (int)cam.resolution.y;

	bool print = false;

	const Vector4Df posi_pix_0_0 = Vector4Df{ -size * 0.5f, -size * 0.5f, 0, 1 };
	const Vector4Df up   = Vector4Df{ 0,1,0,0 } *( size / cam.resolution.y);

	float dx = (size / cam.resolution.x);

	Vector4Df direction{ 1,0,0,0 };
//#pragma omp parallel for num_threads(16) schedule(dynamic)
	for (int y = 0; y < height; y ++) {
		Vector4Df posi = posi_pix_0_0 + up * (y);

		for (int x = 0; x < width; x++) {
			accumulateBuffer[y * width + x] = Vector4Df{0.1,0.1,1,0};
		}

		for (Geometry* obj : objects) {
			const CRAB::RayCollisionList col = obj->Collide(Ray{posi, direction});//Collision
			bool state = col.first_state;
			int i = 0;
			while (i < col.collisions_count && col.collisions[i].distance < 0)
			{
				i++;
				state = !state;
			}
			for (int x = 0; x < width; x++) {
				if (i < col.collisions_count && col.collisions[i].distance < x * dx) {
					i++;
					state = !state;
				}
				if (state) {
					accumulateBuffer[y * width + x] = col.collisions[i].geometry->material.kd;
				}
			}
		}

	}

	t = clock() - t;
	//std::cout << "levou " << t << " clocks ou " << ((float)t) / CLOCKS_PER_SEC << " segundos ou " << 1.0f / (((float)t) / CLOCKS_PER_SEC) << " fps\n";

	return accumulateBuffer;
}
