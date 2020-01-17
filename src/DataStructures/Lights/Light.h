#pragma once
#include "DataStructures/Linear_Algebra.h"
#include "DataStructures/Material/Material.h"

class Light
{
public:
	CRAB::Vector4Df intensity;

	Light();
	Light(CRAB::Vector4Df intensity);
	virtual ~Light() = 0;

	//Return the collor after the interation with the light and material
	virtual CRAB::Vector4Df Illumination(const Material &mat, const CRAB::Vector4Df &normal, const CRAB::Vector4Df &view, const CRAB::Vector4Df &P) = 0;
};