//
//	Main Window class
//	Call the Graphic API to cerate the window
//	Call the events to recieve user input
//	Call the render engines for renderization
//

#include "WindowController.h"
#include "RenderEngine/RenderAPI.h"
#include "RenderEngine/Camera.h"

#include "RenderEngine/OpenGLRender.h"
#include "RenderEngine/RayCastRender.h"

#include "DataStructures/Lights/Light.h"
#include "DataStructures/Lights/PointLight.h"
#include "DataStructures/Lights/DirectionalLight.h"

#include "DataStructures/CSG_Tree.h"

#include "ImGUI.h"
#include "ObjectController.h"
#include "file_dialog/nfd.h"
#include "DataIO/LoadObj.h"


#include "DataStructures/Geometry/Cube.h"

using namespace CRAB;

//pixel buffer
RenderAPI::VertexBuffer vbo;


//list of lights
std::vector<Light *> lights;

//list of materials
Material *Neutral = new Material(Vector4Df{ 0.1f, 0.1f, 0.1f, 0 }, Vector4Df{ 1.0f, 1.0f, 1.0f, 0 }, Vector4Df{ 1.0f, 1.0f, 1.0f, 0 }, 1000);

const int	width  = 512,
			height = 512;

//main camera
Camera cam = Camera(
	//Vector4Df{ 15.0f,10.0f,30.0f,1.0f },//position
	//Vector4Df{ 10.0f, 9.0f,10.0f,1.0f },//lookat
	//Vector4Df{ 10.0f,5.0f,50.0f,1.0f },//position
	//Vector4Df{ 10.0f, 5.0f,15.0f,1.0f },//lookat
	Vector4Df{ 0.0f,0.0f,10.0f,1.0f },//position
	Vector4Df{ 0.0f, 0.0f,0.0f,1.0f },//lookat
	Vector4Df{ 0.0f,1.0f,0.0f,0.0f },//up
	float2{ width*1.0f, height*1.0f }, //resolution
	float2{ 4.0f, 4.0f },			//dimensions
	4.0f							//near
);

#define VIEW_3D 0
#define VIEW_2D 1

int view_dimension = VIEW_3D;
//raycast class for renderization 
RayCast rc(cam);



void LoadObjDialog() {


	nfdchar_t* outPath = NULL;
	nfdresult_t result = NFD_OpenDialog(NULL, NULL, &outPath);
	if (result == NFD_OKAY)
	{
		puts("Success!");
		CSG::Load_Tree(outPath, objs);
		//std::vector<Mesh> meshs = CRAB::Load_Obj(outPath);
		free(outPath);
		//for (Mesh& m : meshs) {
		//	oct.push_back(CreateTree(&m));
		//}
	}
	else if (result == NFD_CANCEL)
	{
		puts("User pressed cancel.");
	}
	else
	{
		printf("Error: %s\n", NFD_GetError());
	}

}


void SaveObjDialog() {


	nfdchar_t* outPath = NULL;
	nfdresult_t result = NFD_SaveDialog(NULL, NULL, &outPath);
	if (result == NFD_OKAY)
	{
		CSG::SaveCSGTree(outPath, objs);
		puts("Success!");
		free(outPath);
	}
	else if (result == NFD_CANCEL)
	{
		puts("User pressed cancel.");
	}
	else
	{
		printf("Error: %s\n", NFD_GetError());
	}

}



void Menu() {
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New")) {
				for (Geometry *o : objs) {
					o->Free();
					delete o;
				}
				objs.clear();
			}
			if (ImGui::MenuItem("Import")) {
				LoadObjDialog();
			}
			if (ImGui::MenuItem("Save")) {
				SaveObjDialog();
			}
			//ShowExampleMenuFile();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Tree View", NULL, tree_window_open)) {
				tree_window_open = !tree_window_open;
			}

			if (ImGui::MenuItem("3D View")) {
				view_dimension = VIEW_3D;
			}
			if (ImGui::MenuItem("2D View")) {
				view_dimension = VIEW_2D;
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Add"))
		{
			if (ImGui::MenuItem("Sphere")) {
				objs.push_back(new Sphere);
			}
			if (ImGui::MenuItem("Cube")) {
				objs.push_back(new Cube);
			}
			if (ImGui::MenuItem("Cylinder")) {
				objs.push_back(new BoundedCylinder);
			}
			if (ImGui::MenuItem("Cone")) {
				objs.push_back(new BoundedCone);
			}
			if (ImGui::MenuItem("Plane")) {
				objs.push_back(new Plane);
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}




// display function called by MainLoop(), gets executed every frame 
void disp(void)
{
	// Start the Dear ImGui frame
	GUI_API::StartFrame();
	
	RenderAPI::BufferClear();

	Vector4Df* colorBuffer;
	//Render using RayCast
	if (view_dimension == VIEW_2D) {
		colorBuffer = rc.Render2D(cam, objs);
	}
	else {
		colorBuffer = rc.Render(cam, objs, lights);
	}
	RenderAPI::BufferBind(vbo);
	RenderAPI::MapBuffer(colorBuffer, width, height, vbo);

	//GUI displays
	RenderObject();
	DisplayTree();
	Menu();

	//GUI rendering call
	GUI_API::RenderFrame();

	//End frame
	RenderAPI::SwapBuffers();
	RenderAPI::ReDisplay();
}

// mouse event handlers
int lastX = 0, lastY = 0;
int theButtonState = 0;
int theModifierState = 0;

// camera mouse controls in X and Y direction
void motion(int x, int y)
{
	ImGui_ImplGLUT_MotionFunc(x, y);

	int deltaX = lastX - x;
	int deltaY = lastY - y;

	if (!(ImGui::GetIO().WantCaptureMouse)) {
		if (deltaX != 0 || deltaY != 0) {

			if (theButtonState == GLUT_LEFT_BUTTON)  // Rotate
			{
				Vector4Df v = cross(cam.view - cam.position, cam.up);
				cam.Transform(rotateArbitrary(deltaY, v), cam.view);
				cam.Transform(rotateY(deltaX), cam.view);
			}
			else if (theButtonState == GLUT_MIDDLE_BUTTON) // Zoom
			{

			}

			if (theButtonState == GLUT_RIGHT_BUTTON) // camera move
			{

				cam.position = cam.position + (cam.position - cam.view)*(deltaY*0.01f);
				//float2{ cam.fov.x * (1+deltaY*0.1f), cam.fov.y * (1 + (deltaY*0.1f)) };
			//interactiveCamera->changeRadius(-deltaY * 0.01);
			}

			lastX = x;
			lastY = y;
		}
	}
}

void mouse(int button, int state, int x, int y)
{
	ImGui_ImplGLUT_MouseFunc(button, state, x, y);
	
	if (!(ImGui::GetIO().WantCaptureMouse)) {
		theButtonState = button;
		theModifierState = glutGetModifiers();
		lastX = x;
		lastY = y;
	
		if (theButtonState == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) // camera move
		{
			selected_obj = nullptr;
			selected_obj = rc.RayPick(cam, objs, x, y);
			if (selected_obj) {
				object_window_open = true;
			}
	
		}
	
	
		motion(x, y);
	}

}

void keyboard(unsigned char key, int x, int y) {
	ImGui_ImplGLUT_KeyboardFunc(key, x, y);
	Object* o;
	switch (key) {

	case(27): exit(0);
	case(' '):
		break;
	case('L'):
		rc.Area_Volume(cam, objs);
		break;
	}
}

void resize(int w, int h) {
	ImGui_ImplGLUT_ReshapeFunc(w, h);
	RenderAPI::Reshape(w, h);
}

void InitScene() {
	//fill the light list
	lights.push_back(new DirectionalLight(Vector4Df{ 1.0f, 1.0f, 1.0f, 0 }, Vector4Df{ 7.0f, 1.5f, 30.0f, 0 }));

	Material Verde		= Material(Vector4Df{ 0.0f, 0.1f, 0.0f, 0 }, Vector4Df{ 0.0f, 1.0f, 0.0f, 0 }, Vector4Df{ 0.0f, 1.0f, 0.0f, 0 }, 1000);
	Material Tronco		= Material(Vector4Df{ 0.04f, 0.02f, 0.01f, 0 }, Vector4Df{ 0.4f, 0.2f, 0.1f, 0 }, Vector4Df{ 0.4f, 0.2f, 0.1f, 0 }, 1000);
	
	//fill the object list

	CSG::CSG_Tree* base = new CSG::CSG_Tree(INTERSECTION);
	base->setSon( new Plane(), 0);
	base->setSon( new BoundedCone(), 1);
	base->getSon(0)->Transform(translate(CRAB::Vector4Df{ 0,0.25,0,0 }));;
	base->getSon(1)->Transform(resizeMatrix(0.9, -0.3, 0.9));
	base->getSon(1)->Transform(translate(CRAB::Vector4Df{0,-1,0,0}));

	base->getSon(0)->material = Tronco;
	base->getSon(1)->material = Tronco;
	//base->Transform(resizeMatrix(1.2, -0.5, 1.2));
	

	BoundedCone* earL = new BoundedCone();
	//earL->sons[0] = Plane_Limites(0, 1);
	//earL->sons[1] = base->sons[1];
	earL->Transform(resizeMatrix(1.75, 1, 1.75));
	earL->Transform(rotateZ(45));
	earL->Transform(translate(Vector4Df{ -0.8,-2.8,0,0 }));

	BoundedCone* earR = new BoundedCone();
	//earR->sons[0] = earL->sons[0];
	//earR->sons[1] = base->sons[1];
	earR->Transform(resizeMatrix(1.75, 1, 1.75));
	earR->Transform(rotateZ(-45));
	earR->Transform(translate(Vector4Df{ 0.8,-2.8,0,0 }));



	Sphere* head = new Sphere();
	head->Transform(resizeMatrix(-0.75, -0.75, -0.75));
	head->Transform(translate(Vector4Df{ 0,-2,0 }));
	
	CSG::CSG_Tree* ears = new CSG::CSG_Tree(UNION);
	ears->setSon(earL, 0);
	ears->setSon(head, 1);


	CSG::CSG_Tree* face = new CSG::CSG_Tree(UNION);
	face->setSon(ears, 0);
	face->setSon(earR, 1);

	//Object o = Object("Copa da arvode 1", Verde, new Sphere());
	earL->material = Verde;
	earR->material = Verde;
	head->material = Verde;

	strcpy_s(face->name, "head");
	strcpy_s(earL->name, "earL");
	strcpy_s(earR->name, "earR");
	strcpy_s(head->name, "sphere");
	strcpy_s(base->name, "base");
	strcpy_s(base->getSon(0)->name, "plane");
	strcpy_s(base->getSon(1)->name, "cone");

	objs.push_back(face);
	objs.push_back(base);
	
}

// Main.
void Start_Window(int argc, char **argv) {
	// Create GLUT window
	RenderAPI::StartRenderAPI(argc, argv, width, height);
	
	// functions for user interaction
	RenderAPI::MouseFunc(mouse);
	RenderAPI::MotionFunc(motion);
	RenderAPI::KeyboardFunc(keyboard);
	RenderAPI::ReshapeFunc(resize);
	
	RenderAPI::DisplayFunc(disp);

	//create the pixel buffer
	RenderAPI::CreateVBO(&vbo, width, height);
	
	GUI_API::Start_ImGUI();

	//Fill the scene
	InitScene();
	
	//start render loop
	RenderAPI::RenderLoop();

	GUI_API::EndImGUI();
	//delete the pixel buffer
	RenderAPI::DeleteVBO(&vbo);
}

