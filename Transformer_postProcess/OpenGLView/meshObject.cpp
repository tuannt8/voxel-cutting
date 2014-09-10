#include "stdafx.h"
#include "meshObject.h"


meshObject::meshObject()
{
}

meshObject::~meshObject()
{
}

void meshObject::initOther()
{
	leftDown = Vec3f(MAX, MAX, MAX);
	rightUp = Vec3f(MIN, MIN, MIN);

	std::vector<cVertex> * vertices = &m_polyMesh->vertices;
	for (int j = 0; j < vertices->size(); j++)
	{
		cVertex curV = vertices->at(j);
		Vec3f curP(curV.v[0], curV.v[1], curV.v[2]);

		for (int k = 0; k < 3; k++)
		{
			if (leftDown[k] > curP[k])
			{
				leftDown[k] = curP[k];
			}
			if (rightUp[k] < curP[k])
			{
				rightUp[k] = curP[k];
			}
		}
	}
}
