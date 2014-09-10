#pragma once
#include "DataTypes/vec.h"
#include "stdafx.h"


enum draw_mode{
	VOXEL_POINT = 0x00001,
	VOXEL_SPHERE = 0x00010,
	VOXEL_CUBE_WIRE = 0x00100,
	VOXEL_CUBE_SOLID = 0x01000,
};

class Cube
{
public:
	Cube();
	Cube(Vec3f &ptf, float &sizeC);
	~Cube();

	void draw(int mode);

public: // Variable
	Vec3i m_pos;		// IJK coordinate
	float m_voxelSize;	// Voxel size

	Vec3f m_centerPos;	// Position in original XYZ
};