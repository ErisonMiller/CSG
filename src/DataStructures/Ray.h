#pragma once

#include "DataStructures/Linear_Algebra.h"
#include <vector>
//
//		Ray Collision Core
//
// This header contains the basic struct for:
//		- Ray
//		- Collison
//		- RayCollisionList
//

class Geometry;//foward declaration for geometry

namespace CRAB {

	//struct just for readbility
	struct Ray
	{
		Vector4Df origin, direction;
	};

	//Because we to know the object that we collide besides the distance
	/*distance, geometry and normal*/
	struct Collision
	{
		float distance;
		Geometry* geometry;
		Vector4Df normal;
		/*Intersection Point*/
	};

	//Since we have a variable size of collisions we create a list of Collisions
	//TODO: in the future this will change for better use of CSG (just the students of Geometric Modeling need to do this part)
	struct RayCollisionList {
		/*if the first point is inside or not*/
		bool first_state = false;
		int collisions_count = 0;
		Collision collisions[10];
		//std::vector<Collision> collisions;
	};
	//template<size_t stack_capacity>
	//struct RayCollisionList {
	//	/*if the first point is inside or not*/
	//	bool first_state = false;
	//	Collision collisions[stack_capacity];
	//	//std::vector<Collision> collisions;
	//};

}