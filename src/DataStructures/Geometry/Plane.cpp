#include "Plane.h"

using namespace CRAB;

Plane::Plane()
{
}

//Plane::Plane(const CRAB::Vector4Df& _plane_point, const CRAB::Vector4Df& _plane_normal) :
//	plane_point(_plane_point), plane_normal(_plane_normal){
//}

Plane::~Plane()
{
}

bool Plane::Inside(const CRAB::Vector4Df& p) const
{
	return p.y >= 0;
	//return dot(plane_point - p, plane_normal) <= 0.0f;
}

//TODO: Implement it
CRAB::Collision Plane::CollideClosest(register const CRAB::Ray &ray)  {
	CRAB::RayCollisionList rcl = Collide(ray);
	if (rcl.collisions_count) {
		return rcl.collisions[0];
	}
	else {
		return CRAB::Collision{ INFINITY, this };
	}
	//const CRAB::Ray _ray{ transform * ray.origin, transform * ray.direction };
	//
	//const float n_p = dot(_ray.direction, plane_normal);
	//
	//const Vector4Df &t = dot_Vec(plane_point - _ray.origin, plane_normal) / n_p;
	//
	//return CRAB::Collision{ _mm_cvtss_f32(t), this, plane_normal };
	//
	//
}
const CRAB::RayCollisionList rcl{ true, 1, {CRAB::Collision{ 0, nullptr, Vector4Df{0,1,0,0} }} };
const CRAB::RayCollisionList rcl_no_collision{ false, 0};

CRAB::RayCollisionList Plane::Collide(const CRAB::Ray &ray)
{
	const CRAB::Ray _ray{ transform * ray.origin, transform * ray.direction };
	//Colisions
	
	const float inside = -_ray.origin.y;
	const float n_p = _ray.direction.y;

	const float t =  inside / n_p;

	CRAB::RayCollisionList rc = rcl;
	rc.first_state = inside <= 0;
	if (t >= 0 && t < INFINITY) {
		rc.collisions[0].geometry = this;
		rc.collisions[0].distance = t;

		rc.collisions[0].normal = (transform.Inverse() * rc.collisions[0].normal).normalized();
		return rc;
	}
	rc.collisions_count = 0;
	return rc;
}


Vector4Df Plane::getNormal(const Vector4Df &point) const
{
	return (transform.Inverse() * Vector4Df{ 0,1,0,0 });
}

