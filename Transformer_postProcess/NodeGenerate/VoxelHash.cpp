#include "StdAfx.h"
#include "VoxelHash.h"


VoxelHash::VoxelHash(void)
{
	m_hashMat = nullptr;
}


VoxelHash::~VoxelHash(void)
{
	if (m_hashMat)
	{
		for (size_t i = 0; i < m_dimSize[0]; i++)
		{
			for (size_t j = 0; j < m_dimSize[1]; j++)
			{
				delete []m_hashMat[i][j];
			}
			delete []m_hashMat[i];
		}
	}
	delete []m_hashMat;
}

void VoxelHash::init(std::vector<Cube> &cubes, Vec3i leftDown, Vec3i rightUp)
{
	// Determine the bounding box of the voxels set
	m_leftDown = leftDown;
	m_rightUp = rightUp;

	m_dimSize = m_rightUp - m_leftDown + Vec3i(1,1,1);
	m_hashSize = m_dimSize[0]*m_dimSize[1]*m_dimSize[2];

	// Use std vector
	m_hashArray.resize(m_hashSize, INVALID_IDX);

	for (int i = 0; i < cubes.size(); i++)
	{
		int idx = hash(cubes[i].m_pos);
	//	ASSERT(m_hashArray[hash(cubes[i].m_pos)] == INVALID_IDX);
		m_hashArray[hash(cubes[i].m_pos)] = i;
	}

	// use pointer
	m_hashMat = new int **[m_dimSize[0]];
	for (size_t i = 0; i < m_dimSize[0]; i++)
	{
		m_hashMat[i] = new int *[m_dimSize[1]];
		for (size_t j = 0; j < m_dimSize[1]; j++)
		{
			m_hashMat[i][j] = new int[m_dimSize[2]];
			std::fill(m_hashMat[i][j], m_hashMat[i][j]+m_dimSize[2], INVALID_IDX);
		}
	}

	for (int i = 0; i < cubes.size(); i++)
	{
		setHash(cubes[i].m_pos, i);
	}
}

long VoxelHash::hash(int ix, int jy, int kz)
{
	Vec3i ofset = Vec3i(ix, jy, kz) - m_leftDown;
	ASSERT(ofset[0]>=0 && ofset[1]>=0 && ofset[2]>=0);
	return ofset[0]*m_dimSize[1]*m_dimSize[2] + ofset[1]*m_dimSize[2] + ofset[2];
}

long VoxelHash::hash(Vec3i ptInt)
{
	Vec3i ofset = ptInt - m_leftDown;
	ASSERT(ofset[0]>=0 && ofset[1]>=0 && ofset[2]>=0);
	return ofset[0]*m_dimSize[1]*m_dimSize[2] + ofset[1]*m_dimSize[2] + ofset[2];
}

int VoxelHash::intersectWithCube(Vec3i ptInt)
{
	if (!validateIntPoint(ptInt))
		return INVALID_IDX;

	//return m_hashArray[hash(ptInt)];
	return getHash(ptInt);
}

BOOL VoxelHash::validateIntPoint(Vec3i pt)
{
	for (int i = 0; i < 3; i++)
	{
		if (pt[i] < m_leftDown[i] || pt[i] > m_rightUp[i])
		{
			return FALSE;
		}
	}
	return TRUE;
}

void VoxelHash::setHash(Vec3i pti, int _value)
{
	Vec3i pt = pti - m_leftDown;
	m_hashMat[pt[0]][pt[1]][pt[2]] = _value;
}

int VoxelHash::getHash(Vec3i pti)
{
	Vec3i pt = pti - m_leftDown;
	return m_hashMat[pt[0]][pt[1]][pt[2]];
}
