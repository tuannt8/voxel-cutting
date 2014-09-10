#pragma once

#include "RTree.h"

/// Simplify handling of 3 dimensional coordinate
struct Vec3
{
	/// Default constructor
	Vec3() {}

	/// Construct from three elements
	Vec3(float a_x, float a_y, float a_z)
	{
		v[0] = a_x;
		v[1] = a_y;
		v[2] = a_z;
	}

	/// Add two vectors and return result
	Vec3 operator+ (const Vec3& a_other) const
	{
		return Vec3(v[0] + a_other.v[0], 
			v[1] + a_other.v[1],
			v[2] + a_other.v[2]);
	}  

	Vec3 operator- (const Vec3& a_other) const
	{
		return Vec3(v[0] - a_other.v[0], 
			v[1] - a_other.v[1],
			v[2] - a_other.v[2]);
	} 

	float v[3];                                     ///< 3 float components for axes or dimensions
};

/// A user type to test with, instead of a simple type such as an 'int'
struct SomeThing
{
	SomeThing()
	{
//		++s_outstandingAllocs;
	}
	~SomeThing()
	{
//		--s_outstandingAllocs;
	}

	int m_creationCounter;                          ///< Just a number for identifying within test program
	Vec3 m_min, m_max;                              ///< Minimal bounding rect, values must be known and constant in order to remove from RTree

//	static int s_outstandingAllocs;                 ///< Count how many outstanding objects remain
};

const int NUM_OBJECTS = 20; // Number of objects in test set, must be > FRAC_OBJECTS for this test
const int FRAC_OBJECTS = 4;
const float MAX_WORLDSIZE = 15.0f;
const float FRAC_WORLDSIZE = MAX_WORLDSIZE / 2;

// Init static
//int SomeThing::s_outstandingAllocs = 0;
typedef RTree<SomeThing*, float, 2> SomeThingTree;
class BVH3
{
public:
	SomeThingTree tree;
	int nrects;
	Vec3 searchMin;
	Vec3 searchMax;



	float RandFloat(float a_min, float a_max)
	{
		const float ooMax = 1.0f / (float)(RAND_MAX+1);

		float retValue = ( (float)rand() * ooMax * (a_max - a_min) + a_min);

		ASSERT(retValue >= a_min && retValue < a_max); // Paranoid check

		return retValue;
	}

	BVH3()
	{
		searchMin = Vec3(0,0,0);
		searchMax = Vec3(FRAC_WORLDSIZE, FRAC_WORLDSIZE, FRAC_WORLDSIZE);
		int index; // general purpose counter, declared here because MSVC 6 is not ansii compliant with 'for' loops.
		// Add some nodes
		for( index = 0; index < NUM_OBJECTS; ++index )
		{
			insert();
		}

	}
	~BVH3()
	{
		SomeThingTree::Iterator it;
		for( tree.GetFirst(it); !tree.IsNull(it); tree.GetNext(it) )
		{
			SomeThing* removeElem = tree.GetAt(it);
			if(removeElem)
			{
				delete removeElem;
			}
		}
		tree.RemoveAll();
	}

	void insert()
	{
		SomeThing* newThing = new SomeThing;
		newThing->m_min = Vec3(RandFloat(-MAX_WORLDSIZE, MAX_WORLDSIZE), RandFloat(-MAX_WORLDSIZE, MAX_WORLDSIZE), RandFloat(-MAX_WORLDSIZE, MAX_WORLDSIZE));
		Vec3 extent = Vec3(RandFloat(0, FRAC_WORLDSIZE), RandFloat(0, FRAC_WORLDSIZE), RandFloat(0, FRAC_WORLDSIZE));
		newThing->m_max = newThing->m_min + extent;

		tree.Insert(newThing->m_min.v, newThing->m_max.v, newThing);
	}

	void drawBox()
	{
		glFrontFace(GL_CW);
		/* draws the sides of a unit cube (0,0,0)-(1,1,1) */
		glBegin(GL_POLYGON);/* f1: front */
		glNormal3f(-1.0f,0.0f,0.0f);
		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(0.0f,0.0f,1.0f);
		glVertex3f(1.0f,0.0f,1.0f);
		glVertex3f(1.0f,0.0f,0.0f);
		glEnd();
		glBegin(GL_POLYGON);/* f2: bottom */
		glNormal3f(0.0f,0.0f,-1.0f);
		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(1.0f,0.0f,0.0f);
		glVertex3f(1.0f,1.0f,0.0f);
		glVertex3f(0.0f,1.0f,0.0f);
		glEnd();
		glBegin(GL_POLYGON);/* f3:back */
		glNormal3f(1.0f,0.0f,0.0f);
		glVertex3f(1.0f,1.0f,0.0f);
		glVertex3f(1.0f,1.0f,1.0f);
		glVertex3f(0.0f,1.0f,1.0f);
		glVertex3f(0.0f,1.0f,0.0f);
		glEnd();
		glBegin(GL_POLYGON);/* f4: top */
		glNormal3f(0.0f,0.0f,1.0f);
		glVertex3f(1.0f,1.0f,1.0f);
		glVertex3f(1.0f,0.0f,1.0f);
		glVertex3f(0.0f,0.0f,1.0f);
		glVertex3f(0.0f,1.0f,1.0f);
		glEnd();
		glBegin(GL_POLYGON);/* f5: left */
		glNormal3f(0.0f,1.0f,0.0f);
		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(0.0f,1.0f,0.0f);
		glVertex3f(0.0f,1.0f,1.0f);
		glVertex3f(0.0f,0.0f,1.0f);
		glEnd();
		glBegin(GL_POLYGON);/* f6: right */
		glNormal3f(0.0f,-1.0f,0.0f);
		glVertex3f(1.0f,0.0f,0.0f);
		glVertex3f(1.0f,0.0f,1.0f);
		glVertex3f(1.0f,1.0f,1.0f);
		glVertex3f(1.0f,1.0f,0.0f);
		glEnd();
	}
	void drawEdgeBox()
	{
		glPushMatrix();
		glTranslatef(searchMin.v[0], searchMin.v[1], searchMin.v[2]);
		Vec3 scale = searchMax - searchMin;
		glScaled(scale.v[0], scale.v[1], scale.v[2]);

		glBegin(GL_LINES);/* f1: front */
		glNormal3f(-1.0f,0.0f,0.0f);
		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(0.0f,0.0f,1.0f);
		glVertex3f(1.0f,0.0f,1.0f);
		glVertex3f(1.0f,0.0f,0.0f);
		glEnd();
		glBegin(GL_LINES);/* f2: bottom */
		glNormal3f(0.0f,0.0f,-1.0f);
		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(1.0f,0.0f,0.0f);
		glVertex3f(1.0f,1.0f,0.0f);
		glVertex3f(0.0f,1.0f,0.0f);
		glEnd();
		glBegin(GL_LINES);/* f3:back */
		glNormal3f(1.0f,0.0f,0.0f);
		glVertex3f(1.0f,1.0f,0.0f);
		glVertex3f(1.0f,1.0f,1.0f);
		glVertex3f(0.0f,1.0f,1.0f);
		glVertex3f(0.0f,1.0f,0.0f);
		glEnd();
		glBegin(GL_LINES);/* f4: top */
		glNormal3f(0.0f,0.0f,1.0f);
		glVertex3f(1.0f,1.0f,1.0f);
		glVertex3f(1.0f,0.0f,1.0f);
		glVertex3f(0.0f,0.0f,1.0f);
		glVertex3f(0.0f,1.0f,1.0f);
		glEnd();
		glBegin(GL_LINES);/* f5: left */
		glNormal3f(0.0f,1.0f,0.0f);
		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(0.0f,1.0f,0.0f);
		glVertex3f(0.0f,1.0f,1.0f);
		glVertex3f(0.0f,0.0f,1.0f);
		glEnd();
		glBegin(GL_LINES);/* f6: right */
		glNormal3f(0.0f,-1.0f,0.0f);
		glVertex3f(1.0f,0.0f,0.0f);
		glVertex3f(1.0f,0.0f,1.0f);
		glVertex3f(1.0f,1.0f,1.0f);
		glVertex3f(1.0f,1.0f,0.0f);
		glEnd();
		glPopMatrix();
	}
	void drawSearchBox()
	{
		glEnable(GL_BLEND); //Enable alpha blending
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(0.7, 0.5, 0.2, 0.4);
		glPushMatrix();
		glTranslatef(searchMin.v[0], searchMin.v[1], searchMin.v[2]);
		Vec3 scale = searchMax - searchMin;
		glScaled(scale.v[0], scale.v[1], scale.v[2]);
		drawBox();
		glPopMatrix();
		glDisable(GL_BLEND);
	}

	void drawRectangle(SomeThing* rect, bool intesect)
	{
		if(intesect)
			glColor3f(1.0,0.0,0.0);
		else
			glColor3f(0.0,1.0,0.0);

		Vec3 min = rect->m_min;
		Vec3 max = rect->m_max;
		Vec3 direc = max - min;

		glPushMatrix();
		glTranslatef(min.v[0], min.v[1], min.v[2]);
		//glScalef(-2,2,2);
		glScalef(direc.v[0], direc.v[1], direc.v[2]);
		drawBox();
		glPopMatrix();
	}

	void render()
	{
		SomeThingTree::Iterator it;
		for( tree.GetFirst(it); !tree.IsNull(it); tree.GetNext(it) )
		{
			SomeThing* curThing = tree.GetAt(it);

			if(BoxesIntersect(searchMin, searchMax, curThing->m_min, curThing->m_max))
			{
				drawRectangle(curThing, true);
			}
			else
			{
				drawRectangle(curThing, false);
			}

		}

		drawSearchBox();
	}

	bool BoxesIntersect(const Vec3& a_boxMinA, const Vec3& a_boxMaxA, 
		const Vec3& a_boxMinB, const Vec3& a_boxMaxB)
	{
		for(int axis=0; axis<3; ++axis)
		{
			if(a_boxMinA.v[axis] > a_boxMaxB.v[axis] ||
				a_boxMaxA.v[axis] < a_boxMinB.v[axis])
			{
				return false;
			}
		}
		return true;
	}

};