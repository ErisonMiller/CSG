#pragma once
#include "Geometry.h"
class Plane :
	public Geometry
{
	
public:
	//CRAB::Vector4Df plane_point, plane_normal;

	Plane();
	//Plane(const CRAB::Vector4Df &_plane_point, const CRAB::Vector4Df &_plane_normal);
	~Plane();

	bool Inside(const CRAB::Vector4Df& p) const;

	CRAB::Collision CollideClosest(register const CRAB::Ray &ray);
	CRAB::RayCollisionList Collide(const CRAB::Ray &ray);


	Geometry* Duplicate() { auto* g = new Plane(); *g = *this; return g; }

	CRAB::Vector4Df getNormal(const CRAB::Vector4Df &point) const;

};

