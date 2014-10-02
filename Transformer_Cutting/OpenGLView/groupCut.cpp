#include "stdafx.h"
#include "groupCut.h"


groupCutNode::groupCutNode()
{
	parent = nullptr;
	depth = 0;
}

groupCutNode::groupCutNode(groupCutNode *parentIn)
	:groupCutNode()
{
	parent = parentIn;
	depth = parentIn->depth + 1;
	boxf = parentIn->boxf;
}

groupCutNode::~groupCutNode()
{
	for (int i = 0; i < child.size(); i++)
	{
		delete child[i];
	}
	child.clear();
}

void groupCutNode::draw(int mode)
{
	static arrayVec3f color = Util_w::randColor(6);
	if (mode == 0)
	{
		// Draw wire frame
		for (int i = 0; i < boxf.size(); i++)
		{
			glColor3fv(color[i].data());
			Util_w::drawBoxWireFrame(boxf[i].leftDown, boxf[i].rightUp);
		}
	}
}

void groupCutNode::draw(std::vector<bone*> bones, std::map<int, int> boneMeshmap)
{
	static arrayVec3f color_1 = Util_w::randColor(6);

	for (int i = 0; i < bones.size(); i++)
	{
		int meshIdx = boneMeshmap[i];
		glColor3fv(color_1[i].data());
		Util_w::drawBoxWireFrame(boxf[meshIdx].leftDown, boxf[meshIdx].rightUp);

		// draw name
		Vec3f center = (boxf[meshIdx].leftDown + boxf[meshIdx].rightUp) / 2;
		Util::printw(center[0], center[1], center[2], "%s", CStringA(bones[i]->m_name.GetBuffer()));
	}
}

void groupCutNode::drawNeighbor(std::vector<bone*> bones, std::map<int, int> boneMeshmap, arrayVec2i neighborInfo, std::vector<neighborPos> posConfig, float voxelSize)
{
	for (int i = 0; i < neighborInfo.size(); i++)
	{
		int meshIdx1 = boneMeshmap[neighborInfo[i][0]];
		int meshIdx2 = boneMeshmap[neighborInfo[i][1]];
		neighborPos relativePos = posConfig[i];


		Vec3f pt1, pt2;
		int idxContact = floor(relativePos / 2);
		int idx1, idx2;
		Util::getTwoOtherIndex(idxContact, idx1, idx2);

		Vec3f ld1 = boxf[meshIdx1].leftDown;
		Vec3f ru1 = boxf[meshIdx1].rightUp;

		Vec3f ld2 = boxf[meshIdx2].leftDown;
		Vec3f ru2 = boxf[meshIdx2].rightUp;

		Vec3f ldContact, ruContact;
		ldContact[idx1] = std::max({ ld1[idx1], ld2[idx1] });
		ldContact[idx2] = std::max({ ld1[idx2], ld2[idx2] });

		ruContact[idx1] = std::min({ ru1[idx1], ru2[idx1] });
		ruContact[idx2] = std::min({ ru1[idx2], ru2[idx2] });

		Vec3f ptMid = (ldContact + ruContact) / 2;
		pt1 = ptMid;
		pt2 = ptMid;

		pt1[idxContact] = (ld1[idxContact] + ru1[idxContact]) / 2;
		pt2[idxContact] = (ld2[idxContact] + ru2[idxContact]) / 2;

		glLineWidth(3.0);
		glBegin(GL_LINES);
		glVertex3fv(pt1.data());
		glVertex3fv(pt2.data());
		glEnd();
		glLineWidth(1.0);

		float radius = voxelSize / 5;
		Util_w::drawSphere(pt1, radius);
		Util_w::drawSphere(pt2, radius);
	}
}


groupCut::groupCut()
{
	m_root = nullptr;
	mark = nullptr;
	voxelOccupy = nullptr;
}


groupCut::~groupCut()
{
	if (m_root)
	{
		delete m_root;
	}

	if (mark)
	{
		delete[]mark;
	}

	if (voxelOccupy)
	{
		delete[]voxelOccupy;
	}

	// TODO: Remove this code after testing
}

void groupCut::constructTree()
{
	m_root = new groupCutNode;
	meshPiece rootBox;
	rootBox.leftDown = sourcePiece->curLeftDown;
	rootBox.rightUp = sourcePiece->curRightUp;
	rootBox.volumei = sourcePiece->voxelIdxs.size();
	rootBox.voxels = voxelIdxs;
	m_root->boxf.push_back(rootBox);

	mark = new int[boxes->size()];
	voxelOccupy = new int[boxes->size()];

	constructTreeRecur(m_root);
}

void groupCut::constructTreeRecur(groupCutNode * node)
{
	if (node->boxf.size() == bones.size())
	{
		// Leaf node
		// TODO: Assign bone and map configuration
		computeError(node);
		boxPose.parserConfigure(node);
		leaves.push_back(node);
		return;
	}

	// Divide the node by 3 dimension
	std::vector<meshPiece>* boxf = &node->boxf;
	for (int i = 0; i < boxf->size(); i++)
	{
		meshPiece curB = boxf->at(i);
		for (int xyzd = 0; xyzd < 3; xyzd++)
		{
			// in each direction, we split the object
			// Assume that the box is small. The cut step is one voxel size
			for (float coord = curB.leftDown[xyzd] + voxelSize; coord < curB.rightUp[xyzd]; coord += voxelSize)
			{
				groupCutNode *newNode = new groupCutNode(node);
				if (!cutBox(newNode, i, xyzd, coord))
				{
					delete newNode;
					continue;
				}

				node->child.push_back(newNode);
				constructTreeRecur(newNode);
			}
		}
	}
}

bool groupCut::cutBox(groupCutNode * newNode, int boxIdx, int xyzd, float coord)
{
	std::vector<meshPiece> *boxf = &newNode->boxf;
	meshPiece curB = boxf->at(boxIdx);

	// now, cut the box
	std::vector<meshPiece> newCutBox;
	arrayInt voxelIdx = curB.voxels;

	std::fill(mark, mark + boxes->size(), 0);
	std::fill(voxelOccupy, voxelOccupy + boxes->size(), 0);

	for (int i = 0; i < voxelIdx.size(); i++)
	{
		voxelOccupy[voxelIdx[i]] = 1;
	}

	int nbBoxCut;

	for (int i = 0; i < voxelIdx.size(); i++)
	{
		if (mark[voxelIdx[i]] != 0)
		{
			continue;
		}

		meshPiece newMesh;
		newMesh.initForVoxel();

		// Queue voxel
		std::queue<int> vQ;
		vQ.push(voxelIdx[i]);
		
		Vec3f curCenter = boxes->at(voxelIdx[i]).center;

		if (curCenter[xyzd] > coord)
		{
			newMesh.isLower = false;
		}
		else
			newMesh.isLower = true;

		while (!vQ.empty())
		{
			int idx = vQ.front();
			vQ.pop();

			mark[idx] = 1; // Mark the voxel has been processed
			newMesh.voxels.push_back(idx);
			newMesh.fitBOundingBox(boxes->at(idx).leftDown, boxes->at(idx).rightUp);

			// Check all adjacent of the voxel
			arrayInt adjacentIdxs = neighborVoxel->at(idx);
			for (int j = 0; j < adjacentIdxs.size(); j++)
			{
				if (mark[adjacentIdxs[j]] == 0
					&& voxelOccupy[adjacentIdxs[j]] == 1)
				{
					// It is valid
					Vec3f nCenter = boxes->at(adjacentIdxs[j]).center;
					bool isLower = nCenter[xyzd] < coord;
					if (isLower == newMesh.isLower) // OK
					{
						vQ.push(adjacentIdxs[j]);
						mark[adjacentIdxs[j]] = 1;
					}
				}
			}
		}

		newCutBox.push_back(newMesh);
	}

	// Modify the node
	int preNbBox = boxf->size();

	boxf->erase(boxf->begin() + boxIdx);
	for (int i = 0; i < newCutBox.size(); i++)
	{
		newCutBox[i].volumei = newCutBox[i].voxels.size();
		if (newCutBox[i].volumei < 1)
		{
			return false;
		}

		boxf->push_back(newCutBox[i]);
	}

	if (boxf->size() <= preNbBox)
	{
		return false;
	}

	// validate the number of the box
	return boxf->size() <= bones.size();
}

bool groupCut::computeError(groupCutNode * node)
{
	std::vector<meshPiece> *boxf = &node->boxf;

	// Compute volume ratio
	int totalVi = sourcePiece->voxelIdxs.size();
	for (int i = 0; i < boxf->size(); i++)
	{
		boxf->at(i).volumeRatio = (float)boxf->at(i).volumei / totalVi;
	}

	// Sort the box by volume order

	// Map to bone array

	// Add to pose manager

	return true;
}

void groupCut::drawLeaveIdx(int idx)
{
	if (idx < 0 || idx >= leaves.size())
		return;

	leaves.at(idx)->draw(0);
}

void groupCut::drawPose(int poseIdx, int configIdx)
{
	neighborPose pp = boxPose.getPoseByIdx(poseIdx);
	if (pp.posConfigId == -1)
	{
		return;
	}
	if (configIdx < 0 || configIdx >= pp.nodeGroupBoneCut.size())
	{
		return;
	}
	groupCutNode *node = pp.nodeGroupBoneCut[configIdx];
	std::map<int, int> boneMeshMap = pp.mapBone_meshIdx[configIdx];

	node->draw(bones, boneMeshMap);
	node->drawNeighbor(bones, boneMeshMap, boxPose.neighborInfo, pp.posConfig, voxelSize);
}