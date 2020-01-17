#pragma once

#include "Light.h"

class PointLight : public Light
{
	CRAB::Vector4Df position;  // light position
public:
	PointLight(CRAB::Vector4Df intensity, CRAB::Vector4Df _position);
	~PointLight();

	CRAB::Vector4Df Illumination(const Material &mat, const CRAB::Vector4Df &normal, const CRAB::Vector4Df &view, const CRAB::Vector4Df &P);
};