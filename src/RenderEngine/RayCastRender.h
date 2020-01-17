#pragma once
#include "RenderEngine/Camera.h"
#include "DataStructures/Object.h"
#include "DataStructures/Lights/Light.h"


//Class to render using raycast


class RayCast {
protected:
	CRAB::float2 resolution;
	CRAB::Vector4Df* accumulateBuffer;

public:

	//RayCast();
	RayCast(const CRAB::Camera& cam);

	~RayCast();

	//return a Vector4df matrix where each Vector4Df is a color
	CRAB::Vector4Df* Render(const CRAB::Camera& cam, const std::vector<Geometry*>& objects, std::vector<Light*>& lights) const;
	CRAB::Vector4Df* Render2D(const CRAB::Camera& cam, const std::vector<Geometry*>& objects) const;
	Geometry* RayPick(const CRAB::Camera& cam, std::vector<Geometry*>& objects, int x, int y) const;
	
	CRAB::Vector4Df* Area_Volume(const CRAB::Camera& cam, const std::vector<Geometry*>& objects) const;

};
