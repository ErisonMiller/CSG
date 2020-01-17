#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glut.h"
#include "imgui/imgui_impl_opengl2.h"

#include "RenderEngine/RenderAPI.h"

namespace GUI_API {
	inline void Start_ImGUI(){

		glutPassiveMotionFunc(ImGui_ImplGLUT_MotionFunc);
		glutKeyboardUpFunc(ImGui_ImplGLUT_KeyboardUpFunc);
		glutSpecialFunc(ImGui_ImplGLUT_SpecialFunc);
		glutSpecialUpFunc(ImGui_ImplGLUT_SpecialUpFunc);

#ifdef __FREEGLUT_EXT_H__
		glutMouseWheelFunc(ImGui_ImplGLUT_MouseWheelFunc);
#endif
	
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		ImGui_ImplGLUT_Init();
		ImGui_ImplOpenGL2_Init();

	}

	inline void EndImGUI() {

		ImGui_ImplOpenGL2_Shutdown();
		ImGui_ImplGLUT_Shutdown();
		ImGui::DestroyContext();
	}

	inline void RenderFrame() {
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
		// Rendering
		ImGui::Render();
		ImGuiIO& io = ImGui::GetIO();
		glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);

		ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

	}

	inline void StartFrame() {

		ImGui_ImplOpenGL2_NewFrame();
		ImGui_ImplGLUT_NewFrame();
	}
	
}
