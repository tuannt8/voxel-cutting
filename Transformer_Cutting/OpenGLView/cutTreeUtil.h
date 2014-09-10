#pragma once
#include "stdafx.h"
#include "boneTransform.h"
#include "boneAbstract.h"

#define VOL_SIMILAR_ERROR_MESH_PIECE 0.1
class meshPiece
{
public:
	meshPiece()
	{
		volumef = 0;
		m_isCoordSet = false;
	};
	meshPiece(Vec3f ld, Vec3f ru)
	{
		leftDown = ld;
		rightUp = ru;
		volumef = 0;
		m_isCoordSet = false;

	}
	~meshPiece()
	{

	}

	float edgeRatioLarge(){
		return sizef[SMLEdgeIdx[2]]/sizef[SMLEdgeIdx[0]];
	};
	float edgeRatioMedium(){
		return sizef[SMLEdgeIdx[1]]/sizef[SMLEdgeIdx[0]];
	};

	void computeAdditionalInfo(float totalVol)
	{
		sizef = rightUp - leftDown;
		volumeRatio = volumef/totalVol;

		// Order of the edge lengths
		if (sizef[0]>sizef[1])
		{
			if (sizef[1] > sizef[2])
				SMLEdgeIdx = Vec3i(2,1,0);
			else if (sizef[2]<sizef[0])
				SMLEdgeIdx = Vec3i(1,2,0);
			else
				SMLEdgeIdx = Vec3i(1,0,2);
		}
		else
		{
			if (sizef[1] < sizef[2])
				SMLEdgeIdx = Vec3i(0,1,2);
			else if (sizef[2]>sizef[0])
				SMLEdgeIdx = Vec3i(0,2,1);
			else
				SMLEdgeIdx = Vec3i(2,0,1);
		}
	}

	void initForVoxel()
	{
		leftDown = Vec3f(MAX, MAX, MAX);
		rightUp = Vec3f(MIN, MIN, MIN);

		volumef = 0;
	}

	void fitBOundingBox(Vec3f ld, Vec3f ru)
	{
		for (int j = 0; j < 3; j++)
		{
			if (leftDown[j] > ld[j])
				leftDown[j] = ld[j];

			if (rightUp[j] < ru[j])
				rightUp[j] = ru[j];
		}
	}

	void mergeIdx(arrayInt boxIdx, Vec3f leftDown, Vec3f rightUp)
	{
		voxels.insert(voxels.end(), boxIdx.begin(), boxIdx.end());
		fitBOundingBox(leftDown, rightUp);
	}




public:
	// Basic info
	Vec3f leftDown;
	Vec3f rightUp;
	float volumef;
	int volumei; // Number of voxel inside

	// For cutting
	std::vector<int> voxels;
	bool isLower;

	// Additional info
	// Have after call compute additional info
	// For aspect and volume scoring
	Vec3i SMLEdgeIdx; // Axis that has smallest, medium and largest length
	Vec3f sizef;
	float volumeRatio;

	// For neighbor score
	boneAbstract * corespondBone; 
	realativeCoord m_realativeCoord;
	bool m_isCoordSet;
};

bool compareMeshPiece_descen(meshPiece &lhs, meshPiece &rhs)
{
	if (lhs.volumef < (1-VOL_SIMILAR_ERROR_MESH_PIECE)*rhs.volumef)
	{
		return true;
	}
	else if (lhs.volumef < (1+VOL_SIMILAR_ERROR_MESH_PIECE)*rhs.volumef)
	{
		return lhs.edgeRatioLarge()+lhs.edgeRatioMedium() > rhs.edgeRatioLarge()+rhs.edgeRatioMedium();
	}
	else
	{
		return false;
	}
}


class cutTreefNode
{
public:
	cutTreefNode();
	~cutTreefNode();
	cutTreefNode(cutTreefNode* parentNode);

	int boxCount();
	void draw(float centerX);
	void draw(float centerX, std::vector<boneAbstract> *centerBoneOrder, std::vector<boneAbstract> *sideBoneOrder);


public:
	cutTreefNode *parent;
	std::vector<cutTreefNode*> children;
	int depth;

	// Box
	std::vector<meshPiece> centerBoxf;
	std::vector<meshPiece> sideBoxf;

	// Cut info
	int xyzd;
	float coord;

	// error
	float neightborError, aspectError, volumeError;

	// Debug
	int debugCode;
	int idx;
};

