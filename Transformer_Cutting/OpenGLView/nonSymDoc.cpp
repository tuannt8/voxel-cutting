#include "stdafx.h"
#include "nonSymDoc.h"

using namespace std;

nonSymDoc::nonSymDoc()
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
		m_voxelObj->drawVoxel();
		glColor3f(0.8, 0.8, 0.8);
		m_voxelObj->drawVoxelLeaf(1);
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

}

void nonSymDoc::updateIdx(int yIdx, int zIdx)
{

}

void nonSymDoc::updateRealtime()
{

}

void nonSymDoc::loadFile()
{
	//1. Skeleton
	char *skePath = "../../Data/Barrel/skeleton.xml";
	cout << "Skeleton: " << skePath << endl;
	m_skeleton = skeletonPtr(new skeleton);
	m_skeleton->loadFromFile(skePath);

	//2. Surface object
	char *surPath = "../../Data/Barrel/barrel.stl";
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
