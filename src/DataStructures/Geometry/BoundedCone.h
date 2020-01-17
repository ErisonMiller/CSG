#pragma once

//
// Cylinder Class
//

//Project Include Files
#include "Geometry.h"

class BoundedCone : public Geometry
{
	
public:
	
	BoundedCone();
	~BoundedCone();

	bool Inside(const CRAB::Vector4Df& p) const;

	CRAB::Collision CollideClosest(const CRAB::Ray &ray);
	CRAB::RayCollisionList Collide(const CRAB::Ray &ray);//return the closest collision 

	//CRAB::Collision CollideClosest(const CRAB::Ray& ray);
	//CRAB::RayCollisionList Collide(const CRAB::Ray& ray);
	Geometry* Duplicate() { BoundedCone* g = new BoundedCone(); *g = *this; return g; }

	CRAB::Vector4Df getNormal(const CRAB::Vector4Df &point) const; // returns the normal vector at point on a surface
};

