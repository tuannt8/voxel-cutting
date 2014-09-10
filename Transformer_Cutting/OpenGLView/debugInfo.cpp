#include "stdafx.h"
#include "debugInfo.h"


debugInfo::debugInfo()
{
}


debugInfo::~debugInfo()
{
}

void debugInfo::reload()
{
	pair[0] ++;
	int idx = s_voxelObj->m_hashTable.getSymmetricBox(pair[0]);
	pair[1] = idx;
}

void debugInfo::reloadData()
{
	FILE * f = fopen("../test/debugInfo.txt", "r");
	ASSERT(f);

	data.clear();

	int row, col;
	fscanf(f, "%d %d", &row, &col);
	for (int i = 0; i < row; i++)
	{
		std::vector <std::string> d;
		for (int j = 0; j < col; j++)
		{
			char tmp[101];
			fscanf(f, "%s", tmp);
			d.push_back(std::string(tmp));
		}
		data.push_back(d);
	}

	fclose(f);

	// Load int array
	voxelIdxs.clear();
	for (int i = 0; i < data.size(); i++)
	{
		voxelIdxs.push_back(atoi(data[i][1].c_str()));
	}
}

void debugInfo::drawVoxelIdxs()
{
	std::vector<voxelBox> *boxes = &s_voxelObj->m_boxes;
	for (int i = 0; i < voxelIdxs.size(); i++)
	{
		Util_w::drawBoxSurface(boxes->at(voxelIdxs[i]).leftDown, boxes->at(voxelIdxs[i]).rightUp);
	}
}

void debugInfo::drawtestVoxelSym()
{

	int idx = s_voxelObj->m_hashTable.getSymmetricBox(pair[0]);
	pair[1] = idx;
	std::vector<voxelBox> *boxes = &s_voxelObj->m_boxes;

	for (int i = 0; i < 2; i++)
	{
		if (pair[i] != -1)
		{
			Util_w::drawBoxSurface(boxes->at(pair[i]).leftDown, boxes->at(pair[i]).rightUp);
		}
	}

}
