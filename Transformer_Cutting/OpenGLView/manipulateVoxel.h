#pragma once
#include "stdafx.h"
#include "voxelObject.h"
#include "bvhVoxel.h"
#include "voxelList.h"
#include <functional>

class manipulateVoxel
{
public:
	manipulateVoxel();
	~manipulateVoxel();

	// Load file that stored by previous swapping
	void loadFromFile();
	void cutCenterBoxByHalf();
	std::vector<arrayInt> getListOfVoxelIdxs();

	void draw(BOOL mode[10]);

	// Not important function
	void updateParam(int idx1, int idx2);
	void resolveVoxelBox();

public: // Variable
	voxelObject *s_voxelObj;
	skeleton *s_skeleton;

	// Data for this class
	std::vector<bone*> boneArray;
	arrayVec3i coords;
	std::vector<voxelList*> meshBox;

	// Data for manipulate
	float curScaleF;

private: // Bump variable
	arrayInt m_hashBoxIdx;

private: // bump function
	void assignBoneSizeToMesh();
	void drawBoxInterfere();
	std::vector<arrayInt> markVoxelHash();
	int getNearestBox(int voxelIdx);
	std::vector<arrayFloat> getCurrentErrorOfBox(const std::vector<arrayInt> &boxVoxelIdxs);
	void drawVoxelBox();

};

typedef std::shared_ptr<manipulateVoxel> manipulateVoxelPtr;