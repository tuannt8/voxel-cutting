#include "StdAfx.h"
#include "CutSurfaceTree.h"


CutSurfaceTree::CutSurfaceTree(void)
{
	m_root = nullptr;
}


CutSurfaceTree::~CutSurfaceTree(void)
{
	if (m_root)
		delete m_root;
}

void CutSurfaceTree::loadObject()
{
	m_voxelMesh.generateVoxelFromMesh("../Data_File/car.stl", 10);
	m_skeleton.initTest();
	m_skeleton.getSortedBoneArray(boneArray);
}

void CutSurfaceTree::constructCutTree()
{
	m_root = new cutNode;
	box boundingBox(m_voxelMesh.m_leftDown, m_voxelMesh.m_rightUp);
	m_root->boundingBoxes.push_back(boundingBox);
	m_root->depth = 0;

	constructChildNode(m_root);
}

void CutSurfaceTree::constructChildNode(cutNode* node)
{
	if (node->depth == boneArray.size() -1) // Child node already
		return;

	std::vector<box> pieceBox = node->getPieces();
	for (size_t i = 0; i < pieceBox.size(); i++)
	{

	}
}

cutNode::~cutNode()
{
	for (int i = 0; i < childCount; i++)
	{
		delete child[i];
	}
}

cutNode::cutNode()
{
	childCount = 0;
	parent = nullptr;
}
