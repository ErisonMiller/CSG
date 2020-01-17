#pragma once

//
// Cylinder Class
//

//Project Include Files
#include "Geometry.h"

class Cone : public Geometry
{
	
public:
	
	Cone();
	~Cone();

	bool Inside(const CRAB::Vector4Df& p) const;

	CRAB::Collision CollideClosest(const CRAB::Ray &ray) ;
	CRAB::RayCollisionList Collide(const CRAB::Ray &ray) ;//return the closest collision 


	Geometry* Duplicate() { Cone* g = new Cone(); *g = *this; return g; }

	CRAB::Vector4Df getNormal(const CRAB::Vector4Df &point) const; // returns the normal vector at point on a surface
};

