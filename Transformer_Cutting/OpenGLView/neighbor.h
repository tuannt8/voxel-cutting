#pragma once
#include "Stdafx.h"
#include "boneTransform.h"
#include "Utility_wrap.h"
#include "cutTreeUtil.h"
#include "boneAbstract.h"



#define CENTER_BONE 0
#define SIDE_BONE 1
struct indexBone
{
	int boneType;
	int idxInArray;

	int getIdx(int nbCenterB)
	{
		if (boneType == CENTER_BONE)
		{
			return idxInArray;
		}
		else
			return idxInArray + nbCenterB;
	}
};


Vec3f XYZDirect_static[3] = {Vec3f(1,0,0), Vec3f(0,1,0), Vec3f(0,0,1)};


#define VOL_SIMILAR_ERROR 0.1
class meshCutRough
{
public:
	meshCutRough(){};
	~meshCutRough(){};

	Boxi boundingBoxi;
	int volumei;
	float volumeRatiof; // With total volume
	float aspectRatiof[2];

	// Coordinate information
	// For trickier neighbor check
	boneAbstract * corespondBone;
	Box boundingBoxf; 
	realativeCoord m_realativeCoord;
	boneTransform2 m_boneTransformToMesh;
	bool m_isCoordSet;

	float m_scale; // TO bone


	void computeAspectRatio()
	{
		Vec3i sizei = boundingBoxi.rightUp - boundingBoxi.leftDown + Vec3i(1,1,1);

		Util_w::sort3_descending(sizei);
		aspectRatiof[0] = (float)sizei[0]/sizei[2];
		aspectRatiof[1] = (float)sizei[1]/sizei[2];


	}

	void computeBoundingBoxf(float voxelSize)
	{
		Vec3f diag(voxelSize/2, voxelSize/2, voxelSize/2);
		boundingBoxf.leftDown = Util_w::IJK2XYZ(boundingBoxi.leftDown, voxelSize) - diag;
		boundingBoxf.rightUp = Util_w::IJK2XYZ(boundingBoxi.rightUp, voxelSize) + diag;

		m_isCoordSet = false;
	}



};

bool compareCutMesh_descen(meshCutRough &lhs, meshCutRough &rhs)
{
	if (lhs.volumeRatiof < (1-VOL_SIMILAR_ERROR)*rhs.volumeRatiof)
	{
		return true;
	}
	else if (lhs.volumeRatiof < (1+VOL_SIMILAR_ERROR)*rhs.volumeRatiof) // similar
	{
		return rhs.aspectRatiof[0]+rhs.aspectRatiof[1] > rhs.aspectRatiof[0]+rhs.aspectRatiof[1];
	}
	else
		return false;
}

class neighbor
{
public:
	neighbor(){};
	~neighbor(){};

	indexBone first;
	indexBone second;
};



class neighborManager
{
public:
	float faceDistanceThres;
public: // Float
	meshPiece* getBox2(neighbor neighborInfo, std::vector<meshPiece> * centerBoxf, std::vector<meshPiece> * sideBoxf)
	{
		if (neighborInfo.second.boneType == TYPE_CENTER_BONE)
			return &(*centerBoxf)[neighborInfo.second.idxInArray];
		else
			return &(*sideBoxf)[neighborInfo.second.idxInArray];
	}
	meshPiece* getBox1(neighbor neighborInfo, std::vector<meshPiece> * centerBoxf, std::vector<meshPiece> * sideBoxf)
	{
		if (neighborInfo.first.boneType == TYPE_CENTER_BONE)
			return &(*centerBoxf)[neighborInfo.first.idxInArray];
		else
			return &(*sideBoxf)[neighborInfo.first.idxInArray];
	}

	boneAbstract* getBone1(neighbor neighborInfo, std::vector<boneAbstract> * centerBone, std::vector<boneAbstract> * sideBone)
	{
		if (neighborInfo.first.boneType == TYPE_CENTER_BONE)
			return &(*centerBone)[neighborInfo.first.idxInArray];
		else
			return &(*sideBone)[neighborInfo.first.idxInArray];
	}
	boneAbstract* getBone2(neighbor neighborInfo, std::vector<boneAbstract> * centerBone, std::vector<boneAbstract> * sideBone)
	{
		if (neighborInfo.second.boneType == TYPE_CENTER_BONE)
			return &(*centerBone)[neighborInfo.second.idxInArray];
		else
			return &(*sideBone)[neighborInfo.second.idxInArray];
	}

	bool isBoxCollid(meshPiece &box1, meshPiece &box2)
	{
		Vec3f radius1 = (box1.rightUp - box1.leftDown)/2;
		Vec3f radius2 = (box2.rightUp - box2.leftDown)/2;

		Vec3f centerLength = (box1.leftDown + box1.rightUp)/2 - (box2.rightUp + box2.leftDown)/2;

		radius1 = radius1 + Vec3f(faceDistanceThres, faceDistanceThres, faceDistanceThres);

		for (int i = 0; i < 3; i++)
		{
			if (fabs(centerLength[i]) > radius1[i] + radius2[i])
			{
				return false;
			}
		}
		return true;
	}


	bool isNeighborsHaveFaceContact(std::vector<meshPiece> *centerBoxf, std::vector<meshPiece> *sideBoxf, std::vector<neighbor> &neighborInfo)
	{
		GeometricFunc geoF;
		for (int i = 0; i < neighborInfo.size(); i++)
		{
			meshPiece* box1 = getBox1(neighborInfo[i], centerBoxf, sideBoxf);
			meshPiece* box2 = getBox2(neighborInfo[i], centerBoxf, sideBoxf);

			if (!isBoxCollid(*box1, *box2))
			{
				return false;
			}
		}
		return true;
	}


public: // integer with voxel
	static bool valueInRange(int value, int min, int max)
	{
		return (value >= min) && (value <= max);
	}
	static bool isBoxiCollidContact(Boxi box1, Boxi box2)
	{
		bool overlap[3];
		box1.leftDown = box1.leftDown - Vec3i(1,1,1);
		box1.rightUp = box1.rightUp + Vec3i(1,1,1);
		for (int i = 0; i < 3; i++)
		{
		overlap[i] = valueInRange(box1.leftDown[i], box2.leftDown[i], box2.rightUp[i])
			|| valueInRange(box2.leftDown[i], box1.leftDown[i], box1.rightUp[i]);
		}

		return overlap[0]&&overlap[1]&&overlap[2];
	}

	static bool isAllNeighborContact(std::vector<meshCutRough> &sortedCut, std::vector<std::pair<int,int>> &neighborPair)
	{
		for (int i = 0; i < neighborPair.size(); i++)
		{
			if (!isBoxiCollidContact(sortedCut[neighborPair[i].first].boundingBoxi, sortedCut[neighborPair[i].second].boundingBoxi))
			{
				return false;
			}
		}

		return true;
	}

	static bool isAllNeighborContactSym(std::vector<meshCutRough> &sortedCenterCut, std::vector<meshCutRough> &sortedSideCut, std::vector<neighbor> &neighborInfo)
	{
		for (int i = 0; i < neighborInfo.size(); i++)
		{
			Boxi box1 = (neighborInfo[i].first.boneType == TYPE_CENTER_BONE)? sortedCenterCut[neighborInfo[i].first.idxInArray].boundingBoxi : sortedSideCut[neighborInfo[i].first.idxInArray].boundingBoxi;

			Boxi box2 = (neighborInfo[i].second.boneType == TYPE_CENTER_BONE)? sortedCenterCut[neighborInfo[i].second.idxInArray].boundingBoxi : sortedSideCut[neighborInfo[i].second.idxInArray].boundingBoxi;

			if (!isBoxiCollidContact(box1, box2))
			{
				return false;
			}
		}

		return true;
	}

	float scoreNeighborPair(meshPiece *meshBox1, meshPiece *meshBox2, boneAbstract *bone11, boneAbstract *bone22)
	{
		// Because of symmetric, there are 4 possible coords to meshbox
		boneAbstract bone1 = *bone11;
		boneAbstract bone2 = *bone22;
		// We need to define Y and Z coord only
		std::vector<realativeCoord> posibleCoord1 = getPossibleCoordf(meshBox1, &bone1);
		std::vector<realativeCoord> posibleCoord2 = getPossibleCoordf(meshBox2, &bone2);

		// Find best match
		float nearest = MAX;
		int iIdx = -1, jIdx = -1;
		boneTransform2 transfrom1, transform2;

		for (int i = 0; i < posibleCoord1.size(); i++)
		{
			for (int j = 0; j < posibleCoord2.size(); j++)
			{
				boneTransform2 trans1, trans2;
 				trans1.init(posibleCoord1[i], meshBox1->leftDown, meshBox1->rightUp, bone1.original->leftDownf, bone1.original->rightUpf);
 				trans2.init(posibleCoord2[j], meshBox2->leftDown, meshBox2->rightUp, bone2.original->leftDownf, bone2.original->rightUpf);

				Vec3f jointParent, jointChild;
				float longestEdge;
				if (bone1.original->parent == bone2.original)
				{
					// bone1 is child
					jointChild = trans1.transform(Vec3f(0,0,0));
					jointParent = trans2.transform(bone1.original->m_jointBegin);
					longestEdge = meshBox2->sizef[meshBox2->SMLEdgeIdx[2]];
				}
				else
				{
					// bone2 is child
					jointParent = trans1.transform(bone1.original->m_jointBegin);
					jointChild = trans2.transform(Vec3f(0,0,0));
					longestEdge = meshBox1->sizef[meshBox1->SMLEdgeIdx[2]];
				}


				Vec3f distanceJ = jointChild - jointParent;
				// Now compare the distance with longest edge of parent
				float err = distanceJ.norm()/longestEdge;
				if (err < nearest)
				{
					nearest = err;
					iIdx = i;
					jIdx = j;
				}
			}
		}

		if (iIdx!= -1 && !meshBox1->m_isCoordSet)
		{
			realativeCoord cc = posibleCoord1[iIdx];
			meshBox1->m_realativeCoord = cc;
			meshBox1->m_isCoordSet = true;
		}
		if (jIdx!= -1 && !meshBox2->m_isCoordSet)
		{
			realativeCoord cc = posibleCoord2[jIdx];
			meshBox2->m_realativeCoord = cc;
			meshBox2->m_isCoordSet = true;
		}

		return nearest;
	}

	float neighborScore(std::vector<meshPiece> *centerBoxf, std::vector<meshPiece> *sideBoxf, std::vector<boneAbstract>* centerBoneOrder, std::vector<boneAbstract>* sideBoneOrder, std::vector<neighbor>* neighborInfo)
	{
		float score = 0;
		for (int i = 0; i < neighborInfo->size(); i++)
		{
			meshPiece* meshBox1 = getBox1((*neighborInfo)[i], centerBoxf, sideBoxf);
			meshPiece* meshBox2 = getBox2((*neighborInfo)[i], centerBoxf, sideBoxf);

			boneAbstract* bone1 = getBone1((*neighborInfo)[i], centerBoneOrder, sideBoneOrder);
			boneAbstract* bone2 = getBone2((*neighborInfo)[i], centerBoneOrder, sideBoneOrder);

			score += scoreNeighborPair(meshBox1, meshBox2, bone1, bone2);
		}

		float aaa = score/neighborInfo->size();
		return aaa;
	}

	static float neighborScore(std::vector<meshCutRough> sortedCenterCut, std::vector<meshCutRough> sortedSideCut, std::vector<boneAbstract> centerBoneOrder, std::vector<boneAbstract> sideBoneOrder, std::vector<neighbor> neighborInfo)
	{
		// Check neighbor score
		// If joint is near to each other
		float score = 0;
		for (int i = 0; i < neighborInfo.size(); i++)
		{
			// lazy check test
			meshCutRough* meshBox1 = (neighborInfo[i].first.boneType == TYPE_CENTER_BONE)? &sortedCenterCut[neighborInfo[i].first.idxInArray] : &sortedSideCut[neighborInfo[i].first.idxInArray];
			meshCutRough* meshBox2 = (neighborInfo[i].second.boneType == TYPE_CENTER_BONE)? &sortedCenterCut[neighborInfo[i].second.idxInArray] : &sortedSideCut[neighborInfo[i].second.idxInArray];
			boneAbstract* bone1 = (neighborInfo[i].first.boneType == TYPE_CENTER_BONE)? &centerBoneOrder[neighborInfo[i].first.idxInArray] : &centerBoneOrder[neighborInfo[i].first.idxInArray];
			boneAbstract* bone2 = (neighborInfo[i].second.boneType == TYPE_CENTER_BONE)? &centerBoneOrder[neighborInfo[i].second.idxInArray] : &sideBoneOrder[neighborInfo[i].second.idxInArray];

			meshBox1->corespondBone = bone1;
			meshBox2->corespondBone = bone2;

			score += scoreNeighborPair(meshBox1, meshBox2, bone1, bone2);
		}
		
		score /= (float)neighborInfo.size();

		return score;
	}
	static float maxEdge(Box boundingBoxf)
	{
		Vec3f diag = boundingBoxf.rightUp-boundingBoxf.leftDown;
		float max = MIN;
		for (int i = 0; i < 3; i++)
		{
			if (diag[i] > max)
			{
				max = diag[i];
			}
		}
		return max;
	}

	static float scoreNeighborPair(meshCutRough *meshBox1, meshCutRough *meshBox2, boneAbstract *bone1, boneAbstract *bone2)
	{
		// Because of symmetric, there are 4 possible coords to meshbox
		// We need to define Y and Z coord only

	//	std::vector<boneTransform2> posibleCoord1test = getPossibleboneTransform2(meshBox1, bone1);
		std::vector<realativeCoord> posibleCoord1 = getPossibleCoord(meshBox1, bone1);
		std::vector<realativeCoord> posibleCoord2 = getPossibleCoord(meshBox2, bone2);

		// Find best match
		float nearest = MAX;
		int iIdx, jIdx;
		boneTransform2 transfrom1, transform2;

		for (int i = 0; i < posibleCoord1.size(); i++)
		{
			for (int j = 0; j < posibleCoord2.size(); j++)
			{
				boneTransform2 trans1, trans2;
				trans1.init(posibleCoord1[i], meshBox1->boundingBoxf.leftDown, meshBox1->boundingBoxf.rightUp, bone1->original->leftDownf, bone1->original->rightUpf);
				trans2.init(posibleCoord2[j], meshBox2->boundingBoxf.leftDown, meshBox2->boundingBoxf.rightUp, bone2->original->leftDownf, bone2->original->rightUpf);

 				Vec3f jointParent, jointChild;
				float longestEdge;
				if (bone1->original->parent == bone2->original)
				{
					// bone1 is child
					jointChild = trans1.transform(Vec3f(0,0,0));
					jointParent = trans2.transform(bone1->original->m_jointBegin);
					longestEdge = maxEdge(meshBox2->boundingBoxf);
				}
				else
				{
					// bone2 is child
					jointParent = trans1.transform(bone1->original->m_jointBegin);
					jointChild = trans2.transform(Vec3f(0,0,0));
					longestEdge = maxEdge(meshBox1->boundingBoxf);
				}

 
 				Vec3f distanceJ = jointChild - jointParent;
				// Now compare the distance with longest edge of parent
				float err = distanceJ.norm()/longestEdge;
				if (err < nearest)
				{
					nearest = err;
					iIdx = i;
					jIdx = j;
				}
			}
		}

		if (!meshBox1->m_isCoordSet)
		{
			meshBox1->m_realativeCoord = posibleCoord1[iIdx];
		}
		if (!meshBox2->m_isCoordSet)
		{
			meshBox2->m_realativeCoord = posibleCoord2[jIdx];
		}

		return nearest;
	}

#define normIntVEqual(a,b) (a[0]==b[0] && a[1]==b[1] && a[2]==b[2])

	static Vec3i normalizedVector(Vec3f sizef)
	{
		int lengthOrder;
		// orientation
		int idxL, idxS;
		float L = MIN, S = MAX;
		for (int i = 0; i < 3; i++)
		{
			if (L < sizef[i])
			{
				idxL = i; L = sizef[i];
			}
			if (S > sizef[i])
			{
				idxS = i; S = sizef[i];
			}
		}

		if (L - S < 0.01*S) // Cube; anything is OK
		{
			return Vec3i(1,1,1);
		}
		else
		{
			int idxM = -1;
			for (int i = 0; i < 3; i++)
			{
				if ((sizef[i] != L && i!=idxS)
					|| (sizef[i] != S && i!=idxL))
				{
					idxM = i;
					break;
				}
			}
			Vec3i normalized;
			normalized[idxS] = 1;
			normalized[idxM] = 2;
			normalized[idxL] = 4;

			return normalized;
		}
	}
	static Vec3i SMLOrder(Vec3f sizef)
	{
		int lengthOrder;
		// orientation
		int idxL, idxS;
		float L = MIN, S = MAX;
		for (int i = 0; i < 3; i++)
		{
			if (L < sizef[i])
			{
				idxL = i; L = sizef[i];
			}
			if (S > sizef[i])
			{
				idxS = i; S = sizef[i];
			}
		}

		if (L - S < 0.01*S) // Cube; anything is OK
		{
			return Vec3i(0,0,0);
		}
		else
		{
			int idxM = -1;
			for (int i = 0; i < 3; i++)
			{
				if ((sizef[i] != L && i!=idxS)
					|| (sizef[i] != S && i!=idxL))
				{
					idxM = i;
					break;
				}
			}

			if (sizef[idxM] - sizef[idxS] < 0.01*sizef[idxM]
			|| sizef[idxL] - sizef[idxM] < 0.01*sizef[idxL])
			{
				return Vec3i(0,0,0); // Not cube, but lazy to check
			}

			return Vec3i(idxS, idxM, idxL);
		}
	}

	static std::vector<realativeCoord> getPossibleCoord(meshCutRough * meshBox, boneAbstract * boneIn)
	{
		std::vector<realativeCoord> coords;
		if (meshBox->m_isCoordSet)
		{
			coords.push_back(meshBox->m_realativeCoord);
			return coords;
		}
		else
		{
			Vec3f boxLocalSize = meshBox->boundingBoxf.rightUp - meshBox->boundingBoxf.leftDown;
 			Vec3i meshBoxSizeOrder = SMLOrder(boxLocalSize);

			Vec3f boneLocalSize = boneIn->original->m_sizef;
 			Vec3i boneSizeOrder = SMLOrder(boneLocalSize);

			if (meshBoxSizeOrder[0]==0 && meshBoxSizeOrder[1] == 0)//Cube
			{
				static std::vector<realativeCoord> posibleDirect = initDirectionList();
				coords = posibleDirect;
			}
			else
			{
				realativeCoord newC;
				for(int i = 0; i < 3; i++)
				{
					newC.XYZDirect[meshBoxSizeOrder[i]] = XYZDirect_static[boneSizeOrder[i]];
				//	newC.scale[meshBoxSizeOrder[i]] = 
				}

				coords.push_back(newC);
			}

			// Each of the coordinate correspond with 4 sign of Y and Z
			// compute original
			std::vector<realativeCoord> finalCoords;
			Vec3f localCenterBoxBone = (boneIn->original->rightUpf + boneIn->original->leftDownf)/2;
			Vec3f meshCenter = (meshBox->boundingBoxf.leftDown + meshBox->boundingBoxf.rightUp)/2.;
			
			//Vec3f BoneSizeInMeshCoord = 

			int direct[2] = {-1,1};
			for (int i = 0; i < coords.size(); i++)
			{
				realativeCoord curC = coords[i];
				for (int iz = 0; iz <= 1; iz ++)
				{
					for (int iy = 0; iy <= 1; iy++)
					{
						realativeCoord newCoord;
						newCoord.XYZDirect[1] = curC.XYZDirect[1]*direct[iy];
						newCoord.XYZDirect[2] = curC.XYZDirect[2]*direct[iy];
						newCoord.XYZDirect[0] = newCoord.XYZDirect[1].cross(newCoord.XYZDirect[2]);

						finalCoords.push_back(newCoord);
					}
				}
			}

			return finalCoords;
		}
	}

	std::vector<realativeCoord> getPossibleCoordf(meshPiece * meshBox, boneAbstract * boneIn)
	{
		std::vector<realativeCoord> coords;
		if (meshBox->m_isCoordSet) // Mesh already has coord
		{
			coords.push_back(meshBox->m_realativeCoord);
			return coords;
		}
		else
		{
			Vec3f boxLocalSize = meshBox->rightUp - meshBox->leftDown;
			Vec3i meshBoxSizeOrder = SMLOrder(boxLocalSize);

			Vec3f boneLocalSize = boneIn->original->m_sizef;
			Vec3i boneSizeOrder = SMLOrder(boneLocalSize);

			if (abs(boneLocalSize[boneSizeOrder[2]] - boneLocalSize[boneSizeOrder[1]]) < 0.1*boneLocalSize[boneSizeOrder[1]]
			|| abs(boneLocalSize[boneSizeOrder[1]] - boneLocalSize[boneSizeOrder[0]]) < 0.1*boneLocalSize[boneSizeOrder[0]])// Exist equal edge
			{
// 				static std::vector<realativeCoord> posibleDirect = initDirectionList();
// 				coords = posibleDirect;
				coords = initDirectionList_nonStatic();

			}
			else
			{
				realativeCoord newC;
				for(int i = 0; i < 3; i++)
				{
					newC.XYZDirect[meshBoxSizeOrder[i]] = XYZDirect_static[boneSizeOrder[i]];
				}

				coords.push_back(newC);
			}

			// Each of the coordinate correspond with 4 sign of Y and Z
			// compute original
			std::vector<realativeCoord> finalCoords;
			Vec3f localCenterBoxBone = (boneIn->original->rightUpf + boneIn->original->leftDownf)/2;
			Vec3f meshCenter = (meshBox->leftDown + meshBox->rightUp)/2.;

			int direct[2] = {-1,1};
			for (int i = 0; i < coords.size(); i++)
			{
				realativeCoord curC = coords[i];
				for (int iz = 0; iz <= 1; iz ++)
				{
					for (int iy = 0; iy <= 1; iy++)
					{
						realativeCoord newCoord;
						newCoord.XYZDirect[1] = curC.XYZDirect[1]*direct[iy];
						newCoord.XYZDirect[2] = curC.XYZDirect[2]*direct[iz];
						newCoord.XYZDirect[0] = newCoord.XYZDirect[1].cross(newCoord.XYZDirect[2]);

						finalCoords.push_back(newCoord);
					}
				}
			}

			return finalCoords;
		}
	}
	static std::vector<realativeCoord> getPossibleCoord(meshPiece * meshBox, boneAbstract * boneIn)
	{
		std::vector<realativeCoord> coords;
		if (meshBox->m_isCoordSet) // Mesh already has coord
		{
			coords.push_back(meshBox->m_realativeCoord);
			return coords;
		}
		else
		{
			Vec3f boxLocalSize = meshBox->rightUp - meshBox->leftDown;
			Vec3i meshBoxSizeOrder = SMLOrder(boxLocalSize);

			Vec3f boneLocalSize = boneIn->original->m_sizef;
			Vec3i boneSizeOrder = SMLOrder(boneLocalSize);

			if (meshBoxSizeOrder[0]==0 && meshBoxSizeOrder[1] == 0)//Cube
			{
				static std::vector<realativeCoord> posibleDirect = initDirectionList();
				coords = posibleDirect;
			}
			else
			{
				realativeCoord newC;
				for(int i = 0; i < 3; i++)
				{
					newC.XYZDirect[meshBoxSizeOrder[i]] = XYZDirect_static[boneSizeOrder[i]];
				}

				coords.push_back(newC);
			}

			// Each of the coordinate correspond with 4 sign of Y and Z
			// compute original
			std::vector<realativeCoord> finalCoords;
			Vec3f localCenterBoxBone = (boneIn->original->rightUpf + boneIn->original->leftDownf)/2;
			Vec3f meshCenter = (meshBox->leftDown + meshBox->rightUp)/2.;

			int direct[2] = {-1,1};
			for (int i = 0; i < coords.size(); i++)
			{
				realativeCoord curC = coords[i];
				for (int iz = 0; iz <= 1; iz ++)
				{
					for (int iy = 0; iy <= 1; iy++)
					{
						realativeCoord newCoord;
						newCoord.XYZDirect[1] = curC.XYZDirect[1]*direct[iy];
						newCoord.XYZDirect[2] = curC.XYZDirect[2]*direct[iy];
						newCoord.XYZDirect[0] = newCoord.XYZDirect[1].cross(newCoord.XYZDirect[2]);

						finalCoords.push_back(newCoord);
					}
				}
			}

			return finalCoords;
		}
	}

	static std::vector<boneTransform2> getPossibleBoneTransform(meshCutRough * meshBox, boneAbstract * boneIn)
	{
		std::vector<boneTransform2> coords;
		if (meshBox->m_isCoordSet)
		{
			coords.push_back(meshBox->m_boneTransformToMesh);
			return coords;
		}
		else
		{
			Vec3f boxLocalSize = meshBox->boundingBoxf.rightUp - meshBox->boundingBoxf.leftDown;
			Vec3i meshBoxSizeOrder = SMLOrder(boxLocalSize);

			Vec3f boneLocalSize = boneIn->original->m_sizef;
			Vec3i boneSizeOrder = SMLOrder(boneLocalSize);

			if (meshBoxSizeOrder[0]==0 && meshBoxSizeOrder[1] == 0)//Cube
			{
				
			}
			else
			{
				
			}
		}

		return coords;
	}

	//static std::vector<realativeCoord> posibleDirect; // orientation
	static void init()
	{

	}

	std::vector<realativeCoord> initDirectionList_nonStatic()
	{
		std::vector<realativeCoord> posibleDirect;

		for (int i = 0; i < 3; i ++)
		{
			for (int j = 0; j < 3; j ++)
			{
				if (j!= i)
				{
					realativeCoord newC;
					newC.XYZDirect[2] = XYZDirect_static[i];
					newC.XYZDirect[1] = XYZDirect_static[j];
					newC.XYZDirect[0] = newC.XYZDirect[1].cross(newC.XYZDirect[2]);
					posibleDirect.push_back(newC);
				}
			}
		}

		return posibleDirect;
	}

	static std::vector<realativeCoord> initDirectionList()
	{
		std::vector<realativeCoord> posibleDirect;

		for (int i = 0; i < 3; i ++)
		{
			for (int j = 0; j < 3; j ++)
			{
				if (j!= 1)
				{
					realativeCoord newC;
					newC.XYZDirect[2] = XYZDirect_static[i];
					newC.XYZDirect[1] = XYZDirect_static[j];
					newC.XYZDirect[0] = newC.XYZDirect[1].cross(newC.XYZDirect[2]);
					posibleDirect.push_back(newC);
				}
			}
		}

		return posibleDirect;
	}








};

//std::vector<realativeCoord> neighborManager:: posibleDirect;