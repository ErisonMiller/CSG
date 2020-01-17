#pragma once
#include "Geometry.h"
class Sphere : public Geometry
{
	
public:
	
	Sphere();
	~Sphere();

	bool Inside(const CRAB::Vector4Df& p) const;

	CRAB::Collision CollideClosest(const CRAB::Ray &ray);
	CRAB::RayCollisionList Collide(const CRAB::Ray &ray);


	Geometry* Duplicate()  { auto* g = new Sphere(); *g = *this; return g; }

	CRAB::Vector4Df getNormal(const CRAB::Vector4Df &point) const;

};

