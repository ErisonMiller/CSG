#include "PointLight.h"

PointLight::PointLight(CRAB::Vector4Df intensity, CRAB::Vector4Df _position) : Light(intensity), position(_position) {}

PointLight::~PointLight() {}

CRAB::Vector4Df PointLight::Illumination(const Material& mat, const CRAB::Vector4Df& normal, const CRAB::Vector4Df& view, const CRAB::Vector4Df& P)
{
	// light vector
	CRAB::Vector4Df L = position - P;
	L.normalize();

	CRAB::Vector4Df Ia = intensity * mat.ka;                                   // ambient reflection
	CRAB::Vector4Df Id = { 0.0f, 0.0f, 0.0f, 0.0f };			           // diffuse reflection
	CRAB::Vector4Df Is = { 0.0f, 0.0f, 0.0f, 0.0f };					  // specular reflection

	float dot_d_n = dot(L, normal);
	if (dot_d_n > 0.0f) {
		Id = (intensity * mat.kd) * dot_d_n; //diffuse reflection
	}
	const CRAB::Vector4Df r = reflection(L, normal);
	float dot_r_v = dot(r, view);
	if (dot_r_v > 0.0f) {
		Is = (intensity * mat.ks) * powf(dot_r_v, mat.alfa);//specular reflection
	}
	
	return Ia + Id + Is;
}