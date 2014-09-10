#include "StdAfx.h"
#include "BoneMapTree.h"


BoneMapTree::BoneMapTree(void)
{
	m_root = nullptr;
	mark = nullptr;
}


BoneMapTree::~BoneMapTree(void)
{
	if (m_root)
	{
		delete m_root;
	}
	if (mark)
	{
		delete mark;
	}
}

void BoneMapTree::constructTree()
{
	m_root = new BoneMapTreeNode;
	m_root->depth = 0;
	m_root->indexOfMesh = -1;

	mark = new int[sortedBone->size()];
	std::fill(mark, mark + sortedBone->size(), 0);
	constructTree(m_root);
}

void BoneMapTree::constructTree(BoneMapTreeNode *node)
{
	if (node->depth == sortedBone->size())
	{// leaf
		boneMeshIdxMapRecur(node, &node->boneMeshIdxMap);
		leaves.push_back(node);
		return;
	}

	// Center bone should be map with center box
	// Side bone should be map with side box
	std::vector<int> availableChild;
	int boneIdx = node->depth; // Current box
	if (boneIdx < nbCenterBone)
	{
		for (int i = 0; i < nbCenterBone; i++)
		{
			if (mark[i] != 1)
			{
				availableChild.push_back(i);
			}
		}
	}
	else
	{
		for (int i = nbCenterBone; i < sortedBone->size(); i++)
		{
			if (mark[i] != 1)
			{
				availableChild.push_back(i);
			}
		}
	}


	for (int i = 0; i < availableChild.size(); i++)
	{
		BoneMapTreeNode *newNode = new BoneMapTreeNode;
		newNode->depth = node->depth + 1;
		newNode->indexOfMesh = availableChild[i];
		newNode->parent = node;

		mark[availableChild[i]] = 1;
		node->children.push_back(newNode);
		constructTree(newNode);
		mark[availableChild[i]] = 0;
	}
}

int BoneMapTree::findIdx(std::vector<bone*>* v, bone* e)
{
	std::vector<bone*>::iterator it = std::find(v->begin(), v->end(), e);
	return std::distance(v->begin(), it);
}

void BoneMapTree::boneMeshIdxMapRecur(BoneMapTreeNode * node, std::map<int,int>* boneMeshIdxMap)
{
	if (node->depth == 0)
	{
		return;
	}

	int boneIdx = node->depth -1 ;
	int meshIdx = node->indexOfMesh;

	boneMeshIdxMap->insert(std::pair<int, int>(boneIdx, meshIdx));

	if (node->parent)
	{
		boneMeshIdxMapRecur(node->parent, boneMeshIdxMap);
	}
}

BoneMapTreeNode::BoneMapTreeNode()
{
	parent = nullptr;
}

BoneMapTreeNode::~BoneMapTreeNode()
{
	for (int i = 0; i < children.size(); i++)
	{
		delete children[i];
	}
}
