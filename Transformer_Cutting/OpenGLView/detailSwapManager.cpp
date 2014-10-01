#include "StdAfx.h"
#include "detailSwapManager.h"
#include "Utility_wrap.h"
#include "neighbor.h"
#include "skeleton.h"
#include "cutSurfTreeMngr2.h"
#include "coordAssignManager.h"


detailSwapManager::detailSwapManager(void)
{
	swapNode = nullptr;
	tempMark = nullptr;
	voxelOccupy = nullptr;
}


detailSwapManager::~detailSwapManager(void)
{
	for (int i = 0; i < meshBox.size(); i++)
	{
		delete meshBox[i];
	}
	meshBox.clear();

	if (tempMark)
		delete []tempMark;

	if (voxelOccupy)
		delete []voxelOccupy;
}

void detailSwapManager::initTest()
{
	// Load data
	m_octree.init("../Data_File/euroFighter.stl", 5);
	m_skeleton.initTest();
	// Cut boxes exported from first phase
	loadMeshBox("../Data_File/airCraft3.txt");

	// Construct hash table and voxel array
	constructVolxeHash();

	// BVH tree of each mesh
	constructBVHOfMeshBoxes();
}

void detailSwapManager::loadMeshBox(char *filePath)
{
	//Load from file
	FILE * f = fopen(filePath, "r");
	ASSERT(f);

	int nbMesh;
	fscanf(f, "%d\n", &nbMesh);

	for (int i = 0; i < nbMesh; i++)
	{
		bvhVoxel* newMeshBox = new bvhVoxel;
		fscanf(f, "%s\n", newMeshBox->boneName.GetBuffer(20));
		fscanf(f, "%d\n", &newMeshBox->boneType);
		fscanf(f, "%f %f %f\n", &newMeshBox->leftDown[0], &newMeshBox->leftDown[1], &newMeshBox->leftDown[2]);
		fscanf(f, "%f %f %f\n", &newMeshBox->rightUp[0], &newMeshBox->rightUp[1], &newMeshBox->rightUp[2]);

		meshBox.push_back(newMeshBox);
	}

	fclose(f);

	// Map with bone; for expected volume ratio and aspect ratio
	std::vector<bone*> boneOrder;
	std::vector<std::pair<int,int>> neighborPair;
	m_skeleton.getBoneAndNeighborInfo(boneOrder, neighborPair);

	float totalBoneVol = 0;
	float totalPerometer = 0;
	for (int i = 0; i < meshBox.size(); i++)
	{
		// Find correspond bone
		for (int j = 0; j < boneOrder.size(); j++)
		{
			if (meshBox[i]->boneName.Compare(boneOrder[j]->m_name) == 0)
			{
				meshBox[i]->expectedVolRatio = boneOrder[j]->m_volumef;
				totalBoneVol += boneOrder[j]->m_volumef;

				Vec3f sizeBone = boneOrder[j]->m_sizef;
				totalPerometer += sizeBone[0] + sizeBone[1] + sizeBone[2];

				Vec3i SMLIdxBone = Util_w::SMLIndexSizeOrder(sizeBone);

				Vec3f sizeMesh = meshBox[i]->rightUp - meshBox[i]->leftDown;
				Vec3i SMLIdxMesh = Util_w::SMLIndexSizeOrder(sizeMesh);

				Vec3f edgeRatio;
				for (int k = 0; k < 3; k++)
				{
					edgeRatio[SMLIdxMesh[k]] = sizeBone[SMLIdxBone[k]]/sizeBone[SMLIdxBone[0]];
				}
				meshBox[i]->xyzRatio = edgeRatio;

				break;
			}
		}
	}

	for (int i = 0; i < meshBox.size(); i++)
	{
		meshBox[i]->expectedVolRatio /= totalBoneVol;
		Vec3f sizeBone = meshBox[i]->rightUp - meshBox[i]->leftDown;
		meshBox[i]->expectedPerimeterRatio = sizeBone[0]*sizeBone[1]*sizeBone[2]/totalPerometer;
	}
}

void detailSwapManager::draw(BOOL displayMode[10])
{
	mirrorDraw mirror;
	mirror.mirrorAxis = 0;
	mirror.mirrorCoord = m_octree.centerMesh[0];
	static arrayVec3f color = Util_w::randColor(16);

	if (displayMode[5])
	{
		glColor3f(1,0,0);
		if (swapNode)
			swapNode->drawBoundingBox();
	}
// 	if (displayMode[1])
// 	{
// 		glColor3f(1,0,0);
// 		m_octree.drawWireOctree();
// 	}
// 	if (displayMode[6])
// 	{
// 		glLineWidth(2.0);
// 		for (int i = 0; i < meshBox.size(); i++)
// 		{
// 			glColor3f(0,0,1);
// 			meshBox[i]->drawOriginalBox();
// 		}
// 		glLineWidth(1.0);
// 	}

	if (displayMode[7])
	{

		for (int i =0 ; i < meshBox.size(); i++)
		{
			if (i != m_idx1 && i != m_idx2)
			{
				glColor3fv(color[i + 1].data());
				meshBox[i]->drawVoxels(&mirror);

				glColor3fv(color[i].data());
				meshBox[i]->drawVoxels(&mirror, 1);
			}
			else
			{
				// draw index
				glColor3fv(color[i + 1].data());
				meshBox[i]->drawVoxels(&mirror);

				meshBox[i]->drawVoxelIndex();
			}
		}
	}

	if (displayMode[8])
	{
		for (int i =0 ; i < meshBox.size(); i++)
		{
			glColor3fv(color[i+1].data());
			meshBox[i]->drawVoxels(&mirror);

			glColor3fv(color[i].data());
			meshBox[i]->drawVoxels(&mirror, 1);

		}
	}

// 	if (!displayMode[5])
// 	{
// 		glColor3f(1,0,0);
// 		for (int i =0 ; i < meshBox.size(); i++)
// 		{
// 			meshBox[i]->drawVoxelBoneIdx(i);
// 		}
// 	}

// 	if (displayMode[6]) // Draw node index
// 	{
// 		if (m_idx1 >= 0 && m_idx1 < meshBox.size())
// 			meshBox[m_idx1]->drawVoxelIndex();
// 	}

	if (displayMode[0])
	{// draw name
		for (int i = 0; i < meshBox.size(); i++)
		{
			Vec3f c = (meshBox[i]->curLeftDown + meshBox[i]->curRightUp)/2;
			Util::printw(c[0], c[1], c[2], (LPCSTR)CStringA(meshBox[i]->boneName));
		}
	}

// 	if (displayMode[8])
// 	{
// 		glBegin(GL_LINES);
// 		Vec3f color[3] = {Vec3f(1,0,0), Vec3f(0,1,0), Vec3f(0,0,1)};
// 		for (int i = 0; i < coords.size(); i++)
// 		{
// 			Vec3i cc = coords[i];
// 			Vec3f c = (meshBox[i]->leftDown + meshBox[i]->rightUp)/2;
// 			for (int j = 0; j < 3; j++)
// 			{
// 				glColor3fv(color[j].data());
// 				glVertex3fv(c.data());
// 				glVertex3fv((c+color[cc[j]]).data());
// 			}
// 
// 		}
// 		glEnd();
// 	}
}

void detailSwapManager::constructVolxeHash()
{
	leftDownVoxel = m_octree.m_root->leftDownTight;
	rightUpVoxel = m_octree.m_root->rightUpTight;
	Vec3f sizef = rightUpVoxel - leftDownVoxel;
	voxelSize = m_octree.boxSize;

	for (int i = 0; i < 3; i++)
	{
		NumXYZ[i] = (sizef[i]/voxelSize);
	}

	hashTable.leftDown = leftDownVoxel;
	hashTable.rightUp = rightUpVoxel;
	hashTable.voxelSize = voxelSize;
	hashTable.NumXYZ = NumXYZ;
	hashTable.boxes = &boxes;

	setVoxelArray();
}

void detailSwapManager::setVoxelArray()
{
	// index of voxel start from left down to right up
	// NX
	// Hash function: idx = i*NX*NY + j*NY + k
	std::vector<int> voxelHash;
	voxelHash.resize(NumXYZ[0]*NumXYZ[1]*NumXYZ[2]);
	std::fill(voxelHash.begin(), voxelHash.end(), -1);

	std::vector<octreeSNode*> *leaves = &m_octree.leaves;
	boxes.resize(leaves->size());
	for (int i = 0; i < leaves->size(); i++)
	{
		octreeSNode* node = leaves->at(i);
		// List
		voxelBox newBox(node->leftDownf, node->rightUpTight);
		Vec3f xyzIdx = hashTable.getVoxelCoord(newBox.center);
		newBox.xyzIndex = xyzIdx;

		boxes[i] = newBox;

		// Hash
		int hashF = xyzIdx[2]*NumXYZ[0]*NumXYZ[1] + xyzIdx[1]*NumXYZ[0] + xyzIdx[0];

		voxelHash[hashF] = i;
		node->idxInLeaf = i;
	}

	hashTable.voxelHash = voxelHash;
}

void detailSwapManager::constructBVHOfMeshBoxes()
{
	for (int i = 0; i < meshBox.size(); i++)
	{
		meshBox[i]->boxes = &boxes;
		meshBox[i]->voxelSize = voxelSize;
		meshBox[i]->hashTable = &hashTable;
		meshBox[i]->s_octree = &m_octree;
		meshBox[i]->constructAABBTree();

		// stupid, but we need it run first
		std::vector<AABBNode*>* leaves = &meshBox[i]->leaves;

		for (int j = 0; j < leaves->size(); j++)
		{
			boxes[leaves->at(j)->IndexInLeafNode].boneIndex = i;
		}
	}
}

void detailSwapManager::swapOneBestVoxel()
{
	// check all swappable voxel
	float ErrorReduced = 0;
	int sourceBoneIdx, desBoneIdx;
	AABBNode* node = nullptr;

	for (int i = 0; i < meshBox.size(); i++)
	{
		// Check all mesh box
		std::vector<AABBNode*>* leaves =  &meshBox[i]->leaves;

		for (int j = 0; j < leaves->size(); j++)
		{
			
			voxelBox curBox = boxes[leaves->at(j)->IndexInLeafNode];
			ASSERT(curBox.boneIndex != -1);
			Vec3i vCoord = hashTable.getVoxelCoord(curBox.center);

			// This loop is not optimized
			for (int d = -1; d < 2; d+=2)
			{
				for (int xyzI = 0; xyzI < 3; xyzI++)
				{
					// available neighbor
					Vec3i neighborVC = vCoord;
					neighborVC[xyzI] += d;
					int neighborIdx = hashTable.getBoxIndexFromVoxelCoord(neighborVC);

					if (neighborIdx != -1)
					{
						voxelBox neighborBox = boxes[neighborIdx];
						if (neighborBox.boneIndex != -1
							&& neighborBox.boneIndex != curBox.boneIndex)
						{
							// Check if newly created obj still is an object
							if (!isObjectIfRemove(curBox.boneIndex, leaves->at(j)->IndexInLeafNode))
							{
								continue;
							}

							// Side bone cannot receive voxel over center surface
							if (meshBox[neighborBox.boneIndex]->boneType == SIDE_BONE)
							{
								if (leaves->at(j)->RightUp[0] > m_octree.centerMesh[0] - m_octree.boxSize/2)
								{
									continue;
								}
							}

							// Neighbor relation must be maintain
							// The box that voxel is removed should be still contact with its neighbor
							if (!isNeighborMaintainIfRemoveBox(curBox.boneIndex, j))
							{
								continue;
							}

							// This voxel can be swap to the neighbor bone
							float eReduced = errorReduceAssumeWrap(leaves->at(j), curBox.boneIndex, neighborBox.boneIndex);

							if (ErrorReduced < eReduced)
							{
								ErrorReduced = eReduced;
								node = leaves->at(j);
								sourceBoneIdx = i;
								desBoneIdx = neighborBox.boneIndex;
							}
						}
					}
				}
			}


		}
	}

	if (node)
	{
		// Update
		wrapVoxel(node, sourceBoneIdx, desBoneIdx);
	}
}

float detailSwapManager::errorReduceAssumeWrap(AABBNode* voxelAABB, int boneSourceIdx, int boneDestIdx)
{

	bvhVoxel* meshSource = meshBox[boneSourceIdx];
	bvhVoxel* meshDest = meshBox[boneDestIdx];

	// Current error
	float sourceEVol, sourceEAsp, sourceEFill, sourcePremeter;
	float destEVol, destEAsp, destEFill, destPremeter;

	meshSource->error(sourceEVol, sourceEAsp, sourceEFill,sourcePremeter);
	meshDest->error(destEVol, destEAsp, destEFill, destPremeter);

	// new error
	float sourceEVol_, sourceEAsp_, sourceEFill_, sourcePremeter_;
	float destEVol_, destEAsp_, destEFill_, destPremeter_;

	meshSource->errorWithAssumpRemoveNode(voxelAABB, sourceEVol_, sourceEAsp_, sourceEFill_, sourcePremeter_);
	meshDest->errorWithAssumpAddNode(voxelAABB, destEVol_, destEAsp_, destEFill_, destPremeter_);

	// Compute perimeter error also
	// Perimeter ratio between bone
	std::vector<float> perimeter = getMeshBoxPerimeter();
	float periE = getPerimeterError(perimeter);
	perimeter[boneSourceIdx] = sourcePremeter_;
	perimeter[boneDestIdx] = destPremeter_;
	float periE_ = getPerimeterError(perimeter);

// 	return (sourceEVol_ + sourceEAsp_ + sourceEFill_ + destEVol_ + destEAsp_ + destEFill_ + periE_) - (sourceEVol + sourceEAsp + sourceEFill + destEVol + destEAsp + destEFill + periE);
	return (sourceEVol_ + sourceEAsp_ + sourceEFill_ + destEVol_ + destEAsp_ + destEFill_) - (sourceEVol + sourceEAsp + sourceEFill + destEVol + destEAsp + destEFill);
} 

void detailSwapManager::wrapVoxel(AABBNode* node, int sourceBoneIdx, int desBoneIdx)
{
	// This node go from Source bone to dest bone
	bvhVoxel *meshSource = meshBox[sourceBoneIdx];
	bvhVoxel *meshDest = meshBox[desBoneIdx];
	int idxInLeaf = node->IndexInLeafNode;

	AABBNode* backUpNode = new AABBNode;
	node->copy(backUpNode);
	meshSource->removeVoxel(node);
	meshDest->addVoxel(backUpNode);

	if(meshSource->boneType == CENTER_BONE && meshDest->boneType == SIDE_BONE)
	{
		// Remove one more from mesh source
		int symIdx = hashTable.getSymmetricBox(idxInLeaf);
		meshSource->removeNodeWithVoxelIdx(symIdx);
	}
	else if(meshSource->boneType == SIDE_BONE && meshDest->boneType == CENTER_BONE)
	{
		// Remove one more from mesh source
		int symIdx = hashTable.getSymmetricBox(idxInLeaf);
		meshDest->addNodeWithVoxelIdx(symIdx);
	}

	if (meshSource->boneType == CENTER_BONE && meshDest->boneType == CENTER_BONE)
	{
		int symIdx = hashTable.getSymmetricBox(idxInLeaf);
		meshSource->removeNodeWithVoxelIdx(symIdx);
		meshDest->addNodeWithVoxelIdx(symIdx);
	}


	// Update node bone index
	// Really need optimization
	std::vector<AABBNode*>* leaveSource = &meshSource->leaves;
	for (int i = 0; i < leaveSource->size(); i++)
	{
		boxes[leaveSource->at(i)->IndexInLeafNode].boneIndex = sourceBoneIdx;
	}

	std::vector<AABBNode*>* leaveDest = &meshDest->leaves;
	for (int i = 0; i < leaveDest->size(); i++)
	{
		boxes[leaveDest->at(i)->IndexInLeafNode].boneIndex = desBoneIdx;
	}

	swapNode = backUpNode;
}

std::vector<float> detailSwapManager::getMeshBoxPerimeter()
{
	std::vector<float> peri;
	for (int i = 0; i < meshBox.size(); i++)
	{
		Vec3f sizef = meshBox[i]->rightUp - meshBox[i]->leftDown;
		peri.push_back(sizef[0] + sizef[1] + sizef[2]);
	}

	return peri;
}

float detailSwapManager::getPerimeterError(std::vector<float> perimeter)
{
	float total = 0;
	for (int i = 0; i < perimeter.size(); i++)
	{
		total += perimeter[i];
	}
	for (int i = 0; i < perimeter.size(); i++)
	{
		perimeter[i] = perimeter[i]/total;
	}

	float error = 0;
	for (int i = 0; i < perimeter.size(); i++)
	{
		error += Util::normSquareAbs(meshBox[i]->expectedPerimeterRatio, perimeter[i]);
	}

	return error/perimeter.size();
}

void detailSwapManager::getInfoFromCutTree(cutSurfTreeMngr2* testCut)
{
	// We need to load
	// 1. Skeleton
	// 2. Octree high res
	// 3. mesh box

	m_octree.init("../Data_File/euroFighter.stl", 5); // Beware of this
													// Change it following cut tree
	m_skeleton.initTest(); // Use same skeleton

	// Load mesh box
	loadMeshBoxFromCutTreeWithPose(testCut);

	constructVolxeHashFromCutTree(testCut);

	constructBVHOfMeshBoxesFromCutTree(testCut);

	computeOtherForCutBoxCase();

	// Test
	// Group bone that does not have aspect ratio
	manualAssignBoneGroup();
}


void detailSwapManager::loadMeshBoxFromCutTreeWithPose(cutSurfTreeMngr2* testCut)
{
	std::vector<meshPiece> centerBox = testCut->leatE2Node2->centerBoxf;
	std::vector<meshPiece> sideBox = testCut->leatE2Node2->sideBoxf;

	// Map with bone; for expected volume ratio and aspect ratio
	std::vector<bone*> boneOrder = testCut->poseMngr.sortedBone;
	neighborPose pose = testCut->poseMngr.getPose(testCut->poseIdx);
	std::vector<arrayInt> boneAroundBone = testCut->poseMngr.boneAroundBone;
	std::map<int,int> boneMeshIdxMap = pose.mapBone_meshIdx[testCut->nodeIdx];

	float totalBoneVol = 0;
	float totalPerometer = 0;

	std::vector<meshPiece> allBoxes;
	allBoxes.insert(allBoxes.end(), centerBox.begin(), centerBox.end());
	allBoxes.insert(allBoxes.end(), sideBox.begin(), sideBox.end());

	meshBox.resize(allBoxes.size());
	meshNeighbor.resize(allBoxes.size());

	std::vector<bone*> correctBoneOrder;
	correctBoneOrder.resize(boneOrder.size());
	for (int i = 0; i < boneOrder.size(); i++)
	{
		int meshIdx = boneMeshIdxMap[i];
		correctBoneOrder[meshIdx] = boneOrder[i];
	}
	
	// Compute parameters of mesh boxes
	for (int i = 0; i < correctBoneOrder.size(); i++)
	{
		meshPiece curB = allBoxes[i];
		bone* curBone = correctBoneOrder[i];
		
		bvhVoxel* newMeshBox = new bvhVoxel;
		newMeshBox->boneName = curBone->m_name;
		newMeshBox->boneType = curBone->m_type;
		newMeshBox->leftDown = curB.leftDown;
		newMeshBox->rightUp = curB.rightUp;

		// Volume ratio
		// Expected volume ratio is easy
		newMeshBox->expectedVolRatio = curBone->m_volumef;
		totalBoneVol += curBone->m_volumef;


		// Edge ratio
		Vec3f sizeBone = curBone->m_sizef;

		// Perimeter
		totalPerometer += sizeBone[0] + sizeBone[1] + sizeBone[2];

		// assign
		meshBox[i] = newMeshBox;
	}

	for (int i = 0; i < meshBox.size(); i++)
	{
		meshBox[i]->expectedVolRatio /= totalBoneVol;
		Vec3f sizeBone = (meshBox[i]->rightUp - meshBox[i]->leftDown);
		meshBox[i]->expectedPerimeterRatio = sizeBone[0]*sizeBone[1]*sizeBone[2]/totalPerometer;
	}

	// Bone array
	std::vector<bone*> boneInDetailSwap;
	boneArray.clear();
	m_skeleton.getSortedBoneArray(boneInDetailSwap);
	for (int i = 0; i < correctBoneOrder.size(); i++)
	{
		CString nameB = correctBoneOrder[i]->m_name;
		for (int j = 0; j < boneInDetailSwap.size(); j++)
		{
			if (nameB.Compare(boneInDetailSwap[j]->m_name) == 0)
			{
				boneArray.push_back(boneInDetailSwap[j]);
				break;
			}
		}
	}
}

void detailSwapManager::loadMeshBoxFromCutTree(cutSurfTreeMngr2* testCut)
{
	std::vector<meshPiece> centerBox = testCut->leatE2Node2->centerBoxf;
	std::vector<meshPiece> sideBox = testCut->leatE2Node2->sideBoxf;

	std::vector<boneAbstract> centerBone = testCut->m_centerBoneOrder;
	std::vector<boneAbstract> sideBone = testCut->m_sideBoneOrder;

	// Map with bone; for expected volume ratio and aspect ratio
	std::vector<bone*> boneOrder;
	std::vector<std::pair<int,int>> neighborPair;
	m_skeleton.getBoneAndNeighborInfo(boneOrder, neighborPair);

	float totalBoneVol = 0;
	float totalPerometer = 0;

	centerBox.insert(centerBox.end(), sideBox.begin(), sideBox.end());
	centerBone.insert(centerBone.end(), sideBone.begin(), sideBone.end());
	for (int i = 0; i < centerBox.size(); i++)
	{
		meshPiece curB = centerBox.at(i);

		bvhVoxel* newMeshBox = new bvhVoxel;
		newMeshBox->boneName = centerBone[i].original->m_name;
		newMeshBox->boneType = centerBone[i].original->m_type;
		newMeshBox->leftDown = curB.leftDown;
		newMeshBox->rightUp = curB.rightUp;

		// Find correspond bone
		for (int j = 0; j < boneOrder.size(); j++)
		{
			if (newMeshBox->boneName.Compare(boneOrder[j]->m_name) == 0)
			{
				newMeshBox->expectedVolRatio = boneOrder[j]->m_volumef;
				totalBoneVol += boneOrder[j]->m_volumef;

				Vec3f sizeBone = boneOrder[j]->m_sizef;
				totalPerometer += sizeBone[0] + sizeBone[1] + sizeBone[2];

				Vec3i SMLIdxBone = Util_w::SMLIndexSizeOrder(sizeBone);

				Vec3f sizeMesh = newMeshBox->rightUp - newMeshBox->leftDown;
				Vec3i SMLIdxMesh = Util_w::SMLIndexSizeOrder(sizeMesh);

				Vec3f edgeRatio;
				for (int k = 0; k < 3; k++)
				{
					edgeRatio[SMLIdxMesh[k]] = sizeBone[SMLIdxBone[k]]/sizeBone[SMLIdxBone[0]];
				}
				newMeshBox->xyzRatio = edgeRatio; // Wrong!!!!!!

				break;
			}
		}

		meshBox.push_back(newMeshBox);
	}

	for (int i = 0; i < meshBox.size(); i++)
	{
		meshBox[i]->expectedVolRatio /= totalBoneVol;
		Vec3f sizeBone = meshBox[i]->rightUp - meshBox[i]->leftDown;
		meshBox[i]->expectedPerimeterRatio = sizeBone[0]*sizeBone[1]*sizeBone[2]/totalPerometer;
	}
}

void detailSwapManager::constructVolxeHashFromCutTree(cutSurfTreeMngr2* testCut)
{
	leftDownVoxel = m_octree.m_root->leftDownTight;
	rightUpVoxel = m_octree.m_root->rightUpTight;
	Vec3f sizef = rightUpVoxel - leftDownVoxel;
	voxelSize = m_octree.boxSize;

	for (int i = 0; i < 3; i++)
	{
		NumXYZ[i] = (sizef[i]/voxelSize);
	}

	hashTable.leftDown = leftDownVoxel;
	hashTable.rightUp = rightUpVoxel;
	hashTable.voxelSize = voxelSize;
	hashTable.NumXYZ = NumXYZ;
	hashTable.boxes = &boxes;

	setVoxelArray();
}

void detailSwapManager::constructBVHOfMeshBoxesFromCutTree(cutSurfTreeMngr2* testCut)
{
	std::vector<meshPiece> centerBox = testCut->leatE2Node2->centerBoxf;
	std::vector<meshPiece> sideBox = testCut->leatE2Node2->sideBoxf;
	centerBox.insert(centerBox.end(), sideBox.begin(), sideBox.end());


	int *mark;
	hashVoxel *hashVBig = &testCut->hashTable;
	{
		std::vector<voxelBox> *voxelBig = &testCut->boxes;
		mark = new int[voxelBig->size()];
		for (int i = 0; i<centerBox.size(); i++)
		{
			std::vector<int> boxIdx = centerBox[i].voxels;
			for (int j = 0; j < boxIdx.size(); j++)
			{
				mark[boxIdx[j]] = i;
			}
		}
	}

	for (int i = 0; i < meshBox.size(); i++)
	{
		meshBox[i]->boxes = &boxes;
		meshBox[i]->voxelSize = voxelSize;
		meshBox[i]->hashTable = &hashTable;
		meshBox[i]->s_octree = &m_octree;
		meshBox[i]->constructAABBTree();

		// Remove wrong node
		meshBox[i]->removeNodeNotInList(hashVBig, mark, i, centerBox[i].voxels);

		// stupid, but we need it run first
		std::vector<AABBNode*>* leaves = &meshBox[i]->leaves;
		arrayInt voxelIdxs;

		for (int j = 0; j < leaves->size(); j++)
		{
			boxes[leaves->at(j)->IndexInLeafNode].boneIndex = i;
			voxelIdxs.push_back(leaves->at(j)->IndexInLeafNode);
		}

		meshBox[i]->voxelIdxs = voxelIdxs;
		meshBox[i]->update();
	}

	delete []mark;
}

bool detailSwapManager::isObjectIfRemove(int boneIndex, int boxIdxToRemove)
{
	std::vector<AABBNode*>* leaves = &meshBox[boneIndex]->leaves;

	// Check if these node is one object
	std::fill(tempMark, tempMark + boxes.size(), 0);
	std::fill(voxelOccupy, voxelOccupy + boxes.size(), 0);
	for (int i = 0; i < leaves->size(); i++)
	{
		voxelOccupy[(*leaves)[i]->IndexInLeafNode] = 1;
	}
	voxelOccupy[boxIdxToRemove] = 0;

	// Start grow

	for (int i = 0; i < leaves->size(); i++)
	{
		if ((*leaves)[i]->IndexInLeafNode != boxIdxToRemove)
		{
			// grow from this node
			std::queue<int> vQ;
			vQ.push((*leaves)[i]->IndexInLeafNode);
			tempMark[(*leaves)[i]->IndexInLeafNode] = 1;

			int count = 0;
			while(!vQ.empty())
			{
				int idx = vQ.front();
				vQ.pop();
				tempMark[idx] = 1;

				count ++;

				std::vector<int> neighborN = neighborVoxel[idx];
				for (int j = 0; j < neighborN.size(); j++)
				{
					if (tempMark[neighborN[j]] == 0
						&& voxelOccupy[neighborN[j]] == 1)
					{
						vQ.push(neighborN[j]);
						tempMark[neighborN[j]] = 1;
					}
				}
			}

			return count == leaves->size()-1;
		}
	}

	return false;
}

bool detailSwapManager::isObjectIfRemove(int boneIndex, arrayInt boxIdxsToRemove)
{
	std::vector<AABBNode*>* leaves = &meshBox[boneIndex]->leaves;

	// Check if these node is one object
	std::fill(tempMark, tempMark + boxes.size(), 0);
	std::fill(voxelOccupy, voxelOccupy + boxes.size(), 0);
	for (int i = 0; i < leaves->size(); i++)
	{
		voxelOccupy[(*leaves)[i]->IndexInLeafNode] = 1;
	}

	for (int i = 0; i < boxIdxsToRemove.size(); i++)
	{
		voxelOccupy[boxIdxsToRemove[i]] = 0;
	}


	// Start grow

	for (int i = 0; i < leaves->size(); i++)
	{
		// avoid the removed voxel
		bool differ = true;
		for (int j = 0; j < boxIdxsToRemove.size(); j++)
		{
			if ((*leaves)[i]->IndexInLeafNode == boxIdxsToRemove[j])
			{
				differ = false;
				break;
			}
		}

		if (differ)
		{
			// grow from this node
			std::queue<int> vQ;
			vQ.push((*leaves)[i]->IndexInLeafNode);
			tempMark[(*leaves)[i]->IndexInLeafNode] = 1;

			int count = 0;
			while(!vQ.empty())
			{
				int idx = vQ.front();
				vQ.pop();
				tempMark[idx] = 1;

				count ++;

				std::vector<int> neighborN = neighborVoxel[idx];
				for (int j = 0; j < neighborN.size(); j++)
				{
					if (tempMark[neighborN[j]] == 0
						&& voxelOccupy[neighborN[j]] == 1)
					{
						vQ.push(neighborN[j]);
						tempMark[neighborN[j]] = 1;
					}
				}
			}

			return count == leaves->size()-boxIdxsToRemove.size();
		}
	}

	return false;
}

void detailSwapManager::computeOtherForCutBoxCase()
{
	tempMark = new int[boxes.size()];
	voxelOccupy = new int[boxes.size()];

	// neighbor
	neighborVoxel.resize(boxes.size());

	for(int i = 0; i < NumXYZ[0]; i++)
	{
		for (int j = 0; j < NumXYZ[1]; j++)
		{
			for(int k = 0; k < NumXYZ[2]; k++)
			{
				int idx = hashTable.getBoxIndexFromVoxelCoord(Vec3i(i,j,k));
				if (idx != -1)
				{
					for (int xyz = 0; xyz < 3; xyz++)
					{
						Vec3i nb(i,j,k);
						nb[xyz] ++;

						int idxN = hashTable.getBoxIndexFromVoxelCoord(nb);
						if (idxN != -1 && idx != idxN)
						{
							neighborVoxel[idx].push_back(idxN);
							neighborVoxel[idxN].push_back(idx);
						}
					}
				}
			}
		}
	}
}

bool detailSwapManager::isNeighborMaintainIfRemoveBox(int meshBoxIndex, int ABNodeIdx)
{
	arrayInt neighborIdx = meshNeighbor[meshBoxIndex];
	std::vector<AABBNode*>* leaves = &meshBox[meshBoxIndex]->leaves;
	bvhVoxel *mesh = meshBox[meshBoxIndex];

	Box meshb;
	mesh->boundingBoxIfRemoveNode(leaves->at(ABNodeIdx), meshb);
	Vec3f diag = Vec3f(voxelSize, voxelSize, voxelSize);
	meshb.leftDown = meshb.leftDown - diag/2.0;
	meshb.rightUp = meshb.rightUp + diag/2.0;
	GeometricFunc geoF;

	for (int i = 0; i < neighborIdx.size(); i++)
	{
		bvhVoxel *nbMesh = meshBox[neighborIdx[i]];
		// We check bounding box only
		if (!geoF.collisionBtwBox(meshb, Box(nbMesh->leftDown, nbMesh->rightUp)))
		{
			return false;
		}
	}

	return true;
}

void detailSwapManager::swapOnePossibleVoxel()
{
	TRACE0("\nCallswap voxels");
	// check all swappable voxel
	float ErrorReduced = 0;
	int sourceBoneIdx, desBoneIdx;
	AABBNode* node = nullptr;

	for (int i = 0; i < meshBox.size(); i++)
	{
		// Check all mesh box
		std::vector<AABBNode*>* leaves =  &meshBox[i]->leaves;

		// The voxel should be as far as possible from center of source mesh
		int sourceAABBIdx;

		for (int j = 0; j < leaves->size(); j++)
		{
			voxelBox curBox = boxes[leaves->at(j)->IndexInLeafNode];
			ASSERT(curBox.boneIndex != -1);
			Vec3i vCoord = hashTable.getVoxelCoord(curBox.center);



			// This loop is not optimized
			for (int d = -1; d < 2; d+=2)
			{
				for (int xyzI = 0; xyzI < 3; xyzI++)
				{
					// available neighbor
					Vec3i neighborVC = vCoord;
					neighborVC[xyzI] += d;
					int neighborIdx = hashTable.getBoxIndexFromVoxelCoord(neighborVC);

					if (neighborIdx != -1)
					{
						voxelBox neighborBox = boxes[neighborIdx];
						if (neighborBox.boneIndex != -1
							&& neighborBox.boneIndex != curBox.boneIndex)
						{
							// Check if newly created obj still is an object
							// If center box <-> center box
							// We swap 2 box, then need to check 2 box
							if (meshBox[i]->boneType == CENTER_BONE
								&& meshBox[neighborBox.boneIndex]->boneType == CENTER_BONE)
							{
								arrayInt idxs;
								idxs.push_back(leaves->at(j)->IndexInLeafNode);
								idxs.push_back(hashTable.getSymmetricBox(leaves->at(j)->IndexInLeafNode));
								if (!isObjectIfRemove(curBox.boneIndex, idxs))
								{
									continue;
								}
							}
							else
							{
								if (!isObjectIfRemove(curBox.boneIndex, leaves->at(j)->IndexInLeafNode))
								{
									continue;
								}
							}

							// Side bone cannot receive voxel over center surface
							if (meshBox[neighborBox.boneIndex]->boneType == SIDE_BONE)
							{
								if (leaves->at(j)->RightUp[0] > m_octree.centerMesh[0] - m_octree.boxSize/2)
								{
									continue;
								}
							}

							// Neighbor relation must be maintain
							// The box that voxel is removed should be still contact with its neighbor
							if (!isNeighborMaintainIfRemoveBox(curBox.boneIndex, j))
							{
								continue;
							}

							if (meshBox[i]->boneType == CENTER_BONE
								&& meshBox[neighborBox.boneIndex]->boneType == CENTER_BONE)
							{
								std::vector<AABBNode*> nodes;
								nodes.push_back(leaves->at(j));
								int symNodeIdx = (hashTable.getSymmetricBox(leaves->at(j)->IndexInLeafNode));
								// Find it
								for (int kk = 0; kk < leaves->size(); kk++)
								{
									if (leaves->at(kk)->IndexInLeafNode == symNodeIdx)
									{
										nodes.push_back(leaves->at(kk));
										break;
									}
								}
								ASSERT(nodes.size() == 2);

								if (shouldSwap(nodes, curBox.boneIndex, neighborBox.boneIndex))
								{
									// We check if we should swap the whole surface
									for (int xyzI = 0; xyzI < 3; xyzI ++)
									{
										//std::vector<AABBNode*> nodes = me
									}

									TRACE0("\nSwapped!");
									node = leaves->at(j);
									sourceBoneIdx = i;
									desBoneIdx = neighborBox.boneIndex;

									wrapVoxel(node, sourceBoneIdx, desBoneIdx);
									return;
								}
							}
							else
							{
								if (shouldSwap(leaves->at(j), curBox.boneIndex, neighborBox.boneIndex))
								{
									TRACE0("\nSwapped!");
									node = leaves->at(j);
									sourceBoneIdx = i;
									desBoneIdx = neighborBox.boneIndex;

									wrapVoxel(node, sourceBoneIdx, desBoneIdx);
									return;
								}
							}
							
						}
					}
				}
			}


		}
	}
}
void detailSwapManager::swapOnePossibleVoxel2()
{
	TRACE0("\nCallswap voxels");
	// check all swappable voxel
	float ErrorReduced = 0;
	int sourceBoneIdx, desBoneIdx;
	AABBNode* node = nullptr;
	bool isCenterCenter = false;

	for (int i = 0; i < meshBox.size(); i++)
	{
		// Check all mesh box
		std::vector<AABBNode*>* leaves =  &meshBox[i]->leaves;

		// The voxel should be as far as possible from center of source mesh
		int sourceAABBIdx = -1;
		int destMeshIdx;
		float minDis = MAX;
		

		for (int j = 0; j < leaves->size(); j++)
		{
			voxelBox curBox = boxes[leaves->at(j)->IndexInLeafNode];
			ASSERT(curBox.boneIndex != -1);
			Vec3i vCoord = hashTable.getVoxelCoord(curBox.center);



			// This loop is not optimized
			for (int d = -1; d < 2; d+=2)
			{
				for (int xyzI = 0; xyzI < 3; xyzI++)
				{
					// available neighbor
					Vec3i neighborVC = vCoord;
					neighborVC[xyzI] += d;
					int neighborIdx = hashTable.getBoxIndexFromVoxelCoord(neighborVC);

					if (neighborIdx != -1)
					{
						voxelBox neighborBox = boxes[neighborIdx];
						if (neighborBox.boneIndex != -1
							&& neighborBox.boneIndex != curBox.boneIndex)
						{
							// Check if newly created obj still is an object
							// If center box <-> center box
							// We swap 2 box, then need to check 2 box
							if (meshBox[i]->boneType == CENTER_BONE
								&& meshBox[neighborBox.boneIndex]->boneType == CENTER_BONE)
							{
								arrayInt idxs;
								idxs.push_back(leaves->at(j)->IndexInLeafNode);
								idxs.push_back(hashTable.getSymmetricBox(leaves->at(j)->IndexInLeafNode));
								if (!isObjectIfRemove(curBox.boneIndex, idxs))
								{
									continue;
								}

								if (!isNeighborMaintainIfRemoveBox(curBox.boneIndex, j))
								{
									continue;
								}

								// Check swap
								std::vector<AABBNode*> nodes;
								nodes.push_back(leaves->at(j));
								int symNodeIdx = (hashTable.getSymmetricBox(leaves->at(j)->IndexInLeafNode));
								// Find it
								for (int kk = 0; kk < leaves->size(); kk++)
								{
									if (leaves->at(kk)->IndexInLeafNode == symNodeIdx)
									{
										nodes.push_back(leaves->at(kk));
										break;
									}
								}
								ASSERT(nodes.size() == 2);

								if (shouldSwap(nodes, curBox.boneIndex, neighborBox.boneIndex))
								{


									// We check if we should swap the whole surface
									TRACE0("\nSwapped!");
									node = leaves->at(j);
									sourceBoneIdx = i;
									desBoneIdx = neighborBox.boneIndex;

									// store
									isCenterCenter = true;
									Vec3f center = (meshBox[desBoneIdx]->leftDown + meshBox[desBoneIdx]->rightUp) /2.0;
									float dd = ((node->LeftDown+node->RightUp)/2.0 - center).norm();
									if (dd < minDis)
									{
										minDis = dd;
										destMeshIdx = desBoneIdx;
										sourceAABBIdx = j;
									}

// 									wrapVoxel(node, sourceBoneIdx, desBoneIdx);
// 									return;
								}
							}
							else if (!isCenterCenter)
							{
								if (!isObjectIfRemove(curBox.boneIndex, leaves->at(j)->IndexInLeafNode))
								{
									continue;
								}
								// Side bone cannot receive voxel over center surface
								if (meshBox[neighborBox.boneIndex]->boneType == SIDE_BONE)
								{
									if (leaves->at(j)->RightUp[0] > m_octree.centerMesh[0] - m_octree.boxSize/2)
									{
										continue;
									}
								}

								if (!isNeighborMaintainIfRemoveBox(curBox.boneIndex, j))
								{
									continue;
								}

								if (shouldSwap(leaves->at(j), curBox.boneIndex, neighborBox.boneIndex))
								{
									TRACE0("\nSwapped!");
									node = leaves->at(j);
									sourceBoneIdx = i;
									desBoneIdx = neighborBox.boneIndex;

									// store
									Vec3f center = (meshBox[desBoneIdx]->leftDown + meshBox[desBoneIdx]->rightUp) /2.0;
									float dd = ((node->LeftDown+node->RightUp)/2.0 - center).norm();
									if (dd < minDis)
									{
										minDis = dd;
										destMeshIdx = desBoneIdx;
										sourceAABBIdx = j;
									}

// 									wrapVoxel(node, sourceBoneIdx, desBoneIdx);
// 									return;
								}
							}
						}
					}
				}
			}


		}

		if (sourceAABBIdx != -1)
		{
			node = leaves->at(sourceAABBIdx);
			wrapVoxel(node, i, destMeshIdx);
		}
	}
}
bool detailSwapManager::shouldSwap(AABBNode* voxelAABB, int boneSourceIdx, int boneDestIdx)
{
	// Error also should weight with bone size
	bvhVoxel* meshSource = meshBox[boneSourceIdx];
	bvhVoxel* meshDest = meshBox[boneDestIdx];

	// Current error
	float sourceEVol, sourceEAsp, sourceEFill, sourcePremeter;
	float destEVol, destEAsp, destEFill, destPremeter;

	meshSource->error(sourceEVol, sourceEAsp, sourceEFill, sourcePremeter);
	meshDest->error(destEVol, destEAsp, destEFill, destPremeter);

	// new error
	float sourceEVol_, sourceEAsp_, sourceEFill_, sourcePremeter_;
	float destEVol_, destEAsp_, destEFill_, destPremeter_;

	meshSource->errorWithAssumpRemoveNode(voxelAABB, sourceEVol_, sourceEAsp_, sourceEFill_, sourcePremeter_);
	meshDest->errorWithAssumpAddNode(voxelAABB, destEVol_, destEAsp_, destEFill_, destPremeter_);

	float aspectg = (sourceEAsp+destEAsp - (sourceEAsp_+destEAsp_))/(sourceEAsp+destEAsp);
	if (sourceEAsp+destEAsp > sourceEAsp_+destEAsp_)
	{
		return true;
	}
	else if (aspectg > -0.01) // Actually, want to compare if it equal
	{
		return ((sourceEVol + destEVol) - (sourceEVol_ + destEVol_)) > 0;
	}
	else // aspect ratio error go down
	{
		return false; // Actually should use some weight
	}

	return false;
}

bool detailSwapManager::shouldSwap(std::vector<AABBNode*> voxelAABBs, int boneSourceIdx, int boneDestIdx)
{
	// Error also should weight with bone size
	bvhVoxel* meshSource = meshBox[boneSourceIdx];
	bvhVoxel* meshDest = meshBox[boneDestIdx];

	// Current error
	float sourceEVol, sourceEAsp, sourceEFill, sourcePremeter;
	float destEVol, destEAsp, destEFill, destPremeter;

	meshSource->error(sourceEVol, sourceEAsp, sourceEFill, sourcePremeter);
	meshDest->error(destEVol, destEAsp, destEFill, destPremeter);

	// new error
	float sourceEVol_, sourceEAsp_, sourceEFill_, sourcePremeter_;
	float destEVol_, destEAsp_, destEFill_, destPremeter_;

	meshSource->errorWithAssumpRemoveNode(voxelAABBs, sourceEVol_, sourceEAsp_, sourceEFill_, sourcePremeter_);
	meshDest->errorWithAssumpAddNode(voxelAABBs, destEVol_, destEAsp_, destEFill_, destPremeter_);

	float aspectg = (sourceEAsp+destEAsp - (sourceEAsp_+destEAsp_))/(sourceEAsp+destEAsp);
	if (sourceEAsp+destEAsp > sourceEAsp_+destEAsp_)
	{
		return true;
	}
	else if (aspectg > -0.01) // Actually, want to compare if it equal
	{
		return ((sourceEVol + destEVol) - (sourceEVol_ + destEVol_)) > 0;
	}
	else // aspect ratio error go down
	{
		return false; // Actually should use some weight
	}

	return false;
}

bool detailSwapManager::shouldSwap(int sourceMeshIdx, int destMeshIdx, arrayInt voxelIdx)
{
	// We compare the aspect ratio
	// If unchanged, we compare volume ratio
	// The object is warrantied to be a valid voxel set (All have connection together)
	bvhVoxel *meshSource = meshBox[sourceMeshIdx];
	bvhVoxel *meshDest = meshBox[destMeshIdx];

	// error of original source mesh and dest mesh
	float aspectE_s, volumeE_s, aspectE_d, volumeE_d;
	meshSource->getCurrentError(aspectE_s, volumeE_s);
	meshDest->getCurrentError(aspectE_d, volumeE_d);

	// error of source mesh and dest mesh after swap
	float aspectE_s_a, volumeE_s_a, aspectE_d_a, volumeE_d_a;
	meshSource->getErrorWithAssumeRemoveVoxel(aspectE_s_a, volumeE_s_a, voxelIdx);
	meshDest->getErrorWithAssumeAddVoxel(aspectE_d_a, volumeE_d_a, voxelIdx);

	float originalE = meshSource->expectedVolRatio*aspectE_s + meshDest->expectedVolRatio*aspectE_d;
	float assumeE = meshSource->expectedVolRatio*aspectE_s_a + meshDest->expectedVolRatio*aspectE_d_a;
	float aspectg = (originalE - assumeE)/originalE;
	if (aspectg > 0)
	{
		return true;
	}
	else if (aspectg > -0.01) // Actually, want to compare if it equal
	{
		return ((volumeE_s + volumeE_d) - (volumeE_s_a + volumeE_d_a)) > 0;
	}
	else // aspect ratio error go down
	{
		return false; // Actually should use some weight
	}

	return false;
}

bool detailSwapManager::shouldSwap(int sourceMeshIdx, int destMeshIdx, arrayInt voxelIdxRemoveFromSource, arrayInt voxelIdxAddToDest)
{
	// We compare the aspect ratio
	// If unchanged, we compare volume ratio
	// The object is warrantied to be a valid voxel set (All have connection together)
	bvhVoxel *meshSource = meshBox[sourceMeshIdx];
	bvhVoxel *meshDest = meshBox[destMeshIdx];

	// error of original source mesh and dest mesh
	float aspectE_s, volumeE_s, aspectE_d, volumeE_d;
	meshSource->getCurrentError(aspectE_s, volumeE_s);
	meshDest->getCurrentError(aspectE_d, volumeE_d);

	// error of source mesh and dest mesh after swap
	float aspectE_s_a, volumeE_s_a, aspectE_d_a, volumeE_d_a;
	meshSource->getErrorWithAssumeRemoveVoxel(aspectE_s_a, volumeE_s_a, voxelIdxRemoveFromSource);
	meshDest->getErrorWithAssumeAddVoxel(aspectE_d_a, volumeE_d_a, voxelIdxAddToDest);

	float originalE = meshSource->expectedVolRatio*aspectE_s + meshDest->expectedVolRatio*aspectE_d;
	float assumeE = meshSource->expectedVolRatio*aspectE_s_a + meshDest->expectedVolRatio*aspectE_d_a;
	float aspectg = (originalE - assumeE)/originalE;
	if (aspectg > 0)
	{
		return true;
	}
	else if (aspectg > -0.01) // Actually, want to compare if it equal
	{
		return ((volumeE_s + volumeE_d) - (volumeE_s_a + volumeE_d_a)) > 0;
	}
	else // aspect ratio error go down
	{
		return false; // Actually should use some weight
	}

	return false;
}

void detailSwapManager::manuallySetupCoord()
{
	coords.push_back(Vec3i(1,0,2));
	coords.push_back(Vec3i(1,0,2));
	coords.push_back(Vec3i(1,2,0));
	coords.push_back(Vec3i(1,2,0));
}

void detailSwapManager::computeAspectRatio()
{
	// Compute expected aspect ratio
	// 1, y/x, z/x
	// On world coordinate
	// Where the mesh lie

	for (int i = 0; i < meshBox.size(); i++)
	{
		Vec3f boneLocalSize = boneArray[i]->m_sizef;
		Vec3f sizeInWord;
		Vec3i coordMap = coords[i];

		for (int j = 0; j < 3; j++)
		{
			sizeInWord[coordMap[j]] = boneLocalSize[j];
		}

		for (int j = 0; j < 3; j++)
		{
			meshBox[i]->xyzRatio[j] = sizeInWord[j]/sizeInWord[0];
		}
	}
}

void detailSwapManager::updateDisplay(int idx1, int idx2)
{
	m_idx1 = idx1;
	m_idx2 = idx2;
}

void detailSwapManager::swapPossibleLayer()
{
	// Consider boxes is already sorted by order of size
	// Priority swapping big box first
	for (int i = 0; i < meshBox.size(); i++)
	{
		for (int j = i+1; j < meshBox.size(); j++)
		{
			if (swapBoxLayer(i, j))
			{
				// swap

				return;
			}
		}
	}
}

bool detailSwapManager::swapBoxLayer(int meshBoxIdx1, int meshBoxIdx2)
{
	bvhVoxel *mesh1 = meshBox[meshBoxIdx1];
	bvhVoxel *mesh2 = meshBox[meshBoxIdx2];

	Box b1(mesh1->curLeftDown, mesh1->curRightUp);
	Box b2(mesh2->curLeftDown, mesh2->curRightUp);

	if (!isBoxInContact(b1, b2))
	{
		return false;
	}

	// Check on face of destIdx, if it can receive a layer from source mesh
	for (int xyzDirect = 0; xyzDirect < 3; xyzDirect ++)
	{
		// Check on both edge of the box
		float cPos[4];
		cPos[0] = b1.leftDown[xyzDirect];
		cPos[1] = b1.rightUp[xyzDirect];
		cPos[2] = b2.leftDown[xyzDirect];
		cPos[3] = b2.rightUp[xyzDirect];

		for (int j = 0; j < 4; j++)
		{
			arrayInt swapableIdx1, swapableIdx2;
			arrayInt remainIdx1, remainIdx2;
			getSwapableVoxel(xyzDirect, cPos[j], meshBoxIdx1, meshBoxIdx2, swapableIdx1, swapableIdx2, remainIdx1, remainIdx2);

			if (swapableIdx1.size() == 0)
			{
				continue;
			}

			// a. meshBoxIdx1 is source
			if (swapVoxel(meshBoxIdx1, meshBoxIdx2, swapableIdx1, remainIdx1, xyzDirect))
			{
				return true;
			}
			else if (swapVoxel(meshBoxIdx2, meshBoxIdx1, swapableIdx2, remainIdx2, xyzDirect))
			{
				return true;
			}
						
		}
	}

	return false;
}

bool detailSwapManager::isBoxInContact(Box b1, Box b2)
{
	Vec3f diag(voxelSize, voxelSize, voxelSize);
	Box bb2 = b2;
	bb2.leftDown = b2.leftDown - diag/2.0;
	bb2.rightUp = b2.rightUp + diag/2.0;
	GeometricFunc geoF;
	return geoF.collisionBtwBox(b1, bb2);
}

void detailSwapManager::getSwapableVoxel(int xyzDirect, float cPos, int meshBoxIdx1, int meshBoxIdx2, arrayInt &swapableIdx1, arrayInt &swapableIdx2, arrayInt &remainIdx1, arrayInt &remainIdx2)
{
	Vec3f LD = hashTable.leftDown;
	Vec3f RU = hashTable.rightUp;
	Vec3i sizei = hashTable.NumXYZ;

	int cPosi = floor((cPos - LD[xyzDirect] - voxelSize/2.0)/voxelSize);

	// The other axis, that differ to xyzDirect
	int xyz1, xyz2;
	getTwoOtherIndex(xyzDirect, xyz1, xyz2);

	for(int i1 = 0; i1 < sizei[xyz1]; i1++)
	{
		for (int i2 = 0; i2 < sizei[xyz2]; i2++)
		{
			// Voxel coordinate of the two voxel
			Vec3i pti;
			pti[xyz1] = i1;
			pti[xyz2] = i2;

			Vec3i pti1 = pti, pti2 = pti;
			pti1[xyzDirect] = cPosi;
			pti2[xyzDirect] = cPosi + 1;

			// Check if they are swappable
			int voxelIdx1 = hashTable.getBoxIndexFromVoxelCoord(pti1);
			int voxelIdx2 = hashTable.getBoxIndexFromVoxelCoord(pti2);

			if (voxelIdx1 == -1 || voxelIdx2 == -1)
			{
				continue;
			}

			if (boxes[voxelIdx1].boneIndex == meshBoxIdx1)
			{
				if (boxes[voxelIdx2].boneIndex == meshBoxIdx2)
				{
					swapableIdx1.push_back(voxelIdx1);
					swapableIdx2.push_back(voxelIdx2);
				}
				else
					remainIdx1.push_back(voxelIdx1);

			}
			else if (boxes[voxelIdx1].boneIndex == meshBoxIdx2)
			{
				if (boxes[voxelIdx2].boneIndex == meshBoxIdx1)
				{
					swapableIdx1.push_back(voxelIdx2);
					swapableIdx2.push_back(voxelIdx1);
				}
				else
					remainIdx2.push_back(voxelIdx1);
			}
			else
			{
				if (boxes[voxelIdx2].boneIndex == meshBoxIdx1)
					remainIdx1.push_back(voxelIdx2);
				else if (boxes[voxelIdx2].boneIndex == meshBoxIdx2)
					remainIdx2.push_back(voxelIdx2);
			}
		}
	}
}

void detailSwapManager::getTwoOtherIndex(int xyzDirect, int &xyz1, int &xyz2)
{
	if (xyzDirect == 0)
	{
		xyz1 = 1;
		xyz2 = 2;
	}
	if (xyzDirect == 1)
	{
		xyz1 = 0;
		xyz2 = 2;
	}
	if (xyzDirect == 2)
	{
		xyz1 = 1;
		xyz2 = 0;
	}
}

bool detailSwapManager::swapVoxel(int sourceMeshIdx, int destMeshIdx, arrayInt voxelIdxsInSource,  arrayInt voxelRemainInLayer, int xyzDirect)
{
	// Hash of swap voxel
	std::fill(voxelOccupy, voxelOccupy + boxes.size(), 0);;
	for (int i = 0; i < voxelIdxsInSource.size(); i++)
	{
		voxelOccupy[voxelIdxsInSource[i]] = 1;
	}

	// Hash of AABBNodes
	// If center box to side box, swap voxel should not over center surface
	bvhVoxel *meshSource = meshBox[sourceMeshIdx];
	bvhVoxel *meshDest = meshBox[destMeshIdx];
	if (meshSource->boneType == CENTER_BONE
		&& meshDest->boneType == SIDE_BONE)
	{
		arrayInt voxelToBeConsider;
		for (int i = 0; i < voxelRemainInLayer.size(); i++)
		{
			voxelBox curB = boxes[voxelRemainInLayer[i]];
			if (curB.center[0] < m_octree.centerMesh[0])
			{
				voxelToBeConsider.push_back(voxelRemainInLayer[i]);
			}
		}
		voxelRemainInLayer = voxelToBeConsider;
	}


	bool bShouldSwap = false;
	// Check if should swap
	while(1)
	{
		bool bValid = false;
		for (int i = 0; i < voxelRemainInLayer.size(); i++)
		{
			// Check if it is neighbor
			int curIdx = voxelRemainInLayer[i];
			if (voxelOccupy[curIdx] == 1)
			{// This voxel is already added
				continue;
			}

			arrayInt nb = neighborVoxel[curIdx];
			bool bIsNeighbor = false;
			for (int j = 0; j < nb.size(); j++)
			{
				if (voxelOccupy[nb[j]] == 1)
				{
					bIsNeighbor = true;
					break;
				}
			}

			if(bIsNeighbor)
			{
				// Check if can be swap
				voxelIdxsInSource.push_back(curIdx);
				if (shouldSwap(sourceMeshIdx, destMeshIdx, voxelIdxsInSource))
				{
					bShouldSwap = true;
					bValid = true;

					voxelOccupy[curIdx] = 1;
					break;
				}
				voxelIdxsInSource.pop_back();
			}
		}

		if (!bValid)
		{
			break;
		}
	}

	if (!bShouldSwap)
	{
		if (shouldSwap(sourceMeshIdx, destMeshIdx, voxelIdxsInSource))
		{
			bShouldSwap = true;
		}
	}

	if (bShouldSwap)
	{// Swap
		arrayInt sourceIdx, destIdx;
		getTheVoxelWeShouldSwap(sourceMeshIdx, destMeshIdx, voxelIdxsInSource, sourceIdx, destIdx);

		// Check if it a an object, the source mesh
		if (isObjectIfRemoveVoxel(sourceMeshIdx, sourceIdx))
		{
			swapLayer(sourceMeshIdx, destMeshIdx, voxelIdxsInSource);
			return true;
		}
	}

	return false;
}

void detailSwapManager::swapLayer(int sourceMeshIdx, int destMeshIdx, arrayInt voxelIdxsInSource)
{
	bvhVoxel *meshSource = meshBox[sourceMeshIdx];
	bvhVoxel *meshDest = meshBox[destMeshIdx];

	arrayInt sourceIdx, destIdx;
	getTheVoxelWeShouldSwap(sourceMeshIdx, destMeshIdx, voxelIdxsInSource, sourceIdx, destIdx);

	meshSource->removeVoxel(sourceIdx);
	meshDest->addVoxel(destIdx);

	// Update node bone index
	for (int i = 0; i < destIdx.size(); i++)
	{
		(*s_boxes)[destIdx[i]].boneIndex = destMeshIdx;
	}
}

bool detailSwapManager::isObjectIfRemoveVoxel(int boneIndex, arrayInt boxIdxsToRemove)
{
	arrayInt meshBoxIdx = meshBox[boneIndex]->voxelIdxs;

	// Check if these node is one object
	std::fill(tempMark, tempMark + s_boxes->size(), 0);
	std::fill(voxelOccupy, voxelOccupy + s_boxes->size(), 0);

	for (int i = 0; i < meshBoxIdx.size(); i++)
	{
		voxelOccupy[meshBoxIdx[i]] = 1;
	}
	// Remove the box in the removing list
	for (int i = 0; i < boxIdxsToRemove.size(); i++)
	{
		voxelOccupy[boxIdxsToRemove[i]] = 0;
	}


	// Start grow

	for (int i = 0; i < meshBoxIdx.size(); i++)
	{
		// avoid the removed voxel
		bool differ = true;
		for (int j = 0; j < boxIdxsToRemove.size(); j++)
		{
			if (meshBoxIdx[i] == boxIdxsToRemove[j])
			{
				differ = false;
				break;
			}
		}

		if (differ)
		{
			// grow from this node
			std::queue<int> vQ;
			vQ.push(meshBoxIdx[i]);
			tempMark[meshBoxIdx[i]] = 1;

			int count = 0;
			while(!vQ.empty())
			{
				int idx = vQ.front();
				vQ.pop();
				tempMark[idx] = 1;

				count ++;

				std::vector<int> neighborN = (*s_boxShareFaceWithBox)[idx];
				for (int j = 0; j < neighborN.size(); j++)
				{
					if (tempMark[neighborN[j]] == 0
						&& voxelOccupy[neighborN[j]] == 1)
					{
						vQ.push(neighborN[j]);
						tempMark[neighborN[j]] = 1;
					}
				}
			}

			return count == meshBoxIdx.size()-boxIdxsToRemove.size();
		}
	}

	return false;
}

void detailSwapManager::swapOneVoxel()
{
	float ErrorReduced = 0;
	int sourceBoneIdx, desBoneIdx;
	AABBNode* node = nullptr;
	bool isCenterCenter = false;

	for (int i = 0; i < meshBox.size(); i++)
	{
		// Check all mesh box
		arrayInt voxelIdx = meshBox[i]->voxelIdxs;

		for (int j = 0; j < voxelIdx.size(); j++)
		{
			voxelBox curBox = boxes[voxelIdx[j]];
			ASSERT(curBox.boneIndex != -1);

			arrayInt nbVoxel = neighborVoxel[voxelIdx[j]];
			for (int k = 0; k < nbVoxel.size(); k++)
			{
				int neighborIdx = nbVoxel[k];
				voxelBox neighborBox = boxes[neighborIdx];

				if (neighborBox.boneIndex != -1
					&& neighborBox.boneIndex != curBox.boneIndex)
				{
					// Can be swapped
					arrayInt voxIdx;
					voxIdx.push_back(voxelIdx[j]);
					arrayInt boxremoveFromSource, boxAddToDest;

					// If center box <-> center box
					// We swap 2 box, then need to check 2 box
					getSwapVoxelList(i, neighborBox.boneIndex, voxIdx, boxremoveFromSource, boxAddToDest);

					if (shouldSwap(i, neighborBox.boneIndex, boxremoveFromSource, boxAddToDest))
					{
						// Check if it a an object, the source mesh
						if(isObjectIfRemoveVoxel(i, boxremoveFromSource))
						{
							swapVoxels(i, neighborBox.boneIndex, boxremoveFromSource, boxAddToDest);
							return;
						}
					}
				}
			}
		}
	}
}

void detailSwapManager::getSwapVoxelList(int sourceMeshIdx, int destMeshIdx, arrayInt voxelIdxsInSource, arrayInt &voxelToRemove, arrayInt &voxelToAdd)
{
	voxelToRemove = voxelIdxsInSource;
	voxelToAdd = voxelIdxsInSource;

	int soureType = meshBox[sourceMeshIdx]->boneType;
	int destType = meshBox[destMeshIdx]->boneType;

	int* mark = new int[boxes.size()];
	std::fill(mark, mark+boxes.size(), 0);
	for (int i = 0; i < voxelIdxsInSource.size(); i++)
	{
		mark[voxelIdxsInSource[i]] = 1;
	}

	for (int i = 0; i < voxelIdxsInSource.size(); i++)
	{
		int symIdx = hashTable.getSymmetricBox(voxelIdxsInSource[i]);
		if (mark[symIdx] == 1)
		{
			continue;
		}
		mark[symIdx] == 1;

		if (soureType == CENTER_BONE && destType == CENTER_BONE)
		{
			voxelToRemove.push_back(symIdx);
			voxelToAdd.push_back(symIdx);
		}
		else if (soureType == CENTER_BONE && destType == SIDE_BONE)
		{
			voxelToRemove.push_back(symIdx);
		}
		else if (soureType == SIDE_BONE && destType == CENTER_BONE)
		{
			voxelToAdd.push_back(symIdx);
		}
	}

	delete []mark;
}

void detailSwapManager::swapVoxels(int sourceMeshIdx, int destMeshIdx, arrayInt voxelIdxsInSource, arrayInt voxelAddToDest)
{
	bvhVoxel *meshSource = meshBox[sourceMeshIdx];
	bvhVoxel *meshDest = meshBox[destMeshIdx];

	meshSource->removeVoxel(voxelIdxsInSource);
	meshDest->addVoxel(voxelAddToDest);

	// Update node bone index
	for (int i = 0; i < voxelAddToDest.size(); i++)
	{
		boxes[voxelAddToDest[i]].boneIndex = destMeshIdx;
	}
	for (int i = 0; i < voxelIdxsInSource.size(); i++)
	{
		boxes[voxelIdxsInSource[i]].boneIndex = destMeshIdx;
	}
}

void detailSwapManager::manualAssignBoneGroup()
{
	std::vector<CString> boneName;
	boneName.push_back(_T("hand"));
	boneName.push_back(_T("leg"));

	for (int i = 0; i < boneArray.size(); i++)
	{
		for (size_t j = 0; j < boneName.size(); j++)
		{
			if (boneName[j].CompareNoCase(boneArray[i]->m_name) == 0)
			{
				boneArray[i]->bIsGroup = true;
				meshBox[i]->bIsGroupBone = true;
			}
		}
	}
}

void detailSwapManager::swapPossibleLayer2()
{
	// Consider boxes is already sorted by order of size
	// Priority swapping big box first
	for (int i = 0; i < meshBox.size(); i++)
	{
		for (int j = i + 1; j < meshBox.size(); j++)
		{
			if (swapBoxLayer2(i, j))
			{
				// swap

				return;
			}
		}
	}
}

bool detailSwapManager::swapBoxLayer2(int meshBoxIdx1, int meshBoxIdx2)
{
	bvhVoxel *mesh1 = meshBox[meshBoxIdx1];
	bvhVoxel *mesh2 = meshBox[meshBoxIdx2];

	Box b1(mesh1->curLeftDown, mesh1->curRightUp);
	Box b2(mesh2->curLeftDown, mesh2->curRightUp);

	if (!isBoxInContact(b1, b2))
	{
		return false;
	}

	// Check on face of destIdx, if it can receive a layer from source mesh
	for (int xyzDirect = 0; xyzDirect < 3; xyzDirect++)
	{
		// Check on both edge of the box
		float cPos[4];
		cPos[0] = b1.leftDown[xyzDirect];
		cPos[1] = b1.rightUp[xyzDirect];
		cPos[2] = b2.leftDown[xyzDirect];
		cPos[3] = b2.rightUp[xyzDirect];

		for (int j = 0; j < 4; j++)
		{
			arrayInt swapableIdx1, swapableIdx2;
			arrayInt remainIdx1, remainIdx2;
			getSwapableVoxel(xyzDirect, cPos[j], meshBoxIdx1, meshBoxIdx2, swapableIdx1, swapableIdx2, remainIdx1, remainIdx2);

			if (swapableIdx1.size() == 0)
			{
				continue;
			}

			// a. meshBoxIdx1 is source
			if (swapVoxel2(meshBoxIdx1, meshBoxIdx2, swapableIdx1, remainIdx1, xyzDirect))
			{
				return true;
			}
			else if (swapVoxel2(meshBoxIdx2, meshBoxIdx1, swapableIdx2, remainIdx2, xyzDirect))
			{
				return true;
			}

		}
	}

	return false;
}

bool detailSwapManager::swapVoxel2(int sourceMeshIdx, int destMeshIdx, arrayInt voxelIdxsInSource, arrayInt voxelRemainInLayer, int xyzDirect)
{
	// Hash of swap voxel
	std::fill(voxelOccupy, voxelOccupy + boxes.size(), 0);;
	for (int i = 0; i < voxelIdxsInSource.size(); i++)
	{
		voxelOccupy[voxelIdxsInSource[i]] = 1;
	}

	// Hash of AABBNodes
	// If center box to side box, swap voxel should not over center surface
	bvhVoxel *meshSource = meshBox[sourceMeshIdx];
	bvhVoxel *meshDest = meshBox[destMeshIdx];
	if (meshSource->boneType == CENTER_BONE
		&& meshDest->boneType == SIDE_BONE)
	{
		arrayInt voxelToBeConsider;
		for (int i = 0; i < voxelRemainInLayer.size(); i++)
		{
			voxelBox curB = boxes[voxelRemainInLayer[i]];
			if (curB.center[0] < m_octree.centerMesh[0])
			{
				voxelToBeConsider.push_back(voxelRemainInLayer[i]);
			}
		}
		voxelRemainInLayer = voxelToBeConsider;
	}


	bool bShouldSwap = false;
	// Check if should swap
	while (1)
	{
		bool bValid = false;
		for (int i = 0; i < voxelRemainInLayer.size(); i++)
		{
			// Check if it is neighbor
			int curIdx = voxelRemainInLayer[i];
			if (voxelOccupy[curIdx] == 1)
			{// This voxel is already added
				continue;
			}

			arrayInt nb = neighborVoxel[curIdx];
			bool bIsNeighbor = false;
			for (int j = 0; j < nb.size(); j++)
			{
				if (voxelOccupy[nb[j]] == 1)
				{
					bIsNeighbor = true;
					break;
				}
			}

			if (bIsNeighbor)
			{
				// Check if can be swap
				voxelIdxsInSource.push_back(curIdx);
				if (shouldSwap2(sourceMeshIdx, destMeshIdx, voxelIdxsInSource))
				{
					bShouldSwap = true;
					bValid = true;

					voxelOccupy[curIdx] = 1;
					break;
				}
				voxelIdxsInSource.pop_back();
			}
		}

		if (!bValid)
		{
			break;
		}
	}

	if (!bShouldSwap)
	{
		if (shouldSwap2(sourceMeshIdx, destMeshIdx, voxelIdxsInSource))
		{
			bShouldSwap = true;
		}
	}

	if (bShouldSwap)
	{// Swap
		// Check if it a an object, the source mesh
		if (isObjectIfRemoveVoxel(sourceMeshIdx, voxelIdxsInSource))
		{
			swapLayer(sourceMeshIdx, destMeshIdx, voxelIdxsInSource);
			return true;
		}
	}

	return false;
}

void detailSwapManager::swapVoxel2()
{
	while (swapPossibleLayerWithVolume())
	{
		TRACE0("O");
	}
}

bool detailSwapManager::shouldSwap2(int sourceMeshIdx, int destMeshIdx, arrayInt voxelIdx)
{
	// We compare the aspect ratio
	// If unchanged, we compare volume ratio
	// The object is warrantied to be a valid voxel set (All have connection together)
	bvhVoxel *meshSource = meshBox[sourceMeshIdx];
	bvhVoxel *meshDest = meshBox[destMeshIdx];

	// error of original source mesh and dest mesh
	arrayFloat errSource = meshSource->getCurrentError2();
	arrayFloat errDest = meshDest->getCurrentError2();

	// error of source mesh and dest mesh after swap
	arrayFloat errSourceAssume = meshSource->getErrorWithAssumeRemoveVoxel2(voxelIdx);
	arrayFloat errDestAssume = meshDest->getErrorWithAssumeAddVoxel2(voxelIdx);

	float originalE = meshSource->expectedVolRatio*errSource[ASPECT_ERROR] + meshDest->expectedVolRatio*errDest[ASPECT_ERROR];

	float assumeE = meshSource->expectedVolRatio*errSourceAssume[ASPECT_ERROR] + meshDest->expectedVolRatio*errDestAssume[ASPECT_ERROR];

	float aspectg = (originalE - assumeE) / originalE;
	
	// Here we need to consider group bone
	if (meshSource->bIsGroupBone || meshDest->bIsGroupBone)
	{
		// Use volume ratio only
		float originalVolE = errSource[VOLUME_ERROR] + errDest[VOLUME_ERROR];
		float originalFillRatio = errSource[FILL_RATIO] + errDest[FILL_RATIO];
		float assumeVolE = errSourceAssume[VOLUME_ERROR] + errDestAssume[VOLUME_ERROR];
		float assumeFillRatio = errSourceAssume[FILL_RATIO] + errDestAssume[FILL_RATIO];
		
		return originalVolE + 1 - originalFillRatio > assumeVolE + 1 - assumeFillRatio;
	}
	else
	{
		// Both of them are bone
		// Use aspect ration
		if (aspectg > 0)
		{
			return true;
		}
		else if (aspectg > -0.01) // Actually, want to compare if it equal
		{
			return ((errSource[VOLUME_ERROR] + errDest[VOLUME_ERROR]) - (errSourceAssume[VOLUME_ERROR] + errDestAssume[VOLUME_ERROR])) > 0;
		}
		else // aspect ratio error go down
		{
			return false; // Actually should use some weight
		}
	}

	return false;
}

void detailSwapManager::saveMeshBox(char* filePath)
{
	FILE *f = fopen(filePath, "w");
	ASSERT(f);

	fprintf(f, "%d\n", meshBox.size()); // Number of mesh box
	for (int i = 0; i < meshBox.size(); i++)
	{
		bvhVoxel *curV = meshBox[i];
		arrayInt vIdx = curV->voxelIdxs;

		fprintf(f, "%s\n", ToAS(curV->boneName));
		fprintf(f, "%d\n", vIdx.size()); // Number of voxel in meshbox
		for (int j = 0; j < vIdx.size(); j++)
		{
			Vec3f center = boxes[vIdx[j]].center;
			fprintf(f, "%f %f %f\n", center[0], center[1], center[2]);
		}
	}

	fclose(f);
}

void detailSwapManager::initFromCutTree2(cutSurfTreeMngr2* testCut)
{
	// Load mesh box
	loadMeshBoxFromCutTreeWithPose2(testCut);
	constructVolxeHashFromCutTree2(testCut);
//	constructBVHOfMeshBoxesFromCutTree2(testCut);

	tempMark = new int[s_boxes->size()];
	voxelOccupy = new int[s_boxes->size()];
}

void detailSwapManager::loadMeshBoxFromCutTreeWithPose2(cutSurfTreeMngr2* testCut)
{
	std::vector<meshPiece> centerBox = testCut->leatE2Node2->centerBoxf;
	std::vector<meshPiece> sideBox = testCut->leatE2Node2->sideBoxf;

	// Map with bone; for expected volume ratio and aspect ratio
	std::vector<bone*> boneOrder = testCut->poseMngr.sortedBone;
	neighborPose pose = testCut->poseMngr.getFilteredPose(testCut->poseIdx);
	std::vector<arrayInt> boneAroundBone = testCut->poseMngr.boneAroundBone;
	std::map<int, int> boneMeshIdxMap = pose.mapBone_meshIdx[testCut->nodeIdx];



	std::vector<meshPiece> allBoxes;
	allBoxes.insert(allBoxes.end(), centerBox.begin(), centerBox.end());
	allBoxes.insert(allBoxes.end(), sideBox.begin(), sideBox.end());

	meshBox.resize(allBoxes.size());
	//meshNeighbor.resize(allBoxes.size());

	// The correct bone order
	// Correspond with the list of the cut box
	std::vector<bone*> correctBoneOrder; // correspond with the cut boxes
	correctBoneOrder.resize(boneOrder.size());
	for (int i = 0; i < boneOrder.size(); i++)
	{
		int meshIdx = boneMeshIdxMap[i];
		correctBoneOrder[meshIdx] = boneOrder[i];
	}

	// Compute parameters of mesh boxes
	for (int i = 0; i < correctBoneOrder.size(); i++)
	{
		meshPiece curB = allBoxes[i];
		bone* curBone = correctBoneOrder[i];

		bvhVoxel* newMeshBox = new bvhVoxel;
		newMeshBox->boneName = curBone->m_name;
		newMeshBox->boneType = curBone->m_type;
		newMeshBox->curLeftDown = curB.leftDown;
		newMeshBox->curRightUp = curB.rightUp;
		newMeshBox->boxes = s_boxes;
		newMeshBox->voxelSize = voxelSize;

		// assign
		meshBox[i] = newMeshBox;
	}

	// Volume ratio
	float totalBoneVol = 0;
	for (int i = 0; i < meshBox.size(); i++)
	{
		meshBox[i]->expectedVolRatio = correctBoneOrder[i]->getVolumef();
		totalBoneVol += correctBoneOrder[i]->getVolumef();
	}
	for (int i = 0; i < meshBox.size(); i++)
	{
		meshBox[i]->expectedVolRatio /= totalBoneVol;
	}

	// Bone array
	boneArray = correctBoneOrder;
}

bool detailSwapManager::swapPossibleLayerWithVolume()
{
	for (int i = 0; i < meshBox.size(); i++)
	{
		for (int j = i + 1; j < meshBox.size(); j++)
		{
			if (swapBoxLayerVolume(i, j))
			{
				// swap
				return true;
			}
		}
	}

	return false;
}

bool detailSwapManager::swapBoxLayerVolume(int meshBoxIdx1, int meshBoxIdx2)
{
	bvhVoxel *mesh1 = meshBox[meshBoxIdx1];
	bvhVoxel *mesh2 = meshBox[meshBoxIdx2];

	Box b1(mesh1->curLeftDown, mesh1->curRightUp);
	Box b2(mesh2->curLeftDown, mesh2->curRightUp);

	if (!isBoxInContact(b1, b2))
	{
		return false;
	}

	// Check on face of destIdx, if it can receive a layer from source mesh
	for (int xyzDirect = 0; xyzDirect < 3; xyzDirect++)
	{
		// Check on both edge of the box
		float cPos[4];
		cPos[0] = b1.leftDown[xyzDirect];
		cPos[1] = b1.rightUp[xyzDirect];
		cPos[2] = b2.leftDown[xyzDirect];
		cPos[3] = b2.rightUp[xyzDirect];

		for (int j = 0; j < 4; j++)
		{
			arrayInt swapableIdx1, swapableIdx2;
			arrayInt remainIdx1, remainIdx2;
			getSwapableVoxel2(xyzDirect, cPos[j], meshBoxIdx1, meshBoxIdx2, swapableIdx1, swapableIdx2, remainIdx1, remainIdx2);

			if (swapableIdx1.size() == 0)
			{
				continue;
			}

			// a. meshBoxIdx1 is source
			if (swapVoxelVolume(meshBoxIdx1, meshBoxIdx2, swapableIdx1, remainIdx1, xyzDirect))
			{
				return true;
			}
			else if (swapVoxelVolume(meshBoxIdx2, meshBoxIdx1, swapableIdx2, remainIdx2, xyzDirect))
			{
				return true;
			}

		}
	}

	return false;
}

bool detailSwapManager::swapVoxelVolume(int sourceMeshIdx, int destMeshIdx, arrayInt voxelIdxsInSource, arrayInt voxelRemainInLayer, int xyzDirect)
{
	// Hash of swap voxel
	std::fill(voxelOccupy, voxelOccupy + (*s_boxes).size(), 0);;
	for (int i = 0; i < voxelIdxsInSource.size(); i++)
	{
		voxelOccupy[voxelIdxsInSource[i]] = 1;
	}

	// Hash of AABBNodes
	// If center box to side box, swap voxel should not over center surface
	bvhVoxel *meshSource = meshBox[sourceMeshIdx];
	bvhVoxel *meshDest = meshBox[destMeshIdx];
	if (meshSource->boneType == CENTER_BONE
		&& meshDest->boneType == SIDE_BONE)
	{
		arrayInt voxelToBeConsider;
		for (int i = 0; i < voxelRemainInLayer.size(); i++)
		{
			voxelBox curB = (*s_boxes)[voxelRemainInLayer[i]];
			if (curB.center[0] < s_octree->centerMesh[0])
			{
				voxelToBeConsider.push_back(voxelRemainInLayer[i]);
			}
		}
		voxelRemainInLayer = voxelToBeConsider;
	}


	bool bShouldSwap = false;

	// Check if should swap
	arrayFloat errSource = meshSource->getCurrentError2();
	arrayFloat errDest = meshDest->getCurrentError2();

	// Grow the swap area
	while (1)
	{
		bool bContinueCheck = false;
		for (int i = 0; i < voxelRemainInLayer.size(); i++)
		{
			// Check if it is neighbor
			int curIdx = voxelRemainInLayer[i];
			if (voxelOccupy[curIdx] == 1)
			{// This voxel is already added
				continue;
			}

			arrayInt nb = (*s_boxShareFaceWithBox)[curIdx];
			bool bIsNeighbor = false;
			for (int j = 0; j < nb.size(); j++)
			{
				if (voxelOccupy[nb[j]] == 1)
				{
					bIsNeighbor = true;
					break;
				}
			}

			if (bIsNeighbor)
			{
				// Check if can be swap
				voxelIdxsInSource.push_back(curIdx);
				
				if (shouldSwapVolumeProgress(sourceMeshIdx, destMeshIdx, voxelIdxsInSource, errSource, errDest))
				{
					bShouldSwap = true;
					bContinueCheck = true;

					voxelOccupy[curIdx] = 1;
					break;
				}
				voxelIdxsInSource.pop_back();
			}
		}

		if (!bContinueCheck)
		{
			break;
		}
	}

	if (!bShouldSwap)
	{
		if (shouldSwapVolume(sourceMeshIdx, destMeshIdx, voxelIdxsInSource))
		{
			bShouldSwap = true;
		}
	}

	if (bShouldSwap)
	{// Swap
		// Check if it a an object, the source mesh
		arrayInt sourceIdx, destIdx;
		getTheVoxelWeShouldSwap(sourceMeshIdx, destMeshIdx, voxelIdxsInSource, sourceIdx, destIdx);

		if (isObjectIfRemoveVoxel(sourceMeshIdx, sourceIdx))
		{
			swapLayer(sourceMeshIdx, destMeshIdx, voxelIdxsInSource);
			return true;
		}
// 		if (isObjectIfRemoveVoxel(sourceMeshIdx, voxelIdxsInSource))
// 		{
// 			swapLayer(sourceMeshIdx, destMeshIdx, voxelIdxsInSource);
// 			return true;
// 		}
	}

	return false;
}

bool detailSwapManager::shouldSwapVolume(int sourceMeshIdx, int destMeshIdx, arrayInt voxelIdx)
{
	// We compare the aspect ratio
	// If unchanged, we compare volume ratio
	// The object is warrantied to be a valid voxel set (All have connection together)
	bvhVoxel *meshSource = meshBox[sourceMeshIdx];
	bvhVoxel *meshDest = meshBox[destMeshIdx];

	// error of original source mesh and dest mesh
	arrayFloat errSource = meshSource->getCurrentError2();
	arrayFloat errDest = meshDest->getCurrentError2();

	// error of source mesh and dest mesh after swap
	arrayInt sourceIdx, destIdx;
	getTheVoxelWeShouldSwap(sourceMeshIdx, destMeshIdx, voxelIdx, sourceIdx, destIdx);
	arrayFloat errSourceAssume = meshSource->getErrorWithAssumeRemoveVoxel2(sourceIdx);
	arrayFloat errDestAssume = meshDest->getErrorWithAssumeAddVoxel2(destIdx);

	float originalE = meshSource->expectedVolRatio*errSource[ASPECT_ERROR] + meshDest->expectedVolRatio*errDest[ASPECT_ERROR];

	float assumeE = meshSource->expectedVolRatio*errSourceAssume[ASPECT_ERROR] + meshDest->expectedVolRatio*errDestAssume[ASPECT_ERROR];

	float aspectg = (originalE - assumeE) / originalE;

	// Here we need to consider group bone
	// Use volume ratio only
	float originalVolE = errSource[VOLUME_ERROR] + errDest[VOLUME_ERROR];
	float originalFillRatio = errSource[FILL_RATIO] + errDest[FILL_RATIO];
	float assumeVolE = errSourceAssume[VOLUME_ERROR] + errDestAssume[VOLUME_ERROR];
	float assumeFillRatio = errSourceAssume[FILL_RATIO] + errDestAssume[FILL_RATIO];

	if (originalVolE + 1 - originalFillRatio > assumeVolE + 1 - assumeFillRatio)
	{
		CString d = _T("dfdsf");
	}

	return originalVolE + 1 - originalFillRatio > assumeVolE + 1 - assumeFillRatio;
}

void detailSwapManager::constructVolxeHashFromCutTree2(cutSurfTreeMngr2* testCut)
{
	leftDownVoxel = s_octree->m_root->leftDownTight;
	rightUpVoxel = s_octree->m_root->rightUpTight;

	voxelSize = s_octree->boxSize;

	for (unsigned int i = 0; i < meshBox.size(); i++)
	{
		bvhVoxel* m = meshBox[i];
		arrayInt vIdxs = s_octree->intersectWithBox(m->curLeftDown, m->curRightUp);
		m->voxelIdxs = vIdxs;
		m->update();

		int bIdx = i;
		for (auto i : vIdxs)
		{
			s_boxes->at(i).boneIndex = bIdx;
		}
	}
}

void detailSwapManager::getSwapableVoxel2(int xyzDirect, float cPos, int meshBoxIdx1, int meshBoxIdx2, arrayInt &swapableIdx1, arrayInt &swapableIdx2, arrayInt &remainIdx1, arrayInt &remainIdx2)
{
	Vec3f LD = s_hashTable->leftDown;
	Vec3f RU = s_hashTable->rightUp;
	Vec3i sizei = s_hashTable->NumXYZ;

	int cPosi = floor((cPos - LD[xyzDirect] - voxelSize / 2.0) / voxelSize);

	if (cPosi < 0 || cPosi > sizei[xyzDirect]-1)
	{
		return;
	}

	// The other axis, that differ to xyzDirect
	int xyz1, xyz2;
	getTwoOtherIndex(xyzDirect, xyz1, xyz2);

	for (int i1 = 0; i1 < sizei[xyz1]; i1++)
	{
		for (int i2 = 0; i2 < sizei[xyz2]; i2++)
		{
			// Voxel coordinate of the two voxel
			Vec3i pti;
			pti[xyz1] = i1;
			pti[xyz2] = i2;

			Vec3i pti1 = pti, pti2 = pti;
			pti1[xyzDirect] = cPosi;
			pti2[xyzDirect] = cPosi + 1; // 2 voxel that share the surface

			// Check if they are swappable
			int voxelIdx1 = s_hashTable->getBoxIndexFromVoxelCoord(pti1);
			int voxelIdx2 = s_hashTable->getBoxIndexFromVoxelCoord(pti2);

			if (voxelIdx1 == -1 || voxelIdx2 == -1)
			{
				continue;
			}

			int boneIdx1 = (*s_boxes)[voxelIdx1].boneIndex;
			int boneIdx2 = (*s_boxes)[voxelIdx2].boneIndex;

			if (boneIdx1 == meshBoxIdx1)
			{
				if (boneIdx2 == meshBoxIdx2)
				{
					swapableIdx1.push_back(voxelIdx1);
					swapableIdx2.push_back(voxelIdx2);
				}
				else
					remainIdx1.push_back(voxelIdx1);

			}
			else if (boneIdx1 == meshBoxIdx2)
			{
				if (boneIdx2 == meshBoxIdx1)
				{
					swapableIdx1.push_back(voxelIdx2);
					swapableIdx2.push_back(voxelIdx1);
				}
				else
					remainIdx2.push_back(voxelIdx1);
			}
			else
			{
				if (boneIdx2 == meshBoxIdx1)
					remainIdx1.push_back(voxelIdx2);
				else if (boneIdx2 == meshBoxIdx2)
					remainIdx2.push_back(voxelIdx2);
			}
		}
	}
}

void detailSwapManager::draw2(BOOL displayMode[10])
{
	mirrorDraw mirror;
	mirror.mirrorAxis = 0;
	mirror.mirrorCoord = m_octree.centerMesh[0];
	static arrayVec3f color = Util_w::randColor(6);

	if (displayMode[6])
	{
		glLineWidth(2.0);
		for (int i = 0; i < meshBox.size(); i++)
		{
			glColor3f(0, 0, 1);
			meshBox[i]->drawOriginalBox();
		}
		glLineWidth(1.0);
	}

	if (displayMode[7])
	{

		for (int i = 0; i < meshBox.size(); i++)
		{
			glColor3fv(color[i].data());
			meshBox[i]->drawVoxels(&mirror);
		}
	}

	if (displayMode[8])
	{
		for (int i = 0; i < meshBox.size(); i++)
		{
			glColor3fv(color[i + 1].data());
			meshBox[i]->drawVoxels(&mirror);

			glColor3fv(color[i].data());
			meshBox[i]->drawVoxels(&mirror, 1);

		}
	}
}

void detailSwapManager::constructBVHOfMeshBoxesFromCutTree2(cutSurfTreeMngr2* testCut)
{
	std::vector<meshPiece> centerBox = testCut->leatE2Node2->centerBoxf;
	std::vector<meshPiece> sideBox = testCut->leatE2Node2->sideBoxf;
	centerBox.insert(centerBox.end(), sideBox.begin(), sideBox.end());

	int *mark;
	hashVoxel *hashVBig = &testCut->s_voxelObj->m_hashTable;
	{
		std::vector<voxelBox> *voxelBig = &testCut->s_voxelObj->m_boxes;
		mark = new int[voxelBig->size()];
		for (int i = 0; i < centerBox.size(); i++)
		{
			std::vector<int> boxIdx = centerBox[i].voxels;
			for (int j = 0; j < boxIdx.size(); j++)
			{
				mark[boxIdx[j]] = i;
			}
		}
	}

	for (int i = 0; i < meshBox.size(); i++)
	{
		meshBox[i]->boxes = s_boxes;
		meshBox[i]->voxelSize = voxelSize;
		meshBox[i]->hashTable = s_hashTable;
		meshBox[i]->s_octree = s_octree;

		meshBox[i]->constructAABBTree();

		// Remove wrong node
		meshBox[i]->removeNodeNotInList(hashVBig, mark, i, centerBox[i].voxels);

		// stupid, but we need it run first
		std::vector<AABBNode*>* leaves = &meshBox[i]->leaves;
		arrayInt voxelIdxs;

		for (int j = 0; j < leaves->size(); j++)
		{
			(*s_boxes)[leaves->at(j)->IndexInLeafNode].boneIndex = i;
			voxelIdxs.push_back(leaves->at(j)->IndexInLeafNode);
		}

		meshBox[i]->voxelIdxs = voxelIdxs;
		meshBox[i]->update();
	}

	delete[]mark;
}

void detailSwapManager::initFromAssignCoord(coordAssignManager * coordAssign)
{
	//Load mesh box
	loadMeshBoxFromCoordAssign(coordAssign);
	constructBVHOfMeshBoxesAndHashVoxel();

	tempMark = new int[s_boxes->size()];
	voxelOccupy = new int[s_boxes->size()];
}

void detailSwapManager::loadMeshBoxFromCoordAssign(coordAssignManager * coordAssign)
{
	// Array of bone and meshbox
	boneArray = coordAssign->m_boneFullArray;
	std::vector<bvhVoxel> * allMeshBox = &coordAssign->m_meshBoxFull;

	for (int i = 0; i < allMeshBox->size(); i++)
	{
		bvhVoxel* newMeshBox = new bvhVoxel;
		*newMeshBox = allMeshBox->at(i);
		meshBox.push_back(newMeshBox);
	}

	// Volume ratio 
	float totalBoneVol = 0;
	for (int i = 0; i < meshBox.size(); i++)
	{
		meshBox[i]->expectedVolRatio = boneArray[i]->getVolumef();
		totalBoneVol += boneArray[i]->getVolumef();
	}
	for (int i = 0; i < meshBox.size(); i++)
	{
		meshBox[i]->expectedVolRatio /= totalBoneVol;
	}

	// Aspect ratio
	coords = coordAssign->dlg->coords;

	computeAspectRatio();

	leftDownVoxel = s_octree->m_root->leftDownTight;
	rightUpVoxel = s_octree->m_root->rightUpTight;
	voxelSize = s_octree->boxSize;
}

void detailSwapManager::constructBVHOfMeshBoxesAndHashVoxel()
{
	for (int i = 0; i < meshBox.size(); i++)
	{
		arrayInt voxelIdx = meshBox[i]->voxelIdxs;

		for (int j = 0; j < voxelIdx.size(); j++)
		{
			s_boxes->at(voxelIdx[j]).boneIndex = i;
		}
	}
}

bool detailSwapManager::swapPossibleLayerWithVolumeAndAspect()
{
	for (int i = 0; i < meshBox.size(); i++)
	{
		for (int j = i + 1; j < meshBox.size(); j++)
		{
			if (swapBoxLayerVolumeAndRatio(i, j))
			{
				// swap
				return true;
			}
		}
	}

	return false;
}

bool detailSwapManager::swapBoxLayerVolumeAndRatio(int meshBoxIdx1, int meshBoxIdx2)
{
	bvhVoxel *mesh1 = meshBox[meshBoxIdx1];
	bvhVoxel *mesh2 = meshBox[meshBoxIdx2];

	Box b1(mesh1->curLeftDown, mesh1->curRightUp);
	Box b2(mesh2->curLeftDown, mesh2->curRightUp);

	if (!isBoxInContact(b1, b2))
	{
		return false;
	}

	// Check on face of destIdx, if it can receive a layer from source mesh
	for (int xyzDirect = 0; xyzDirect < 3; xyzDirect++)
	{
		// Check on both edge of the box
		float cPos[4];
		cPos[0] = b1.leftDown[xyzDirect];
		cPos[1] = b1.rightUp[xyzDirect];
		cPos[2] = b2.leftDown[xyzDirect];
		cPos[3] = b2.rightUp[xyzDirect];

		for (int j = 0; j < 4; j++)
		{
			arrayInt swapableIdx1, swapableIdx2;
			arrayInt remainIdx1, remainIdx2;
			getSwapableVoxel2(xyzDirect, cPos[j], meshBoxIdx1, meshBoxIdx2, swapableIdx1, swapableIdx2, remainIdx1, remainIdx2);

			if (swapableIdx1.size() == 0)
			{
				continue;
			}

			// a. meshBoxIdx1 is source
			if (swapVoxelVolumeAndRatio(meshBoxIdx1, meshBoxIdx2, swapableIdx1, remainIdx1, xyzDirect))
			{
				return true;
			}
			else if (swapVoxelVolumeAndRatio(meshBoxIdx2, meshBoxIdx1, swapableIdx2, remainIdx2, xyzDirect))
			{
				return true;
			}

		}
	}

	return false;
}

bool detailSwapManager::swapVoxelVolumeAndRatio(int sourceMeshIdx, int destMeshIdx, arrayInt voxelIdxsInSource, arrayInt voxelRemainInLayer, int xyzDirect)
{
	// Hash of swap voxel
	std::fill(voxelOccupy, voxelOccupy + (*s_boxes).size(), 0);;
	for (int i = 0; i < voxelIdxsInSource.size(); i++)
	{
		voxelOccupy[voxelIdxsInSource[i]] = 1;
	}

	// Hash of AABBNodes
	// If center box to side box, swap voxel should not over center surface
	bvhVoxel *meshSource = meshBox[sourceMeshIdx];
	bvhVoxel *meshDest = meshBox[destMeshIdx];
	if (meshSource->boneType == CENTER_BONE
		&& meshDest->boneType == SIDE_BONE)
	{
		arrayInt voxelToBeConsider;
		for (int i = 0; i < voxelRemainInLayer.size(); i++)
		{
			voxelBox curB = (*s_boxes)[voxelRemainInLayer[i]];
			if (curB.center[0] < s_octree->centerMesh[0])
			{
				voxelToBeConsider.push_back(voxelRemainInLayer[i]);
			}
		}
		voxelRemainInLayer = voxelToBeConsider;
	}


	bool bShouldSwap = false;

	// Check if should swap
	arrayFloat errSource = meshSource->getCurrentError2();
	arrayFloat errDest = meshDest->getCurrentError2();

	// Check if should swap
	// Grow the wap area
	while (1)
	{
		bool bContinueCheck = false;
		for (int i = 0; i < voxelRemainInLayer.size(); i++)
		{
			// Check if it is neighbor
			int curIdx = voxelRemainInLayer[i];
			if (voxelOccupy[curIdx] == 1)
			{// This voxel is already added
				continue;
			}

			arrayInt nb = (*s_boxShareFaceWithBox)[curIdx];
			bool bIsNeighbor = false;
			for (int j = 0; j < nb.size(); j++)
			{
				if (voxelOccupy[nb[j]] == 1)
				{
					bIsNeighbor = true;
					break;
				}
			}

			if (bIsNeighbor)
			{
				// Check if can be swap
				voxelIdxsInSource.push_back(curIdx);
				if (shouldSwapVolumeAndRatioProgress(sourceMeshIdx, destMeshIdx, voxelIdxsInSource, errSource, errDest))
				{
					bShouldSwap = true;
					bContinueCheck = true;

					voxelOccupy[curIdx] = 1;
					break;
				}
				voxelIdxsInSource.pop_back();
			}
		}

		if (!bContinueCheck)
		{
			break;
		}
	}

	if (!bShouldSwap)
	{
		if (shouldSwapVolumeAndRatio(sourceMeshIdx, destMeshIdx, voxelIdxsInSource))
		{
			bShouldSwap = true;
		}
	}

	if (bShouldSwap)
	{// Swap
		// Check if it a an object, the source mesh
		arrayInt sourceIdx, destIdx;
		getTheVoxelWeShouldSwap(sourceMeshIdx, destMeshIdx, voxelIdxsInSource, sourceIdx, destIdx);

		if (isObjectIfRemoveVoxel(sourceMeshIdx, sourceIdx))
		{
			swapLayer(sourceMeshIdx, destMeshIdx, voxelIdxsInSource);
			return true;
		}
	}

	return false;
}

bool detailSwapManager::shouldSwapVolumeAndRatio(int sourceMeshIdx, int destMeshIdx, arrayInt voxelIdx)
{
	// We compare the aspect ratio
	// If unchanged, we compare volume ratio
	// The object is warrantied to be a valid voxel set (All have connection together)
	bvhVoxel *meshSource = meshBox[sourceMeshIdx];
	bvhVoxel *meshDest = meshBox[destMeshIdx];

	// error of original source mesh and dest mesh
	arrayFloat errSource = meshSource->getCurrentError2();
	arrayFloat errDest = meshDest->getCurrentError2();

	// error of source mesh and dest mesh after swap
	arrayInt sourceIdx, destIdx;
	getTheVoxelWeShouldSwap(sourceMeshIdx, destMeshIdx, voxelIdx, sourceIdx, destIdx);
	arrayFloat errSourceAssume = meshSource->getErrorWithAssumeRemoveVoxel2(sourceIdx);
	arrayFloat errDestAssume = meshDest->getErrorWithAssumeAddVoxel2(destIdx);

	float originalE = (meshSource->expectedVolRatio*errSource[ASPECT_ERROR] + meshDest->expectedVolRatio*errDest[ASPECT_ERROR]) / (meshSource->expectedVolRatio + meshDest->expectedVolRatio);

	float assumeE = (meshSource->expectedVolRatio*errSourceAssume[ASPECT_ERROR] + meshDest->expectedVolRatio*errDestAssume[ASPECT_ERROR]) / (meshSource->expectedVolRatio + meshDest->expectedVolRatio);

	float aspectg = (originalE - assumeE) / originalE;

	if (aspectg > 0)
	{
		return true;
	}
	else if (aspectg > -0.001)
	{
		// Here we need to consider group bone
		// Use volume ratio only
		float originalVolE = errSource[VOLUME_ERROR] + errDest[VOLUME_ERROR];
		float originalFillRatio = errSource[FILL_RATIO] + errDest[FILL_RATIO];
		float assumeVolE = errSourceAssume[VOLUME_ERROR] + errDestAssume[VOLUME_ERROR];
		float assumeFillRatio = errSourceAssume[FILL_RATIO] + errDestAssume[FILL_RATIO];

		if (originalVolE + 1 - originalFillRatio > assumeVolE + 1 - assumeFillRatio)
		{
			return true;
		}
	}

	return false;
}

void detailSwapManager::swapVoxelTotal()
{
	while (m_bStopThread && swapPossibleLayerWithVolumeAndAspect())
	{
		TRACE0("O");
	}
}

bool detailSwapManager::shouldSwapVolumeProgress(int sourceMeshIdx, int destMeshIdx, arrayInt voxelIdx, arrayFloat &errSource, arrayFloat &errDest)
{
	// We compare the aspect ratio
	// If unchanged, we compare volume ratio
	// The object is warrantied to be a valid voxel set (All have connection together)
	bvhVoxel *meshSource = meshBox[sourceMeshIdx];
	bvhVoxel *meshDest = meshBox[destMeshIdx];

	// error of source mesh and dest mesh after swap
	arrayInt sourceIdx, destIdx;
	getTheVoxelWeShouldSwap(sourceMeshIdx, destMeshIdx, voxelIdx, sourceIdx, destIdx);

	arrayFloat errSourceAssume = meshSource->getErrorWithAssumeRemoveVoxel2(sourceIdx);
	arrayFloat errDestAssume = meshDest->getErrorWithAssumeAddVoxel2(destIdx);

	float originalE = meshSource->expectedVolRatio*errSource[ASPECT_ERROR] + meshDest->expectedVolRatio*errDest[ASPECT_ERROR];

	float assumeE = meshSource->expectedVolRatio*errSourceAssume[ASPECT_ERROR] + meshDest->expectedVolRatio*errDestAssume[ASPECT_ERROR];

	float aspectg = (originalE - assumeE) / originalE;

	// Here we need to consider group bone
	// Use volume ratio only
	float originalVolE = errSource[VOLUME_ERROR] + errDest[VOLUME_ERROR];
	float originalFillRatio = errSource[FILL_RATIO] + errDest[FILL_RATIO];
	float assumeVolE = errSourceAssume[VOLUME_ERROR] + errDestAssume[VOLUME_ERROR];
	float assumeFillRatio = errSourceAssume[FILL_RATIO] + errDestAssume[FILL_RATIO];

	if (originalVolE + 1 - originalFillRatio > assumeVolE + 1 - assumeFillRatio)
	{
		CString d = _T("dfdsf");
		errSource = errSourceAssume;
		errDest = errDestAssume;

		return true;
	}

	return false;
}

void detailSwapManager::getTheVoxelWeShouldSwap(int sourceMeshIdx, int destMeshIdx, arrayInt voxelIdxsInSource, arrayInt &sourceIdx, arrayInt &destIdx)
{
	bvhVoxel *meshSource = meshBox[sourceMeshIdx];
	bvhVoxel *meshDest = meshBox[destMeshIdx];

	sourceIdx = voxelIdxsInSource;
	destIdx = voxelIdxsInSource;

	if (meshSource->boneType == CENTER_BONE
		&& meshDest->boneType == SIDE_BONE)
	{
		// Symmetric remove
		arrayInt voxelToRemove = voxelIdxsInSource;
		for (int i = 0; i < voxelIdxsInSource.size(); i++)
		{
			int idx = s_hashTable->getSymmetricBox(voxelIdxsInSource[i]);
			if (idx != -1)
			{
				voxelToRemove.push_back(idx);
			}
		}

		sourceIdx = voxelToRemove;
	}

	if (meshSource->boneType == SIDE_BONE
		&& meshDest->boneType == CENTER_BONE)
	{
		// Symmetric add
		arrayInt voxelsym = voxelIdxsInSource;
		for (int i = 0; i < voxelIdxsInSource.size(); i++)
		{
			int idx = s_hashTable->getSymmetricBox(voxelIdxsInSource[i]);
			if (idx != -1)
			{
				voxelsym.push_back(idx);
			}

		}

		destIdx = voxelsym;
	}
}

bool detailSwapManager::shouldSwapVolumeAndRatioProgress(int sourceMeshIdx, int destMeshIdx, arrayInt voxelIdx, arrayFloat &errSource, arrayFloat &errDest)
{

	// We compare the aspect ratio
	// If unchanged, we compare volume ratio
	// The object is warrantied to be a valid voxel set (All have connection together)
	bvhVoxel *meshSource = meshBox[sourceMeshIdx];
	bvhVoxel *meshDest = meshBox[destMeshIdx];

	// error of source mesh and dest mesh after swap
	arrayInt sourceIdx, destIdx;
	getTheVoxelWeShouldSwap(sourceMeshIdx, destMeshIdx, voxelIdx, sourceIdx, destIdx);
	arrayFloat errSourceAssume = meshSource->getErrorWithAssumeRemoveVoxel2(sourceIdx);
	arrayFloat errDestAssume = meshDest->getErrorWithAssumeAddVoxel2(destIdx);

	float originalE = (meshSource->expectedVolRatio*errSource[ASPECT_ERROR] + meshDest->expectedVolRatio*errDest[ASPECT_ERROR]) / (meshSource->expectedVolRatio + meshDest->expectedVolRatio);

	float assumeE = (meshSource->expectedVolRatio*errSourceAssume[ASPECT_ERROR] + meshDest->expectedVolRatio*errDestAssume[ASPECT_ERROR]) / (meshSource->expectedVolRatio + meshDest->expectedVolRatio);

	float aspectg = (originalE - assumeE) / originalE;

	if (aspectg > 0)
	{
		errSource = errSourceAssume;
		errDest = errDestAssume;
		return true;
	}
	else if (aspectg > -0.001)
	{
		// Here we need to consider group bone
		// Use volume ratio only
		float originalVolE = errSource[VOLUME_ERROR] + errDest[VOLUME_ERROR];
		float originalFillRatio = errSource[FILL_RATIO] + errDest[FILL_RATIO];
		float assumeVolE = errSourceAssume[VOLUME_ERROR] + errDestAssume[VOLUME_ERROR];
		float assumeFillRatio = errSourceAssume[FILL_RATIO] + errDestAssume[FILL_RATIO];

		if (originalVolE + 1 - originalFillRatio > assumeVolE + 1 - assumeFillRatio)
		{
			errSource = errSourceAssume;
			errDest = errDestAssume;
			return true;
		}
		return  false;
	}

	return false;
}

void detailSwapManager::growSwap()
{
	for (int i = 0; i < meshBox.size(); i++)
	{
		for (int j = i + 1; j < meshBox.size(); j++)
		{
			if(!swapBetweenBox(i, j))
				swapBetweenBox(j, i);
		}
	}
}

bool detailSwapManager::swapBetweenBox(int sourceIdx, int destIdx)
{
	// Find index of possible swap
	int idx = findPossibleSwapVoxel(sourceIdx, destIdx);
	if (idx == -1)
	{
		return false;
	}

	// Grow swap
	arrayInt swapIdxs = { idx };
	int *mark = new int[s_boxes->size()];
	std::fill(mark, mark + s_boxes->size(), 0);
	mark[idx] = 1; // in swap

	while (true)
	{
		bool outWhile = true;
		for (int i = 0; i < swapIdxs.size(); i++)
		{
			bool bcontinue = true;
			// Check all it neighbor
			arrayInt nbface = (*s_boxShareFaceWithBox)[swapIdxs[i]];
			for (int j = 0; j < nbface.size(); j++)
			{
				if (s_boxes->at(nbface[j]).boneIndex == sourceIdx
					&& mark[nbface[j]] == 0)
				{
					// Check if this box is good
					arrayInt temp1 = swapIdxs;
					temp1.push_back(nbface[j]);
					arrayInt temp = getIdxsShoudSwap(sourceIdx, destIdx, temp1);
					if (canSwap(sourceIdx, destIdx, temp))
					{
						bcontinue = false;
						outWhile = false;
						swapIdxs = temp;
						for (int k = 0; k < swapIdxs.size(); k++)
						{
							mark[swapIdxs[k]] = 1;
						}
						break;
					}
				}
			}
			if (!bcontinue)
			{
				break;
			}
		}

		if (outWhile)
		{
			break;
		}
	}

	// Now swap the voxel
	bvhVoxel *meshSource = meshBox[sourceIdx];
	bvhVoxel *meshDest = meshBox[destIdx];

	meshSource->removeVoxel(swapIdxs);
	meshDest->addVoxel(swapIdxs);

	// Update node bone index
	for (int i = 0; i < swapIdxs.size(); i++)
	{
		(*s_boxes)[swapIdxs[i]].boneIndex = destIdx;
	}

	return true;
}

void detailSwapManager::cutCenterMeshByHalf()
{
	for (int i = 0; i < meshBox.size(); i++)
	{
		bvhVoxel* curM = meshBox[i];
		if (curM->boneType == CENTER_BONE)
		{
			arrayInt vIdxs = curM->voxelIdxs;
			arrayInt newVIdxs;
			for (int j = 0; j < vIdxs.size(); j++)
			{
				if (s_boxes->at(vIdxs[j]).center[0] < s_voxelObj->m_centerf[0])
				{
					newVIdxs.push_back(vIdxs[j]);
				}
			}
			curM->voxelIdxs = newVIdxs;
		}
	}

	// Update bone index in box
	for (int i = 0; i < s_boxes->size(); i++)
	{
		s_boxes->at(i).boneIndex = -1;
	}

	for (int i = 0; i < meshBox.size(); i++)
	{
		arrayInt vIdx = meshBox[i]->voxelIdxs;
		for (int j = 0; j < vIdx.size(); j++)
		{
			s_boxes->at(vIdx[j]).boneIndex = i;
		}
	}
}

int detailSwapManager::findPossibleSwapVoxel(int sourceIdx, int destIdx)
{
	arrayInt vIdxsInSourse = meshBox[sourceIdx]->voxelIdxs;
	for (int i = 0; i < vIdxsInSourse.size(); i++)
	{
		// It should be on boundary
		arrayInt nbIdxs = s_boxShareFaceWithBox->at(vIdxsInSourse[i]);
		for (int j = 0; j < nbIdxs.size(); j++)
		{
			if (s_boxes->at(nbIdxs[j]).boneIndex == destIdx)
			{
				arrayInt temp = { vIdxsInSourse[i]};
				if (canSwap(sourceIdx, destIdx, temp))
				{
					return vIdxsInSourse[i];
				}
			}
		}

	}

	return -1;
}

bool detailSwapManager::canSwap(int sourceIdx, int destIdx, const arrayInt & temp)
{
	bvhVoxel* meshSourse = meshBox[sourceIdx];
	bvhVoxel* meshDest = meshBox[destIdx];
	
	arrayFloat eSource = meshSourse->getErrorSym();
	arrayFloat eDest = meshDest->getErrorSym();
	arrayFloat eSource_assume = meshSourse->getESymAssumeRemove(temp);
	arrayFloat eDest_assume = meshDest->getESymAssumeAdd(temp);

	arrayFloat w = { 0.0, 1.0, 1.0 };
	float benefitSource = getBenefit(w, eSource, eSource_assume);
	float benefitDest = getBenefit(w, eDest, eDest_assume);

	if (benefitDest + benefitSource > 0)
	{
		return true;
	}
	return false;

	// Compare error depend on a flag
	float ws = meshSourse->getExpectedVolumeRatioSym();
	float wd = meshDest->getExpectedVolumeRatioSym();

	arrayFloat eOrigin = getTotalError(ws, wd, eSource, eDest);
	arrayFloat eAssume = getTotalError(ws, wd, eSource_assume, eDest_assume);

// 	if (eOrigin[ASPECT_ERROR] > eAssume[ASPECT_ERROR])
// 	{
// 		return true;
// 	}
// 	else
	{
		arrayFloat weight = {0.0, 1.0, 1.0};
		float tEO = getErrorSum(weight, eOrigin);
		float tEA = getErrorSum(weight, eAssume);

		if (tEO > tEA)
		{
			return true;
		}
		return false;
	}
}

arrayFloat detailSwapManager::getTotalError(float ws, float wd, arrayFloat & eSource, arrayFloat & eDest)
{
	arrayFloat out;
	out.resize(eSource.size());
	for (int i = 0; i < eSource.size(); i++)
	{
		out[i] = (ws*eSource[i] + wd*eDest[i])/(ws+wd);
	}

	return out;
}

float detailSwapManager::getErrorSum(arrayFloat weight, arrayFloat eOrigin)
{
	float sum = 0;
	for (int i = 0; i < weight.size(); i++)
	{
		sum += weight[i] * eOrigin[i];
	}

	return sum;
}

arrayInt detailSwapManager::getIdxsShoudSwap(int sourceIdx, int destIdx, arrayInt temp1)
{
	// some time the swap voxel split the object
	// We merge them all
	arrayInt allIdxs = meshBox[sourceIdx]->voxelIdxs;

	arrayInt remainIdx = Util_w::substractArrayInt(allIdxs, temp1, s_boxes->size());

	std::vector<arrayInt> parts = getIndependentGroup(s_boxes, remainIdx);

	arrayInt out = temp1;
	if (parts.size() > 1)
	{
		// Merge small part
		int largestIdx, largestCount=0;
		for (int i = 0; i < parts.size(); i++)
		{
			if (largestCount < parts[i].size())
			{
				largestCount = parts[i].size();
				largestIdx = i;
			}
		}

		for (int i = 0; i < parts.size(); i++)
		{
			if (i != largestIdx)
			{
				out.insert(out.end(), parts[i].begin(), parts[i].end());
			}
		}
	}

	return out;
}

std::vector<arrayInt> detailSwapManager::getIndependentGroup(std::vector<voxelBox>* boxes, arrayInt idxs)
{
	int* mark = new int[boxes->size()];
	std::fill(mark, mark + boxes->size(), 0);
	for (int i = 0; i < idxs.size(); i++)
	{
		mark[idxs[i]] = 1;
	}

	int* hashvQ = new int[boxes->size()];
	std::fill(hashvQ, hashvQ + boxes->size(), 0);

	std::vector<arrayInt> out;
	arrayInt remain = idxs;
	while (remain.size() > 0)
	{
		arrayInt newObj;
		std::queue<int> vQ;
		vQ.push(remain[0]);

		while (!vQ.empty())
		{
			int idx = vQ.front();
			vQ.pop();
			newObj.push_back(idx);
			mark[idx] = 0; // No longer available

			// add all its neighbor
			arrayInt neighborN = s_boxShareFaceWithBox->at(idx);
			for (int j = 0; j < neighborN.size(); j++)
			{
				if (mark[neighborN[j]] == 1
					&& hashvQ[neighborN[j]] == 0)
				{
					hashvQ[neighborN[j]] = 1;
					vQ.push(neighborN[j]);
				}
			}
		}

		out.push_back(newObj);
		remain = Util_w::substractArrayInt(remain, newObj, boxes->size());
	}

	return out;
}

void detailSwapManager::initFromSaveFile()
{
	std::vector<arrayInt> meshIdx;

	// Load voxel box indexes of cut mesh
	{
		FILE* f = fopen("../stateFinalSwap_meshBoxIdxs.txt", "r");
		ASSERT(f);
		int nbMesh;
		fscanf(f, "%d\n", &nbMesh);
		for (int i = 0; i < nbMesh; i++)
		{
			arrayInt idxs;
			int nbv;
			fscanf(f, "%d\n", &nbv);
			idxs.resize(nbv);
			for (int j = 0; j < nbv; j++)
			{
				fscanf(f, "%d\n", &idxs[j]);
			}
			meshIdx.push_back(idxs);
		}
		fclose(f);
	}

	// Load bone order
	boneArray.clear();
	std::vector<CString> name;
	coords.clear();

	{
		FILE *f = fopen("../stateFinalSwap_boneArray.txt", "r");
		int nbBone;
		fscanf(f, "%d\n", &nbBone);
		coords.resize(nbBone);
		for (int i = 0; i < nbBone; i++)
		{
			char tmp[50];
			fscanf(f, "%s\n", tmp);
			name.push_back(CString(tmp));
			fscanf(f, "%d %d %d\n", &coords[i][0], &coords[i][1], &coords[i][2]);
		}

		std::vector<bone*> bones;
		s_skeleton->getSortedBoneArray(bones);
		for (int i = 0; i < name.size(); i++)
		{
			CString n = name[i];
			for (int j = 0; j < bones.size(); j++)
			{
				if ((bones[j]->m_name.Compare(n)) == 0)
				{
					boneArray.push_back(bones[j]);
					break;
				}
			}
		}
	}

	// Now init
	for (int i = 0; i < meshIdx.size(); i++)
	{
		bvhVoxel* newMeshBox = new bvhVoxel;
		newMeshBox->voxelIdxs = meshIdx[i];
		meshBox.push_back(newMeshBox);

		// Bone info
		newMeshBox->boxes = s_boxes;
		newMeshBox->voxelSize = voxelSize;
		newMeshBox->hashTable = s_hashTable;
		newMeshBox->update();
		newMeshBox->getInfoFromBone(boneArray[i]);
	}

	// Volume ratio 
	float totalBoneVol = 0;
	for (int i = 0; i < meshBox.size(); i++)
	{
		meshBox[i]->expectedVolRatio = boneArray[i]->getVolumef();
		totalBoneVol += boneArray[i]->getVolumef();
	}
	for (int i = 0; i < meshBox.size(); i++)
	{
		meshBox[i]->expectedVolRatio /= totalBoneVol;
	}

	// Bone index
	for (int i = 0; i < meshBox.size(); i++)
	{
		arrayInt voxelIdx = meshBox[i]->voxelIdxs;

		for (int j = 0; j < voxelIdx.size(); j++)
		{
			s_boxes->at(voxelIdx[j]).boneIndex = i;
		}
	}

	computeAspectRatio();
	leftDownVoxel = s_octree->m_root->leftDownTight;
	rightUpVoxel = s_octree->m_root->rightUpTight;
	voxelSize = s_octree->boxSize;

	tempMark = new int[s_boxes->size()];
	voxelOccupy = new int[s_boxes->size()];
}

bool detailSwapManager::swap1Voxel(int sourceIdx, int destIdx)
{
	bvhVoxel *meshSource = meshBox[sourceIdx];
	bvhVoxel *meshDest = meshBox[destIdx];

	// First, check if we should swap, by volume ratio
	if (!isBenefitVolume(sourceIdx, destIdx))
	{
		return false;
	}

	// Find voxel that is best for fill
	arrayInt vIdxsInSourse = meshBox[sourceIdx]->voxelIdxs;
	arrayInt toSwapIdxs;
	float bestBenefit = MIN;
	for (int i = 0; i < vIdxsInSourse.size(); i++)
	{
		// It should be on boundary
		arrayInt nbIdxs = s_boxShareFaceWithBox->at(vIdxsInSourse[i]);
		for (int j = 0; j < nbIdxs.size(); j++)
		{
			if (s_boxes->at(nbIdxs[j]).boneIndex == destIdx)
			{
				arrayInt temp = { vIdxsInSourse[i] };
				temp = getIdxsShoudSwap(sourceIdx, destIdx, temp);
				if (temp.size() > 1)
				{
					// Check if volume still benefit
				}
				float bene = benefitWithoutVolume(sourceIdx, destIdx, temp);

				if (bestBenefit < bene)
				{
					bestBenefit = bene;
					toSwapIdxs = temp;
				}
			}
		}

	}

	// swap
	meshSource->removeVoxel(toSwapIdxs);
	meshDest->addVoxel(toSwapIdxs);

	// Update node bone index
	for (int i = 0; i < toSwapIdxs.size(); i++)
	{
		(*s_boxes)[toSwapIdxs[i]].boneIndex = destIdx;
	}

	return true;
}

float detailSwapManager::getBenefit(arrayFloat weight, arrayFloat before, arrayFloat after)
{
	float b = 0;
	for (int i = 0; i < before.size(); i++)
	{
		b += weight[i]*(before[i] - after[i])*before[i];
	}

	return b;
}

inline float min_d(float a, float b)
{
	return a > b ? b : a;
}

bool detailSwapManager::isBenefitVolume(int sourceIdx, int destIdx)
{
	bvhVoxel* meshSourse = meshBox[sourceIdx];
	bvhVoxel* meshDest = meshBox[destIdx];

	arrayInt temp = {meshSourse->voxelIdxs[0]};

	arrayFloat eSource = meshSourse->getErrorSym();
	arrayFloat eDest = meshDest->getErrorSym();
	arrayFloat eSource_assume = meshSourse->getESymAssumeRemove(temp);
	arrayFloat eDest_assume = meshDest->getESymAssumeAdd(temp);

	float b1 = min_d(eSource[VOLUME_ERROR], eSource_assume[VOLUME_ERROR]) * (eSource[VOLUME_ERROR] - eSource_assume[VOLUME_ERROR]);
	float b2 = min_d(eDest[VOLUME_ERROR], eDest_assume[VOLUME_ERROR]) * (eDest[VOLUME_ERROR] - eDest_assume[VOLUME_ERROR]);

	return b1 + b2 > 0;
}

float detailSwapManager::benefitWithoutVolume(int sourceIdx, int destIdx, arrayInt idxs)
{
	bvhVoxel* meshSourse = meshBox[sourceIdx];
	bvhVoxel* meshDest = meshBox[destIdx];

	arrayFloat eSource = meshSourse->getErrorSym();
	arrayFloat eDest = meshDest->getErrorSym();
	arrayFloat eSource_assume = meshSourse->getESymAssumeRemove(idxs);
	arrayFloat eDest_assume = meshDest->getESymAssumeAdd(idxs);

	float b1 = eSource[FILL_RATIO] - eSource_assume[FILL_RATIO];
	float b2 = eDest[FILL_RATIO] - eDest_assume[FILL_RATIO];

	return b1 + b2;
}

bool detailSwapManager::swapBetweenBox1by1(int sourceIdx, int destIdx)
{
	bool swaped = false;
	while (swap1Voxel(sourceIdx, destIdx))
	{
		swaped = true;
	}

	return swaped;
}

void detailSwapManager::restoreCenterMesh()
{
	for (int i = 0; i < meshBox.size(); i++)
	{
		bvhVoxel* curM = meshBox[i];
		if (curM->boneType == CENTER_BONE)
		{
			arrayInt vIdxs = curM->voxelIdxs;
			arrayInt newVIdxs = vIdxs;
			for (int j = 0; j < vIdxs.size(); j++)
			{
				int idx = s_hashTable->getSymmetricBox(vIdxs[j]);
				if (idx != -1)
				{
					newVIdxs.push_back(idx);
				}
			}
			curM->voxelIdxs = newVIdxs;
		}
	}

	// Update bone index in box
	for (int i = 0; i < s_boxes->size(); i++)
	{
		s_boxes->at(i).boneIndex = -1;
	}

	for (int i = 0; i < meshBox.size(); i++)
	{
		arrayInt vIdx = meshBox[i]->voxelIdxs;
		for (int j = 0; j < vIdx.size(); j++)
		{
			s_boxes->at(vIdx[j]).boneIndex = i;
		}
	}
}

void detailSwapManager::initGroupVoxelFromSaveFile(char* filePath)
{
	FILE *f = fopen(filePath, "r");
	ASSERT(f);
	int nbMesh;
	fscanf(f, "%d\n", &nbMesh);
	for (int i = 0; i < nbMesh; i++)
	{
		bvhVoxel * newBox = new bvhVoxel;
		char nameB[100];
		fscanf(f, "%s\n", &nameB);
		newBox->boneName = CString(nameB);
		fscanf(f, "%d\n", &newBox->boneType);
		Vec3f ld, ru;
		fscanf(f, "%f %f %f\n", &ld[0], &ld[1], &ld[2]);
		fscanf(f, "%f %f %f\n", &ru[0], &ru[1], &ru[2]);
		newBox->curLeftDown = ld;
		newBox->curRightUp = ru;

		newBox->boxes = s_boxes;
		newBox->voxelSize = voxelSize;

		meshBox.push_back(newBox);
	}
	fclose(f);

	// Bone order
	std::vector<bone*> boneOrder;
	s_skeleton->getSortedGroupBoneArray(boneOrder);

	// Correspond with the list of the cut box
	std::vector<bone*> correctBoneOrder; // correspond with the cut boxes
	for (int i = 0; i < boneOrder.size(); i++)
	{
		CString name = meshBox[i]->boneName;
		// Find the corresponding bone
		for (auto b : boneOrder)
		{
			if (name.Compare(b->m_name) == 0)
			{
				correctBoneOrder.push_back(b);
				break;
			}
		}
	}

	// Volume ratio
	float totalBoneVol = 0;
	for (int i = 0; i < meshBox.size(); i++)
	{
		meshBox[i]->expectedVolRatio = correctBoneOrder[i]->getVolumef();
		totalBoneVol += correctBoneOrder[i]->getVolumef();
	}
	for (int i = 0; i < meshBox.size(); i++)
	{
		meshBox[i]->expectedVolRatio /= totalBoneVol;
	}

	// Bone array
	boneArray = correctBoneOrder;

	// Init voxel 
	constructVolxeHashFromCutTree2();

	tempMark = new int[s_boxes->size()];
	voxelOccupy = new int[s_boxes->size()];
}

