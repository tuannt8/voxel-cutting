#include "stdafx.h"
#include "voxelList.h"
#include "voxelObject.h"
#include "neighbor.h"


voxelList::voxelList()
{
}


voxelList::~voxelList()
{
}

void voxelList::init()
{
	updateBoundingBox();
}

void voxelList::updateBoundingBox()
{
	Box b = getBoundingOfVoxels(m_voxelIdxs);
	m_curLeftDown3F = b.leftDown;
	m_curRightUp3F = b.rightUp;
}

Vec3f voxelList::centerPoint()
{
	if (s_corressBondP->m_type == CENTER_BONE)
	{
		Vec3f sizef = m_curRightUp3F - m_curLeftDown3F;
		sizef[0] = sizef[0] * 2;
		return m_curLeftDown3F + sizef / 2;
	}
	else
	{
		return (m_curLeftDown3F + m_curRightUp3F) / 2;
	}

}

Box voxelList::getBoundingOfVoxels(arrayInt idxs)
{
	std::vector<voxelBox> *boxes = &s_voxelObj->m_boxes;

	// Get current bounding box
	Vec3f LD(MAX, MAX, MAX);
	Vec3f RU(MIN, MIN, MIN);
	Box b(LD, RU);

	for (int i = 0; i < idxs.size(); i++)
	{
		voxelBox curB = boxes->at(idxs[i]);
		b = combineBox(b, Box(curB.leftDown, curB.rightUp));
	}

	return b;
}

Box voxelList::combineBox(Box box1, Box box2)
{
	Box newBox;
	for (int i = 0; i < 3; i++)
	{
		newBox.leftDown[i] = Util::min_(box1.leftDown[i], box2.leftDown[i]);
		newBox.rightUp[i] = Util::max_(box1.rightUp[i], box2.rightUp[i]);
	}

	return newBox;
}

arrayFloat voxelList::getErrorAssumeVoxelList(arrayInt idxs)
{
	std::vector<voxelBox> *boxes = &s_voxelObj->m_boxes;
	float voxelSize = s_voxelObj->m_voxelSizef;
	// Optimize later if work
	Box b = getBoundingOfVoxels(idxs);

	// Volume
	float volRatio = (float)idxs.size() / boxes->size();
	float volumeE = Util::normSquareAbs(getExpectedVolumeRatioSym(), volRatio);

	// aspect error
	Vec3f curSize = b.rightUp - b.leftDown;
	Vec3f xyzR = getSizeSym();

	float e2 = 0;
	for (int i = 0; i < 3; i++)
	{
		e2 += Util::normSquareAbs(xyzR[i]/xyzR[0], curSize[i] / curSize[0]);
	}

	float aspectE = e2 / 3;

	// Fill error
	float volF = idxs.size()*pow(voxelSize, 3);
	float boxVolF = curSize[0] * curSize[1] * curSize[2];
	float fillRatio = volF / boxVolF;

	// Assign
	arrayFloat err;
	err.resize(ERROR_TYPE_NUM);

	err[ASPECT_ERROR] = aspectE;
	err[VOLUME_ERROR] = volumeE;
	err[FILL_RATIO] = 1 - fillRatio;

	return err;
}

float voxelList::getExpectedVolumeRatioSym()
{
	if (s_corressBondP->m_type == CENTER_BONE)
	{
		return m_volumeRatio / 2.0;
	}
	else
	{
		return m_volumeRatio;
	}
}

Vec3f voxelList::getSizeSym()
{
	Vec3f s = m_BoxSize3F;
	if (s_corressBondP->m_type == CENTER_BONE)
	{
		s[0] = s[0] / 2;
	}

	return s;
}

void voxelList::updateIdxs(arrayInt idxs)
{
	m_voxelIdxs = idxs;
	updateBoundingBox();
}

void voxelList::drawWire()
{
	std::vector<voxelBox> * boxes = &s_voxelObj->m_boxes;
	for (int i = 0; i < m_voxelIdxs.size(); i++)
	{
		voxelBox curB = boxes->at(m_voxelIdxs[i]);
		Util_w::drawBoxWireFrame(curB.leftDown, curB.rightUp);
	}
}

void voxelList::drawFace()
{
	std::vector<voxelBox> * boxes = &s_voxelObj->m_boxes;
	for (int i = 0; i < m_voxelIdxs.size(); i++)
	{
		voxelBox curB = boxes->at(m_voxelIdxs[i]);
		Util_w::drawBoxSurface(curB.leftDown, curB.rightUp);
	}
}
