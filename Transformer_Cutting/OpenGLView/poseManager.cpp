#include "StdAfx.h"
#include "poseManager.h"
#include "errorCompute.h"
#include "groupCut.h"


poseManager::poseManager(void)
{
}


poseManager::~poseManager(void)
{
}

void poseManager::addPose(cutTreefNode * node)
{
	// 1. Define bone order
	// Does not base on bone size
	// Base on neighbor information
	std::vector<meshPiece> *centerBox = &node->centerBoxf;
	std::vector<meshPiece> *sideBox = &node->sideBoxf;

	// Define possible neighbor of box
	posibleMeshNeighbor = findPossibleNeighbor(node);
	countMeshNeighbor();

	// Possible mapping
	meshBoxes.clear();
	meshBoxes.insert(meshBoxes.end(), centerBox->begin(), centerBox->end());
	meshBoxes.insert(meshBoxes.end(), sideBox->begin(), sideBox->end());

	findPossibleMap(m_boneMapTree.m_root, node);
}

neighborPos poseManager::possibleNeighbor(meshPiece* parent, meshPiece* child)
{
	// To be neighbor, they should be in contact or there is no other box between them
	// Best is to check overlap. maximum is the size of voxel
	GeometricFunc geoF;
	Vec3f diag(voxelSizef, voxelSizef, voxelSizef);
	if(geoF.collisionBtwBox(Box(parent->leftDown, parent->rightUp), Box(child->leftDown - diag/2.0, child->rightUp + diag/2.0)))
	{
		Vec3f parentLD = parent->leftDown;
		Vec3f parentRU = parent->rightUp;
		Vec3f childLD = child->leftDown;
		Vec3f childRU = child->rightUp;
		neighborPos *pp = posArray();
		float error = 0.001*voxelSizef;
		for (int xyzd = 0; xyzd < 3; xyzd++) // Test on three direction
		{
			// Check if Plus
			if (childLD[xyzd] > parentRU[xyzd]-error)
			{
				return pp[xyzd * 2];
			}
			if (childRU[xyzd] < parentLD[xyzd] + error)
			{
				return pp[xyzd * 2 + 1];
			}
		}
	}

	return NONE_NB;
}

bool neighborPose::operator==(const neighborPose& b)
{
	return posConfigId == b.posConfigId;
}

bool neighborPose::operator<(const neighborPose& b)
{
	return posConfigId < b.posConfigId;
}

neighborPose::neighborPose()
{
	smallestVolumeError = MAX;
	smallestErrorIdx = -1;
	posConfigId = -1;
}

neighborPose::~neighborPose()
{

}

void neighborPose::computeUniqeID()
{
	// Unique ID 
	posConfigId = 0;
	int num = posConfig.size()-1;
	for (int i = 0; i < posConfig.size(); i++)
	{
		posConfigId += posConfig[i]*pow((float)NUM_POS,num-i);
	}
}

bool neighborPose::containFilter(std::vector<neighborPos> pp)
{
	for (int i = 0; i < pp.size(); i++)
	{
		if (pp[i] != NONE_NB)
		{
			if (pp[i] != posConfig[i])
			{
				return false;
			}
		}
	}

	return true;
}

neighborPos * poseManager::posArray()
{
	static neighborPos pos[6] = {X_PLUS, X_MINUS, Y_PLUS, Y_MINUS, Z_PLUS, Z_MINUS};
	return pos;	
}

std::vector<std::vector<indexBone>> poseManager::findPossibleNeighbor(cutTreefNode * node)
{
	std::vector<std::vector<indexBone>> neighborOut;
	std::vector<meshPiece> *centerBox = &node->centerBoxf;
	std::vector<meshPiece> *sideBox = &node->sideBoxf;

	// Neighbor of center box
	for (int i = 0; i < centerBox->size(); i++)
	{
		// Consider this box is parent 
		std::vector<indexBone> neighborC;

		// Center neighbor
		for (int j = 0; j < centerBox->size(); j++)
		{	
			if (i!=j)
			{
				neighborPos nbp = possibleNeighbor(&(*centerBox)[i], &(*centerBox)[j]);
				if(nbp != NONE_NB)
				{
					indexBone newN;
					newN.boneType = CENTER_BONE;
					newN.idxInArray = j;
					neighborC.push_back(newN);
				}
			}
		}

		// Side neighbor
		for (int j = 0; j < sideBox->size(); j++)
		{

			neighborPos nbp = possibleNeighbor(&(*centerBox)[i], &(*sideBox)[j]);
			if(nbp != NONE_NB)
			{
				indexBone newN;
				newN.boneType = SIDE_BONE;
				newN.idxInArray = j;
				neighborC.push_back(newN);
			}
		}

		neighborOut.push_back(neighborC);
	}

	// Neighbor of side box
	for (int i = 0; i < sideBox->size(); i++)
	{
		// Consider this box is parent 
		std::vector<indexBone> neighborC;

		// Center neighbor 
		// Side box can not be parent of center box
		for (int j = 0; j < centerBox->size(); j++)
		{
			neighborPos nbp = possibleNeighbor(&(*sideBox)[i], &(*centerBox)[j]);
			if(nbp != NONE_NB)
			{
				indexBone newN;
				newN.boneType = CENTER_BONE;
				newN.idxInArray = j;
				neighborC.push_back(newN);
			}
		}

		// Side neighbor
		for (int j = 0; j < sideBox->size(); j++)
		{
			if (i!= j)
			{
				neighborPos nbp = possibleNeighbor(&(*sideBox)[i], &(*sideBox)[j]);
				if(nbp != NONE_NB)
				{
					indexBone newN;
					newN.boneType = SIDE_BONE;
					newN.idxInArray = j;
					neighborC.push_back(newN);
				}
			}

		}

		neighborOut.push_back(neighborC);
	}

	return neighborOut;
}

void poseManager::init()
{
	neighborCenterNum.resize(centerBoneOrder.size());
	neighborSideNum.resize(sideBoneOrder.size());

	for (int i = 0; i < neighborInfo.size(); i++)
	{
		neighbor nb = neighborInfo[i];

		if (nb.first.boneType == CENTER_BONE)
		{
			neighborCenterNum[nb.first.idxInArray][nb.second.boneType] ++;
		}
		else
		{
			neighborSideNum[nb.first.idxInArray][nb.second.boneType] ++;
		}

		if (nb.second.boneType == CENTER_BONE)
		{
			neighborCenterNum[nb.second.idxInArray][nb.first.boneType] ++;
		}
		else
		{
			neighborSideNum[nb.second.idxInArray][nb.first.boneType] ++;
		}
	}

	neighborBoneNum.insert(neighborBoneNum.end(), neighborCenterNum.begin(), neighborCenterNum.end());
	neighborBoneNum.insert(neighborBoneNum.end(), neighborSideNum.begin(), neighborSideNum.end());

	constructMapTree();
}


bool compareBoneNeighbor_descen(const bone *lhs, const bone *rhs)
{
	return lhs->nbNeighbor() > rhs->nbNeighbor();
}

void poseManager::constructMapTree()
{
	s_skeleton->getSortedGroupBoneArray(sortedBone);

	// now sort the bone by order of number of neighbor
	std::sort(sortedBone.begin(), sortedBone.end(), compareBoneNeighbor_descen);
	std::vector<bone*> center;
	std::vector<bone*> side;
	for (int i = 0; i < sortedBone.size(); i++)
	{
		if (sortedBone[i]->m_type == CENTER_BONE)
			center.push_back(sortedBone[i]);
		else
			side.push_back(sortedBone[i]);
	}
	sortedBone.clear();
	sortedBone.insert(sortedBone.end(), center.begin(), center.end());
	sortedBone.insert(sortedBone.end(), side.begin(), side.end());

	// Compute volume ratio
	computeVolumeRatioOfBone();


	// Neighbor info
	for (int i = 0; i < sortedBone.size(); i++)
	{
		arrayInt neighborIdx;
		std::vector<bone*> *child = &sortedBone[i]->child;
		if (!sortedBone[i]->bIsGroup)
		{
			for (int j = 0; j < child->size(); j++)
			{
				int idx = findIdx(&sortedBone, child->at(j));

				neighborPair.push_back(Vec2i(i, idx));
				neighborIdx.push_back(idx);
			}			
		}

		if (sortedBone[i]->parent)
		{
			int idx = findIdx(&sortedBone, sortedBone[i]->parent);

			neighborIdx.push_back(idx);
		}

		boneAroundBone.push_back(neighborIdx);
	}

	// The tree contains all available mapping
	m_boneMapTree.sortedBone = &sortedBone;
	m_boneMapTree.boneAroundBone = &boneAroundBone;
	m_boneMapTree.neighborPair = &neighborPair;
	m_boneMapTree.nbCenterBone = center.size();

	m_boneMapTree.constructTree();
}


int poseManager::findIdx(std::vector<bone*>* v, bone* e)
{
	std::vector<bone*>::iterator it = std::find(v->begin(), v->end(), e);
	return std::distance(v->begin(), it);
}

void poseManager::findPossibleMap(BoneMapTreeNode *node, cutTreefNode* cutTNode)
{
	// Check possible of this current node
	if (node->depth > 0) // not root node
	{
		int boneIdx = node->depth - 1;
		int meshIdx = node->indexOfMesh;

		if (!isNeighborSufficient(boneIdx, meshIdx))
		{
			return;
		}
	}

	if (node->depth == sortedBone.size())
	{// Leaf node
		// This configuration satisfy number of neighbor
		std::map<int, int>* boneMeshIdxMap = &node->boneMeshIdxMap;

		// store in the poses array
		neighborPose pose;
		for (int i = 0; i < neighborPair.size(); i++)
		{
			int meshParentIdx = boneMeshIdxMap->find(neighborPair[i][0])->second;
			int meshChildIdx = boneMeshIdxMap->find(neighborPair[i][1])->second;

			neighborPos relativePos = possibleNeighbor(&meshBoxes[meshParentIdx], &meshBoxes[meshChildIdx]);
			
			if (relativePos == NONE_NB)
			{
				return;
			}

			pose.posConfig.push_back(relativePos);
		}

		if (pose.posConfig.size() == neighborPair.size())
		{
			pose.computeUniqeID();

			// Push
			std::map<int, neighborPose>::iterator it = poseMap.find(pose.posConfigId);
			if (it == poseMap.end()) // Not found
			{
				poseMap.insert(std::pair<int, neighborPose>(pose.posConfigId, pose));
				it = poseMap.find(pose.posConfigId);
			}

			it->second.nodes.push_back(cutTNode);
			it->second.mapBone_meshIdx.push_back(*boneMeshIdxMap);
			// re order the box
			
			float volError = getVolumeError(cutTNode, boneMeshIdxMap);
			if (volError < it->second.smallestVolumeError)
			{
				it->second.smallestVolumeError = volError;
				it->second.smallestErrorIdx = it->second.nodes.size() - 1;
			}
		}
	}

	for (int i = 0; i < node->children.size(); i++)
	{
		findPossibleMap(node->children[i], cutTNode);
	}
}

bool poseManager::isNeighborSufficient(int boneIdx, int meshIdx)
{
	Vec2i nbMesh = neighborMeshNum[meshIdx];
	Vec2i nbBone = neighborBoneNum[boneIdx];
	
// 	bone* b = sortedBone[boneIdx];
// 	int centerNB = 0, sideNB = 0;
// 
// 	for (int i = 0; i < b->child.size(); i++)
// 	{
// 		if (b->child[i]->m_type == CENTER_BONE)
// 		{
// 			centerNB ++;
// 		}
// 		else
// 			sideNB ++;
// 	}
// 
// 	if (b->parent)
// 	{
// 		if (b->parent->m_type == CENTER_BONE)
// 		{
// 			centerNB ++;
// 		}
// 		else
// 			sideNB ++;
// 	}

	return (nbMesh[CENTER_BONE] >= nbBone[CENTER_BONE]) && (nbMesh[SIDE_BONE] >= nbBone[SIDE_BONE]);
}

void poseManager::countMeshNeighbor()
{
	neighborMeshNum.clear();
	for (int i = 0; i < posibleMeshNeighbor.size(); i++)
	{
		int center = 0, side = 0;
		std::vector<indexBone> curN = posibleMeshNeighbor[i];
		for (int j = 0; j < curN.size(); j++)
		{
			if (curN[j].boneType == CENTER_BONE)
				center++;
			else
				side++;
		}
		neighborMeshNum.push_back(Vec2i(center, side));
	}
}

float poseManager::evaluateError(int idx1, int idx2)
{
	ASSERT(idx1 >= 0 && idx2 >= 0);
	ASSERT (idx1 < poseMap.size());

	std::map<int, neighborPose>::iterator it = poseMap.begin();
	for (int i = 0; i < idx1; i++)
	{
		++it;
	}
	neighborPose pose = (*it).second;

	std::vector<cutTreefNode*> *nodes = &pose.nodes;

	ASSERT ( idx2 < nodes->size());

	// We have the node
	cutTreefNode* curNode = nodes->at(idx2);

	// Bone name

	std::map<int, int> boneMeshMapIdx = pose.mapBone_meshIdx[idx2];
	std::vector<meshPiece> *centerBox = &curNode->centerBoxf;
	std::vector<meshPiece> *sideBox = &curNode->sideBoxf;

	float volumeE2 = 0;
	for (int i = 0; i < sortedBone.size(); i++)
	{
		bone* boneb = sortedBone[i];
		int meshIdx = boneMeshMapIdx[i];
		meshPiece mesh;
		if (meshIdx < centerBox->size())
		{
			mesh = centerBox->at(meshIdx);
		}
		else
			mesh = sideBox->at(meshIdx-centerBox->size());

		float meshV = mesh.volumeRatio;
		float boneV = boneb->m_volumeRatio;
		volumeE2 += pow((meshV-boneV)/boneV, 2);
	}

	return volumeE2;
}

void poseManager::computeVolumeRatioOfBone()
{
	float totalV = 0;
	for (int i = 0; i < sortedBone.size(); i++)
	{
		totalV += sortedBone[i]->getVolumef();
	}
	for (int i = 0; i < sortedBone.size(); i++)
	{
		sortedBone[i]->volumeRatio() = sortedBone[i]->getVolumef() / totalV;
	}
}

neighborPose poseManager::getPose(int poseIdx)
{
	ASSERT(poseIdx >= 0 && poseIdx < poseMap.size());

	std::map<int, neighborPose>::iterator it = poseMap.begin();
	for (int i = 0; i < poseIdx; i++)
	{
		++it;
	}
	neighborPose pose = (*it).second;

	return pose;
}

float poseManager::getVolumeError(cutTreefNode* cutTNode, std::map<int, int>* boneMeshIdxMap)
{
	float e = 0;
	for (int i = 0; i < sortedBone.size(); i++)
	{
		int meshIdx = boneMeshIdxMap->at(i);
		e += errorCompute::normE2(sortedBone[i]->volumeRatio(), meshBoxes[meshIdx].volumeRatio);
	}

	return e / sortedBone.size();
}

void poseManager::updateFilteredList(std::vector<neighborPos> pp)
{
	filteredPose.clear();

	for (auto b : poseMap)
	{
		neighborPose curB = b.second;
		if (curB.containFilter(pp))
		{
			filteredPose.push_back(curB);
		}
	}

	std::cout << "Filtered list has " << filteredPose.size() << " poses\n";
}

neighborPose poseManager::getFilteredPose(int idx1)
{
	if (idx1 < 0 || idx1 >= filteredPose.size())
	{
		throw std::exception("Out of range vector");
		return neighborPose();
	}
	return filteredPose[idx1];
}

poseGroupCutManager::poseGroupCutManager()
{

}

poseGroupCutManager::~poseGroupCutManager()
{

}

void poseGroupCutManager::parserConfigure(groupCutNode * node)
{
	// Sort it by order of size
	// How about optimize it later
	// We just simply take all configuration

	std::vector<meshPiece> meshBoxes = node->boxf;

	// Optimize later
	// Because of the small number, then currently we just test it
	std::vector<BoneMapTreeNode*> *leaves = &m_boneMapTree.leaves;
	for (int i = 0; i < leaves->size(); i++)
	{
		std::map<int, int> boneMeshIdxMap = leaves->at(i)->boneMeshIdxMap;

		// relative position of mesh box
		neighborPose pose;
		for (int i = 0; i < neighborInfo.size(); i++)
		{
			Vec2i boneIdx = neighborInfo[i];
			int meshParentIdx = boneMeshIdxMap[boneIdx[0]];
			int meshChildIdx = boneMeshIdxMap[boneIdx[1]];

			neighborPos relativePos = possibleNeighbor(&meshBoxes[meshParentIdx], &meshBoxes[meshChildIdx]);

			if (relativePos == NONE_NB)
			{
				continue;
			}

			pose.posConfig.push_back(relativePos);
		}

		if (pose.posConfig.size() == neighborInfo.size())
		{
			pose.computeUniqeID();

			// Push
			std::map<int, neighborPose>::iterator it = poseMap.find(pose.posConfigId);
			if (it == poseMap.end()) // Not found
			{
				poseMap.insert(std::pair<int, neighborPose>(pose.posConfigId, pose));
				it = poseMap.find(pose.posConfigId);
			}

			it->second.nodeGroupBoneCut.push_back(node);
			it->second.mapBone_meshIdx.push_back(boneMeshIdxMap);
			// re order the box

			float volError = getVolumeError(node, &boneMeshIdxMap);
			if (volError < it->second.smallestVolumeError)
			{
				it->second.smallestVolumeError = volError;
				it->second.smallestErrorIdx = it->second.nodeGroupBoneCut.size() - 1;
			}

		}
	}
}

void poseGroupCutManager::init()
{
	m_boneMapTree.nbCenterBone = 0;
	m_boneMapTree.sortedBone = boneArray;
	m_boneMapTree.neighborPair = &neighborInfo;

	// Bone neighbor
	getBoneNeighbor(boneAroundBone, boneArray);

	m_boneMapTree.boneAroundBone = &boneAroundBone;

	m_boneMapTree.constructTree();
}

neighborPos poseGroupCutManager::possibleNeighbor(meshPiece* parent, meshPiece* child)
{
	// To be neighbor, they should be in contact or there is no other box between them
	// Best is to check overlap. maximum is the size of voxel
	GeometricFunc geoF;
	Vec3f diag(voxelSizef, voxelSizef, voxelSizef);


	// Otherwise, use center distance
	if (geoF.collisionBtwBox(Box(parent->leftDown, parent->rightUp), Box(child->leftDown - diag / 2.0, child->rightUp + diag / 2.0)))
	{
		// If have face contact
		int direct;
		if (geoF.isBoxFaceContactBox(Box(parent->leftDown, parent->rightUp), Box(child->leftDown, child->rightUp), direct))
		{
			int p = direct*2;
			if (parent->leftDown[direct] > child->leftDown[direct])
			{
				p++;
			}
			return (neighborPos)p;
		}
		else
		{
			Vec3f pSizef = parent->rightUp - parent->leftDown;
			Vec3f centerP = (parent->rightUp + parent->leftDown) / 2.0;
			Vec3f centerC = (child->rightUp + child->leftDown) / 2.0;

			Vec3f dd = centerC - centerP;
			// scale
			float longest = 0;
			int idx = -1;
			for (int j = 0; j < 3; j++)
			{
				dd[j] = dd[j] / (pSizef[j] / 2.0);
				if (longest < abs(dd[j]))
				{
					longest = abs(dd[j]);
					idx = j;
				}
			}

			neighborPos *pp = posArray();
			if (dd[idx] > 0)
				return pp[idx * 2]; // plus axis direction
			else
				return pp[idx * 2 + 1]; // minus axis direction
		}
	}

	return NONE_NB;
}

neighborPos * poseGroupCutManager::posArray()
{
	static neighborPos pos[6] = { X_PLUS, X_MINUS, Y_PLUS, Y_MINUS, Z_PLUS, Z_MINUS };
	return pos;
}

float poseGroupCutManager::getVolumeError(groupCutNode * node, std::map<int, int>* boneMeshIdxMap)
{
	float e = 0;
	std::vector<meshPiece> meshBoxes = node->boxf;

	for (int i = 0; i < boneArray->size(); i++)
	{
		int meshIdx = boneMeshIdxMap->at(i);
		e += errorCompute::normE2((*boneArray)[i]->m_volumeRatio, meshBoxes[meshIdx].volumeRatio);
	}

	return e / boneArray->size();
}

neighborPose poseGroupCutManager::getPoseByIdx(int poseIdx)
{
	if (poseIdx < 0 || poseIdx >= poseMap.size())
		return neighborPose();

	std::map<int, neighborPose>::iterator it = poseMap.begin();
	for (int i = 0; i < poseIdx; i++)
	{
		++it;
	}
	neighborPose pose = (*it).second;

	return pose;
}

void poseGroupCutManager::getBoneNeighbor(std::vector<arrayInt> &boneAroundB, std::vector<bone*> * boneList)
{
	for (int i = 0; i < boneList->size(); i++)
	{
		auto b = boneList->at(i);
		// Find neighbor of b
		arrayInt nb;
		int idx = findIdx(boneList, b->parent);
		if (idx != -1)
		{
			nb.push_back(idx);
		}

		for (auto c : b->child)
		{
			int cIdx = findIdx(boneList, c);
			if (cIdx != -1)
			{
				nb.push_back(cIdx);
			}
		}

		boneAroundB.push_back(nb);
	}
}

int poseGroupCutManager::findIdx(std::vector<bone*> * boneList, bone* b)
{
	for (int i = 0; i < boneList->size(); i++)
	{
		if (boneList->at(i) == b)
		{
			return i;
		}
	}

	return -1;
}
