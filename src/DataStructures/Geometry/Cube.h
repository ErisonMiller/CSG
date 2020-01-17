#pragma once
#include "DataStructures/Geometry/Geometry.h"

class Cube :
	public Geometry
{

public:
	Cube();
	~Cube();

	bool Inside(const CRAB::Vector4Df& p) const;

	CRAB::Collision CollideClosest(const CRAB::Ray& ray);
	CRAB::RayCollisionList Collide(const CRAB::Ray& ray);


	Geometry* Duplicate() { auto* g = new Cube(); *g = *this; return g; }

	CRAB::Vector4Df getNormal(const CRAB::Vector4Df& point) const;
};