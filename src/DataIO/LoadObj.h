#pragma once

// Iostream - STD I/O Library
#include <iostream>

// String - STD String Library
#include <string>

// fStream - STD File I/O Library
#include <fstream>


#include "DataStructures/CSG_Tree.h"

#include "DataStructures/Geometry/Cylinder.h"
#include "DataStructures/Geometry/Plane.h"
#include "DataStructures/Geometry/Cone.h"
#include "DataStructures/Geometry/Sphere.h"
#include "DataStructures/Geometry/Cube.h"
#include "DataStructures/Geometry/BoundedCone.h"
#include "DataStructures/Geometry/BoundedCylinder.h"


// Namespace: OBJL
//
// Description: The namespace that holds eveyrthing that
//	is needed and used for the OBJ Model Loader


namespace CSG
{
#define CSG_2D  0
#define CSG_3D  1

#define PRIMITIVE		1
#define GEOMETRYC_OP	2
#define BOOLEAN_OP		3


#define SPHERE		1
#define CUBE		2
#define CYLINDER	3
#define CONE		4
#define PLANE		5

	inline Geometry* Load_CSG_3DTree(std::ifstream& infile) {
		int tag;
		infile >> tag;
		if (tag == PRIMITIVE) {
			int type;
			infile >> type;

			switch (type)
			{
			case (SPHERE):		return new Sphere(); break;
			case (CUBE):		return new Cube(); break;
			case (CYLINDER):	return new BoundedCylinder(); break;
			case (CONE):		return new BoundedCone(); break;
			case (PLANE):		return new Plane(); break;
			default:
				return new Sphere();
				break;
			}
		}
		else if(tag == GEOMETRYC_OP){
			CRAB::Matrix4 mat4 = CRAB::Identity();
			for (int i = 0; i < 16; i++) {
				infile >> mat4.m[i];
			}
			Geometry* g = Load_CSG_3DTree(infile);
			g->Transform(mat4);
			return g;
		}
		else if (tag == BOOLEAN_OP) {
			int boolean_type;
			infile >> boolean_type;
			CSG::CSG_Tree* t = new CSG_Tree(boolean_type);
			t->setSon(Load_CSG_3DTree(infile), 0);
			t->setSon(Load_CSG_3DTree(infile), 1);
			return t;
		}
	}


	inline Geometry* Load_CSG_2DTree(std::ifstream& infile) {
		int tag;
		infile >> tag;
		if (tag == PRIMITIVE) {
			int type;
			infile >> type;

			switch (type)
			{
			case (SPHERE):		return new Sphere(); break;
			case (CUBE):		return new Cube(); break;
			case (CYLINDER):	return new Sphere(); break;
			case (CONE):		return new BoundedCone(); break;
			case (PLANE):		return new Plane(); break;
			default:
				return new Sphere();
				break;
			}
		}
		else if (tag == GEOMETRYC_OP) {
			CRAB::Matrix4 mat4 = CRAB::Identity();
			for (int i = 0; i < 2; i++) {
				for (int j = 0; j < 2; j++) {
					infile >> mat4._m[i][j];
				}
				infile >> mat4._m[i][3];
			}
			for (int j = 0; j < 2; j++) {
				infile >> mat4._m[3][j];
			}
			infile >> mat4._m[3][3];
			
			Geometry* g = Load_CSG_2DTree(infile);
			g->Transform(mat4);
			return g;
		}
		else if (tag == BOOLEAN_OP) {
			int boolean_type;
			infile >> boolean_type;
			CSG::CSG_Tree* t = new CSG_Tree(boolean_type);
			t->setSon(Load_CSG_2DTree(infile), 0);
			t->setSon(Load_CSG_2DTree(infile), 1);
			return t;
		}
	}


	inline void Load_Tree(const std::string& name, std::vector<Geometry *>& objs) {
		std::ifstream infile(name);

		int type;

		while (infile >> type) {
			if (type == CSG_3D) {
				objs.push_back( Load_CSG_3DTree(infile));
			}
			if (type == CSG_2D) {
				objs.push_back( Load_CSG_2DTree(infile));
				return;
			}
		}
	}


	inline void PrintTree3D(Geometry* g, std::string &s) {
		if (!(g->transform == CRAB::Identity())) {
			s += " 2";
			for (int i = 0; i < 16; i++) {
				s += " " + std::to_string(g->transform.m[i]);
			}
			s += "\n";
		}
		if (typeid(*g) == typeid(CSG_Tree)) { 
			CSG_Tree* t = (CSG_Tree*)g;
			s += " 3 " + std::to_string(t->operation) + "\n";
			PrintTree3D(t->getSon(0), s);
			PrintTree3D(t->getSon(1), s);

		}
		else {
			s += " 1";
			if (typeid(*g) == typeid(Sphere))			{ s += " 1"; }
			if (typeid(*g) == typeid(Cube))				{ s += " 2"; }
			if (typeid(*g) == typeid(BoundedCylinder))	{ s += " 3"; }
			if (typeid(*g) == typeid(BoundedCone))		{ s += " 4"; }
			if (typeid(*g) == typeid(Plane))			{ s += " 5"; }
			s += "\n";
		}
	}

	inline void SaveCSGTree(const std::string &name, std::vector<Geometry*>& objs) {
		std::ofstream fileoutput;
		fileoutput.open(name);
		
		for (Geometry *obj : objs) {
			std::string tree = "1\n";
			PrintTree3D(obj, tree);
			fileoutput << tree;
		}
		fileoutput.close();
	}
}
