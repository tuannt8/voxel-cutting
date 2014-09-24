#include "stdafx.h"
#include "nonSymDoc.h"

using namespace std;

nonSymDoc::nonSymDoc()
: idx1(0)
,idx2(0)
{
	loadFile();
}


nonSymDoc::~nonSymDoc()
{
}

void nonSymDoc::draw(BOOL mode[10])
{
	if (mode[1] && m_surObj)
	{
		glColor3f(0.7, 0.7, 0.7);
		m_surObj->drawObject();
	}
	if (mode[2] && m_voxelObj)
	{
		glColor3f(0.8, 0, 0);
		m_voxelObj->drawVoxelLeaf(0, 0.9);
		glColor3f(0.8, 0.8, 0.8);
		m_voxelObj->drawVoxelLeaf(1, 0.9);
	}
	if (mode[3] && m_cutTree)
	{
		m_cutTree->drawLeave(idx1);
	}
}

void nonSymDoc::draw2(bool mode[10])
{
	if (mode[0] && m_skeleton)
	{
		m_skeleton->draw();
	}
}

void nonSymDoc::receiveKey(char c)
{
	if (c == 'C')
	{
		constructTree();
	}
}

void nonSymDoc::updateIdx(int yIdx, int zIdx)
{
	idx1 = yIdx;
	idx2 = zIdx;
}

void nonSymDoc::updateRealtime()
{

}

void nonSymDoc::loadFile()
{
	//1. Skeleton
	char *skePath = "../../Data/skeleton.xml";
	cout << "Skeleton: " << skePath << endl;
	m_skeleton = skeletonPtr(new skeleton);
	m_skeleton->loadFromFile(skePath);

	//2. Surface object
	char *surPath = "../../Data/Fighter/fighter.stl";
	cout << "Surface path: " << surPath << endl;
	m_surObj = SurfaceObjPtr(new SurfaceObj);
	m_surObj->readObjDataSTL(surPath);
	m_surObj->centerlize();
	m_surObj->constructAABBTree();

	//3. Voxel
	int res = 4;
	cout << "Voxel resolution: " << res << endl;
	m_voxelObj = voxelObjectPtr(new voxelObject);
	m_voxelObj->init(m_surObj.get(), res);

}

void nonSymDoc::constructTree()
{
	m_skeleton->computeTempVar();
	m_skeleton->groupBone();

	m_cutTree = nonSymTreePtr(new nonSymTree);
	m_cutTree->s_groupSkeleton = m_skeleton;
	m_cutTree->surObj = m_surObj;
	m_cutTree->voxelObj = m_voxelObj;
	m_cutTree->init();
}
