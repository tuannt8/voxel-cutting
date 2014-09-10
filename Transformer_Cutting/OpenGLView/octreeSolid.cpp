#include "StdAfx.h"
#include "octreeSolid.h"
#include "Modules/CollisionManager.h"
#include "Utility_wrap.h"



octreeSolid::octreeSolid(void)
{
	m_root = nullptr;
	sufObj = nullptr;
	surfaceLoaded = false;
}


octreeSolid::~octreeSolid(void)
{
	if (m_root)
	{
		delete m_root;
	}

	if (surfaceLoaded && sufObj)
	{
		delete sufObj; // remember this
	}

}

void octreeSolid::initTest()
{
	surfaceLoaded = true;
	sufObj = new SurfaceObj;
	sufObj->readObjDataSTL("../Data_File/euroFighter.stl");
	sufObj->constructAABBTree();

	constructTree(sufObj,  4);
	computeBoxAndVolume(m_root);
}

void octreeSolid::init(char* filePath, int res)
{
	surfaceLoaded = true;
	sufObj = new SurfaceObj;
	sufObj->readObjDataSTL(filePath);
	sufObj->constructAABBTree();

	constructTree(sufObj,  res);
	computeBoxAndVolume(m_root);
}

void octreeSolid::init(SurfaceObj* obj, int res)
{
	sufObj = obj;

	constructTree(sufObj, res);
	computeBoxAndVolume(m_root);
}

void octreeSolid::draw(BOOL mode[10])
{
	if (mode[1])
	{
		for (int i = 0; i < leaves.size(); i++)
		{
			leaves[i]->drawWireBox();
		}
	}

	if (mode[2])
	{
		if (sufObj)
		{
			sufObj->drawObject();	
		}
	}

	if (mode[3])
	{
		if (m_root)
		{
			drawTree(m_root);
		}
	}

	if (mode[4])
	{
		if (sufObj)
		{
			sufObj->drawBVH();
		}
	}
	if (mode[5])
	{
		drawTightTree(m_root);
	}
}


void octreeSolid::drawMesh()
{
	if (sufObj)
	{
		glColor3f(0,0,1);
		sufObj->drawWireFrame();

		glColor3f(1,0,0);
		sufObj->drawObject();	
	}
}

void octreeSolid::drawWireOctree(int mode)
{
	if (m_root)
	{
		drawTree(m_root, mode);
	}
}

void octreeSolid::constructTree(SurfaceObj *obj, int depth)
{
	sufObj = obj;
	treeDepth = depth;

	Vec3f ld = sufObj->getBVH()->root()->LeftDown;
	Vec3f ru = sufObj->getBVH()->root()->RightUp;
	centerMesh = (ld+ru)/2;
	Vec3f diag = ru-ld;
	float maxEdge = Util::max_(Util::max_(diag[0], diag[1]), diag[2]);
	boxSize = maxEdge / std::pow((float)2, (int)treeDepth);
	Vec3f newDiag(maxEdge, maxEdge, maxEdge);

	m_root = new octreeSNode;
	m_root->leftDownf = centerMesh - newDiag/2;
	m_root->rightUpf = centerMesh + newDiag/2;
	m_root->nodeDepth = 0;

	constructTreeRecur(m_root, depth);
}

void octreeSolid::constructTreeRecur(octreeSNode * node, int depth)
{
	// Stop at leaf
	if (node->nodeDepth == depth)
	{
		node->bEnd = true;
		leaves.push_back(node);
		return;
	}

	Vec3f mid = (node->leftDownf + node->rightUpf)/2;
	std::vector<Box> boxes = get8ChildrenBox(node->leftDownf, node->rightUpf);

	GeometricFunc geoFunc;
	for (int i = 0; i < 8; i++)
	{
		if (isSurfIntersectWithBox(sufObj, boxes[i])
			|| geoFunc.isPointInSurf(sufObj->point(), sufObj->face(), sufObj->getBVH()->root(), boxes[i].center))
		{
			octreeSNode* newN = new octreeSNode;
			newN->leftDownf = boxes[i].leftDown;
			newN->rightUpf = boxes[i].rightUp;
			newN->nodeDepth = node->nodeDepth+1;
			newN->parent = node;

			node->children[i] = newN;
			constructTreeRecur(newN, depth);
		}
		else
		{
			node->children[i] = nullptr;
		}
	}
}

std::vector<Box> octreeSolid::get8ChildrenBox(Vec3f leftDownf, Vec3f rightUpf)
{
	Vec3f mid = (leftDownf + rightUpf)/2;
	Vec3f pt[3] = {leftDownf, mid, rightUpf};
	std::vector<Box> boxes;

	for (int i = 0; i <= 1; i ++)
	{
		for (int j = 0; j <= 1; j ++)
		{
			for (int k = 0; k <= 1; k ++)
			{
				Vec3f ld = Vec3f(pt[i][0], pt[j][1], pt[k][2]);
				Vec3f ru = Vec3f(pt[i+1][0], pt[j+1][1], pt[k+1][2]);
				Box newb(ld, ru);
				boxes.push_back(newb);
			}
		}
	}

	return boxes;
}

bool octreeSolid::isSurfIntersectWithBox(SurfaceObj * sufObj, Box boxf)
{
	CollisionManager colMngr;
	if (colMngr.isSurfaceWithAABBIntersectBox(sufObj->getBVH()->root(), boxf, sufObj->point(), sufObj->face()))
	{
		return true;
	}
	else
	{
		GeometricFunc geoFunc;
		if (geoFunc.isPointInSurf(sufObj->point(), sufObj->face(), sufObj->getBVH()->root(), (boxf.leftDown+boxf.rightUp)/2))
		{
			return true;
			// For optimization, all of it children should automatically set as inside
		}

		return false;
	}
}

void octreeSolid::drawTree(octreeSNode *node, int mode)
{

	if (node->bEnd)
	{
		glColor3f(1,0,0);
		Util_w::drawBoxWireFrame(node->leftDownf, node->rightUpf);
	}
	else
	{
		glColor3f(0,1,0);
		if (mode == 0)
		{
			Util_w::drawBoxWireFrame(node->leftDownf, node->rightUpf);
		}

		for (int i = 0; i < 8; i++)
		{
			if (node->children[i])
			{
				drawTree(node->children[i], mode);
			}
		}
	}
}

_inline float volumeRectf(Vec3f v){return v[0]*v[1]*v[2];}

void octreeSolid::computeBoxAndVolume(octreeSNode *node)
{
	if (node->bEnd)
	{
		// Leaf node
		node->leftDownTight = node->leftDownf;
		node->rightUpTight = node->rightUpf;
		node->volumef = volumeRectf((node->rightUpTight - node->leftDownTight));
	}
	else
	{
		Vec3f ld(MAX, MAX, MAX);
		Vec3f ru(MIN, MIN, MIN);
		node->volumef = 0;
		for (int i = 0; i < 8; i++)
		{
			if (node->children[i])
			{
				computeBoxAndVolume(node->children[i]);
				node->volumef += node->children[i]->volumef;

				for (int j = 0; j < 3; j ++)
				{
					if (ld[j] > node->children[i]->leftDownTight[j])
					{
						ld[j] = node->children[i]->leftDownTight[j];
					}
					if (ru[j] < node->children[i]->rightUpTight[j])
					{
						ru[j] = node->children[i]->rightUpTight[j];
					}
				}
			}
		}

		node->leftDownTight = ld;
		node->rightUpTight = ru;
	}
}

void octreeSolid::drawTightTree(octreeSNode *node)
{
	if (node->bEnd)
	{
		glColor3f(1,0,0);
		Util_w::drawBoxWireFrame(node->leftDownTight, node->rightUpTight);
	}
	else
	{
		glColor3f(0,1,0);
		Util_w::drawBoxWireFrame(node->leftDownTight, node->rightUpTight);
		for (int i = 0; i < 8; i++)
		{
			if (node->children[i])
			{
				drawTree(node->children[i]);
			}
		}
	}
}

void octreeSolid::intersectWithBox(Box boxIn, Box &intersectBox, float &volumeSide)
{
	volumeSide = 0;
	intersectBox = boxIn;

	intersectWithBox(m_root, boxIn, intersectBox, volumeSide);
}

void octreeSolid::intersectWithBox(octreeSNode* node, Box boxIn, Box &intersectBox, float &volumeSide)
{
	if (!node)
		return;

	GeometricFunc geoFc;
	BOOL isInside;

	if (geoFc.isBoxInBox(boxIn.leftDown, boxIn.rightUp, node->leftDownTight, node->rightUpTight))
	{
		// Cover entirely. Fit intersect box
		volumeSide += node->volumef;
		geoFc.fitBoxToCoverBox(intersectBox, node->leftDownTight, node->rightUpTight);
	}
	else
	{
		Box boxNode(node->leftDownTight, node->rightUpTight);
		if(geoFc.collisionBtwBox(boxIn, boxNode))
		{
			if (node->bEnd
				&& geoFc.isPointInBox(boxIn.leftDown, boxIn.rightUp, boxNode.center))
			{
				// Sine boxIn may slightly cover the node box, we check the center point
				volumeSide += node->volumef;
				geoFc.fitBoxToCoverBox(intersectBox, node->leftDownTight, node->rightUpTight);
			}
			else
			{
				for (int i = 0; i < 8; i++)
				{
					intersectWithBox(node->children[i], boxIn, intersectBox, volumeSide);
				}
			}
		}	
	}

}

meshPiece octreeSolid::intersectWithBox(meshPiece &boxIn)
{
	meshPiece boxOut(Vec3f(MAX, MAX,MAX), Vec3f(MIN, MIN, MIN));
	boxOut.volumef = 0;
	intersectWithBox(m_root, boxOut, boxIn);

	return boxOut;
}

void octreeSolid::intersectWithBox(octreeSNode* node, meshPiece &boxOut, meshPiece &boxIn)
{
	if (!node)
		return;

	GeometricFunc geoFc;
	BOOL isInside;

	if (geoFc.isBoxInBox(boxIn.leftDown, boxIn.rightUp, node->leftDownTight, node->rightUpTight))
	{
		// Cover entirely. Fit intersect box
		boxOut.volumef += node->volumef;
		enlargerToCoverBox(boxOut, node->leftDownTight, node->rightUpTight);
	}
	else
	{
		Box boxNode(node->leftDownTight, node->rightUpTight);
		if(geoFc.collisionBtwBox(Box(boxIn.leftDown, boxIn.rightUp), boxNode))
		{
			if (node->bEnd
				&& geoFc.isPointInBox(boxIn.leftDown, boxIn.rightUp, boxNode.center))
			{
				// Sine boxIn may slightly cover the node box, we check the center point
				boxOut.volumef += node->volumef;
				enlargerToCoverBox(boxOut, node->leftDownTight, node->rightUpTight);
			}
			else
			{
				for (int i = 0; i < 8; i++)
				{
					intersectWithBox(node->children[i], boxOut, boxIn);
				}
			}
		}	
	}
}

void octreeSolid::enlargerToCoverBox(meshPiece& boxOut, Vec3f leftDown, Vec3f rightUp)
{
	for (int i = 0; i < 3; i++)
	{
		if (boxOut.leftDown[i] > leftDown[i])
		{
			boxOut.leftDown[i] = leftDown[i];
		}

		if (boxOut.rightUp[i] < rightUp[i])
		{
			boxOut.rightUp[i] = rightUp[i];
		}
	}
}

bool octreeSolid::isColidWithBox(Box b)
{
	return isColidWithBox(m_root, b);
}

bool octreeSolid::isColidWithBox(octreeSNode* node, Box &b)
{
	GeometricFunc geoF;
	if (geoF.collisionBtwBox(b, Box(node->leftDownTight, node->rightUpTight)))
	{
		if (node->bEnd)
		{
			if (geoF.isPointInBox(b.leftDown, b.rightUp, (node->leftDownTight+node->rightUpTight)/2))
			{	
				return true;
			}
			return false;
		}
		else
		{
			for (int i = 0; i < 8; i++)
			{
				if (node->children[i])
				{
					if (isColidWithBox(node->children[i], b))
					{
						return true;
					}
				}
			}
			return false;
		}

	}
	else
		return false;

}



octreeSNode::octreeSNode()
{
	for (int i = 0; i < 8; i++)
	{
		children[i] = nullptr;
	}
	nodeDepth = 0;
	bEnd = false;
	parent = nullptr;
}

octreeSNode::~octreeSNode()
{
	for (int i = 0; i < 8; i++)
	{
		if (children[i])
		{
			delete children[i];
		}
	}
}

void octreeSNode::drawWireBox()
{
	Util_w::drawBoxWireFrame(leftDownf, rightUpf);
}
