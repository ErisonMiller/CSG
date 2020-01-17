#include "Object.h"


Object::Object()
{
}
Object::Object(Geometry *_geometry) : geometry(_geometry)
{
	name = "untitled"; // default name
	material = new Material(CRAB::Vector4Df{ 1.0f, 1.0f, 1.0f, 0 }, CRAB::Vector4Df{ 0.0f, 0.0f, 0.0f, 0 }, CRAB::Vector4Df{ 0.0f, 0.0f, 0.0f, 0 }, 100.0f); // default color
}

Object::Object(const string &_name, Material * _material, Geometry *_geometry) : name(_name), geometry(_geometry), material(_material), visible(true)
{
}

Object::~Object()
{
}

CRAB::Collision Object::Collide(register const CRAB::Ray &ray) const {
	//CRAB::RayCollisionList Coll = geometry->Collide(ray);
	//if (Coll.collisions.size() == 0)return INFINITY;
	//return Coll.collisions[0].distance;

	return geometry->CollideClosest(CRAB::Ray{ ray.origin, ray.direction });
	//return geometry->CollideClosest(ray);
}

CRAB::RayCollisionList Object::CollideAll(const CRAB::Ray &ray) const {
	return geometry->Collide(ray);
}

string Object::getName() const {
	return name;
}

Geometry* Object::getGeometry() const {
	return geometry;
}

Material* Object::getMaterial() const {
	return material;
}

CRAB::Vector4Df Object::getNormalVector(const CRAB::Vector4Df &point) const {
	return geometry->getNormal(point);
}