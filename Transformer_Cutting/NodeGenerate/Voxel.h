// Voxel.h : main header file for the Voxel DLL
//

#pragma once
#include "DataTypes/vec.h"
#include "Cube.h"
//#include "stdafx.h"
#include "Utility_wrap.h"
#include <memory>
#include "VoxelHash.h"
#include "Graphics/Surfaceobj.h"
#include "sumTable.h"

#define VOXEL_MIN -9999
#define VOXEL_MAX 9999

class VoxelObj
{
public:
	VoxelObj();
	~VoxelObj();

	// Visualize
	void draw(drawMode mode = DRAW_WIRE);
	void drawVoxelIdx();
	void drawMesh();
	void drawTable();

	// New
	int volumeInBox(Vec3i leftDown, Vec3i rightUp);
	int volumeInBox(Boxi b)
	{
		return volumeInBox(b.leftDown, b.rightUp);
	}
	int objectVolumei();

	// Initialize

	void generateVoxelFromMesh_1(char* filePath, float res);
	void generateVoxelFromMesh(char* filePath, int res);
	void writeVoxelData(char* filePath);
	void readVoxelData(char* filePath);

	void loadVoxelFromFileFloat(char* filePath);

	BOOL intersectionWithBox(Vec3f leftDown, Vec3f rightUp, std::vector<int> &intersectedIdxs);
	BOOL intersectionWithBox(Vec3i leftDown, Vec3i rightUp, std::vector<int> &intersectedIdxs);

	// Get set
	arrayVec3i &points(){return m_points;};

	// testing
	void getPointFromSumtable();
public:
	std::vector<Cube> m_cubes;

	

	long m_volume;

	VoxelHash *hashTable;

	// Voxel data
	std::vector<Vec3i> m_points;
	float m_voxelSize;

	Vec3i m_leftDown;
	Vec3i m_rightUp;
	Vec3i m_sizei;
	int *** m_voxelArray;


	sumTable m_sumTable;

	// Mesh
	SurfaceObj m_sufObj;
};

typedef std::shared_ptr<VoxelObj> VoxelObjPtr;


