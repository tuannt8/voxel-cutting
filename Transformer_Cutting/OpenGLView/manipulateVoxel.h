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
	void drawCoord(int boneIdx) const;
	void drawVoxelBox();
	void assignBoneSizeToMesh();
	void drawBoxInterfere();

	// Not important function
	void updateBox(int percent, arrayVec3f centerBox);
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
	arrayVec3f m_centerBox;

private: // Bump variable
	arrayInt m_hashBoxIdx;
	std::vector<arrayInt> boxVoxelIdxs;
	std::vector<arrayInt> m_voxelState;

private: // bump function

	std::vector<arrayInt> markVoxelHash();
	int getNearestBox(int voxelIdx);
	std::vector<arrayFloat> getCurrentErrorOfBox(const std::vector<arrayInt> &boxVoxelIdxs);

	int getNearestNeighborBox(int voxelIdx);
	std::vector<arrayInt> getIndependentGroup(std::vector<voxelBox>* boxes, arrayInt idxs);
	void mergeUnconnectedPiece(int meshIdx, std::vector<arrayInt> independGroup);
	arrayInt findNeighborMesh(int meshIdx, arrayInt idxs);
	float averageDistanceToMesh(arrayInt idxs, int meshIdx);

};

typedef std::shared_ptr<manipulateVoxel> manipulateVoxelPtr;