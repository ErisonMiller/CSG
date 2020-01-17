#include "DirectionalLight.h"



DirectionalLight::DirectionalLight()
{
}

DirectionalLight::DirectionalLight(CRAB::Vector4Df _intensity, CRAB::Vector4Df _direction): 
	direction(_direction.normalized()){
	intensity = _intensity;

}

DirectionalLight::~DirectionalLight()
{
}

CRAB::Vector4Df DirectionalLight::Illumination(const Material &mat, const CRAB::Vector4Df &normal, const CRAB::Vector4Df &view, const CRAB::Vector4Df &P) {

	// the direction of a perfect reflector
	
	CRAB::Vector4Df Id = intensity * mat.ka;
	float dot_d_n = dot(direction, normal);
	if (dot_d_n > 0.0f) {
		Id = Id + (intensity * mat.kd) * dot_d_n; //diffuse reflection
	}
	const CRAB::Vector4Df r = reflection(direction, normal);
	float dot_r_v = dot(r, view);
	if (dot_r_v > 0.0f) {
		Id = Id + (intensity * mat.ks) * powf(dot_r_v, mat.alfa);//specular reflection
	}
	return Id;
}
