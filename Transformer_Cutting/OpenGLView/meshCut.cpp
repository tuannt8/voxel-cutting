#include "stdafx.h"
#include "meshCut.h"


meshCut::meshCut()
{
}

meshCut::meshCut(arrayInt idxs)
{
	voxelIdx = idxs;
}


meshCut::~meshCut()
{
}

meshCutPtr meshCut::clone()
{
	meshCutPtr c = meshCutPtr(new meshCut);
	*c = *this;

	return c;
}

void meshCut::updateOther(voxelObjectPtr vObj)
{
	std::vector<voxelBox> *boxes = &vObj->m_boxes;

	// bounding box
	leftDownf = Vec3f(MAX, MAX, MAX);
	rightUpf = Vec3f(MIN, MIN, MIN);

	for (auto idx : voxelIdx)
	{
		voxelBox curB = boxes->at(idx);
		for (int i = 0; i < 3; i++)
		{
			if (leftDownf[i] > curB.leftDown[i])
			{
				leftDownf[i] = curB.leftDown[i];
			}

			if (rightUpf[i] < curB.rightUp[i])
			{
				rightUpf[i] = curB.rightUp[i];
			}
		}
	}
}
