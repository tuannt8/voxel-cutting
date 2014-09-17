#include "stdafx.h"
#include "sphere.h"


sphere::sphere()
{
	m_radius = 0;
}

sphere::sphere(Vec3f centerPos, float r)
{
	m_center = centerPos;
	m_radius = r;
}


sphere::~sphere()
{
}

Vec3f & sphere::centerPos()
{
	return m_center;
}

float & sphere::radius()
{
	return m_radius;
}

void sphere::drawSphere() const
{
	glPushMatrix();
	glEnable(GL_CULL_FACE);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
	glEnable(GL_BLEND);

	glColor4f(1, 0, 0, 0.2);
	glTranslatef(m_center[0], m_center[1], m_center[2]);
	glutSolidSphere(m_radius, 10, 10);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	glPopMatrix();
}

skeSphere::skeSphere()
{

}

skeSphere::skeSphere(Vec3f centerPos, float r) 
: sphere(centerPos, r)
{

}

skeSphere::skeSphere(skeSphere * s)
: sphere(s->centerPos(), s->radius())
{

}

skeSphere::~skeSphere()
{

}

void skeSphere::draw() const
{
	drawSphere();
}

meshSphere::meshSphere(Vec3f c, float r) 
: sphere(c,r)
{

}

meshSphere::~meshSphere()
{

}
