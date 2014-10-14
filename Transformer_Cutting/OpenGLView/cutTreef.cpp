#include "StdAfx.h"
#include "cutTreef.h"
#include "errorCompute.h"


cutTreef::cutTreef(void)
{
	m_root = nullptr;
	voxelOccupy = nullptr;
	mark = nullptr;
	lestE2Node = nullptr;
}


cutTreef::~cutTreef(void)
{
	if (m_root)
	{
		delete m_root;
	}
	if (voxelOccupy)
	{
		delete []voxelOccupy;
	}
	if (mark)
	{
		delete []mark;
	}
}

void cutTreef::constructTree()
{
	boxNum = centerBoneOrder.size() + sideBoneOrder.size();
	centerMesh = (octreeS->m_root->leftDownf + octreeS->m_root->rightUpf)/2;
	meshVolume = octreeS->m_root->volumef;
	minVol = cutStep*cutStep*cutStep;

	m_hashErrorTable.initTable(Vec3i(10,10,10));

	// Root node
	m_root = new cutTreefNode;
	m_root->centerBoxf.push_back(meshPiece(octreeS->m_root->leftDownTight, octreeS->m_root->rightUpTight));


	leatE2 = MAX;
	lestE2Node = nullptr;
	constructTreeRecur(m_root);
}


void cutTreef::constructTreeWithVoxel()
{
	boxNum = centerBoneOrder.size() + sideBoneOrder.size();
	centerMesh = (octreeS->m_root->leftDownf + octreeS->m_root->rightUpf)/2;
	meshVolume = octreeS->m_root->volumef;
	minVol = cutStep*cutStep*cutStep;

	m_hashErrorTable.initTable(Vec3i(10,10,10));

	poseMngr->centerBoneOrder = centerBoneOrder;
	poseMngr->sideBoneOrder = sideBoneOrder;
	poseMngr->neighborInfo = neighborInfo;
	poseMngr->voxelSizef = octreeS->boxSize;
	poseMngr->init();

	// Root node
	m_root = new cutTreefNode;
	m_root->centerBoxf.push_back(meshPiece(octreeS->m_root->leftDownTight, octreeS->m_root->rightUpTight));

	for (int i = 0; i < boxes->size(); i++)
	{
		m_root->centerBoxf[0].voxels.push_back(i);
	}



	leatE2 = MAX;
	lestE2Node = nullptr;
	mark = new int[boxes->size()];
	voxelOccupy = new int[boxes->size()];

	constructTreeVoxelRecur(m_root);
}


void cutTreef::constructTreeRecur(cutTreefNode *node)
{
	if (node->boxCount() == boxNum) // Leaf
	{
		ASSERT(node->centerBoxf.size() == centerBoneOrder.size() &&
			node->sideBoxf.size() == sideBoneOrder.size());

		// Evaluate the leaf
		if(computeErrorLeaf(node)){
			m_hashErrorTable.addNode(node);
			leaves.push_back(node);
		}
		
		return;
	}

	std::vector<meshPiece>* centerBoxf = &node->centerBoxf;
	std::vector<meshPiece>* sideBoxf = &node->sideBoxf;

	// 1. Divide center box
	for (int i = 0; i < centerBoxf->size(); i++)
	{
		meshPiece curB = centerBoxf->at(i);

		if (sideBoxf->size() < sideBoneOrder.size())
		{
			//1.1. Side box is not full, can create more
			// We can divide center box by x
			for (float cx = curB.leftDown[0] + cutStep; cx < centerMesh[0]; cx += cutStep)
			{
				// Each cut surface has a mirror surface. It create 3 boxes
				cutTreefNode* newNode = new cutTreefNode(node);
				if (!cutNodeSym(newNode, i, cx)
					|| newNode->boxCount() <= node->boxCount())
				{
					delete newNode;
					continue;
				}
				newNode->xyzd = 0; newNode->coord = cx;
				node->children.push_back(newNode);
				constructTreeRecur(newNode);
			}

			//1.2. center surface (perpendicular)
			// Create 2 side boxes
			if (centerBoxf->size() > 1)
			{
				cutTreefNode* newNode = new cutTreefNode(node);
				if (!cutNodeSymAtCenter(newNode, i)
					|| newNode->boxCount() <= node->boxCount())
				{
					delete newNode;
					continue;
				}
				newNode->xyzd = 0; newNode->coord = 0;
				node->children.push_back(newNode);
				constructTreeRecur(newNode);
			}
		}

		// 1.3. Divide by y and z freely
		for (int yz = 1; yz < 3; yz++)
		{
			for (float coord = curB.leftDown[yz] + cutStep; coord < curB.rightUp[yz]; coord += cutStep)
			{
				cutTreefNode* newNode = new cutTreefNode(node);
				if (!cutNodeSymYZ(newNode, i, coord, yz)
					|| newNode->boxCount() <= node->boxCount())
				{
					delete newNode;
					continue;
				}
				newNode->xyzd = yz; newNode->coord = coord;
				node->children.push_back(newNode);
				constructTreeRecur(newNode);
			}
		}
	}

	//2. Cut side box
	// Side box always create side boxes
	if (sideBoxf->size() < sideBoneOrder.size())
	{
		for (int i = 0; i < sideBoxf->size(); i++)
		{
			meshPiece curB = (*sideBoxf)[i];
			for (int d = 0; d < 3; d++)
			{
				for (float coord = curB.leftDown[d]+cutStep; coord < curB.rightUp[d]; coord+=cutStep)
				{
					cutTreefNode * newNode = new cutTreefNode(node);
					if (!cutSideBox(newNode, i, d, coord)
						|| newNode->boxCount() <= node->boxCount())
					{
						delete newNode;
						continue;
					}
					newNode->xyzd = d; newNode->coord = coord;
					node->children.push_back(newNode);
					constructTreeRecur(newNode);
				}
			}
		}
	}
}

bool cutTreef::cutNodeSym(cutTreefNode* newNode, int centerBoxIdx, float cx)
{
	std::vector<meshPiece> *centerBoxf = &newNode->centerBoxf;
	std::vector<meshPiece> *sideBoxf = &newNode->sideBoxf;
	meshPiece cBox = centerBoxf->at(centerBoxIdx);

	if (cx < cBox.leftDown[0] || cx > cBox.rightUp[0])
	{
		return false;
	}

	//1. Divide the box to 3 box
	meshPiece boxSide = cBox;
	meshPiece boxCenter = cBox;

	boxSide.rightUp[0] = cx;
	boxCenter.leftDown[0] = cx;
	boxCenter.rightUp[0] = 2*centerMesh[0] - cx;

	// Find real box intersection
	meshPiece tightSideBox =  octreeS->intersectWithBox(boxSide);
	meshPiece tightCenterBox =  octreeS->intersectWithBox(boxCenter);

	if (tightSideBox.volumef < minVol || tightCenterBox.volumef < minVol)
	{
		return false;
	}

	//2. Modify the box list
	centerBoxf->erase(centerBoxf->begin() + centerBoxIdx);
	centerBoxf->push_back(tightCenterBox);
	sideBoxf->push_back(tightSideBox);

	return validateBoxCount(newNode);
}

bool cutTreef::validateBoxCount(cutTreefNode* newNode)
{
	return newNode->boxCount() <= boxNum && newNode->sideBoxf.size() <= sideBoneOrder.size();
}

bool cutTreef::cutNodeSymAtCenter(cutTreefNode* newNode, int centerBoxIdx)
{
	std::vector<meshPiece> *centerBoxf = &newNode->centerBoxf;
	std::vector<meshPiece> *sideBoxf = &newNode->sideBoxf;
	
	// 1. Cut the box
	meshPiece cBox = centerBoxf->at(centerBoxIdx);
	cBox.rightUp[0] = centerMesh[0];

	meshPiece tightBox =  octreeS->intersectWithBox(cBox);

	if (tightBox.volumef < minVol)
	{
		return false;
	}

	// 2. Modify the list
	centerBoxf->erase(centerBoxf->begin() + centerBoxIdx);
	sideBoxf->push_back(tightBox);

	return validateBoxCount(newNode);
}

bool cutTreef::cutNodeSymYZ(cutTreefNode* newNode, int centerBoxIdx, float coord, int yz)
{
	std::vector<meshPiece> *centerBoxf = &newNode->centerBoxf;
	// 1. Cut the box
	meshPiece cBox = centerBoxf->at(centerBoxIdx);
	meshPiece box1 = cBox, box2 = cBox;
	box1.leftDown[yz] = coord;
	box2.rightUp[yz] = coord;

	meshPiece boxTight1 =  octreeS->intersectWithBox(box1);
	meshPiece boxTight2 =  octreeS->intersectWithBox(box2);

	if (boxTight1.volumef < minVol || boxTight1.volumef < minVol)
	{
		return false;
	}

	// 2. Modify the list
	centerBoxf->erase(centerBoxf->begin() + centerBoxIdx);
	centerBoxf->push_back(boxTight1);
	centerBoxf->push_back(boxTight2);

	return validateBoxCount(newNode);
}

bool cutTreef::cutSideBox(cutTreefNode * newNode, int sideBIdx, int dxyz, float coord)
{
	std::vector<meshPiece> *sideBoxf = &newNode->sideBoxf;

	meshPiece curB = sideBoxf->at(sideBIdx);
	meshPiece box1 = curB, box2 = curB;
	box1.leftDown[dxyz] = coord;
	box2.rightUp[dxyz] = coord;

	meshPiece boxTight1 =  octreeS->intersectWithBox(box1);
	meshPiece boxTight2 =  octreeS->intersectWithBox(box2);

	if (boxTight1.volumef < minVol || boxTight2.volumef < minVol)
	{
		return false;
	}

	// 2. Modify the list
	sideBoxf->erase(sideBoxf->begin() + sideBIdx);
	sideBoxf->push_back(boxTight1);
	sideBoxf->push_back(boxTight2);

	return validateBoxCount(newNode);
}

bool cutTreef::computeErrorLeaf(cutTreefNode * node)
{
	std::vector<meshPiece> *centerBoxf = &node->centerBoxf;
	std::vector<meshPiece> *sideBoxf = &node->sideBoxf;

	for (int i = 0; i < centerBoxf->size(); i++)
	{
		centerBoxf->at(i).computeAdditionalInfo(meshVolume);
	}
	for (int i = 0; i < sideBoxf->size(); i++)
	{
		sideBoxf->at(i).computeAdditionalInfo(meshVolume);
	}

// 	std::sort(centerBoxf->begin(), centerBoxf->end(), compareMeshPiece_descen);
// 	std::sort(sideBoxf->begin(), sideBoxf->end(), compareMeshPiece_descen);

	// Pose manager
	poseMngr->addPose(node);
	//


// 	neighborManager neighM;
// 	neighM.faceDistanceThres = cutStep*0.1;
// 	if (!neighM.isNeighborsHaveFaceContact(centerBoxf, sideBoxf, neighborInfo))
// 	{
// 		// This leaf is invalid
// 		// return false;
// 	}
// 	node->debugCode = node->debugCode | CODE_NEIGHBOR_CONTACT_SURFACE;
// 
// 	float nb = 0.4, asp = 0.3, vol = 0.3;
// 	// Neighbor error
// 	float neighborE = neighM.neighborScore(centerBoxf, sideBoxf, &centerBoneOrder, &sideBoneOrder, &neighborInfo);
// 	if(neighborE > 1)
// 		return false;
// 
// 	// Volume and aspect ratio error
// 	float aspectE2 = (errorCompute::computeAspectError(centerBoxf, &centerBoneOrder) + errorCompute::computeAspectError(sideBoxf, &sideBoneOrder))/2.0; // We may weight it different
// 	ASSERT(aspectE2 < 1);
// 
// 	float volumeE2 = (errorCompute::computeVolumeError(centerBoxf, &centerBoneOrder) + errorCompute::computeVolumeError(sideBoxf, &sideBoneOrder))/2.0;
// 	ASSERT(volumeE2 < 1);
// 
// 	if (leatE2 > nb*neighborE + asp*aspectE2 + vol*volumeE2)
// 	{
// 		leatE2 = nb*neighborE + asp*aspectE2 + vol*volumeE2;
// 		lestE2Node = node;
// 	}
// 
// 	node->neightborError = neighborE;
// 	node->aspectError = aspectE2;
// 	node->volumeError = volumeE2;

	return true;
}

std::vector<meshCutRough> cutTreef::convertBoxesToMeshCut(std::vector<Box> boxesf)
{
	std::vector<meshCutRough> sortedCut;
	return sortedCut;
}

void cutTreef::drawLeaf(int nodeIdx)
{
	if (nodeIdx < leaves.size())
		leaves[nodeIdx]->draw(centerMesh[0]);
}

void cutTreef::constructTreeVoxelRecur(cutTreefNode *node)
{
/*	ASSERT(leaves.size() < 90);*/

	ASSERT(node->boxCount() <= boxNum);
	if (node->centerBoxf.size() == centerBoneOrder.size()
		&& node->sideBoxf.size() == sideBoneOrder.size()) // Leaf
	{
		ASSERT(node->centerBoxf.size() == centerBoneOrder.size() &&
			node->sideBoxf.size() == sideBoneOrder.size());

		// Evaluate the leaf
		if(computeErrorLeaf(node)){
	//		m_hashErrorTable.addNode(node);
			leaves.push_back(node);
			node->idx = leaves.size() - 1;
		}
		else
		{
			// Remove this node from the tree. Reduce memory
		}

		return;
	}

	std::vector<meshPiece>* centerBoxf = &node->centerBoxf;
	std::vector<meshPiece>* sideBoxf = &node->sideBoxf;

	int nbRemainBox = boxNum - node->boxCount() + 1;

	// 1. Divide center box
	for (int i = 0; i < centerBoxf->size(); i++)
	{
		meshPiece curB = centerBoxf->at(i);

		if (sideBoxf->size() < sideBoneOrder.size())
		{
			//1.1. Side box is not full, can create more
			// We can divide center box by x
			float cuts = (centerMesh[0] - curB.leftDown[0]) / nbRemainBox;
			if (bUniformCutStep || cuts < cutStep)
				cuts = cutStep;

			for (float cx = curB.leftDown[0] + cuts; cx < centerMesh[0]; cx += cuts)
			{
				// Each cut surface has a mirror surface. It create 3 boxes
				cutTreefNode* newNode = new cutTreefNode(node);
				if (!cutNodeSymVoxel(newNode, i, cx)
					|| newNode->boxCount() <= node->boxCount())
				{
					delete newNode;
				}
				else
				{
					newNode->xyzd = 0; newNode->coord = cx;
					node->children.push_back(newNode);
					constructTreeVoxelRecur(newNode);
				}

				if (node->depth == 0) // root node
				{
					std::cout << " - Cut side box X, # children of root node: " << node->children.size() << "\n";
				}
			}

			//1.2. center surface (perpendicular)
			// Create 2 side boxes
			// The number of box remain
			if (centerBoxf->size() > 1)
			{
				cutTreefNode* newNode = new cutTreefNode(node);
				if (!cutNodeSymAtCenterVoxel(newNode, i)
					|| newNode->boxCount() != node->boxCount())
				{
					delete newNode;
				}
				else
				{
					newNode->xyzd = 0; newNode->coord = -1;
					node->children.push_back(newNode);
					constructTreeVoxelRecur(newNode);
				}

				if (node->depth == 0) // root node
				{
					std::cout << " - Cut side box x, # children of root node: " << node->children.size() << "\n";
				}

			}
		}

		// 1.3. Divide by y and z freely
		for (int yz = 1; yz < 3; yz++)
		{
			float cuts = (curB.rightUp[yz] - curB.leftDown[yz]) / nbRemainBox;
			if (bUniformCutStep || cuts < cutStep)
				cuts = cutStep;

			for (float coord = curB.leftDown[yz] + cuts; coord < curB.rightUp[yz]; coord += cuts)
			{

				cutTreefNode* newNode = new cutTreefNode(node);
				if (!cutNodeSymYZVoxel(newNode, i, coord, yz)
					|| newNode->boxCount() <= node->boxCount())
				{
					delete newNode;
				}
				else
				{
					newNode->xyzd = yz; newNode->coord = coord;
					node->children.push_back(newNode);
					constructTreeVoxelRecur(newNode);
				}

				if (node->depth == 0) // root node
				{
					std::cout << " - Cut center box y,z, # children of root node: " << node->children.size() << "\n";
				}
			}
		}
	}

	//2. Cut side box
	// Side box always create side boxes
	if (sideBoxf->size() < sideBoneOrder.size())
	{
		for (int i = 0; i < sideBoxf->size(); i++)
		{
			meshPiece curB = (*sideBoxf)[i];
			for (int d = 0; d < 3; d++)
			{
				float cuts = (curB.rightUp[d] - curB.leftDown[d]) / nbRemainBox;
				if (bUniformCutStep || cuts < cutStep)
					cuts = cutStep;

				for (float coord = curB.leftDown[d] + cuts; coord < curB.rightUp[d]; coord += cuts)
				{
					cutTreefNode * newNode = new cutTreefNode(node);
					if (!cutSideBoxVoxel(newNode, i, d, coord)
					|| newNode->boxCount() <= node->boxCount())
					{
						delete newNode;
					}
					else
					{
						newNode->xyzd = d; newNode->coord = coord;
						node->children.push_back(newNode);
						constructTreeVoxelRecur(newNode);
					}

				}
			}

			if (node->depth == 0) // root node
			{
				std::cout << " - Cut side box x, y, z, # children of root node " << node->children.size() << "\n";
			}
		}
	}
}

bool cutTreef::cutNodeSymVoxel(cutTreefNode* newNode, int centerBoxIdx, float cx)
{
	std::vector<meshPiece> *centerBoxf = &newNode->centerBoxf;
	std::vector<meshPiece> *sideBoxf = &newNode->sideBoxf;
	meshPiece cBox = centerBoxf->at(centerBoxIdx);

	float cx2 = 2*centerMesh[0] - cx;

	if (cx < cBox.leftDown[0] || cx > cBox.rightUp[0])
	{
		return false;
	}

	std::vector<meshPiece> cutPiecesCenter;
	std::vector<meshPiece> cutPiecesSide;
	std::vector<int> *vIdxs = &cBox.voxels;

	// Split the voxel object
	std::fill(mark, mark + boxes->size(), 0);
	std::fill(voxelOccupy, voxelOccupy + boxes->size(), 0);
	for (int i = 0; i < vIdxs->size(); i++)
	{
		voxelOccupy[(*vIdxs)[i]] = 1;
	}
	int nbSideBox;

	for (int i = 0; i < vIdxs->size(); i++)
	{
		if (mark[(*vIdxs)[i]] != 0)
			continue;

		meshPiece newPiece;
		newPiece.initForVoxel();

		std::queue<int> vQ;
		vQ.push(vIdxs->at(i));
		mark[vIdxs->at(i)] = 1;

		int idx = vIdxs->at(i);
		Vec3f curCenter = boxes->at(idx).center;
		if (curCenter[0] > cx2) // On the other side of symmetric
			continue;
		else if (curCenter[0] > cx) // This is center 
			newPiece.isLower = false;
		else
			newPiece.isLower = true;

		while(!vQ.empty())
		{
			int idx = vQ.front();
			vQ.pop();
			mark[idx] = 1; // Can mark it as index of meshpiece
			newPiece.voxels.push_back(idx);
			newPiece.fitBOundingBox(boxes->at(idx).leftDown, boxes->at(idx).rightUp);

			// Check all adjacent of this vertex
			std::vector<int> adjacentIdxs = hashTable->getNeighbor(idx);
			for (int j = 0; j < adjacentIdxs.size(); j++)
			{
				if (mark[adjacentIdxs[j]] == 0
					&& voxelOccupy[adjacentIdxs[j]] == 1)
				{
					// Validate
					Vec3f nCenter = boxes->at(adjacentIdxs[j]).center;
					if (nCenter[0] > cx2)
						continue;
					
					bool isLower = nCenter[0] < cx;
					if (isLower == newPiece.isLower) // OK
					{
						vQ.push(adjacentIdxs[j]);
						mark[adjacentIdxs[j]] = 1;
					}
				}
			}
		}

		newPiece.volumef = newPiece.voxels.size()*pow(octreeS->boxSize, 3);

		if (newPiece.isLower)
			cutPiecesSide.push_back(newPiece);
		else
			cutPiecesCenter.push_back(newPiece);
	}

	if (cutPiecesCenter.size() <= 0 || cutPiecesSide.size() <= 0)
	{
		return false;
	}

//	ASSERT(cutPiecesCenter.size() == 1);
	if (cutPiecesCenter.size() > 1)
		return false;

	// Now check the result
	// Accept the biggest box as side box
	int idxSideMesh = -1;
	int vol = MAX;
	for (int i = 0; i < cutPiecesSide.size(); i++)
	{
		if (cutPiecesSide[i].isLower)
		{
			if (cutPiecesSide[i].voxels.size() < vol)
			{
				vol = cutPiecesSide[i].voxels.size();
				idxSideMesh = i;
			}
		}
	}

	ASSERT(idxSideMesh != -1);

	meshPiece boxSide = cutPiecesSide[idxSideMesh];

	// merge cutPiece center
	meshPiece boxCenter = cutPiecesCenter[0]; // Assume there is only 1
	for (int i = 0;  i < cutPiecesSide.size(); i++)
	{
		if (i != idxSideMesh)
		{
			arrayInt sideBoxIdx = cutPiecesSide[i].voxels;
			arrayInt sideBoxSymIdx;
			for (int j = 0; j < sideBoxIdx.size(); j++)
			{
				int idx = hashTable->getSymmetricBox(sideBoxIdx[j]);
				if (idx != -1)
					sideBoxSymIdx.push_back(idx);
			}

			Box symBox = Util_w::getSymetrixBox(Box(cutPiecesSide[i].leftDown, cutPiecesSide[i].rightUp), 0, centerMesh[0]);
			// Merge
			boxCenter.mergeIdx(sideBoxIdx, cutPiecesSide[i].leftDown, cutPiecesSide[i].rightUp);
			boxCenter.mergeIdx(sideBoxSymIdx, symBox.leftDown, symBox.rightUp);
		}
	}

	if (boxCenter.volumef < minVol || boxSide.volumef < minVol)
	{
		return false;
	}

	//2. Modify the box list
	centerBoxf->erase(centerBoxf->begin() + centerBoxIdx);
	centerBoxf->push_back(boxCenter);
	sideBoxf->push_back(boxSide);

	return validateBoxCount(newNode);
}

bool cutTreef::cutNodeSymAtCenterVoxel(cutTreefNode* newNode, int centerBoxIdx)
{
	std::vector<meshPiece> *centerBoxf = &newNode->centerBoxf;
	std::vector<meshPiece> *sideBoxf = &newNode->sideBoxf;

	meshPiece cBox = centerBoxf->at(centerBoxIdx);

	// 1. Cut the box
	// Create two side box
	std::vector<int> *vIdxs = &cBox.voxels;
	meshPiece newPiece;

	// Split the voxel object
	for (int i = 0; i < vIdxs->size(); i++)
	{
		voxelBox curBox = boxes->at((*vIdxs)[i]);
		if (curBox.center[0] < centerMesh[0])
		{
			newPiece.voxels.push_back((*vIdxs)[i]);
			newPiece.fitBOundingBox(curBox.leftDown, curBox.rightUp);
		}
	}

	newPiece.volumef = newPiece.voxels.size()*std::pow(octreeS->boxSize, 3);

	if (newPiece.volumef < minVol)
	{
		return false;
	}

	// 2. Modify the list
	centerBoxf->erase(centerBoxf->begin() + centerBoxIdx);
	sideBoxf->push_back(newPiece);

	return validateBoxCount(newNode);
}

bool cutTreef::cutNodeSymYZVoxel(cutTreefNode* newNode, int centerBoxIdx, float coord, int yz)
{
	std::vector<meshPiece> *centerBoxf = &newNode->centerBoxf;
	meshPiece cBox = centerBoxf->at(centerBoxIdx);
	// 1. Cut the box
	std::vector<meshPiece> cutPiecesCenter;
	std::vector<int> *vIdxs = &cBox.voxels;

	// Split the voxel object
	std::fill(mark, mark + boxes->size(), 0);
	std::fill(voxelOccupy, voxelOccupy + boxes->size(), 0);
	for (int i = 0; i < vIdxs->size(); i++)
	{
		voxelOccupy[(*vIdxs)[i]] = 1;
	}

	int nbSideBox;

	for (int i = 0; i < vIdxs->size(); i++)
	{
		if (mark[(*vIdxs)[i]] != 0)
			continue;

		meshPiece newPiece;
		newPiece.initForVoxel();

		std::queue<int> vQ;
		vQ.push(vIdxs->at(i));
		mark[vIdxs->at(i)] = 1;

		Vec3f curCenter = boxes->at(vIdxs->at(i)).center;

		if (curCenter[yz] > coord) // This is center 
			newPiece.isLower = false;
		else
			newPiece.isLower = true;

		while(!vQ.empty())
		{
			int idx = vQ.front();
			vQ.pop();
			mark[idx] = 1; // Can mark it as index of meshpiece
			newPiece.voxels.push_back(idx);
			newPiece.fitBOundingBox(boxes->at(idx).leftDown, boxes->at(idx).rightUp);

			// Check all adjacent of this vertex
			std::vector<int> adjacentIdxs = hashTable->getNeighbor(idx);
			for (int j = 0; j < adjacentIdxs.size(); j++)
			{
				int aa = mark[adjacentIdxs[j]];
				int bb = voxelOccupy[adjacentIdxs[j]];
				if ((mark[adjacentIdxs[j]] == 0)
					&& (voxelOccupy[adjacentIdxs[j]] == 1))
				{
					// Validate
					Vec3f nCenter = boxes->at(adjacentIdxs[j]).center;

					bool isLower = nCenter[yz] < coord;
					if (isLower == newPiece.isLower) // OK
					{
						vQ.push(adjacentIdxs[j]);
						mark[adjacentIdxs[j]] = 1;
					}
				}
			}
		}

		cutPiecesCenter.push_back(newPiece);
	}

	// ASSERT(cutPiecesCenter.size()<3);

	// 2. Modify the list
	centerBoxf->erase(centerBoxf->begin() + centerBoxIdx);
	for (int i = 0; i < cutPiecesCenter.size(); i++)
	{
		cutPiecesCenter[i].volumef = cutPiecesCenter[i].voxels.size()*pow((float)octreeS->boxSize, 3);
		if (cutPiecesCenter[i].volumef < minVol)
		{
			return false;
		}

		centerBoxf->push_back(cutPiecesCenter[i]);
	}

	return validateBoxCount(newNode);
}

bool cutTreef::cutSideBoxVoxel(cutTreefNode * newNode, int sideBIdx, int dxyz, float coord)
{
	std::vector<meshPiece> *sideBoxf = &newNode->sideBoxf;

	meshPiece cBox = sideBoxf->at(sideBIdx);

	// 1. Cut the box
	std::vector<meshPiece> cutPiecesSide;
	std::vector<int> *vIdxs = &cBox.voxels;

	// Split the voxel object
	std::fill(mark, mark + boxes->size(), 0);
	std::fill(voxelOccupy, voxelOccupy + boxes->size(), 0);
	for (int i = 0; i < vIdxs->size(); i++)
	{
		voxelOccupy[(*vIdxs)[i]] = 1;
	}
	int nbSideBox;

	for (int i = 0; i < vIdxs->size(); i++)
	{
		if (mark[(*vIdxs)[i]] != 0)
			continue;

		meshPiece newPiece;
		newPiece.initForVoxel();

		std::queue<int> vQ;
		vQ.push(vIdxs->at(i));

		Vec3f curCenter = boxes->at(vIdxs->at(i)).center;

		if (curCenter[dxyz] > coord) // This is center 
			newPiece.isLower = false;
		else
			newPiece.isLower = true;

		while(!vQ.empty())
		{
			int idx = vQ.front();
			vQ.pop();

			mark[idx] = 1; // Can mark it as index of meshpiece
			newPiece.voxels.push_back(idx);
			newPiece.fitBOundingBox(boxes->at(idx).leftDown, boxes->at(idx).rightUp);

			// Check all adjacent of this vertex
			std::vector<int> adjacentIdxs = hashTable->getNeighbor(idx);
			for (int j = 0; j < adjacentIdxs.size(); j++)
			{
				if (mark[adjacentIdxs[j]] == 0
					&& voxelOccupy[adjacentIdxs[j]] == 1)
				{
					// Validate
					Vec3f nCenter = boxes->at(adjacentIdxs[j]).center;

					bool isLower = nCenter[dxyz] < coord;
					if (isLower == newPiece.isLower) // OK
					{
						vQ.push(adjacentIdxs[j]);
						mark[adjacentIdxs[j]] = 1;
					}
				}
			}
		}

		cutPiecesSide.push_back(newPiece);
	}

	// 2. Modify the list
	sideBoxf->erase(sideBoxf->begin() + sideBIdx);
	for (int i = 0; i < cutPiecesSide.size(); i++)
	{
		cutPiecesSide[i].volumef = cutPiecesSide[i].voxels.size()*pow(octreeS->boxSize, 3);
		if (cutPiecesSide[i].volumef < minVol)
		{
			return false;
		}

		sideBoxf->push_back(cutPiecesSide[i]);
	}

	return validateBoxCount(newNode);
}

cutTreefNode::cutTreefNode()
{
	parent = nullptr;
	depth = 0;
	
	debugCode = CODE_NULL;
}

cutTreefNode::cutTreefNode(cutTreefNode* parentNode)
{
	centerBoxf = parentNode->centerBoxf;
	sideBoxf = parentNode->sideBoxf;
	parent = parentNode;
	depth = parentNode->depth + 1;
	childIdxToParent = parentNode->children.size();

	// Debug
#ifdef _DEBUG
	arrayInt parentChild = {1, 3, 9, 2};
	if (depth == parentChild.size())
	{

		cutTreefNode* p = this;
		int idx = 0;
		bool found = true;
		for (int i = 0; i < parentChild.size(); i++)
		{
			if (p->childIdxToParent == parentChild[i])
			{
				p = p->parent;
			}
			else
				found = false;
		}

		if (found)
		{
			std::cout << "Me Me";
		}
	}
#endif

	debugCode = CODE_NULL;
}

cutTreefNode::~cutTreefNode()
{
	for (int i = 0; i < children.size(); i++)
	{
		delete children[i];
	}
}

int cutTreefNode::boxCount()
{
	return centerBoxf.size() + sideBoxf.size();
}

void cutTreefNode::draw(float centerX)
{
	for (int i = 0; i < centerBoxf.size(); i++)
	{
		Util_w::drawBoxWireFrame(centerBoxf[i].leftDown, centerBoxf[i].rightUp);
		Vec3f centerB = (centerBoxf[i].leftDown + centerBoxf[i].rightUp)/2;
		Util::printw(centerB[0], centerB[1], centerB[2], "c %d", i);
	}

	for (int i = 0; i < sideBoxf.size(); i++)
	{
		Util_w::drawBoxWireFrame(sideBoxf[i].leftDown, sideBoxf[i].rightUp);
		Vec3f centerB = (sideBoxf[i].leftDown + sideBoxf[i].rightUp)/2;
		Util::printw(centerB[0], centerB[1], centerB[2], "s %d", i);

		// Mirror other box
		Vec3f ld = sideBoxf[i].leftDown;
		Vec3f ru = sideBoxf[i].rightUp;
		float offset = (centerX-ru[0]) + centerX - ld[0];
		ru[0] += offset;
		ld[0] += offset;
		Util_w::drawBoxWireFrame(ld, ru);
		Vec3f centerB1 = (ld + ru)/2;
		Util::printw(centerB1[0], centerB1[1], centerB1[2], "s- %d", i);
	}
}

void cutTreefNode::draw(float centerX, std::vector<boneAbstract> *centerBoneOrder, std::vector<boneAbstract> *sideBoneOrder)
{
	for (int i = 0; i < centerBoxf.size(); i++)
	{
		Util_w::drawBoxWireFrame(centerBoxf[i].leftDown, centerBoxf[i].rightUp);
		Vec3f centerB = (centerBoxf[i].leftDown + centerBoxf[i].rightUp)/2;
		Util::printw(centerB[0], centerB[1], centerB[2], "%s", centerBoneOrder->at(i).original->m_name.GetBuffer());
	}

	for (int i = 0; i < sideBoxf.size(); i++)
	{
		Util_w::drawBoxWireFrame(sideBoxf[i].leftDown, sideBoxf[i].rightUp);
		Vec3f centerB = (sideBoxf[i].leftDown + sideBoxf[i].rightUp)/2;
		Util::printw(centerB[0], centerB[1], centerB[2], "%s", ToAS(sideBoneOrder->at(i).original->m_name));

		// Mirror other box
		Vec3f ld = sideBoxf[i].leftDown;
		Vec3f ru = sideBoxf[i].rightUp;
		float offset = (centerX-ru[0]) + centerX - ld[0];
		ru[0] += offset;
		ld[0] += offset;
		Util_w::drawBoxWireFrame(ld, ru);
		Vec3f centerB1 = (ld + ru)/2;
		Util::printw(centerB1[0], centerB1[1], centerB1[2], "(%s)", ToAS(sideBoneOrder->at(i).original->m_name));
	}
}

void cutTreef::drawVoxel(cutTreefNode*node, std::vector<voxelBox>* boxes)
{
	bool boneName = false;
	bool bDrawVoxel = false;
	bool bDrawTransparant = true;
	//glEnable(GL_CULL_FACE);

	static std::vector<Vec3f> color = Util_w::randColor(6);
	color[0] = Vec3f(1, 1, 1);
	color[1] = Vec3f(0.5, 0.5, 0.5);
 	int idxC = 0;
	for (int i = 0; i < node->centerBoxf.size(); i++)
	{
		arrayInt idxs = node->centerBoxf[i].voxels;
		glColor3fv(color[idxC].data());

		if (bDrawVoxel)
		{
			for (int i = 0; i < idxs.size(); i++)
			{
				voxelBox vBox = boxes->at(idxs[i]);
				glColor3f(0, 0, 0);
				Util_w::drawBoxWireFrame(vBox.leftDown, vBox.rightUp);
				glColor3fv(color[idxC].data());
				Util_w::drawBoxSurface(vBox.leftDown, vBox.rightUp);
			}
		}


		glLineWidth(2.0);
		glColor3f(0, 0, 0);
		Util_w::drawBoxWireFrame(node->centerBoxf[i].leftDown, node->centerBoxf[i].rightUp);
		glLineWidth(1.0);

		if (bDrawTransparant)
		{
			Util::setUpTranparentGL();

			glColor4f(color[idxC][0], color[idxC][1], color[idxC][2], 0.1);
			Util_w::drawBoxSurface(node->centerBoxf[i].leftDown, node->centerBoxf[i].rightUp);
			
			Util::endTransparentGL();
		}

		if (boneName)
		{
			Vec3f centerB = (node->centerBoxf[i].leftDown + node->centerBoxf[i].rightUp)/2;
			Util::printw(centerB[0], centerB[1], centerB[2], "c %d", i);
		}

		idxC++;
	}

	for (int i = 0; i < node->sideBoxf.size(); i++)
	{
		arrayInt idxs = node->sideBoxf[i].voxels;
		glColor3fv(color[idxC].data());

		if (bDrawVoxel)
		{
			for (int i = 0; i < idxs.size(); i++)
			{
				voxelBox vBox = boxes->at(idxs[i]);
				glColor3f(0, 0, 0);
				Util_w::drawBoxWireFrame(vBox.leftDown, vBox.rightUp);
				glColor3fv(color[idxC].data());
				Util_w::drawBoxSurface(vBox.leftDown, vBox.rightUp);
			}

			// Symmetric
			for (int i = 0; i < idxs.size(); i++)
			{
				voxelBox vBox = boxes->at(idxs[i]);
				Box a = Util_w::getSymetrixBox(Box(vBox.leftDown, vBox.rightUp), 0, centerMesh[0]);
				glColor3f(0, 0, 0);
				Util_w::drawBoxWireFrame(a.leftDown, a.rightUp);
				glColor3fv(color[idxC].data());
				Util_w::drawBoxSurface(a.leftDown, a.rightUp);
			}
		}

		// Bounding
		glLineWidth(2.0);
		glColor3f(0, 0, 0);
		Util_w::drawBoxWireFrame(node->sideBoxf[i].leftDown, node->sideBoxf[i].rightUp);
		glLineWidth(1.0);

		if (bDrawTransparant)
		{
			Util::setUpTranparentGL();
			glColor4f(color[idxC][0], color[idxC][1], color[idxC][2], 0.5);
			Util_w::drawBoxSurface(node->sideBoxf[i].leftDown, node->sideBoxf[i].rightUp);
			Util::endTransparentGL();
		}

		if (boneName)
		{
			Vec3f centerB = (node->sideBoxf[i].leftDown + node->sideBoxf[i].rightUp)/2;
			Util::printw(centerB[0], centerB[1], centerB[2], "s %d", i);
		}
		// Mirror other box
		glColor3fv(color[idxC].data());
		Vec3f ld = node->sideBoxf[i].leftDown;
		Vec3f ru = node->sideBoxf[i].rightUp;
		Box b = Util_w::getSymetrixBox(Box(ld, ru), 0, centerMesh[0]);

		glColor3fv(color[idxC].data());
		glLineWidth(2.0);
		Util_w::drawBoxWireFrame(b.leftDown, b.rightUp);
		glLineWidth(1.0);

		if (bDrawTransparant)
		{
			Util::setUpTranparentGL();
			glColor4f(color[idxC][0], color[idxC][1], color[idxC][2], 0.5);
			Util_w::drawBoxSurface(b.leftDown, b.rightUp);
			Util::endTransparentGL();
		}

		if (boneName)
		{
			Vec3f centerB1 = (ld + ru)/2;
			Util::printw(centerB1[0], centerB1[1], centerB1[2], "s- %d", i);
		}


		idxC++;
	}

	glDisable(GL_CULL_FACE);
}