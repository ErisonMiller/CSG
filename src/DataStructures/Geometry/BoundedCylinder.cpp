//TODO: - Collision:
//			- Handle a == 0;
//			- Handle Base Intersection
#include "BoundedCylinder.h"

BoundedCylinder::BoundedCylinder()
{

}

BoundedCylinder::~BoundedCylinder()
{

}



using namespace CRAB;

bool BoundedCylinder::Inside(const CRAB::Vector4Df& p) const
{
	return p.y >= 0 && p.y <= 1 && (p.x * p.x + p.z * p.z) <= 1.0f;
}

//TODO: Implement it
CRAB::Collision BoundedCylinder::CollideClosest(const CRAB::Ray &ray)  {
	RayCollisionList rcl = Collide(ray);
	if (rcl.collisions_count) {
		return rcl.collisions[0];
	}
	else {
		return CRAB::Collision{ INFINITY, this };
	}

	//const CRAB::Ray &_ray = RayTransform(ray, transform);
	//
	//CRAB::Collision col{ INFINITY, this };
	//
	//
	////Aux Variables	
	//const CRAB::Vector4Df& v = Vector4Df{ _ray.origin.x, 0, _ray.origin.z, 0 };
	//const CRAB::Vector4Df &w = Vector4Df{ _ray.direction.x, 0, _ray.direction.z, 0 };
	//
	//const float r_2 = 1.0f;
	////Equantion Constants
	//const float a = dot(w, w);
	//const float c = dot(v, v) - r_2;
	//const float b = dot(v, w);
	////Delta
	//const float delta = (b*b) - (a*c);
	//if (a != 0.0f && delta >= 0.0f) {
	//	const float distance = (sqrtf(delta) + b) / (-a);
	//	col.distance = distance;
	//	const CRAB::Vector4Df& p = _ray.origin + (_ray.direction * distance); // Intersection Point
	//	col.normal = getNormal(p);
	//}
	//
	//return col;
}

CRAB::RayCollisionList BoundedCylinder::Collide(const CRAB::Ray &ray) 
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
			CRAB::Vector4Df p = _ray.origin + (_ray.direction * t.distance); // Intersection Point
			if(t.distance >= 0 && p.y >= 0 && p.y <= 1){
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
			p = _ray.origin + (_ray.direction * t.distance); // Intersection Point
			if (t.distance >= 0 && p.y >= 0 && p.y <= 1) {
				t.normal = getNormal(p);
				col.collisions[col.collisions_count] = (t);
				col.collisions_count++;
			}
			//Second Point
			t.distance = (delta - b) / (a);
			p = _ray.origin + (_ray.direction * t.distance); // Intersection Point
			if (t.distance >= 0 && p.y >= 0 && p.y <= 1) {
				t.normal = getNormal(p);
				col.collisions[col.collisions_count] = (t);
				col.collisions_count++;
			}
		}
	}

	t.distance = _ray.origin.y / (-_ray.direction.y);
	CRAB::Vector4Df p = _ray.origin + (_ray.direction * t.distance); // Intersection Point
	if (t.distance >= 0 && p.lengthsq() <= 1) {//The point intercept tha base iff its distance from the center is less than the radius.
		t.normal = (transform.Inverse() * CRAB::Vector4Df{ 0,-1,0,0 });
		col.collisions[col.collisions_count] = (t);
		col.collisions_count++;
	}

	//Top
	t.distance = (1 - ray.origin.y) / ray.direction.y;
	p = _ray.origin + (_ray.direction * t.distance); // Intersection Point
	if (t.distance >= 0 && (p - CRAB::Vector4Df{0,1,0,1}).lengthsq() <= 1) {//The point intercept tha base iff its distance from the center is less than the radius.
		t.normal = (transform.Inverse() * CRAB::Vector4Df{ 0,-1,0,0 });
		col.collisions[col.collisions_count] = (t);
		col.collisions_count++;
	}


	if (col.collisions_count > 1 && col.collisions[0].distance > col.collisions[1].distance) {
		std::swap(col.collisions[0], col.collisions[1]);
	}
	if (col.collisions_count > 2 && col.collisions[1].distance > col.collisions[2].distance) {
		std::swap(col.collisions[2], col.collisions[1]);
	}
	if (col.collisions_count > 1 && col.collisions[0].distance > col.collisions[1].distance) {
		std::swap(col.collisions[0], col.collisions[1]);
	}

	return col;

}

CRAB::Vector4Df BoundedCylinder::getNormal(const CRAB::Vector4Df &point) const
{
	CRAB::Vector4Df n = Vector4Df{ point.x,0,point.z,0 };
	return (transform.Inverse() * n).normalized();
}
