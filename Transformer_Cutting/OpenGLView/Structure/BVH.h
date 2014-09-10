#pragma once

#include "../stdafx.h"
#include "RTree.h"
#include "../Utility.h"

struct Rect
{
	Rect()  {}

	Rect(int a_minX, int a_minY, int a_maxX, int a_maxY)
	{
		min[0] = a_minX;
		min[1] = a_minY;

		max[0] = a_maxX;
		max[1] = a_maxY;
	}


	int min[2];
	int max[2];
};

class BVH
{
public:
	RTree<int, int, 2, float> tree;
	int nrects;


	BVH()
	{

		struct Rect rects[] = 
		{
			Rect(0, 0, 2, 2), // xmin, ymin, xmax, ymax (for 2 dimensional RTree)
			Rect(5, 5, 7, 7),
			Rect(8, 5, 9, 6),
			Rect(7, 1, 9, 2),
		};

		nrects = sizeof(rects) / sizeof(rects[0]);
		for(int i=0; i<nrects; i++)
		{
			tree.Insert(rects[i].min, rects[i].max, i); // Note, all values including zero are fine in this version
		}
	}
	~BVH()
	{

	}

	void drawRectangle(int minx, int miny, int maxx, int maxy)
	{
		glBegin(GL_LINES);
		glVertex3f(minx, miny,0.0);
		glVertex3f(minx,maxy,0.0);

		glVertex3f(minx,maxy,0.0);
		glVertex3f(maxx,maxy,0.0);

		glVertex3f(maxx,maxy,0.0);
		glVertex3f(maxx,miny,0.0);

		glVertex3f(maxx,miny,0.0);
		glVertex3f(minx, miny,0.0);
		glEnd();

		Utility::printw((minx+maxx)/2.0,(miny + maxy)/2.0,0.0,"3");
	}

	void render()
	{
		int itIndex = 0;
		RTree<int, int, 2, float>::Iterator it;
		for( tree.GetFirst(it); 
			!tree.IsNull(it);
			tree.GetNext(it) )
		{
			int value = tree.GetAt(it);

			int boundsMin[2] = {0,0};
			int boundsMax[2] = {0,0};
			it.GetBounds(boundsMin, boundsMax);
			TRACE(_T("it[%d] %d = (%d,%d,%d,%d)\n"), itIndex++, value, boundsMin[0], boundsMin[1], boundsMax[0], boundsMax[1]);
			drawRectangle(boundsMin[0], boundsMin[1], boundsMax[0], boundsMax[1]);
		}
	}


};