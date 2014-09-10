#include "StdAfx.h"
#include "cutTree.h"
#include "errorCompute.h"
#include "neighbor.h"


cutTree::cutTree(void)
{
	m_rootNode = nullptr;
	lestE2Node = nullptr;
}

cutTree::~cutTree(void)
{
	if (m_rootNode)
	{
		delete m_rootNode;
	}
}

void cutTree::constructTree(VoxelObj* voxel, std::vector<boneAbstract> &m_boneOrder, int m_roughScaleStep)
{
	m_treeDepth = m_boneOrder.size()-1;
	m_voxelSize = voxel->m_voxelSize;
	meshObj = voxel;
	boneOrder = m_boneOrder;

	// First face
	Vec3i sizei = voxel->m_sizei;
	Vec3i leftDown = voxel->m_leftDown;

	Boxi curBox;
	curBox.leftDown = leftDown;
	curBox.rightUp = voxel->m_rightUp;

	// root node
	m_rootNode = new cutTreeNode;
	m_rootNode->depth = 0;
	m_rootNode->parent = nullptr;
	m_rootNode->m_boxes.push_back(curBox);
	if (m_boneOrder.size() == 1)
	{
		lestE2Node = m_rootNode;
		return;
	}

	// Construct
	leatE2 = MAX;
	treeSize = 0;
//	minVoli = 0; // 
	constructTreeRer(voxel, m_boneOrder, m_roughScaleStep, m_rootNode);
	TRACE1("Tree size: %d ---------------------------------\n", treeSize);
}

bool cutTree::cutBox(Boxi ParentBox, cutTreeNode* node)
{
	cutTreeNode* parent = node->parent;

// 	Vec3i leftDown = parent->m_boxes[node->m_parentCutBoxIdx].leftDown;
// 	Vec3i rightUp = parent->m_boxes[node->m_parentCutBoxIdx].rightUp;

	cutDirection direct = node->m_cutDirect;
	int pos = node->m_cutPos;

	Boxi box1 = ParentBox, box2 = ParentBox;

	box1.rightUp[(int)direct] = pos-1;
	box2.leftDown[(int)direct] = pos;

	// validate the surface
	// Volume should not be too small
	// Base on level, for example: The biggest box can still be divided
	int tt1 = meshObj->volumeInBox(box1);
	int tt2 = meshObj->volumeInBox(box2);
	if(meshObj->volumeInBox(box1) <= minVoli || meshObj->volumeInBox(box2) <= minVoli)
		return false;

	// Neighbor should be maintain


	node->m_boxes = parent->m_boxes;
	node->m_boxes.erase(node->m_boxes.begin()+node->m_parentCutBoxIdx);
	node->m_boxes.push_back(box1); // Should insert in shorted order
	node->m_boxes.push_back(box2);

	return true;
}

void cutTree::constructTreeRer(VoxelObj* voxel, std::vector<boneAbstract> &m_boneOrder, int m_roughScaleStep, cutTreeNode *curNode)
{
	if (curNode->depth == m_boneOrder.size()-1)//Leaf node
	{
		treeSize ++;
		// <-- Check the error
		// Sort the cut pieces of the mesh
		// Optimize later. We may need class for mesh box
		std::vector<Boxi> *boxes = curNode->boxes();
		std::vector<meshCutRough> sortedCut;
		for (int i = 0; i < boxes->size(); i++)
		{
			meshCutRough newCut;
			newCut.boundingBoxi = boxes->at(i);
			newCut.volumei = voxel->volumeInBox(newCut.boundingBoxi);
			newCut.volumeRatiof = (float)newCut.volumei / voxel->objectVolumei();
			newCut.computeAspectRatio();

			// Insert to sorted array
			sortedCut.push_back(newCut);
		}
		std::sort(sortedCut.begin(), sortedCut.end(), compareCutMesh_descen);

		// Filter the neighbor information
		// At least they should be in contact
		if (!neighborManager::isAllNeighborContact(sortedCut, neighborPair))
		{
			return;
		}
		
		// Now compute error; Volume ratio error and aspect ratio error
		float e2 = errorCompute::errorBtwMeshAndboneArray(m_boneOrder, sortedCut);
		if (leatE2 > e2)
		{
			leatE2 = e2;
			lestE2Node = curNode;
		}
		// End check the error -->
		return;
	}

	std::vector<Boxi> *boxes = curNode->boxes();

	for (int i = 0; i < boxes->size(); i++)
	{
		// Cut box index i
		Boxi curBox = boxes->at(i);
		Vec3i sizei = curBox.rightUp - curBox.leftDown + Vec3i(1,1,1);
		// 3 direction
		for (int d = 0; d < 3; d++)
		{
			// Need special treat for x, to maintain symmetric property

			for (int ii = m_roughScaleStep; ii <= sizei[d]; ii+= m_roughScaleStep)
			{
				cutTreeNode* newNode = new cutTreeNode;
				newNode->depth = curNode->depth + 1;
				newNode->parent = curNode;
				newNode->m_cutDirect = (cutDirection)d;
				newNode->m_cutPos = curBox.leftDown[d] + ii;
				newNode->m_parentCutBoxIdx = i;

				// validate the surface

				// Cut the box
				if(!cutBox(curBox, newNode))
				{
					delete newNode;
					continue;
				}

				curNode->children.push_back(newNode);

				constructTreeRer(voxel, m_boneOrder, m_roughScaleStep, newNode);
			}
		}
	}

}

void cutTree::drawLeaf(int nodeIdx)
{
	cutTreeNode *node = findLeafIdx(nodeIdx);
	if(!node)
		return;

	node->draw(m_voxelSize, centerXi);
}

cutTreeNode * cutTree::findLeafIdx(int idx)
{
	int curIdx = -1;
	return findLeafIdxRer(m_rootNode, curIdx, idx);
}

cutTreeNode * cutTree::findLeafIdxRer(cutTreeNode *node, int &curIdx, int idx)
{
	if (node->boxCount() == boxCount)//leaf
	{
		curIdx ++;
		if (curIdx == idx)
		{
			return node;
		}
		else
		{
			return nullptr;
		}
	}
	else
	{
		for (int i = 0; i < node->children.size(); i++)
		{
			cutTreeNode*fnode = findLeafIdxRer(node->children[i], curIdx, idx);
			if (fnode)
			{
				return fnode;
			}
		}
		return nullptr;
	}
}

void cutTree::constructTreeSymetric()
{
	boxCount = centerBoneOrder.size() + sideBoneOrder.size();
	// First face
	sizei = meshObj->m_sizei;
	leftDown = meshObj->m_leftDown;
	centerXi = leftDown[0] + ceil(sizei[0]/2.0);

	Boxi curBox;
	curBox.leftDown = leftDown;
	curBox.rightUp = meshObj->m_rightUp;

	// root node
	m_rootNode = new cutTreeNode;
	m_rootNode->depth = 0;
	m_rootNode->parent = nullptr;
	m_rootNode->centerBoxi.push_back(curBox);

	treeSize = 0;
	leatE2 = MAX;
	neighborManager::init();
	constructTreeSymetricRcur(m_rootNode);
}

void cutTree::constructTreeSymetricRcur(cutTreeNode* node)
{
	if (node->boxCount() >= boxCount) // Leaf
	{
		ASSERT(node->centerBoxi.size() == centerBoneOrder.size() &&
			node->sideBoxi.size() == sideBoneOrder.size());

		treeSize++;

		// Evaluate the error
		std::vector<meshCutRough> sortedCenterCut = convertBoxesToMeshCut(node->centerBoxi);
		std::vector<meshCutRough> sortedSideCut = convertBoxesToMeshCut(node->sideBoxi);

		// Evaluate neighbor information
		// Check if the neighbor skeleton is near to each other only
		if (!neighborManager::isAllNeighborContactSym(sortedCenterCut, sortedSideCut, neighborInfo))
		{
			return;
		}

		// Optimized later
		float eNeibor = neighborManager::neighborScore(sortedCenterCut, sortedSideCut, centerBoneOrder, sideBoneOrder, neighborInfo);

		// Evaluate volume and aspect ratio
		float e21 = errorCompute::errorBtwMeshAndboneArray(centerBoneOrder, sortedCenterCut);
		float e22 = errorCompute::errorBtwMeshAndboneArray(sideBoneOrder, sortedSideCut);

		if (leatE2 > e21+e22 + eNeibor)
		{
			leatE2 = e21 + e22 + eNeibor;
			lestE2Node = node;
		}

		return;
	}

	std::vector<Boxi> centerBoxi = node->centerBoxi;
	std::vector<Boxi> sideBoxi = node->sideBoxi;
	Vec3i sizei = meshObj->m_sizei;

	// 1. Divide center box
	for (int i = 0; i < centerBoxi.size(); i++)
	{
		Boxi curBox = centerBoxi[i];
		
		if (sideBoxi.size() < sideBoneOrder.size()) 
		{
			//1.1. Side box is not full, can create more
			// We can divide center box by x
			for (int co = curBox.leftDown[0] + cutStep; co < centerXi; co += cutStep)
			{
				// Each cut surface has a mirror surface. It create 3 boxes
				cutTreeNode* newNode = new cutTreeNode(node);
				if(!cutNodeSym(newNode, i, co)
					|| !validateBoxCount(newNode))
				{
					delete newNode;
					continue;
				}

				node->children.push_back(newNode);
				constructTreeSymetricRcur(newNode);
			}

			// 1.2. cut by center surface; create 2 side boxes only
			if (centerBoxi.size() > 1)
			{
				cutTreeNode* newNode = new cutTreeNode(node);
				if(!cutNodeSymAtCenter(newNode, i)
					|| !validateBoxCount(newNode))
				{
					delete newNode;
					continue;
				}

				node->children.push_back(newNode);
				constructTreeSymetricRcur(newNode);
			}

		}
		
		// 1.3. Divide by y and z freely
		for (int d = 1; d < 3; d++)
		{
			for (int co = curBox.leftDown[d] + cutStep; co < curBox.rightUp[d]; co += cutStep)
			{
				cutTreeNode *newNode = new cutTreeNode(node);
				if (!cutNodeSymYZ(newNode, i, co, d)
					|| !validateBoxCount(newNode))
				{
					delete newNode;
					continue;
				}
				node->children.push_back(newNode);
				constructTreeSymetricRcur(newNode);
			}
		}
	}


	
	// 2. Cut side box
	// Side box always create side box
	if (sideBoxi.size() < sideBoneOrder.size()) 
	{
		for (int i = 0; i < sideBoxi.size(); i++)
		{
			Boxi curB = sideBoxi[i];
			for (int d = 0; d < 3; d++)
			{
				for (int co = curB.leftDown[d]+cutStep; co < curB.rightUp[d]; co+=cutStep)
				{
					cutTreeNode* newNode = new cutTreeNode(node);
					if (!cutSideBox(newNode, i, d, co)
						|| !validateBoxCount(newNode))
					{
						delete newNode;
						continue;
					}

					node->children.push_back(newNode);
					constructTreeSymetricRcur(newNode);
				}
			}
		}
	}
}

bool cutTree::cutNodeSym(cutTreeNode* newNode, int boxIdx, int xPos)
{
	std::vector<Boxi> centerBoxi = newNode->centerBoxi;
	Boxi cBox = centerBoxi[boxIdx];

	if (xPos > cBox.leftDown[0] && xPos < cBox.rightUp[0])
	{
		Boxi boxside = cBox;
		Boxi boxcenter = cBox;
		boxside.rightUp[0] = xPos -1;
		boxcenter.leftDown[0] = xPos;
		boxcenter.rightUp[0] = 2*centerXi - xPos - 1;

		if(meshObj->volumeInBox(boxside) <= minVoli || meshObj->volumeInBox(boxcenter) <= minVoli)
			return false;

		newNode->centerBoxi.erase(newNode->centerBoxi.begin() + boxIdx);
		newNode->sideBoxi.push_back(boxside);
		newNode->centerBoxi.push_back(boxcenter);

		if (newNode->boxCount() > boxCount)
		{
			return false;
		}

		return true;
	}

	return false;
}

bool cutTree::cutNodeSymAtCenter(cutTreeNode* newNode, int boxIdx)
{
	std::vector<Boxi> centerBoxi = newNode->centerBoxi;
	Boxi cBox = centerBoxi[boxIdx];

	Boxi box1 = cBox;
	box1.rightUp[0] = centerXi-1;

	if(meshObj->volumeInBox(box1) <= minVoli)
		return false;

	newNode->centerBoxi.erase(newNode->centerBoxi.begin() + boxIdx);
	newNode->sideBoxi.push_back(box1);
	return true;
}

// Cut center box, make center boxes
bool cutTree::cutNodeSymYZ(cutTreeNode * newNode, int boxIdx, int Pos, int direct)
{
	std::vector<Boxi> centerBoxi = newNode->centerBoxi;
	Boxi cBox = centerBoxi[boxIdx];

	Boxi box1 = cBox, box2 = cBox;
	box1.leftDown[direct] = Pos;
	box2.rightUp[direct] = Pos - 1;

	if (meshObj->volumeInBox(box1) < minVoli || meshObj->volumeInBox(box2) < minVoli)
	{
		return false;
	}

	newNode->centerBoxi.erase(newNode->centerBoxi.begin() + boxIdx);
	newNode->centerBoxi.push_back(box1);
	newNode->centerBoxi.push_back(box2);
}

bool cutTree::cutSideBox(cutTreeNode* newNode, int boxIdx, int direct, int Pos)
{
	std::vector<Boxi> sideBoxi = newNode->sideBoxi;
	Boxi cBox = sideBoxi[boxIdx];

	Boxi box1 = cBox, box2 = cBox;
	box1.leftDown[direct] = Pos;
	box2.rightUp[direct] = Pos - 1;

	if (meshObj->volumeInBox(box1) < minVoli || meshObj->volumeInBox(box2) < minVoli)
	{
		return false;
	}

	newNode->sideBoxi.erase(newNode->sideBoxi.begin()+boxIdx);
	newNode->sideBoxi.push_back(box1);
	newNode->sideBoxi.push_back(box2);
}

std::vector<meshCutRough> cutTree::convertBoxesToMeshCut(std::vector<Boxi> &boxes)
{
	std::vector<meshCutRough> sortedCut;
	for (int i = 0; i < boxes.size(); i++)
	{
		meshCutRough newCut;
		newCut.boundingBoxi = boxes[i];
		newCut.volumei = meshObj->volumeInBox(newCut.boundingBoxi);
		newCut.volumeRatiof = (float)newCut.volumei / meshObj->objectVolumei();
		newCut.computeAspectRatio();
		newCut.computeBoundingBoxf(m_voxelSize);

		// Insert to sorted array
		sortedCut.push_back(newCut);
	}

	std::sort(sortedCut.begin(), sortedCut.end(), compareCutMesh_descen);
	return sortedCut;
}

bool cutTree::validateBoxCount(cutTreeNode* newNode)
{
	if (newNode->boxCount() == boxCount) // leaf
	{
		return newNode->centerBoxi.size() == centerBoneOrder.size() 
			&& newNode->sideBoxi.size() == sideBoneOrder.size();
	}
	else // otherwise
		return newNode->sideBoxi.size() <= sideBoneOrder.size();
}


void cutTreeNode::draw(float voxelSize)
{
	for (int i =0; i<m_boxes.size(); i++)
	{
		Util_w::drawIntCoordBox(m_boxes[i].leftDown, m_boxes[i].rightUp, voxelSize);
	}


}

void cutTreeNode::draw(float voxelSize, int centerX)
{
	int idx = 0;
	for (int i =0; i<centerBoxi.size(); i++)
	{
		Util_w::drawIntCoordBox(centerBoxi[i].leftDown, centerBoxi[i].rightUp, voxelSize);
		Vec3f center = Util_w::IJK2XYZ((centerBoxi[i].leftDown + centerBoxi[i].rightUp)/2, voxelSize);
		Util::printw(center[0], center[1], center[2], "c-%d", idx++);
	}

	for (int i =0; i<sideBoxi.size(); i++)
	{
		Util_w::drawIntCoordBox(sideBoxi[i].leftDown, sideBoxi[i].rightUp, voxelSize);
		Vec3f center = Util_w::IJK2XYZ((sideBoxi[i].leftDown + sideBoxi[i].rightUp)/2, voxelSize);
		Util::printw(center[0], center[1], center[2], "s-%d", idx++);

		Vec3i ld = sideBoxi[i].leftDown;
		Vec3i ru = sideBoxi[i].rightUp;
		Vec3i sizei = ru-ld + Vec3i(1,1,1);
		ld[0] = 2*centerX - ru[0] - 1;
		ru = ld+ sizei - Vec3i(1,1,1);
		Util_w::drawIntCoordBox(ld, ru, voxelSize);

		center = Util_w::IJK2XYZ((ld + ru)/2, voxelSize);
		Util::printw(center[0], center[1], center[2], "s-%d", idx++);
	}
}

int cutTreeNode::boxCount()
{
	return centerBoxi.size() + sideBoxi.size();
}

cutTreeNode::cutTreeNode(cutTreeNode *node)
{
	centerBoxi = node->centerBoxi;
	sideBoxi = node->sideBoxi;
	parent = node;
	depth = parent->depth+1;
}
