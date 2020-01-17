#pragma once

//
// Base class for geometry
//

#include "DataStructures/Ray.h"
#include "DataStructures/Material/Material.h"

class Geometry
{
public:
	CRAB::Matrix4 transform;
	Geometry* father = nullptr;
	char name[20] = "new obj";
	Material material = Material();

	Geometry() : transform(CRAB::Identity()) {}
	virtual ~Geometry() = 0;

	virtual bool Inside(const CRAB::Vector4Df& p) const = 0;
	
	virtual CRAB::Collision CollideClosest(const CRAB::Ray &ray) = 0;//return the closest collision distance of a ray and the geometry
	virtual CRAB::RayCollisionList Collide(const CRAB::Ray &ray) = 0;//return all collisions of a ray

	virtual CRAB::Vector4Df getNormal(const CRAB::Vector4Df &surface_point) const = 0; // returns the normal vector at point on a surface

	void Transform(const CRAB::Matrix4& trasnform_matrix) {
		transform = transform * trasnform_matrix;
	}

	virtual Geometry* Duplicate() = 0;

	void Free() {}
};

inline CRAB::Ray RayTransform(const CRAB::Ray& r, const CRAB::Matrix4& m) {
	return CRAB::Ray{ m*r.origin,m * r.direction };

}

