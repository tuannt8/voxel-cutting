#pragma once
#include "DataTypes/Define.h"
#include "DataTypes/Vec.h"
#include "DataTypes/Mat.h"
#include <vector>
#include "Voxel.h"

// Octree for voxel representation
#define I_MIN -9999
#define I_MAX 9999

class OctreeIntegerNode
{
public:
	OctreeIntegerNode();
	~OctreeIntegerNode();

public:
	std::shared_ptr<OctreeIntegerNode> m_childNode[8];
	Vec3i m_LeftDown;
	Vec3i m_UpRight;
	int m_depth;

	std::vector<int> voxelIdx;
};

typedef std::shared_ptr<OctreeIntegerNode> OctreeIntegerNodePtr;

class OctreeInteger
{
public:
	OctreeInteger(void);
	~OctreeInteger(void);

	void initOctree(); // depth to individual voxel
public:
	VoxelObjPtr voxelObj;
	OctreeIntegerNodePtr m_root;
};

