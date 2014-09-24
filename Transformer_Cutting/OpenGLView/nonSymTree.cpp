#include "stdafx.h"
#include "nonSymTree.h"
#include "voxelObject.h"

using namespace std;

nonSymTree::nonSymTree()
{
}


nonSymTree::~nonSymTree()
{
}

void nonSymTree::init()
{
	parserSkeletonGroup();
	constructTree();
}

bool compareBone(bone* lhs, bone* rhs)
{
	if (lhs->getGroupChildCount() == rhs->getGroupChildCount())
	{
		return lhs->getVolumef() > rhs->getVolumef();
	}
	else
	{
		return lhs->getGroupChildCount() > rhs->getGroupChildCount();
	}
}

void nonSymTree::parserSkeletonGroup()
{
	std::vector<std::pair<int, int>> nb;
	s_groupSkeleton->getBoneGroupAndNeighborInfo(sortedBone, nb);

	// Process the skeleton
	// Sort by order of number of neighbor first, volume later
	for (int i = 0; i < sortedBone.size(); i++)
	{
		sortedBone[i]->indexOfMeshBox = i;
	}

	std::sort(sortedBone.begin(), sortedBone.end(), compareBone);
	
	// neighbor
	// hash index
	arrayInt idxs;
	idxs.resize(sortedBone.size());
	for (int i = 0; i < sortedBone.size(); i++)
	{
		idxs[sortedBone[i]->indexOfMeshBox] = i;
	}

	for (auto b:nb)
	{
		neighborPair.push_back(Vec2i(idxs[b.first], idxs[b.second]));
	}
}

void nonSymTree::constructTree()
{
	std::vector<voxelBox> *boxes = &voxelObj->m_boxes;

	// Root node
	meshCutPtr rootMesh(new meshCut);
	for (int i = 0; i < boxes->size(); i++)
	{
		rootMesh->voxelIdx.push_back(i);
	}
	rootMesh->leftDownf = surObj->getBVH()->root()->LeftDown;
	rootMesh->rightUpf = surObj->getBVH()->root()->RightUp;

	root = nonSymNodePtr(new nonSymNode);
	root->meshArray.push_back(move(rootMesh));
	root->depth = 0;

	std::cout << endl << "Start building cut tree" << endl;
	constructTreeRecur(root);
}

void nonSymTree::constructTreeRecur(nonSymNodePtr node)
{
	if (node->boxCount() == boneCount()) // Leaf node
	{
		leaves.push_back(node);
		return;
	}

	// Cut arbitrary on x, y, z
	// On arbitrary mesh
	std::vector<meshCutPtr> meshes = node->meshArray;
	int remainBox = boneCount() - meshes.size();
	for (auto i = 0; i < meshes.size(); i++) // All mesh
	{
		auto mesh = meshes[i];
		Vec3f ldf = mesh->leftDownf;
		Vec3f ruf = mesh->rightUpf;
		for (int direct = 0; direct < 3; direct ++) // x, y, z
		{
			// Decide cut step
			int debugCutCount = 0;
			float cutStep = (ruf - ldf)[direct] / (remainBox+1);
			for (float coord = ldf[direct] + cutStep; coord < ruf[direct]; coord += cutStep)
			{
				std::vector<std::vector<meshCutPtr>> pairs = splitMesh(node, i, direct, coord);

				for (auto g : pairs)
				{
					nonSymNodePtr newNode = nonSymNodePtr(new nonSymNode(node));
					newNode->meshArray.erase(newNode->meshArray.begin() + i);
					newNode->meshArray.insert(newNode->meshArray.end(), g.begin(), g.end());
					// We have a new mesh
					constructTreeRecur(newNode);
					node->childs.push_back(move(newNode));
				}


				if (node->depth == 0)
				{
					std::cout << "Building tree, root node cut by " << direct << " direction, " << debugCutCount++ << "/" << remainBox << endl;
				}
			}
		}
	}
}

int nonSymTree::boneCount()
{
	return sortedBone.size();
}

vector<vector<meshCutPtr>> nonSymTree::splitMesh(nonSymNodePtr node, int meshIdx, int direct, float coord)
{

	vector<meshCutPtr> meshes = node->getMeshArray();

	// Split the mesh
	vector<meshCutPtr> cutPiece = splitOnePiece(meshes[meshIdx], direct, coord);

	// Alway create 2 mesh only. Other parts may appear in later cut
	vector<vector<meshCutPtr>> twoPieceGroup;
	if (cutPiece.size() == 2)
	{
  		twoPieceGroup.push_back(cutPiece);
	}
	else if (cutPiece.size() > 2)
	{
		twoPieceGroup = group2pieces(meshes[meshIdx], cutPiece);
	}
	else
	{
		// Do nothing
	}

	for (auto group : twoPieceGroup)
	{
		for (auto mesh : group)
		{
			mesh->updateOther(voxelObj);
		}
	}

	return twoPieceGroup;
	// Modify the list
// 	meshes[meshIdx].reset();
// 	meshes.erase(meshes.begin() + meshIdx);
// 	meshes.insert(meshes.end(), cutPiece.begin(), cutPiece.end());
// 
// 	node->meshArray = meshes;
// 
// 	return validateBoxCount(node);
}

std::vector<meshCutPtr> nonSymTree::splitOnePiece(meshCutPtr meshes, int direct, float coord)
{
	using namespace std;
	std::vector<voxelBox> *boxes = &voxelObj->m_boxes;
	hashVoxel * hashTable = &voxelObj->m_hashTable;

	// Hash mark
	arrayInt mark = arrayInt(boxes->size(), 0);
	arrayInt voxelOccupy = arrayInt(boxes->size(), 0);
	arrayInt vIdxs = meshes->voxelIdx;
	for (auto vId : vIdxs)
	{
		voxelOccupy[vId] = 1;
	}

	std::vector<meshCutPtr> allPieces;
	for (auto vId : vIdxs)
	{
		if (mark[vId] != 0)
		{
			continue;
		}

		meshCutPtr newMesh(new meshCut);
		// The cut side
		Vec3f voxelCenter = boxes->at(vId).center;
		bool isLower = voxelCenter[direct] < coord;

		// Breath first search
		queue<int> vQ;
		vQ.push(vId);

		while (!vQ.empty())
		{
			int idx = vQ.front();
			vQ.pop();

			mark[idx] = 1;
			newMesh->voxelIdx.push_back(idx);

			// Check all adjacent of this vertex
			arrayInt adjacent = voxelObj->m_boxShareFaceWithBox[idx];
			for (auto nbIdx : adjacent)
			{
				if (mark[nbIdx] == 0 && voxelOccupy[nbIdx] == 1)
				{
					Vec3f nbPos = boxes->at(nbIdx).center;
					bool isNbLower = nbPos[direct] < coord;
					if (isLower == isNbLower)
					{
						vQ.push(nbIdx);
						mark[nbIdx] = 1;
					}
				}
			}
		}

		// We have new mesh
		allPieces.push_back(std::move(newMesh));
	}

	return allPieces;
}

std::vector<std::vector<meshCutPtr>> nonSymTree::group2pieces(meshCutPtr originMesh, std::vector<meshCutPtr> cutPiece)
{
	ASSERT(cutPiece.size() > 2);

	std::vector<std::vector<meshCutPtr>> outG;
	std::vector<voxelBox> *boxes = &voxelObj->m_boxes;

	arrayInt originIdxs = originMesh->voxelIdx;
	for (auto m : cutPiece)
	{
		// Check if remaining mesh is an object
		arrayInt curIdxs = m->voxelIdx;
		arrayInt remainIdx = Util_w::substractArrayInt(originIdxs, curIdxs, boxes->size());

		if (isConnected(remainIdx, boxes))
		{
			std::vector<meshCutPtr> pair;
			pair.push_back(move(meshCutPtr(new meshCut(remainIdx))));
			pair.push_back(move(meshCutPtr(new meshCut(curIdxs))));
			outG.push_back(pair);
		}
	}

	return outG;
}

std::vector<meshCutPtr> nonSymTree::mergeToTwo(std::vector<meshCutPtr> cutPiece)
{
	// Build neighbor
	arrayVec2i neighborB;
	return cutPiece;
}

bool nonSymTree::validateBoxCount(nonSymNodePtr node)
{
	if (node->boxCount() > boneCount())
	{
		return false;
	}

	if (node->boxCount() <= node->parent->boxCount())
	{
		return false;
	}

	return true;
}

void nonSymTree::drawLeave(int idx1) const
{
	if (idx1 < leaves.size())
		leaves[idx1]->drawBoundingBox();
}

bool nonSymTree::isConnected(arrayInt remainIdx, std::vector<voxelBox> * boxes)
{
	// Hash mark
	arrayInt mark = arrayInt(boxes->size(), 0);
	arrayInt voxelOccupy = arrayInt(boxes->size(), 0);
	for (auto vId : remainIdx)
	{
		voxelOccupy[vId] = 1;
	}


	int vId = remainIdx[0];
	arrayInt newIdxs;
	// The cut side
	Vec3f voxelCenter = boxes->at(vId).center;

	// Breath first search
	queue<int> vQ;
	vQ.push(vId);

	while (!vQ.empty())
	{
		int idx = vQ.front();
		vQ.pop();

		mark[idx] = 1;
		newIdxs.push_back(idx);

		// Check all adjacent of this vertex
		arrayInt adjacent = voxelObj->m_boxShareFaceWithBox[idx];
		for (auto nbIdx : adjacent)
		{
			if (mark[nbIdx] == 0 && voxelOccupy[nbIdx] == 1)
			{
				Vec3f nbPos = boxes->at(nbIdx).center;
				vQ.push(nbIdx);
				mark[nbIdx] = 1;
			}
		}
	}

	return newIdxs.size() == remainIdx.size();
}

nonSymNode::nonSymNode()
{

}

nonSymNode::nonSymNode(nonSymNodePtr parentN)
{
	parent = parentN;
	depth = parentN->depth + 1;
	// Clone mesh array
	for (auto m : parentN->meshArray)
	{
		meshCutPtr cloneMesh = m->clone();
		meshArray.push_back(cloneMesh);
	}
}

nonSymNode::~nonSymNode()
{

}

int nonSymNode::boxCount()
{
	return meshArray.size();
}

void nonSymNode::drawBoundingBox() const
{
	glEnable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);//for black back ground

	glEnable(GL_BLEND);

	static arrayVec3f color = Util_w::randColor(20);
	int idx = 0;

	for (auto m : meshArray)
	{
		glColor4f(color[idx][0], color[idx][1], color[idx][2], 0.2);
		Util_w::drawBoxSurface(m->leftDownf, m->rightUpf);

		glColor4f(color[idx+1][0], color[idx+1][1], color[idx+1][2], 1);
		Util_w::drawBoxWireFrame(m->leftDownf, m->rightUpf);
		idx++;
	}

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
}

std::vector<meshCutPtr>& nonSymNode::getMeshArray()
{
	return meshArray;
}
