#pragma once
#include "DataStructures/Object.h"
#include "DataStructures/Geometry/Geometry.h"
#include "ImGUI.h"


#include "DataStructures/Geometry/Cylinder.h"
#include "DataStructures/Geometry/Plane.h"
#include "DataStructures/Geometry/Cone.h"
#include "DataStructures/Geometry/Cube.h"
#include "DataStructures/Geometry/Sphere.h"
#include "DataStructures/Geometry/BoundedCone.h"
#include "DataStructures/Geometry/BoundedCylinder.h"


#include <typeinfo>


//list of objects
std::vector<Geometry*> objs;

static Geometry *selected_obj = nullptr;
static Geometry* selected_boolean = nullptr;
static bool object_window_open = true;
static bool tree_window_open = true;
static char object_name[20] = " ";


//static bool geometry_window_open = true;

static CRAB::Vector4Df translation, scale{ 1,1,1,0 }, rotation;


void Remove(Geometry* g1) {
	if (g1->father) {
		CSG::CSG_Tree* t = (CSG::CSG_Tree*)g1->father;
		if (t->father) {
			Geometry* other_son = nullptr;

			if (t->getSon(0) == g1) {
				other_son = t->getSon(1);
				//t->setSon(nullptr, 1);
			}
			else {
				other_son = t->getSon(0);
				//t->setSon(nullptr, 0);
			}

			CSG::CSG_Tree* tf = (CSG::CSG_Tree*)t->father;
			if (tf->getSon(0) == t) {
				tf->setSon(other_son, 0);
			}
			else {
				tf->setSon(other_son, 1);
			}

			delete g1->father;
		}
		else {
			if (t->getSon(0) == g1) {
				t->getSon(1)->father = nullptr;
				objs.push_back(t->getSon(1));
			}
			else {
				t->getSon(0)->father = nullptr;
				objs.push_back(t->getSon(0));
			}

			auto f = find(objs.begin(), objs.end(), t);
			if (f != objs.end()) {
				objs.erase(f);
			}

			delete t;
		}
	}
	else {
		auto f = find(objs.begin(), objs.end(), g1);
		if (f != objs.end()) {
			objs.erase(f);
		}
	}


	g1->Free();
	delete g1;
	
	selected_obj = nullptr;
	selected_boolean = nullptr;
}

void Boolean(Geometry* g1, Geometry* g2) {
	if (g1->father) {
		CSG::CSG_Tree* t = (CSG::CSG_Tree*)g1->father;

		CSG::CSG_Tree* new_node = new CSG::CSG_Tree(UNION);
		new_node->setSon(g1, 0);
		new_node->setSon(g2, 1);
		if (t->getSon(0) == g1) {
			t->setSon(new_node, 0);
		}
		else {
			t->setSon(new_node, 1);
		}

	}
	else {

		CSG::CSG_Tree* new_node = new CSG::CSG_Tree(UNION);
		new_node->setSon(g1, 0);
		new_node->setSon(g2, 1);

		auto f = find(objs.begin(), objs.end(), g1);
		if (f != objs.end()) {
			objs.erase(f);
		}

		objs.push_back(new_node);
	}
	selected_boolean = nullptr;
}



inline void DisplayGeometry() {
	if (!selected_obj)return;

	//ImGui::Text("Transform: ");
	if (ImGui::CollapsingHeader("Transform Properties:")) {
		//ImGui::SameLine();
		//ImGui::Text(typeid(*selected_obj).name());

		ImGui::AlignFirstTextHeightToWidgets(); // not needed to solve bug. Helps make vertical spacing more consistent

		ImGui::InputFloat3("T", (float*)&translation, 3);
		ImGui::InputFloat3("R", (float*)&rotation, 3);
		ImGui::InputFloat3("S", (float*)&scale, 3);

		if (ImGui::Button("Rotate", ImVec2(50, 25))) {
			CRAB::Matrix4 r = CRAB::rotateX(-rotation.x) * CRAB::rotateY(-rotation.y) * CRAB::rotateZ(-rotation.z);
			selected_obj->Transform(r);
		}
		ImGui::SameLine();
		if (ImGui::Button("Resize", ImVec2(50, 25))) {
			selected_obj->Transform(CRAB::resizeMatrix(1.0f / scale.x, 1.0f / scale.y, 1.0f / scale.z));
		}
		ImGui::SameLine();
		if (ImGui::Button("Translate", ImVec2(50, 25))) {
			selected_obj->Transform(CRAB::translate(translation * -1));
		}
	}
	

	if (typeid(*selected_obj) == typeid(CSG::CSG_Tree)) {
		CSG::CSG_Tree* n = (CSG::CSG_Tree*)selected_obj;
		ImGui::Combo(" ", &n->operation, "INVISIBLE\0UNION\0INTERSECTION\0DIFFERENCE\0\0");
		
		if (ImGui::Button("Swap", ImVec2(50, 25))) {
			n->Swap();
		}
		ImGui::SameLine();
		if (ImGui::Button("Duplicate", ImVec2(50, 25))) {
			objs.push_back(n->Duplicate());
		}
		
	}
	if (ImGui::Button("Boolean", ImVec2(50, 25))) {
		selected_boolean = selected_obj;
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete", ImVec2(50, 25))) {
		Remove(selected_obj);
	}

	

}



static void DisplayTree(Geometry *t) {

	if (!t)return;

	ImGui::PushID(t);

	if (typeid(*t) != typeid(CSG::CSG_Tree)) {
		//ImGui::AlignFirstTextHeightToWidgets(); // not needed to solve bug. Helps make vertical spacing more consistent
		
		ImGui::Text(t->name);
		ImGui::SameLine();
		if (ImGui::SmallButton("+")) {
			selected_obj = t;
			object_window_open = true;
		}
		ImGui::NextColumn();
	}
	else if (typeid(*t) == typeid(CSG::CSG_Tree)) {
		
		CSG::CSG_Tree* n = (CSG::CSG_Tree*)t;


		//ImGui::AlignFirstTextHeightToWidgets();

		bool treeopen = ImGui::TreeNodeEx(t->name, ImGuiTreeNodeFlags_AllowItemOverlap);

		ImGui::SameLine();
		if (ImGui::SmallButton("+")) {
			selected_obj = t;
			object_window_open = true;
		}
		
		if(treeopen){
			// ADDED THESE TWO LINES
			ImGui::NextColumn();
			
			DisplayTree(n->getSon(0)); 
			ImGui::NextColumn();
			DisplayTree(n->getSon(1));
			
			ImGui::TreePop();
		}
		

	}
	else {
		ImGui::Text(typeid(*t).name());
	}
	ImGui::PopID();

}

static void DisplayTree() {
	if (tree_window_open) {
		ImGui::Begin("Tree view", &tree_window_open, ImGuiWindowFlags_AlwaysAutoResize);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		for (Geometry* t : objs) {
			DisplayTree(t);
		}
		ImGui::End();
	}
}


inline void RenderObject() {
	if (selected_boolean && selected_boolean != selected_obj) {
		Boolean(selected_boolean, selected_obj->Duplicate());
	}

	if (object_window_open && selected_obj) {
		ImGui::Begin("Object Properties", &object_window_open, ImGuiWindowFlags_AlwaysAutoResize);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		
		ImGui::Text(typeid(*selected_obj).name());
		ImGui::Text("Object Name:");// ImGui::SameLine();
		//strcpy_s(object_name, selected_obj->name());
		ImGui::InputText("", selected_obj->name, 20, ImGuiInputTextFlags_EnterReturnsTrue);

		if (typeid(*selected_obj) != typeid(CSG::CSG_Tree)) {
			if (ImGui::CollapsingHeader("Material Properties:")) {
				ImGui::ColorEdit4("Color Ambient", (float*)&selected_obj->material.ka, ImGuiColorEditFlags_NoInputs);
				ImGui::ColorEdit4("Color Difuse", (float*)&selected_obj->material.kd, ImGuiColorEditFlags_NoInputs);
				ImGui::ColorEdit4("Color Specular", (float*)&selected_obj->material.ks, ImGuiColorEditFlags_NoInputs);
				ImGui::DragFloat("Alpha", &selected_obj->material.alfa);
			}
		}

		DisplayGeometry();
		//selected_geometry = selected_obj->getGeometry();
		//DisplayTree(selected_obj);
		//if (ImGui::CollapsingHeader("Geometry Properties:")) {
		//	ImGui::Text(typeid(*selected_geometry).name());
		//	RenderGeometry();
		//}

		ImGui::End();
	}
}
