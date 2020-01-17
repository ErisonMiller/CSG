#include "OpenGLRender.h"
#include "Camera.h"

#include <GL/glew.h>
#include <GL/glut.h>

void StartOpenGLRender(const CRAB::Camera &cam){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, cam.resolution.x, cam.resolution.y);
	glFrustum(-cam.dimensions.x * 0.5f, cam.dimensions.x * 0.5f, -cam.dimensions.y * 0.5f, cam.dimensions.y * 0.5f, cam.n, 100.0f);
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	gluLookAt(cam.position.x, cam.position.y, cam.position.z,
		cam.view.x, cam.view.y, cam.view.z,
		cam.up.x, cam.up.y, cam.up.z);


	glPointSize(4.0f);
	glLineWidth(2.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	//look at
	glBegin(GL_POINTS);
	glVertex3f(cam.view.x, cam.view.y, cam.view.z);
	glEnd();
	// Arrows
	glBegin(GL_LINES);
	glColor3f(0, 0.8f, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 1.0f, 0);
	glEnd();
	glLineWidth(2.0f);
	//   grid

	glLineWidth(1.0f);
	glBegin(GL_LINES);
	for (float i = -10; i <= 10; ++i) {

		glColor3f(0.3f, 0.3f, 0.3f);
		if (i == 0.0f) {
			glColor3f(0, 0, 0.8f);
		}
		glVertex3f(i, 0.01f, -10.0f);
		glVertex3f(i, 0.01f, 10.0f);

		if (i == 0.0f)glColor3f(0.8f, 0, 0);
		glVertex3f(10.0f, 0.01f, i);
		glVertex3f(-10.0f, 0.01f, i);
	}
	glEnd();

	glColor3f(0.99f, 0.3f, 0.3f);
	glBegin(GL_TRIANGLES);
	glVertex3f(-1,0,0);
	glVertex3f(0,2,0);
	glVertex3f(1,0,0);
	glEnd();
}

void OpenGLRender(const CRAB::Camera &cam){
	StartOpenGLRender(cam);
}