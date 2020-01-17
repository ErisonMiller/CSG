#include "Cube.h"

using namespace CRAB;

Cube::Cube()
{
}

Cube::~Cube()
{
}

bool Cube::Inside(const CRAB::Vector4Df& p) const
{
	return p.x >= -0.5 && p.x < 0.5
		&& p.y >= -0.5 && p.y < 0.5
		&& p.z >= -0.5 && p.z < 0.5;
	//return dot(plane_point - p, plane_normal) <= 0.0f;
}


CRAB::Collision Cube::CollideClosest(register const CRAB::Ray &ray) {
	CRAB::RayCollisionList rcl = Collide(ray);
	if (rcl.collisions_count) {
		return rcl.collisions[0];
	}
	else {
		return CRAB::Collision{ INFINITY, this };
	}
}

CRAB::RayCollisionList Cube::Collide(const CRAB::Ray &ray)
{

	const CRAB::Ray _ray{ transform * ray.origin, transform * ray.direction };
	//Colisions
	CRAB::RayCollisionList col{ Inside(_ray.origin), 0 };

	
	const CRAB::Vector4Df v11 = (CRAB::Vector4Df{ -0.5f,-0.5f,-0.5f,1 } - _ray.origin) / _ray.direction;
	const CRAB::Vector4Df v22 = (CRAB::Vector4Df{ 0.5f,0.5f,0.5f,1 } - _ray.origin) / _ray.direction;

	const CRAB::Vector4Df minv = CRAB::min(v11, v22);
	const CRAB::Vector4Df maxv = CRAB::max(v11, v22);
	const float min = fmaxf(minv.x, fmaxf(minv.y, minv.z));
	const float max = fminf(maxv.x, fminf(maxv.y, maxv.z));
	
	if ((min > max) || (max < 0)) {
		return col;
	}
	CRAB::Vector4Df p;
	p = _ray.origin + _ray.direction * min;
	col.collisions[0] = CRAB::Collision{ min,this,getNormal(p) };
	col.collisions_count++;
	p = _ray.origin + _ray.direction * max;
	col.collisions[1] = CRAB::Collision{ max,this,getNormal(p) };
	col.collisions_count++;
	return col;

}

CRAB::Vector4Df Cube::getNormal(const CRAB::Vector4Df& point) const
{
	CRAB::Vector4Df n;
	if (fabs(point.y - 0.5f) < 0.001) {
		n = CRAB::Vector4Df{ 0,1,0,0 };
	}
	else if (fabs(point.y + 0.5f) < 0.001) {
		n = CRAB::Vector4Df{ 0,-1,0,0 };
	}

	else if (fabs(point.x - 0.5f) < 0.001) {
		n = CRAB::Vector4Df{ 1,0,0,0 };
	}
	else if (fabs(point.x + 0.5f) < 0.001) {
		n = CRAB::Vector4Df{ -1,0,0,0 };
	}

	else if (fabs(point.z - 0.5f) < 0.001) {
		n = CRAB::Vector4Df{ 0,0,1,0 };
	}
	else if (fabs(point.z + 0.5f) < 0.001) {
		n = CRAB::Vector4Df{ 0,0,-1,0 };
	}

	return (transform.Inverse() * n).normalized();
}
