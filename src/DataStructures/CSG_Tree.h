#pragma once


#include "DataStructures/Linear_Algebra.h"
#include "DataStructures/Ray.h"

#include "DataStructures/Geometry/Geometry.h"

#include "DataStructures/Geometry/Cylinder.h"
#include "DataStructures/Geometry/Plane.h"
#include "DataStructures/Geometry/Cone.h"
#include "DataStructures/Geometry/Sphere.h"
#include "DataStructures/Geometry/Cube.h"
#include "DataStructures/Geometry/BoundedCone.h"
#include "DataStructures/Geometry/BoundedCylinder.h"
#include <typeinfo>


#define TREE_EMPTY_NODE		0
#define TREE_FULL_NODE		1
#define TREE_SEMI_NODE		2



namespace CSG {

	#define DIFFERENCE		3
	#define INTERSECTION	2
	#define UNION			1

	inline Geometry* CopyTree(Geometry* g);
	
	class CSG_Tree : public Geometry
	{
		Geometry* sons[2];
	public:
		int operation;

		CSG_Tree(int _operation) : operation(_operation), sons{nullptr, nullptr}{};
		
		bool Inside(const CRAB::Vector4Df& p) const { return true; };

		CRAB::RayCollisionList Collide(const CRAB::Ray& ray)  {
			const CRAB::Ray& r2 = RayTransform(ray, transform);

			const CRAB::RayCollisionList &rcl1 = sons[0]->Collide(r2);
			const CRAB::RayCollisionList &rcl2 = sons[1]->Collide(r2);

			bool state1 = rcl1.first_state;
			bool state2 = rcl2.first_state;
			unsigned i1 = 0, i2 = 0;
			CRAB::RayCollisionList rcl{false, 0};
			
			if (operation == INTERSECTION) {
				bool state = rcl1.first_state && rcl2.first_state;
				rcl.first_state = state;

				while (i1 < rcl1.collisions_count || i2 < rcl2.collisions_count) {
					float d1 = INFINITE, d2 = INFINITE;
					if (i1 < rcl1.collisions_count)d1 = rcl1.collisions[i1].distance;
					if (i2 < rcl2.collisions_count)d2 = rcl2.collisions[i2].distance;
					//if (d1 == d2) d2 += SMALL_NUMBER;
					if (d1 < d2) {
						state1 = !state1;
						if (state2) {
							rcl.collisions[rcl.collisions_count] = (rcl1.collisions[i1]);
							rcl.collisions_count++;
						}
						i1++;
					}
					else {
						state2 = !state2;
						if (state1) {
							rcl.collisions[rcl.collisions_count] = rcl2.collisions[i2];
							rcl.collisions_count++;
						}
						i2++;
					}
				}
			}

			if (operation == UNION) {
				//recl1 is the left collision list
				//rcl2 is the right collision list
				//first state is a bool that say if the first point in in our out
				bool state = rcl1.first_state || rcl2.first_state;
				rcl.first_state = state;
				//colision count is the number of collisions
				while (i1 < rcl1.collisions_count || i2 < rcl2.collisions_count) {
					if (i2 >= rcl2.collisions_count || (i1 < rcl1.collisions_count && rcl1.collisions[i1].distance < rcl2.collisions[i2].distance)) {
						state1 = !state1;
						if (!state2) {
							rcl.collisions[rcl.collisions_count] = (rcl1.collisions[i1]);
							rcl.collisions_count++;
						}
						i1++;
					}
					else {
						state2 = !state2;
						if (!state1) {
							rcl.collisions[rcl.collisions_count] = (rcl2.collisions[i2]);
							rcl.collisions_count++;
						}
						i2++;
					}
				}
			}

			if (operation == DIFFERENCE) {
				bool state = rcl1.first_state && !rcl2.first_state;
				rcl.first_state = state;

				while (i1 < rcl1.collisions_count || i2 < rcl2.collisions_count) {
					float d1 = INFINITE, d2 = INFINITE;
					if (i1 < rcl1.collisions_count)d1 = rcl1.collisions[i1].distance;
					if (i2 < rcl2.collisions_count)d2 = rcl2.collisions[i2].distance;
					//if (d1 == d2) d2 += SMALL_NUMBER;
					if (d1 < d2) {
						state1 = !state1;
						if (!state2) {
							rcl.collisions[rcl.collisions_count] = (rcl1.collisions[i1]);
							rcl.collisions_count++;
						}
						i1++;
					}
					else {
						state2 = !state2;
						if (state1) {
							CRAB::Collision c = rcl2.collisions[i2];
							c.normal = c.normal * (-1.0f);
							rcl.collisions[rcl.collisions_count] = (c);
							rcl.collisions_count++;
						}
						i2++;
					}
				}
			}
			
			for (int i = 0; i < rcl.collisions_count; i++) {
				rcl.collisions[i].normal = (transform.Inverse() * rcl.collisions[i].normal);
			}

			return rcl;
		};

		CRAB::Collision CollideClosest(const CRAB::Ray& ray)  {
			const CRAB::RayCollisionList& rcl = Collide(ray);
			if(rcl.collisions_count == 0) {
				return CRAB::Collision{ INFINITY };
			}
			return rcl.collisions[0]; 
		}

		virtual CRAB::Vector4Df getNormal(const CRAB::Vector4Df& surface_point) const {
			return CRAB::Vector4Df{ 1,1,1,0 };
		}


		Geometry* getSon(int i) { return sons[i]; }
		void setSon(Geometry* g, int i) { if (g) { g->father = this; } sons[i] = g; }
		void OverWriteSon(Geometry* g, int i) { sons[i]->Free(); delete sons[i]; g->father = this; sons[i] = g; }

		void Swap() {
			std::swap(sons[0], sons[1]);
		}

		Geometry* Duplicate() {
			
			CSG::CSG_Tree* new_tree = new CSG::CSG_Tree(operation);
			
			new_tree->setSon(sons[0]->Duplicate(), 0);
			new_tree->setSon(sons[1]->Duplicate(), 1);
			new_tree->transform = transform;
			return new_tree;
		}


		
		//void Remove(std::vector<Geometry *> &objs) {
		//	Free();
		//	
		//	if (father) {
		//		Geometry* other_son = nullptr;
		//		CSG::CSG_Tree* t = (CSG::CSG_Tree*)father;
		//
		//		if (t->getSon(0) == this) {
		//			other_son = t->getSon(1);
		//		}
		//		else {
		//			other_son = t->getSon(0);
		//		}
		//		if (t->father) {
		//			CSG::CSG_Tree* t = (CSG::CSG_Tree*)father;
		//		}
		//	}
		//	else {
		//		auto f = find(objs.begin(), objs.end(), this);
		//		if (f != objs.end()) {
		//			objs.erase(f);
		//		}
		//	}
		//	
		//
		//	delete this;
		//}

		void Free() { for (Geometry* t : sons) { t->Free(); delete t; } }
	};

	inline Geometry* CopyTree(Geometry* g) {
		if (typeid(*g) == typeid(CSG::CSG_Tree)) {
			CSG::CSG_Tree* t = (CSG::CSG_Tree*)g;

			CSG::CSG_Tree* new_tree = new CSG::CSG_Tree(t->operation);
			//*new_tree = *t;
			
			new_tree->setSon(CopyTree(t->getSon(0)), 0);
			new_tree->setSon(CopyTree(t->getSon(1)), 1);
			new_tree->transform = t->transform;
			return new_tree;
		}
		else {
			Geometry* p = nullptr;
			if (typeid(*g) == typeid(Sphere)) { p = new Sphere(); }
			if (typeid(*g) == typeid(Cube)) { p = new Cube(); }
			if (typeid(*g) == typeid(BoundedCylinder)) { p = new BoundedCylinder(); }
			if (typeid(*g) == typeid(BoundedCone)) { p = new BoundedCone(); }
			if (typeid(*g) == typeid(Plane)) { p = new Plane(); }

			*p = *g;
			return p;
		}
	}

	//struct CSG_Leaf : public TreeElement
	//{
	//	Geometry* geometry;
	//
	//	CSG_Leaf() : geometry(nullptr) {};
	//	CSG_Leaf(Geometry* _geometry) : geometry(_geometry) {};
	//
	//	CRAB::RayCollisionList RayClassify(const CRAB::Ray& ray) const {
	//		const CRAB::Ray r2{ transform * ray.origin, transform * ray.direction };
	//		return geometry->Collide(r2);
	//	};
	//};


	//class CSG_Tree : public Geometry {
	//public:
	//	TreeElement* tree = nullptr;
	//
	//	CSG_Tree() = default;
	//	CSG_Tree(TreeElement* _tree) : tree(_tree) {};
	//
	//	bool Inside(const CRAB::Vector4Df& p) const { return true; };
	//
	//	CRAB::Collision CollideClosest(const CRAB::Ray& ray) const{ 
	//		const CRAB::RayCollisionList &rcl = tree->RayClassify(ray);
	//		if(rcl.collisions_count == 0) {
	//			return CRAB::Collision{ INFINITY };
	//		}
	//		return rcl.collisions[0]; 
	//	};//return the closest collision distance of a ray and the geometry
	//	CRAB::RayCollisionList Collide(const CRAB::Ray& ray) const { return CRAB::RayCollisionList(); };//return all collisions of a ray
	//
	//	CRAB::Vector4Df getNormal(const CRAB::Vector4Df& point) const { return CRAB::Vector4Df{ 1,0,0,0 }; } // returns the normal vector at point on a surface
	//
	//	void Free() { tree->Free(); delete tree; }
	//
	//};
}
