//TODO: - Collision:
//			- Handle a == 0;
//			- Handle Base Intersection

#include "Cone.h"

Cone::Cone()
{

}

Cone::~Cone()
{

}

bool Cone::Inside(const CRAB::Vector4Df& p) const
{
	return (p.x * p.x + p.z * p.z) <= (1.0f - p.y) * (1.0f - p.y); //1.0f - 0.75f * p.y;
}


//TODO: Implement it
CRAB::Collision Cone::CollideClosest(const CRAB::Ray &ray) 
{
	const CRAB::Ray _ray{ transform * ray.origin, transform * ray.direction };

	CRAB::Collision col{INFINITY, this};


	//Aux Variables
	const float cos_alfa_2 = 0.5f;

	float d_dot_n = _ray.direction.y;
	float d_dot_d = dot(_ray.direction, _ray.direction);

	CRAB::Vector4Df v = CRAB::Vector4Df{0,1,0,1} - _ray.origin; // Vector between the Cone top vertex and the _ray origin.

	float v_dot_n = v.y;

	//Equantion Constants
	float a = (d_dot_n * d_dot_n) - (d_dot_d * cos_alfa_2);
	float b = 2 * ((dot(v, _ray.direction) * cos_alfa_2) - (v_dot_n * d_dot_n));
	float c = (v_dot_n * v_dot_n) - (dot(v, v) * cos_alfa_2);

	//Delta
	float delta = (b * b) - (4.0f * a * c);

	if (a != 0.0f && delta >= 0.0f) {
		const float distance = (sqrtf(delta) - b) / (2.0f * a);
		const CRAB::Vector4Df &p = _ray.origin + (_ray.direction * distance); // Intersection Point
		col.normal = getNormal(p);
		col.distance = distance;
		return col;
	}

	return col;
}

CRAB::RayCollisionList Cone::Collide(const CRAB::Ray &ray)
{
	const CRAB::Ray _ray{ transform * ray.origin, transform * ray.direction };

	//Colisions
	CRAB::RayCollisionList col{ Inside(_ray.origin), 0 };
	CRAB::Collision t{ INFINITY, this };

	//Aux Variables
	const float cos_alfa_2 = 0.5f;

	float d_dot_n = _ray.direction.y;
	float d_dot_d = dot(_ray.direction, _ray.direction);
	CRAB::Vector4Df v = CRAB::Vector4Df{ 0,1,0,1 } -_ray.origin; // Vector between the Cone top vertex and the _ray origin.

	float v_dot_n = v.y;

	//Equantion Constants
	float a = (d_dot_n * d_dot_n) - (d_dot_d * cos_alfa_2);
	float b = 2 * ((dot(v, _ray.direction) * cos_alfa_2) - (v_dot_n * d_dot_n));
	float c = (v_dot_n * v_dot_n) - (dot(v, v) * cos_alfa_2);

	//Delta
	float delta = (b * b) - (4.0f * a * c);


	if (a != 0) {

		if (delta == 0) { // One intersection
			t.distance = (-b / (2.0f * a));
			if (t.distance >= 0) {
				CRAB::Vector4Df p = _ray.origin + (_ray.direction * t.distance); // Intersection Point

				t.normal = getNormal(p);
				col.collisions[0] = (t);
				col.collisions_count++;
			}
			return col;
		}
		else if (delta > 0) {
			delta = sqrtf(delta);
			//First Point
			t.distance = (-b + delta) / (2.0f * a);
			CRAB::Vector4Df p;
			if (t.distance >= 0) {
				p = _ray.origin + (_ray.direction * t.distance); // Intersection Point
				t.normal = getNormal(p);
				col.collisions[0] = (t);
				col.collisions_count++;
			}
			//Second Point
			t.distance = (-b - delta) / (2 * a);
			if (t.distance >= 0) {
				p = _ray.origin + (_ray.direction * t.distance); // Intersection Point
				t.normal = getNormal(p);
				col.collisions[col.collisions_count] = (t);
				col.collisions_count++;
			}
		}

		//col.collisions.size() > 1 && 
		if (col.collisions_count > 1 && col.collisions[0].distance > col.collisions[1].distance) {
			std::swap(col.collisions[0], col.collisions[1]);
		}
	}
	
	return col;
}

CRAB::Vector4Df Cone::getNormal(const CRAB::Vector4Df &point) const
{

	CRAB::Vector4Df vg = CRAB::Vector4Df{ 0,1,0,1 } - point;
	CRAB::Vector4Df vt = CRAB::cross(vg, CRAB::Vector4Df{0,1,0,0});

	return (transform.Inverse() * cross(vt, vg)).normalized();
}
