//TODO: - Collision:
//			- Handle a == 0;
//			- Handle Base Intersection
#include "Cylinder.h"

Cylinder::Cylinder()
{

}

Cylinder::~Cylinder()
{

}



using namespace CRAB;

bool Cylinder::Inside(const CRAB::Vector4Df& p) const
{
	return (p.x * p.x + p.z * p.z) <= 1.0f;
}

//TODO: Implement it
CRAB::Collision Cylinder::CollideClosest(const CRAB::Ray &ray)  {
	const CRAB::Ray &_ray = RayTransform(ray, transform);

	CRAB::Collision col{ INFINITY, this };


	//Aux Variables	
	const CRAB::Vector4Df& v = Vector4Df{ _ray.origin.x, 0, _ray.origin.z, 0 };
	const CRAB::Vector4Df &w = Vector4Df{ _ray.direction.x, 0, _ray.direction.z, 0 };

	const float r_2 = 1.0f;
	//Equantion Constants
	const float a = dot(w, w);
	const float c = dot(v, v) - r_2;
	const float b = dot(v, w);
	//Delta
	const float delta = (b*b) - (a*c);
	if (a != 0.0f && delta >= 0.0f) {
		const float distance = (sqrtf(delta) + b) / (-a);
		col.distance = distance;
		const CRAB::Vector4Df& p = _ray.origin + (_ray.direction * distance); // Intersection Point
		col.normal = getNormal(p);
	}

	return col;
}

CRAB::RayCollisionList Cylinder::Collide(const CRAB::Ray &ray) 
{
	const CRAB::Ray& _ray = RayTransform(ray, transform);

	//Colisions
	CRAB::RayCollisionList col{ Inside(_ray.origin), 0 };
	CRAB::Collision t{ INFINITY, this };

	//Aux Variables	
	const CRAB::Vector4Df& v = Vector4Df{ _ray.origin.x, 0, _ray.origin.z, 0 };
	const CRAB::Vector4Df& w = Vector4Df{ _ray.direction.x, 0, _ray.direction.z, 0 };

	const float r_2 = 1.0f;
	//Equantion Constants
	const float a = dot(w, w);
	const float c = dot(v, v) - r_2;
	const float b = dot(v, w);
	//Delta
	float delta = (b * b) - (a * c);

	if (a != 0){
		if (delta == 0) { // One intersection
			t.distance = (-b / (a));
			if(t.distance >= 0){
			CRAB::Vector4Df p = _ray.origin + (_ray.direction * t.distance); // Intersection Point
			t.normal = getNormal(p); 
			col.collisions[0] = (t);
			col.collisions_count++;
			}
		}
		else if (delta > 0) {
			//First Point
			delta = sqrtf(delta);
			t.distance = (delta + b) / (-a);
			CRAB::Vector4Df p;
			if (t.distance >= 0) {
				p = _ray.origin + (_ray.direction * t.distance); // Intersection Point
				t.normal = getNormal(p);
				col.collisions[0] = (t);
				col.collisions_count++;
			}
			//Second Point
			t.distance = (delta - b) / (a);
			if (t.distance >= 0) {
				p = _ray.origin + (_ray.direction * t.distance); // Intersection Point
				t.normal = getNormal(p);
				col.collisions[col.collisions_count] = (t);
				col.collisions_count++;
			}
		}
	}

	return col;

}

CRAB::Vector4Df Cylinder::getNormal(const CRAB::Vector4Df &point) const
{
	CRAB::Vector4Df n = Vector4Df{ point.x,0,point.z,0 };
	return (transform.Inverse() * n).normalized();
}
