#include "Sphere.h"

using namespace CRAB;

Sphere::Sphere()
{

}

Sphere::~Sphere()
{

}

bool Sphere::Inside(const CRAB::Vector4Df& p) const
{
	return p.lengthsq() < 1.0f;
}

//TODO: Get the Intercept Point
CRAB::Collision Sphere::CollideClosest(const CRAB::Ray &ray) 
{
	const CRAB::Ray _ray{ transform * ray.origin, transform * ray.direction };

	CRAB::Collision col;
	col.geometry = this;

	const CRAB::Vector4Df &W = _ray.origin;		// The difference of P0 - C

	// Coefficients of the equation
	//const float A = 1.0f;//considering that the direction is already normalized
	const float A = dot(_ray.direction, _ray.direction);
	const float B = dot(W, _ray.direction);
	const float C = dot(W, W) - 1;
	// Discriminant
	const float Delta = (B * B - A * C);


	if (Delta < 0.0f) { col.distance = INFINITY; return col; }

	//Distance
	col.distance = -(B + sqrtf(Delta)) / A;

	//Intersection Point
	col.normal = _ray.origin + (_ray.direction * col.distance); // Surface 

	return col;
}

//TODO: Implement it
CRAB::RayCollisionList Sphere::Collide(const CRAB::Ray &ray) 
{
	const CRAB::Ray _ray{ transform * ray.origin, transform * ray.direction };
	
	//Colisions
	CRAB::RayCollisionList col{Inside(_ray.origin), 0};
	CRAB::Collision t{INFINITY, this};

	const CRAB::Vector4Df& W = _ray.origin;		// The difference of P0 - C
	// Coefficients of the equation
	const float A = dot(_ray.direction, _ray.direction);
	const float B = dot(W, _ray.direction);
	const float C = dot(W, W) - 1;
	// Discriminant
	const float Delta = (B * B - A * C);

	if (Delta < 0.0f) { return col; }

	if (Delta == 0.0) // One intersection
	{
		t.distance = (-B) / (A);
		if (t.distance >= 0) {
			t.normal = getNormal(_ray.origin + (_ray.direction * t.distance));
			col.collisions[0] = (t);
			col.collisions_count++;
		}
	}
	else if (Delta > 0.0) // Two intersections
	{
		// First point
		t.distance = (-B - sqrtf(Delta)) / (A);
		if (t.distance >= 0) {
			t.normal = getNormal(_ray.origin + (_ray.direction * t.distance));
			col.collisions[0] = (t);
			col.collisions_count++;
		}
		// Second point
		t.distance = (-B + sqrtf(Delta)) / (A);
		if (t.distance >= 0) {
			t.normal = getNormal(_ray.origin + (_ray.direction * t.distance));
			col.collisions[1] = (t);
			col.collisions_count++;
		}
	}

	return col;
}

CRAB::Vector4Df Sphere::getNormal(const CRAB::Vector4Df &point) const
{
	CRAB::Vector4Df n = point - CRAB::Vector4Df{ 0,0,0,1 };
	return (transform.Inverse()*n).normalized();
}
