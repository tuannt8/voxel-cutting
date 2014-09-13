#pragma once
#include <gl\gl.h>
#include <gl\glut.h>
#include <iterator>
#include <iostream>
#include <string>
#include <sstream>
// Does not use any specific data type
// MFC only

namespace Util
{
	// Draw
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

	void drawBoxFace(float xmin, float ymin, float zmin, 
		float xMax, float yMax, float zMax) 
	{
		glPushMatrix();
		glTranslatef((xmin+xMax)/2, (ymin+yMax)/2, (zmin+zMax)/2);
		glScaled(xMax-xmin, yMax-ymin, zMax-zmin);
		glBegin(GL_QUADS);

		// Front face
		glVertex3f(  0.5, -0.5, 0.5 );
		glVertex3f(  0.5,  0.5, 0.5 );
		glVertex3f( -0.5,  0.5, 0.5 );
		glVertex3f( -0.5, -0.5, 0.5 );

		// Back face
		glVertex3f( -0.5, -0.5, -0.5 );
		glVertex3f( -0.5,  0.5, -0.5 );
		glVertex3f(  0.5,  0.5, -0.5 );
		glVertex3f(  0.5, -0.5, -0.5 );

		// Purple side - RIGHT
		glVertex3f( 0.5, -0.5, -0.5 );
		glVertex3f( 0.5,  0.5, -0.5 );
		glVertex3f( 0.5,  0.5,  0.5 );
		glVertex3f( 0.5, -0.5,  0.5 );

		// Green side - LEFT
		glVertex3f( -0.5, -0.5,  0.5 );
		glVertex3f( -0.5,  0.5,  0.5 );
		glVertex3f( -0.5,  0.5, -0.5 );
		glVertex3f( -0.5, -0.5, -0.5 );

		// Blue side - TOP
		glVertex3f( -0.5,  0.5,  0.5 );
		glVertex3f(  0.5,  0.5,  0.5 );
		glVertex3f(  0.5,  0.5, -0.5 );
		glVertex3f( -0.5,  0.5, -0.5 );

		// Red side - BOTTOM
		glVertex3f( -0.5, -0.5, -0.5 );
		glVertex3f(  0.5, -0.5, -0.5 );
		glVertex3f(  0.5, -0.5,  0.5 );
		glVertex3f( -0.5, -0.5,  0.5 );

		glEnd();
		glPopMatrix();
	}

	void drawBoxWire(float centerx, float centery, float centerz, float halfSize)
	{
		drawBox(centerx - halfSize, centery - halfSize, centerz - halfSize, centerx + halfSize, centery + halfSize, centerz + halfSize);
	}

	void drawBoxSurface(float centerx, float centery, float centerz, float halfSize)
	{
		drawBoxFace(centerx - halfSize, centery - halfSize, centerz - halfSize, centerx + halfSize, centery + halfSize, centerz + halfSize);
	}

	void printw(float x, float y,float z, const char *format, ...)
	{
		glDisable(GL_LIGHTING);
		glColor3f(0.6, 0.2, 0.2);

		GLvoid *font_style = GLUT_BITMAP_8_BY_13;

		va_list args;   //  Variable argument list
		int len;        // String length
		int i;          //  Iterator
		char * text;    // Text

		//  Initialize a variable argument list
		va_start(args, format);

		//  Return the number of characters in the string referenced the list of arguments.
		// _vscprintf doesn't count terminating '\0' (that's why +1)
		len = _vscprintf(format, args) + 1;

		//  Allocate memory for a string of the specified size
		text = new char[len];

		//  Write formatted output using a pointer to the list of arguments
		vsprintf_s(text, len, format, args);

		//  End using variable argument list
		va_end(args);

		//  Specify the raster position for pixel operations.
		glRasterPos3f (x, y, z);

		//  Draw the characters one by one
		for (i = 0; text[i] != '\0'; i++)
			glutBitmapCharacter(font_style, text[i]);

		//  Free the allocated memory for the string
		delete []text;

		glEnable(GL_LIGHTING);
	}

	int *** cloneArray(int ***arr, int s1, int s2, int s3)
	{
		int ***	vTable = new int **[s1];
		for (size_t i = 0; i < s1; i++)
		{
			vTable[i] = new int *[s2];
			for (size_t j = 0; j < s2; j++)
			{
				vTable[i][j] = new int[s3];
				std::memcpy(vTable[i][j], arr[i][j], s3*sizeof(int));
			}
		}

		return vTable;
	}

	void sort3_descen( int * arr )
	{
		float  arr12[ 2 ];

		// sort first two values
		if( arr[ 0 ] > arr[ 1 ] )
		{
			arr12[ 0 ] = arr[ 0 ];
			arr12[ 1 ] = arr[ 1 ];
		} // if
		else
		{
			arr12[ 0 ] = arr[ 1 ];
			arr12[ 1 ] = arr[ 0 ];
		} // else

		// decide where to put arr12 and the third original value arr[ 3 ]
		if( arr12[ 1 ] > arr[ 2 ] )
		{
			arr[ 0 ] = arr12[ 0 ];
			arr[ 1 ] = arr12[ 1 ];
		} // if
		else if( arr[ 2 ] > arr12[ 0 ] )
		{
			arr[ 0 ] = arr  [ 2 ];
			arr[ 1 ] = arr12[ 0 ];
			arr[ 2 ] = arr12[ 1 ];
		} // if
		else
		{
			arr[ 0 ] = arr12[ 0 ];
			arr[ 1 ] = arr  [ 2 ];
			arr[ 2 ] = arr12[ 1 ];
		} // else
	}

	void log(const char *fmt, ...)
	{
		char text[256];
		va_list ap;

		if(fmt == NULL) {
			return;
		}

		va_start(ap, fmt);
		vsprintf(text, fmt, ap);
		va_end(ap);

		FILE* f = fopen("../log.txt", "a");
		if(!f)
			return;

		fprintf(f, text);

		fclose(f);
	}

	float max_(float a, float b)
	{
		return a>b? a:b;
	}

	float min_(float a, float b)
	{
		return a>b? b:a;
	}

	float normSquare(float a, float b)
	{
		return std::pow((a-b)/max_(a,b), 2);
	}

	float normSquareAbs(float goal, float cur)
	{
		return abs(goal-cur)/goal;
	}

	void trace(int *val, int num)
	{
		for (int i = 0; i < num; i++)
		{
			TRACE1(" %d", val[i]);
		}
	}

	std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
		std::stringstream ss(s);
		std::string item;
		while (std::getline(ss, item, delim)) {
			elems.push_back(item);
		}
		return elems;
	}

	std::vector<std::string> split(const std::string &s, char delim) {
		std::vector<std::string> elems;
		std::stringstream ss(s);
		std::string item;
		while (std::getline(ss, item, delim)) {
			elems.push_back(item);
		}
		return elems;
	}
}

