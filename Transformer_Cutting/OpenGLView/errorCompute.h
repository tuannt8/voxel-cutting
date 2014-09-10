#pragma once

#include "stdafx.h"
#include "boneAbstract.h"
#include "cutTree.h"

#define VOLUME_COEF 0.9
#define ASPECT_COEF (1.0-VOLUME_COEF)

#define maxf(a1,a2) (a1>a2? a1:a2)

// This compute error square.
// We avoid square root

namespace errorCompute
{
	float absolutErrorBase(float base, float cur)
	{
		return abs(base - cur) / maxf(base, cur);
	}

	float normE2(float base, float cur)
	{
		return pow((float)(base-cur)/maxf(base,cur), 2);
	}
	float volumeError2(float vBoxratio, float vMeshRatio)
	{
		return normE2(vBoxratio, vMeshRatio);
	};

	float aspectE2(float aspectBox[2], float aspectMesh[2])
	{
		return normE2(aspectBox[0], aspectMesh[0]) + normE2(aspectBox[1], aspectMesh[1]); 
	}

	float errorBetwnBoneAndMesh(boneAbstract &boneOrder, meshCutRough sortedCut)
	{
		return VOLUME_COEF*volumeError2(boneOrder.volumeRatiof, sortedCut.volumeRatiof) + ASPECT_COEF*aspectE2(boneOrder.aspectRatiof, sortedCut.aspectRatiof);
	}

	float errorBtwMeshAndboneArray(std::vector<boneAbstract> & m_boneOrder, std::vector<meshCutRough> &sortedCut)
	{
		if (m_boneOrder.size() == 0)
		{
			return 0;
		}

		ASSERT(m_boneOrder.size() == sortedCut.size());

		float e2 = 0;
		for (int i =0; i < sortedCut.size(); i++)
		{
			e2 += errorBetwnBoneAndMesh(m_boneOrder[i], sortedCut[i]);
		}

		float aaa = e2/sortedCut.size();
		return aaa;
	}

	static float computeAspectError(std::vector<meshPiece> * meshBoxes, std::vector<boneAbstract> * boneOrder)
	{
		if (meshBoxes->size() == 0)
		{
			return 0;
		}

		ASSERT(meshBoxes->size() == boneOrder->size());

		float e2 = 0;
		for (int i = 0; i < meshBoxes->size(); i++)
		{
			e2 += normE2(boneOrder->at(i).aspectRatiof[0], meshBoxes->at(i).edgeRatioLarge());
			e2 += normE2(boneOrder->at(i).aspectRatiof[1], meshBoxes->at(i).edgeRatioMedium());
		}

		float aaa = e2/(meshBoxes->size()*2);
		return aaa;

	}

	static float computeVolumeError(std::vector<meshPiece> * meshBoxes, std::vector<boneAbstract> * boneOrder)
	{
		if (meshBoxes->size() == 0)
		{
			return 0;
		}

		ASSERT(meshBoxes->size() == boneOrder->size());
		float e2 = 0;
		for (int i = 0; i < meshBoxes->size(); i++)
		{
			e2 += normE2(boneOrder->at(i).volumeRatiof, meshBoxes->at(i).volumeRatio);
		}

		return e2/meshBoxes->size();
	}


}