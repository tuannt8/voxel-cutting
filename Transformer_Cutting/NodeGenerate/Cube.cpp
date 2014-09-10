#include "stdafx.h"
#include "Cube.h"
#include <gl\gl.h>
#include <gl\glu.h>
#include "Utility_wrap.h"

void Cube::draw(int mode)
{

	if (mode & VOXEL_POINT)//Draw point
	{
		glBegin(GL_POINT);
		glVertex3fv(m_centerPos.data());
		glEnd();
	}

	if (mode & VOXEL_SPHERE)
	{
		GLUquadricObj *qobj = 0;
		qobj = gluNewQuadric();

		glPushMatrix();
		glTranslatef(m_centerPos[0],m_centerPos[1],m_centerPos[2]);
		gluSphere(qobj, m_voxelSize/4, 5, 5);
		glPopMatrix();
	}

	if (mode & VOXEL_CUBE_WIRE)
	{
		Vec3f diag(m_voxelSize/2, m_voxelSize/2, m_voxelSize/2);
		Util_w::drawBoxWireFrame(m_centerPos-diag, m_centerPos+diag);
	}

	if (mode & VOXEL_CUBE_SOLID)
	{
		Vec3f diag(m_voxelSize/2, m_voxelSize/2, m_voxelSize/2);
		Util_w::drawBoxSurface(m_centerPos-diag, m_centerPos+diag);
	}
}

Cube::Cube(Vec3f &ptf, float &sizeC)
{
	m_pos = Util_w::XYZ2IJK(ptf, sizeC);
	m_voxelSize = sizeC;
	m_centerPos = Util_w::IJK2XYZ(m_pos, m_voxelSize);
}

Cube::Cube()
{

}

Cube::~Cube()
{

}
