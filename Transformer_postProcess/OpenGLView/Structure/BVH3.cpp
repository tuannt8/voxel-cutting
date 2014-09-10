#include "../stdafx.h"
#include "BVH3.h"

#include "../Utility.h"

float BVH3::RandFloat(float a_min, float a_max)
{
	const float ooMax = 1.0f / (float)(RAND_MAX+1);

	float retValue = ( (float)rand() * ooMax * (a_max - a_min) + a_min);

	ASSERT(retValue >= a_min && retValue < a_max); // Paranoid check

	return retValue;
}

BVH3::BVH3()
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
BVH3::~BVH3()
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

void BVH3::insert()
{
	SomeThing* newThing = new SomeThing;
	newThing->m_min = Vec3(RandFloat(-MAX_WORLDSIZE, MAX_WORLDSIZE), RandFloat(-MAX_WORLDSIZE, MAX_WORLDSIZE), RandFloat(-MAX_WORLDSIZE, MAX_WORLDSIZE));
	Vec3 extent = Vec3(RandFloat(0, FRAC_WORLDSIZE), RandFloat(0, FRAC_WORLDSIZE), RandFloat(0, FRAC_WORLDSIZE));
	newThing->m_max = newThing->m_min + extent;

	tree.Insert(newThing->m_min.v, newThing->m_max.v, newThing);
}

void BVH3::drawBox()
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
void BVH3::drawSearchBox()
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

void BVH3::drawRectangle(SomeThing* rect, bool intesect)
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

void BVH3::render()
{
	drawSearchBox();
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
}

bool BVH3::BoxesIntersect(const Vec3& a_boxMinA, const Vec3& a_boxMaxA, 
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


float BVH3::RandFloat(float a_min, float a_max)
{
	const float ooMax = 1.0f / (float)(RAND_MAX+1);

	float retValue = ( (float)rand() * ooMax * (a_max - a_min) + a_min);

	ASSERT(retValue >= a_min && retValue < a_max); // Paranoid check

	return retValue;
}

