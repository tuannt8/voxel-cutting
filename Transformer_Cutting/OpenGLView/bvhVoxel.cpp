#include "StdAfx.h"
#include "bvhVoxel.h"
#include "Utility_wrap.h"
#include <math.h>
#include "Utility.h"
#include "neighbor.h"


bvhVoxel::bvhVoxel(void)
{
	bIsGroupBone = false;
}


bvhVoxel::~bvhVoxel(void)
{
}

void bvhVoxel::constructAABBTree()
{
	Root = new AABBNode;
	Root->LeftDown = leftDown;
	Root->RightUp = rightUp;
	Root->Parent = nullptr;
	Root->Depth = 0;

	constructAABBTree(Root);
}

void bvhVoxel::constructAABBTree(AABBNode* node)
{
	// the node is checked collided with object
	Vec3f sizef = node->RightUp - node->LeftDown;

	// Validate the current node
	if (sizef[0] < 1.5*voxelSize
		&& sizef[1] < 1.5*voxelSize
		&& sizef[2] < 1.5*voxelSize)
	{
		// Leaf. 
		node->IndexInLeafNode = hashTable->getBoxIndexFromCoord(node->LeftDown, node->RightUp);
		leaves.push_back(node);
		node->End = true;
		return;
	}

	// Divide the box
	Vec3i SMLIdx = Util_w::SMLIndexSizeOrder(sizef);
	int nbBox = ((sizef[SMLIdx[2]]+voxelSize/5)/voxelSize);
	int nbBox1 = nbBox/2;

	Box newBox[2];
	newBox[0] = Box(node->LeftDown, node->RightUp);
	newBox[1] = newBox[0];
	newBox[0].rightUp[SMLIdx[2]] = newBox[0].leftDown[SMLIdx[2]] + nbBox1*voxelSize;
	newBox[1].leftDown[SMLIdx[2]] += nbBox1*voxelSize;
	newBox[0].center = (newBox[0].leftDown + newBox[0].rightUp)/2;
	newBox[1].center = (newBox[1].leftDown + newBox[1].rightUp)/2;

	// Check validity of the two box
	if (s_octree->isColidWithBox(newBox[0]))
	{
		AABBNode* newNode = new AABBNode;
		newNode->setBoundingBox(newBox[0].leftDown, newBox[0].rightUp);
		node->Left = newNode;
		newNode->Parent = node;
		newNode->Depth = node->Depth+1;
		constructAABBTree(node->Left);
	}

	if (s_octree->isColidWithBox(newBox[1]))
	{
		AABBNode* newNode = new AABBNode;
		newNode->setBoundingBox(newBox[1].leftDown, newBox[1].rightUp);
		node->Right = newNode;
		newNode->Parent = node;
		newNode->Depth = node->Depth+1;
		constructAABBTree(node->Right);
	}

}

void bvhVoxel::drawOriginalBox()
{
	Util_w::drawBoxWireFrame(curLeftDown, curRightUp);
	Vec3f mid = (curLeftDown + curRightUp)/2;
	Util::printw(mid[0], mid[1], mid[2], (LPCSTR)CStringA(boneName));
}

void bvhVoxel::boundingBoxIfRemoveNode(AABBNode *node, Box &boxOut)
{
	if (!node->Parent)
	{
		return;
	}

	AABBNode* siblingNode = (node == node->Parent->Left)?  node->Parent->Right : node->Parent->Left;

	if (siblingNode)
	{
		boxOut = combineBox(boxOut, Box(siblingNode->LeftDown, siblingNode->RightUp));
	}

	boundingBoxIfRemoveNode(node->Parent, boxOut);
}

Box bvhVoxel::combineBox(Box box1, Box box2)
{
	Box newBox;
	for (int i = 0; i < 3; i++)
	{
		newBox.leftDown[i] = Util::min_(box1.leftDown[i], box2.leftDown[i]);
		newBox.rightUp[i] = Util::max_(box1.rightUp[i], box2.rightUp[i]);
	}

	return newBox;
}

void bvhVoxel::removeVoxel(AABBNode* node)
{
	removeNode(node);
	updateAABBTreeBottomUp();

	// Need optimization
	leaves.clear();
	reconstructLeaves(Root);
}

void bvhVoxel::removeVoxel(arrayInt newVoxelIdx)
{
	int *markRemove = new int[boxes->size()];
	std::fill(markRemove, markRemove + boxes->size(), 0);
	for (int i = 0; i < newVoxelIdx.size(); i++)
	{
		markRemove[newVoxelIdx[i]] = 1;
	}

	arrayInt remainIdx;
	for (int i = 0; i < voxelIdxs.size(); i++)
	{
		if (markRemove[voxelIdxs[i]] == 0)
		{
			remainIdx.push_back(voxelIdxs[i]);
		}
	}

	voxelIdxs = remainIdx;
	delete []markRemove;

	update();
}

void bvhVoxel::addVoxel(AABBNode* node)
{
	addNode(node);
	updateAABBTreeBottomUp();

	leaves.clear();
	reconstructLeaves(Root);
}

void bvhVoxel::addVoxel(arrayInt newVoxelIdx)
{
	voxelIdxs.insert(voxelIdxs.end(), newVoxelIdx.begin(), newVoxelIdx.end());
	update();
}

void bvhVoxel::updateAABBTreeBottomUp()
{
	updateAABBTreeBottomUp(Root);
}

void bvhVoxel::updateAABBTreeBottomUp(AABBNode* node)
{
	if (!node)
		return;

	if (node->End)
		return;

	updateAABBTreeBottomUp(node->Left);
	updateAABBTreeBottomUp(node->Right);

	// Update bounding box
	Box boxL(Vec3f(MAX, MAX, MAX), Vec3f(MIN, MIN, MIN));
	Box boxR = boxL;
	if (node->Left)
	{
		boxL = Box(node->Left->LeftDown, node->Left->RightUp);
	}
	if (node->Right)
	{
		boxR = Box(node->Right->LeftDown, node->Right->RightUp);
	}
	Box newBox = combineBox(boxL, boxR);

	node->LeftDown = newBox.leftDown;
	node->RightUp = newBox.rightUp;
}

void bvhVoxel::drawLeaves()
{
	for (int i = 0; i < leaves.size(); i++)
	{
		Util_w::drawBoxWireFrame(leaves[i]->LeftDown, leaves[i]->RightUp);
	}
}

void bvhVoxel::drawVoxelBoneIdx(int idx)
{
	for (int i = 0; i < leaves.size(); i++)
	{
		Vec3f center = (leaves[i]->LeftDown + leaves[i]->RightUp)/2;
		Util::printw(center[0], center[1], center[2], "%d", idx);
	}
}

void bvhVoxel::reconstructLeaves(AABBNode* node)
{
	if(node->End)
		leaves.push_back(node);

	if (node->Left)
	{
		reconstructLeaves(node->Left);
	}

	if (node->Right)
	{
		reconstructLeaves(node->Right);
	}
}

void bvhVoxel::errorWithAssumpRemoveNode(AABBNode* voxelAABB, float &volE, float &aspectE, float &volFillE, float &premeter)
{
	Box b(Vec3f(MAX, MAX, MAX), Vec3f(MIN, MIN, MIN));
	boundingBoxIfRemoveNode(voxelAABB, b);

	// volume error
	float volRatio = ((float)leaves.size()-1)/boxes->size();
	volE = Util::normSquareAbs(expectedVolRatio, volRatio);

	// aspect
	Vec3f curSize = b.rightUp - b.leftDown;

	float e2 = 0;
	for (int i = 0; i < 3; i++)
	{
		e2 += Util::normSquareAbs(xyzRatio[i], curSize[i]/curSize[0]);
	}

	aspectE = e2/3;

	// Fill
	volFillE = ((float)leaves.size()-1)*pow(voxelSize, 3) / (curSize[0]*curSize[1]*curSize[2]);

	// premeter
	premeter = curSize[0] + curSize[1] + curSize[2];
}

void bvhVoxel::errorWithAssumpRemoveNode(std::vector<AABBNode*> voxelAABBs, float &volE, float &aspectE, float &volFillE, float &premeter)
{
	Box b(Vec3f(MAX, MAX, MAX), Vec3f(MIN, MIN, MIN));
	boundingBoxIfRemoveNodes(voxelAABBs, b);

	// volume error
	float volRatio = ((float)leaves.size()-1)/boxes->size();
	volE = Util::normSquareAbs(expectedVolRatio, volRatio);

	// aspect
	Vec3f curSize = b.rightUp - b.leftDown;

	float e2 = 0;
	for (int i = 0; i < 3; i++)
	{
		e2 += Util::normSquareAbs(xyzRatio[i], curSize[i]/curSize[0]);
	}

	aspectE = e2/3;

	// Fill
	volFillE = ((float)leaves.size()-1)*pow(voxelSize, 3) / (curSize[0]*curSize[1]*curSize[2]);

	// premeter
	premeter = curSize[0] + curSize[1] + curSize[2];
}

void bvhVoxel::error(float &volE, float &aspectE, float &volFillE, float &premeter)
{
	// Volume
	float volRatio = (float)leaves.size()/boxes->size();
	volE = Util::normSquareAbs(expectedVolRatio, volRatio);

	// aspect
	Vec3f curSize = Root->RightUp - Root->LeftDown;

	float e2 = 0;
	for (int i = 0; i < 3; i++)
	{
		e2 += Util::normSquareAbs(xyzRatio[i], curSize[i]/curSize[0]);
	}

	aspectE = e2/3;

	// fill
	volFillE = ((float)leaves.size()-1)*pow(voxelSize, 3) / (curSize[0]*curSize[1]*curSize[2]);

	// premeter
	premeter = curSize[0] + curSize[1] + curSize[2];
}

void bvhVoxel::errorWithAssumpAddNode(AABBNode* node, float &volE, float &aspectE, float &volFillE, float &premeter)
{
	// volume error
	float volRatio = ((float)leaves.size() + 1)/boxes->size();
	volE = Util::normSquareAbs(expectedVolRatio, volRatio);

	// aspect
	Box meshBox(Root->LeftDown, Root->RightUp);
	Box leafBox(node->LeftDown, node->RightUp);

	Box newB = combineBox(meshBox, leafBox);

	Vec3f curSize = newB.rightUp - newB.leftDown;

	float e2 = 0;
	for (int i = 0; i < 3; i++)
	{
		e2 += Util::normSquareAbs(xyzRatio[i], curSize[i]/curSize[0]);
	}

	aspectE = e2/3;

	// Fill
	volFillE = ((float)leaves.size()+1)*pow(voxelSize, 3) / (curSize[0]*curSize[1]*curSize[2]);

	// premeter
	premeter = curSize[0] + curSize[1] + curSize[2];
}

void bvhVoxel::errorWithAssumpAddNode(std::vector<AABBNode*> voxelAABBs, float &volE, float &aspectE, float &volFillE, float &premeter)
{
	// volume error
	float volRatio = ((float)leaves.size() + 1)/boxes->size();
	volE = Util::normSquareAbs(expectedVolRatio, volRatio);

	// aspect
	Box meshBox(Root->LeftDown, Root->RightUp);
	Box newB = meshBox;
	for (int i = 0; i < voxelAABBs.size(); i++)
	{
		Box leafBox(voxelAABBs[i]->LeftDown, voxelAABBs[i]->RightUp);
		newB = combineBox(newB, leafBox);
	}


	Vec3f curSize = newB.rightUp - newB.leftDown;

	float e2 = 0;
	for (int i = 0; i < 3; i++)
	{
		e2 += Util::normSquareAbs(xyzRatio[i], curSize[i]/curSize[0]);
	}

	aspectE = e2/3;

	// Fill
	volFillE = ((float)leaves.size()+1)*pow(voxelSize, 3) / (curSize[0]*curSize[1]*curSize[2]);

	// premeter
	premeter = curSize[0] + curSize[1] + curSize[2];
}

void bvhVoxel::removeNodeWithVoxelIdx(int idx)
{
	// Find voxel
	for (int i = 0; i < leaves.size(); i++)
	{
		if (leaves[i]->IndexInLeafNode == idx)
		{
			removeVoxel(leaves[i]);
			return;
		}
	}
	ASSERT(0);
}

void bvhVoxel::addNodeWithVoxelIdx(int idx)
{
	voxelBox curV = boxes->at(idx);
	AABBNode* newNode = new AABBNode;

	newNode->setBoundingBox(curV.leftDown, curV.rightUp);
	newNode->IndexInLeafNode = idx;

	addVoxel(newNode);
}

void bvhVoxel::drawBoundingBoxSym(mirrorDraw *mirror)
{
	mirror->drawMirrorBox(Root->LeftDown, Root->RightUp);
}

void bvhVoxel::removeNodeNotInList(hashVoxel* hashBigVox, int *mark, int boxIdx, arrayInt &idxsBig)
{
	removeNodeNotInList(Root, hashBigVox, mark, boxIdx, idxsBig);
	updateAABBTreeBottomUp();

	// Need optimization
	leaves.clear();
	reconstructLeaves(Root);
}

void bvhVoxel::removeNodeNotInList(AABBNode* node, hashVoxel* hashBigVox, int *mark, int boxIdx, arrayInt &idxsBig)
{
	if (node->End) // leaf
	{
		Vec3f center = (node->LeftDown + node->RightUp)/2.0;
		int idxInBig = hashBigVox->getBoxIndexFromCoord(center);
		if (mark[idxInBig] == boxIdx) // Correct
		{
		}
		else
		{
			node->Updated = true; // meaning need to remove
			// this variable is inited false
		}

		return;
	}

	// not leaf
	if (node->Left)
	{
		removeNodeNotInList(node->Left, hashBigVox, mark, boxIdx, idxsBig);
		if (node->Left->Updated)// meaning need to remove
		{
			delete node->Left;
			node->Left = nullptr;
		}
	}
	if (node->Right)
	{
		removeNodeNotInList(node->Right, hashBigVox, mark, boxIdx, idxsBig);

		if (node->Right->Updated)// meaning need to remove
		{
			delete node->Right;
			node->Right = nullptr;
		}
	}

	if (!node->Left && !node->Right)
	{
		node->Updated = true;// meaning need to remove
	}
}

void bvhVoxel::drawLeavesSym(mirrorDraw *mirror)
{
	for (int i = 0; i < leaves.size(); i++)
	{
		mirror->drawMirrorBox(leaves[i]->LeftDown, leaves[i]->RightUp);
	}
}

void bvhVoxel::drawVoxelIndex()
{
	for (int i = 0; i < voxelIdxs.size(); i++)
	{
		Vec3f center = boxes->at(voxelIdxs[i]).center;
		Util::printw(center[0], center[1], center[2], "%d", voxelIdxs[i]);
	}
}

void bvhVoxel::boundingBoxIfRemoveNodes(std::vector<AABBNode *> nodes, Box &boxOut)
{
	// Mark node for remove
	for (int i = 0; i < nodes.size(); i++)
	{
		nodes[i]->markedAssumeRemoved = true;
	}

	boxOut = boundingBoxAssumeRemove(Root);

	for (int i = 0; i < nodes.size(); i++)
	{
		nodes[i]->markedAssumeRemoved = false;
	}
}

Box bvhVoxel::boundingBoxAssumeRemove(AABBNode* node)
{
	if (node == nullptr)
	{
		return Box(Vec3f(MAX,MAX,MAX), Vec3f(MIN, MIN, MIN));
	}

	if (node->End) //leaf
	{
		if (node->markedAssumeRemoved)
		{
			return Box(Vec3f(MAX,MAX,MAX), Vec3f(MIN, MIN, MIN));
		}
		else
			return Box(node->LeftDown, node->RightUp);
	}

	Box b1 = boundingBoxAssumeRemove(node->Left);
	Box b2 = boundingBoxAssumeRemove(node->Right);

	return combineBox(b1, b2);
}

void bvhVoxel::getCurrentError(float &aspectE, float &volumeE)
{
	// Optimize later if work
	Box b = getBoundingOfVoxels(voxelIdxs);

	// Volume
	float volRatio = (float)voxelIdxs.size()/boxes->size();
	volumeE = Util::normSquareAbs(expectedVolRatio, volRatio);

	// aspect
	Vec3f curSize = b.rightUp - b.leftDown;

	float e2 = 0;
	for (int i = 0; i < 3; i++)
	{
		e2 += Util::normSquareAbs(xyzRatio[i], curSize[i]/curSize[0]);
	}

	aspectE = e2/3;
}

Box bvhVoxel::getBoundingOfVoxels(arrayInt idxs)
{
	// Get current bounding box
	Vec3f LD(MAX, MAX, MAX);
	Vec3f RU(MIN, MIN, MIN);
	Box b(LD, RU);

	for (int i = 0; i < idxs.size(); i++)
	{
		voxelBox curB = boxes->at(idxs[i]);
		b = combineBox(b, Box(curB.leftDown, curB.rightUp));
	}

	return b;
}

void bvhVoxel::getErrorWithAssumeAddVoxel(float &aspectE, float &volumeE, arrayInt addIdxs)
{
	// Optimize later if work
	Box b = getBoundingOfVoxels(voxelIdxs);
	Box ba = getBoundingOfVoxels(addIdxs);

	b = combineBox(b,ba);

	// Volume
	float volRatio = ((float)voxelIdxs.size() + addIdxs.size())/boxes->size();
	volumeE = Util::normSquareAbs(expectedVolRatio, volRatio);

	// aspect
	Vec3f curSize = b.rightUp - b.leftDown;

	float e2 = 0;
	for (int i = 0; i < 3; i++)
	{
		e2 += Util::normSquareAbs(xyzRatio[i], curSize[i]/curSize[0]);
	}

	aspectE = e2/3;
}

void bvhVoxel::getErrorWithAssumeRemoveVoxel(float &aspectE, float &volumeE, arrayInt removeIdxs)
{

	int *markRemove = new int[boxes->size()];
	std::fill(markRemove, markRemove + boxes->size(), 0);
	for (int i = 0; i < removeIdxs.size(); i++)
	{
		markRemove[removeIdxs[i]] = 1;
	}

	arrayInt remainIdx;
	for (int i = 0; i < voxelIdxs.size(); i++)
	{
		if (markRemove[voxelIdxs[i]] == 0)
		{
			remainIdx.push_back(voxelIdxs[i]);
		}
	}


	// Optimize later if work
	Box b = getBoundingOfVoxels(remainIdx);

	// Volume
	float volRatio = (float)remainIdx.size()/boxes->size();
	volumeE = Util::normSquareAbs(expectedVolRatio, volRatio);

	// aspect
	Vec3f curSize = b.rightUp - b.leftDown;

	float e2 = 0;
	for (int i = 0; i < 3; i++)
	{
		e2 += Util::normSquareAbs(xyzRatio[i], curSize[i]/curSize[0]);
	}

	aspectE = e2/3;

	delete []markRemove;
}

void bvhVoxel::update()
{
	Box b = getBoundingOfVoxels(voxelIdxs);
	curLeftDown = b.leftDown;
	curRightUp = b.rightUp;
}

void bvhVoxel::drawVoxels(mirrorDraw* mirror, int mode)
{


	// Draw voxel
	for (int i = 0; i < voxelIdxs.size(); i++)
	{
		if (mode == 0)
		{


			Util_w::drawBoxWireFrame(boxes->at(voxelIdxs[i]).leftDown, boxes->at(voxelIdxs[i]).rightUp);
		}
		else if (mode == 1)
		{
			// Draw bounding box
			glLineWidth(3.0);
			Util_w::drawBoxWireFrame(curLeftDown, curRightUp);
			glLineWidth(1.0);

			Util_w::drawBoxSurface(boxes->at(voxelIdxs[i]).leftDown, boxes->at(voxelIdxs[i]).rightUp);
		}

	}

// 	if (mirror && boneType == SIDE_BONE)
// 	{
// 		if (mode == 0)
// 		{
// 			for (int i = 0; i < voxelIdxs.size(); i++)
// 			{
// 				mirror->drawMirrorBox(boxes->at(voxelIdxs[i]).leftDown, boxes->at(voxelIdxs[i]).rightUp);
// 			}
// 
// 
// 		}
// 		if (mode == 1)
// 		{
// 			glLineWidth(3.0);
// 			mirror->drawMirrorBox(curLeftDown, curRightUp);
// 			glLineWidth(1.0);
// 		}
// 
// 	}
}

arrayFloat bvhVoxel::getCurrentError2()
{
	// Optimize later if work
	Box b = getBoundingOfVoxels(voxelIdxs);

	// Volume error
	float volRatio = (float)voxelIdxs.size() / boxes->size();
	float volumeE = Util::normSquareAbs(expectedVolRatio, volRatio);

	// aspect error
	Vec3f curSize = b.rightUp - b.leftDown;

	float e2 = 0;
	for (int i = 0; i < 3; i++)
	{
		e2 += Util::normSquareAbs(xyzRatio[i], curSize[i] / curSize[0]);
	}

	float aspectE = e2 / 3;

	// Fill error
	float volF = voxelIdxs.size()*pow(voxelSize, 3);
	float boxVolF = curSize[0] * curSize[1] * curSize[2];
	float fillRatio = volF / boxVolF;

	// Assign
	arrayFloat err;
	err.resize(ERROR_TYPE_NUM);

	err[ASPECT_ERROR] = aspectE;
	err[VOLUME_ERROR] = volumeE;
	err[FILL_RATIO] = fillRatio;

	return err;
}

arrayFloat bvhVoxel::getErrorWithAssumeRemoveVoxel2(arrayInt removeIdxs)
{
//	int *markRemove = new int[boxes->size()];
	arrayInt markRemove;
	markRemove.resize(boxes->size());
	std::fill(markRemove.begin(), markRemove.end(), 0);
	for (int i = 0; i < removeIdxs.size(); i++)
	{
		markRemove[removeIdxs[i]] = 1;
	}

	arrayInt remainIdx;
	for (int i = 0; i < voxelIdxs.size(); i++)
	{
		if (markRemove[voxelIdxs[i]] == 0)
		{
			remainIdx.push_back(voxelIdxs[i]);
		}
	}
//	delete[]markRemove;

	// Optimize later if work
	Box b = getBoundingOfVoxels(remainIdx);

	// Volume
	float volRatio = (float)remainIdx.size() / boxes->size();
	float volumeE = Util::normSquareAbs(expectedVolRatio, volRatio);

	// aspect
	Vec3f curSize = b.rightUp - b.leftDown;

	float e2 = 0;
	for (int i = 0; i < 3; i++)
	{
		e2 += Util::normSquareAbs(xyzRatio[i], curSize[i] / curSize[0]);
	}

	float aspectE = e2 / 3;



	// Fill error
	float volF = (remainIdx.size()) * pow(voxelSize, 3);
	float boxVolF = curSize[0] * curSize[1] * curSize[2];
	float fillRatio = volF / boxVolF;

	// Assign
	arrayFloat err;
	err.resize(ERROR_TYPE_NUM);

	err[ASPECT_ERROR] = aspectE;
	err[VOLUME_ERROR] = volumeE;
	err[FILL_RATIO] = fillRatio;

	return err;
}

arrayFloat bvhVoxel::getErrorWithAssumeAddVoxel2(arrayInt addIdxs)
{
	// Optimize later if work
	Box b = getBoundingOfVoxels(voxelIdxs);
	Box ba = getBoundingOfVoxels(addIdxs);

	b = combineBox(b, ba);

	// Volume
	float volRatio = ((float)(voxelIdxs.size() + addIdxs.size())) / boxes->size();
	float volumeE = Util::normSquareAbs(expectedVolRatio, volRatio);

	// aspect
	Vec3f curSize = b.rightUp - b.leftDown;

	float e2 = 0;
	for (int i = 0; i < 3; i++)
	{
		e2 += Util::normSquareAbs(xyzRatio[i], curSize[i] / curSize[0]);
	}

	float aspectE = e2 / 3;

	// Fill error
	float volF = ((float)voxelIdxs.size() + addIdxs.size()) * pow(voxelSize, 3);
	float boxVolF = curSize[0] * curSize[1] * curSize[2];
	float fillRatio = volF / boxVolF;

	// Assign
	arrayFloat err;
	err.resize(ERROR_TYPE_NUM);

	err[ASPECT_ERROR] = aspectE;
	err[VOLUME_ERROR] = volumeE;
	err[FILL_RATIO] = fillRatio;

	return err;
}

void bvhVoxel::initOther()
{
	Box b = getBoundingOfVoxels(voxelIdxs);
	leftDown = b.leftDown;
	rightUp = b.rightUp;
}

arrayFloat bvhVoxel::getErrorSym()
{
	return getErrorSymWithidxs(voxelIdxs);
}

arrayFloat bvhVoxel::getESymAssumeRemove(const arrayInt & removeIdxs)
{
	arrayInt markRemove;
	markRemove.resize(boxes->size());
	std::fill(markRemove.begin(), markRemove.end(), 0);
	for (int i = 0; i < removeIdxs.size(); i++)
	{
		markRemove[removeIdxs[i]] = 1;
	}

	arrayInt remainIdx;
	for (int i = 0; i < voxelIdxs.size(); i++)
	{
		if (markRemove[voxelIdxs[i]] == 0)
		{
			remainIdx.push_back(voxelIdxs[i]);
		}
	}

	return getErrorSymWithidxs(remainIdx);
}

arrayFloat bvhVoxel::getESymAssumeAdd(const arrayInt & temp)
{
	arrayInt newIdxs = voxelIdxs;
	newIdxs.insert(newIdxs.end(), temp.begin(), temp.end());

	return getErrorSymWithidxs(newIdxs);
}



float bvhVoxel::getExpectedVolumeRatioSym()
{
	if (boneType == CENTER_BONE)
	{
		return expectedVolRatio / 2.0;
	}
	else
	{
		return expectedVolRatio;
	}
}

Vec3f bvhVoxel::getSizeSym()
{
	Vec3f s = xyzRatio;
	if (boneType == CENTER_BONE)
	{
		s[1] = s[1] * 2;
		s[2] = s[2] * 2;
	}
	
	return s;
}

arrayFloat bvhVoxel::getErrorSymWithidxs(const arrayInt & idxs)
{
	// Optimize later if work
	Box b = getBoundingOfVoxels(idxs);

	// Volume
	float volRatio = (float)idxs.size() / boxes->size();
	float volumeE = Util::normSquareAbs(getExpectedVolumeRatioSym(), volRatio);

	// aspect error
	Vec3f curSize = b.rightUp - b.leftDown;
	Vec3f xyzR = getSizeSym();

	float e2 = 0;
	for (int i = 0; i < 3; i++)
	{
		e2 += Util::normSquareAbs(xyzR[i], curSize[i] / curSize[0]);
	}

	float aspectE = e2 / 3;

	// Fill error
	float volF = idxs.size()*pow(voxelSize, 3);
	float boxVolF = curSize[0] * curSize[1] * curSize[2];
	float fillRatio = volF / boxVolF;

	// Assign
	arrayFloat err;
	err.resize(ERROR_TYPE_NUM);

	err[ASPECT_ERROR] = aspectE;
	err[VOLUME_ERROR] = volumeE;
	err[FILL_RATIO] = 1- fillRatio;

	return err;
}

void bvhVoxel::getInfoFromBone(bone* b)
{
	boneName = b->m_name;
	boneType = b->m_type;
}

Vec3f bvhVoxel::centerPoint()
{
	return (curLeftDown + curRightUp) / 2;
}

