#pragma once


//Project Include Files
#include "Geometry.h"

//
// Cylinder Class
//
class BoundedCylinder : public Geometry
{
	
public:
	
	BoundedCylinder();
	~BoundedCylinder();

	bool Inside(const CRAB::Vector4Df& p) const;

	CRAB::Collision CollideClosest(const CRAB::Ray &ray) ;
	CRAB::RayCollisionList Collide(const CRAB::Ray &ray) ;


	Geometry* Duplicate() { BoundedCylinder* g = new BoundedCylinder(); *g = *this; return g; }

	CRAB::Vector4Df getNormal(const CRAB::Vector4Df &point) const;

};