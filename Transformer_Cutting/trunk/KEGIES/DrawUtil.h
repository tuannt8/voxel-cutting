#include "stdafx.h"
#include <GL\glut.h>

// This class will not use any specific data structure

namespace drawUtil
{
	void drawBox(float xmin, float ymin, float zmin, 
		float xMax, float yMax, float zMax)
	{
		glBegin(GL_LINES);

		// Lower face
		glVertex3f(xmin, ymin, zmin);
		glVertex3f(xMax, ymin, zmin);

		glVertex3f(xmin, ymin, zmin);
		glVertex3f(xmin, yMax, zmin);

		glVertex3f(xmin, yMax, zmin);
		glVertex3f(xMax, yMax, zmin);

		glVertex3f(xMax, yMax, zmin);
		glVertex3f(xMax, ymin, zmin);

		// Upper face
		glVertex3f(xmin, ymin, zMax);
		glVertex3f(xMax, ymin, zMax);

		glVertex3f(xmin, ymin, zMax);
		glVertex3f(xmin, yMax, zMax);

		glVertex3f(xmin, yMax, zMax);
		glVertex3f(xMax, yMax, zMax);

		glVertex3f(xMax, yMax, zMax);
		glVertex3f(xMax, ymin, zMax);

		// Vertical line
		glVertex3f(xmin, ymin, zmin);
		glVertex3f(xmin, ymin, zMax);

		glVertex3f(xMax, ymin, zmin);
		glVertex3f(xMax, ymin, zMax);

		glVertex3f(xMax, yMax, zmin);
		glVertex3f(xMax, yMax, zMax);

		glVertex3f(xmin, yMax, zmin);
		glVertex3f(xmin, yMax, zMax);

		glEnd();
	}

	void drawRoundPoint(float x, float y, float z, float radius)
	{
		glPushMatrix();
		glTranslatef(x,y,z);
		glutSolidSphere(radius, 10, 10);
		glPopMatrix();
	}
}