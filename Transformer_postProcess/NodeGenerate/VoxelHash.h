#pragma once
#include "DataTypes/vec.h"
#include "Cube.h"
#include "stdafx.h"
#include <vector>

#define INVALID_IDX -1

class VoxelHash
{
public:
	VoxelHash(void);
	~VoxelHash(void);

	void init(std::vector<Cube> &cubes, Vec3i leftDown, Vec3i rightUp);
	int intersectWithCube(Vec3i ptInt); // Return index of collided voxel

	long hash(int ix, int jy, int kz);// Return index in hash array
	long hash(Vec3i ptInt);
	BOOL validateIntPoint(Vec3i pt);

	// Hash array
	Vec3i m_leftDown;
	Vec3i m_rightUp;
	Vec3i m_dimSize;
	long m_hashSize;
	std::vector<int> m_hashArray;

	int ***m_hashMat;
	void setHash(Vec3i pt, int _value);
	int getHash(Vec3i pt);
};

