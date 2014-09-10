#include "StdAfx.h"
#include "OctreeInteger.h"


OctreeIntegerNode::OctreeIntegerNode()
{

}

OctreeIntegerNode::~OctreeIntegerNode()
{

}

OctreeInteger::OctreeInteger(void)
{
}


OctreeInteger::~OctreeInteger(void)
{
}

void OctreeInteger::initOctree()
{
	std::vector<Cube> cubes = voxelObj->m_cubes;
	
	// 1. Bounding box
	Vec3i leftDown(I_MAX, I_MAX, I_MAX);
	Vec3i upRight(I_MIN, I_MIN, I_MIN);

	for (int i = 0; i < cubes.size(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (leftDown[j] < cubes[i].m_pos[j])
				leftDown[j] = cubes[i].m_pos[j];

			if (upRight[j] > cubes[i].m_pos[j])
				upRight[j] = cubes[i].m_pos[j];
		}
	}


	// Root node
	m_root = OctreeIntegerNodePtr(new OctreeIntegerNode);
	m_root->m_LeftDown = leftDown;
	m_root->m_UpRight = upRight;
	m_root->m_depth = 0;
	
	// Generate 8 child

}
