#include "StdAfx.h"
#include "skeleton.h"
#include "Utility_wrap.h"
#include "MeshCutting.h"
#include "neighbor.h"
#include "rapidxml_utils.hpp"
#include "myXML.h"
#include "matTranform.h"

// XML key
#define BONE_KEY "bone"
#define PROPERTIES_KEY "bone_properties"
#define CHILD_KEY "children"
#define NAME_KEY "bone_name"
#define BONE_SIZE_KEY "bone_size"
#define JOINT_PARENT_KEY "joint_pos_parent"
#define JOINT_CHILD_KEY "joint_pos_child"
#define BONE_TYPE_KEY "bone_type"
#define ROTATION_ANGLE_KEY "rotation_about_xyz"

using namespace rapidxml;

skeleton::skeleton(void)
{
	m_root = NULL;
	meshScale = 1;
}


skeleton::~skeleton(void)
{
	if (m_root)
		delete m_root;
}

void skeleton::initTest()
{
	m_root = (new bone);
	m_root->parent = nullptr;
	m_root->m_posCoord = Vec3f(0,0,0);
	m_root->m_angle = Vec3f(0,0,0);
	m_root->m_sizef = Vec3f(3,5,10);
	m_root->m_name = CString("Torso");
	m_root->m_type = TYPE_CENTER_BONE;
	m_root->initOther();

	{
		bone* child0(new bone);
		child0->parent = m_root;
		child0->m_jointBegin = Vec3f(0,0,10);
		child0->m_posCoord = Vec3f(0,0,10);
		child0->m_angle = Vec3f(0,0,0);
		child0->m_sizef = Vec3f(3,3,3);
		child0->m_name = CString("head");
		child0->m_type = TYPE_CENTER_BONE;
		child0->neighborType = NEIGHBOR_SIDE;
		child0->initOther();
		m_root->child.push_back(child0);
	}

	{
		bone* child1(new bone);
		child1->parent = m_root;
		child1->m_jointBegin = Vec3f(0,3,8);
		child1->m_posCoord = Vec3f(0,3,8);
		child1->m_angle = Vec3f(0,-45,0);
		child1->m_sizef = Vec3f(3,3,5);
		child1->m_name = CString("hand");
		child1->m_type = TYPE_SIDE_BONE;
		child1->neighborType = NEIGHBOR_SIDE;
		child1->initOther();
 		m_root->child.push_back(child1);

		bone* child2(new bone);
		child2->parent = m_root;
		child2->m_jointBegin = Vec3f(0, 0, 5);
		child2->m_posCoord = Vec3f(0, 0, 5);
		child2->m_angle = Vec3f(0, 0, 0);
		child2->m_sizef = Vec3f(4, 3, 5);
		child2->m_name = CString("hand2");
		child2->m_type = TYPE_SIDE_BONE;
		child2->neighborType = NEIGHBOR_SIDE;
		child2->initOther();
		child1->child.push_back(child2);
	}

	{
// 		bone* child3(new bone);
// 		child3->parent = m_root;
// 		child3->m_jointBegin = Vec3f(0, -2, 0);
// 		child3->m_posCoord = Vec3f(0, -2, 0);
// 		child3->m_angle = Vec3f(0, 180, 0);
// 		child3->m_sizef = Vec3f(2, 2, 6);
// 		child3->m_name = CString("leg");
// 		child3->m_type = TYPE_SIDE_BONE;
// 		child3->initOther();
// 		m_root->child.push_back(child3);
// 
// 		bone* child4(new bone);
// 		child4->parent = m_root;
// 		child4->m_jointBegin = Vec3f(0,0,6);
// 		child4->m_posCoord = Vec3f(0,0,6);
// 		child4->m_angle = Vec3f(0,0,0);
// 		child4->m_sizef = Vec3f(3,3,6);
// 		child4->m_name = CString("leg2");
// 		child4->m_type = TYPE_SIDE_BONE;
// 		child4->initOther();
// 		child3->child.push_back(child4);
	}

// 	{
// 		bone* wing(new bone);
// 		wing->parent = m_root;
// 		wing->m_jointBegin = Vec3f(1, 2, 8);
// 		wing->m_posCoord = Vec3f(1, 2, 8);
// 		wing->m_angle = Vec3f(0, 0, 0);
// 		wing->m_sizef = Vec3f(2, 2, 6);
// 		wing->m_name = CString("wing");
// 		wing->m_type = TYPE_SIDE_BONE;
// 		wing->initOther();
// 		m_root->child.push_back(wing);
// 	}

} 

void skeleton::draw(int mode)
{
	if (m_root != nullptr)
	{
		colorIndex = 0;
		sideBoneDrawFlag = false;
		drawBoneRecursive(m_root, mode);
	}

}

void skeleton::drawBoneRecursive(bone* node, int mode, bool mirror)
{
	if (node == nullptr)
		return;

	glPushMatrix();
	glTranslatef(node->m_posCoord[0], node->m_posCoord[1], node->m_posCoord[2]);
	// Rotate global x-y-z
	// In GL, we do invert
	glRotatef(node->m_angle[2], 0, 0, 1);// z
	glRotatef(node->m_angle[1], 0, 1, 0);// y
	glRotatef(node->m_angle[0], 1, 0, 0);// x

// 	static arrayVec3f color = Util_w::randColor(30);
// 	glColor3fv(color[colorIndex++].data());

	node->draw(mode, meshScale, mirror);

	for (size_t i = 0; i < node->child.size(); i++)
	{
		drawBoneRecursive(node->child[i], mode, mirror);

		if (node == m_root && node->child[i]->m_type == TYPE_SIDE_BONE)
		{
			sideBoneDrawFlag = true;
			glPushMatrix();
			glScalef(1, -1, 1);
			drawBoneRecursive(node->child[i], mode, true);
			glPopMatrix();
			sideBoneDrawFlag = false;
		}
	}


	glPopMatrix();
}

void skeleton::getSortedBoneArray(std::vector<bone*> &sortedArray)
{
	getSortedBoneArrayRecur(m_root, sortedArray);
}

void skeleton::getSortedBoneArrayRecur(bone* node, std::vector<bone*> &sortedArray)
{
	sortedArray.push_back(node);

	for (size_t i = 0; i < node->child.size(); i++)
	{
		getSortedBoneArrayRecur(node->child[i], sortedArray);
	}
}

void skeleton::getBoneAndNeighborInfo(std::vector<bone*> &boneArray, std::vector<std::pair<int,int>> &neighborA)
{
	getBoneAndNeighborInfoRecur(m_root, -1, boneArray, neighborA);
}

void skeleton::getBoneAndNeighborInfoRecur(bone* node, int parentIdx, std::vector<bone*> &boneArray, std::vector<std::pair<int,int>> &neighborA)
{
	boneArray.push_back(node);
	int idx = boneArray.size() -1;
	if (node->parent)
	{
		std::pair<int, int> nb(parentIdx, idx);
		neighborA.push_back(nb);
	}

	for (size_t i = 0; i < node->child.size(); i++)
	{
		getBoneAndNeighborInfoRecur(node->child[i], idx, boneArray, neighborA);
	}
}

void skeleton::groupBone()
{
	groupChildren(m_root);
}

void skeleton::groupChildren(bone* node)
{
	for (int i = 0; i < node->child.size(); i++)
	{
		bone* curChild = node->child[i];
		if (curChild->isLeaf())
		{
			continue;
		}
		else
		{
			// TO DO: Check if this bone can be group
			//	before mark it as group
			curChild->bIsGroup = true;

			// Update volume and volume ratio
			curChild->m_groupVolumef = volumeOfGroupBone(curChild);
			curChild->m_groupVolumeRatio = volumeRatioOfGroupBone(curChild);
		}
	}
}

float skeleton::volumeOfGroupBone(bone* node)
{
	if (node->isLeaf())
	{
		return node->m_volumef;
	}

	float vol = 0;
	for (int i = 0; i < node->child.size(); i++)
	{
		vol += volumeOfGroupBone(node->child[i]);
	}

	return vol + node->m_volumef;
}

float skeleton::volumeRatioOfGroupBone(bone* node)
{
	if (node->isLeaf())
	{
		return node->m_volumeRatio;
	}

	float vol = 0;
	for (int i = 0; i < node->child.size(); i++)
	{
		vol += volumeOfGroupBone(node->child[i]);
	}

	return vol + node->m_volumeRatio;
}

void skeleton::getBoneGroupAndNeighborInfo(std::vector<bone*> &boneArray, std::vector<std::pair<int, int>> &neighborA)
{
	getBoneGroupAndNeighborInfoRecur(m_root, -1, boneArray, neighborA);
}

void skeleton::getBoneGroupAndNeighborInfoRecur(bone* node, int parentIdx, std::vector<bone*> & boneArray, std::vector<std::pair<int, int>> & neighborA)
{
	boneArray.push_back(node);
	int idx = boneArray.size() - 1;
	if (node->parent)
	{
		std::pair<int, int> nb(parentIdx, idx);
		neighborA.push_back(nb);
	}

	if (node->bIsGroup)
		return;

	for (size_t i = 0; i < node->child.size(); i++)
	{
		getBoneGroupAndNeighborInfoRecur(node->child[i], idx, boneArray, neighborA);
	}
}

void skeleton::getSortedGroupBoneArray(std::vector<bone*> &sortedArray)
{
	getSortedBoneGroupArrayRecur(m_root, sortedArray);
}

void skeleton::getSortedBoneGroupArrayRecur(bone* node, std::vector<bone*> & sortedArray)
{
	sortedArray.push_back(node);
	for (int i = 0; i < node->child.size(); i++)
	{
		if (node->child[i]->bIsGroup)
		{
			sortedArray.push_back(node->child[i]);
		}
		else
		{
			getSortedBoneGroupArrayRecur(node->child[i], sortedArray);
		}
	}
}

void skeleton::getGroupBone(bone* node, std::vector<bone*> &groupBone)
{
	if (node->bIsGroup)
	{
		groupBone.push_back(node);
		return;
	}

	for (int i = 0; i < node->child.size(); i++)
	{
		getGroupBone(node->child[i], groupBone);
	}
}

void skeleton::getBoneInGroup(bone* node, std::vector<bone*> &boneInGroup)
{
	boneInGroup.push_back(node);
	for (int i = 0; i < node->child.size(); i++)
	{
		getBoneInGroup(node->child[i], boneInGroup);
	}
}

void skeleton::getNeighborPair(bone* node, std::vector<Vec2i> &neighbor, std::vector<bone*> boneArray)
{
	for (int i = 0; i < node->child.size(); i++)
	{
		// Find the index
		std::vector<bone*>::iterator it = find(boneArray.begin(), boneArray.end(), node);
		ASSERT(it != boneArray.end());
		int idx1 = it - boneArray.begin();

		it = find(boneArray.begin(), boneArray.end(), node->child[i]);
		ASSERT(it != boneArray.end());
		int idx2 = it - boneArray.begin();

		neighbor.push_back(Vec2i(idx1, idx2));

		getNeighborPair(node->child[i], neighbor, boneArray);
	}
}

void skeleton::computeTempVar()
{
	std::vector<bone*> boneArray;
	getSortedBoneArray(boneArray);
	float vol = 0;
	for (int i = 0; i < boneArray.size(); i++)
	{
		vol += boneArray[i]->m_volumef;
	}

	for (int i = 0; i < boneArray.size(); i++)
	{
		boneArray[i]->m_volumeRatio = boneArray[i]->m_volumef / vol;
	}
}

void skeleton::writeToFile(char* filePath)
{
	myXML * doc = new myXML;
	myXMLNode * node = doc->addNode(BONE_KEY);
	writeBoneToXML(doc, node, m_root);

	doc->save(filePath);
	delete doc;
}

void skeleton::writeBoneToXML(myXML * doc, myXMLNode * node, bone* boneNode)
{
	// Write properties
	myXMLNode * pNode = doc->addNodeToNode(node, PROPERTIES_KEY);
 	doc->addElementToNode(pNode, NAME_KEY, std::string(CStringA(boneNode->m_name)));
 	doc->addVectorDatafToNode(pNode, BONE_SIZE_KEY, boneNode->m_sizef);
	doc->addVectorDatafToNode(pNode, JOINT_PARENT_KEY, boneNode->m_jointBegin);
	doc->addVectorDatafToNode(pNode, JOINT_CHILD_KEY, boneNode->m_posCoord);
	doc->addElementToNode(pNode, BONE_TYPE_KEY, boneNode->getTypeString());
	doc->addVectorDatafToNode(pNode, ROTATION_ANGLE_KEY, boneNode->m_angle);

	// Now children
	if (boneNode->child.size() > 0)
	{
		myXMLNode * childNode = doc->addNodeToNode(node, CHILD_KEY);
		for (int i = 0; i < boneNode->child.size(); i++)
		{
			myXMLNode * newChild = doc->addNodeToNode(childNode, BONE_KEY);
			writeBoneToXML(doc, newChild, boneNode->child[i]);
		}
	}

}

void skeleton::loadFromFile(char *filePath)
{
	myXML * doc = new myXML;
	doc->load(filePath);

	myXMLNode * rootNode = doc->first_node(BONE_KEY);

	m_root = new bone;
	loadBoneData(doc, rootNode, m_root);

	ASSERT(!rootNode->next_sibling());
	delete doc;
}

void skeleton::loadBoneData(myXML * doc, myXMLNode * xmlNode, bone* boneNode)
{
	// Load data to bone
	myXMLNode * properties = xmlNode->first_node(PROPERTIES_KEY);
	boneNode->m_posCoord = doc->getVec3f(properties, JOINT_CHILD_KEY);
	boneNode->m_jointBegin = doc->getVec3f(properties, JOINT_PARENT_KEY);
	boneNode->m_angle = doc->getVec3f(properties, ROTATION_ANGLE_KEY);
	boneNode->m_sizef = doc->getVec3f(properties, BONE_SIZE_KEY);
	boneNode->m_name = CString(doc->getStringProperty(properties, NAME_KEY).c_str());
	boneNode->setBoneType(doc->getStringProperty(properties, BONE_TYPE_KEY));
	boneNode->initOther();

	// Load child bone
	myXMLNode * child = xmlNode->first_node(CHILD_KEY);
	if (child)
	{
		for (myXMLNode * nBone = child->first_node(BONE_KEY); nBone; nBone = nBone->next_sibling())
		{
			bone* newBone = new bone;
			newBone->parent = boneNode;
			loadBoneData(doc, nBone, newBone);

			boneNode->child.push_back(newBone);
		}
	}
}

void skeleton::buildTransformMatrix()
{
	m_root->transformMat = m_root->getLocalTransMat();

	buildTransformMatrixRecur(m_root);
}

void skeleton::buildTransformMatrixRecur(bone* node)
{
	for (int i = 0; i < node->child.size(); i++)
	{
		bone* childB = node->child[i];
		childB->transformMat = node->transformMat * childB->getLocalTransMat();
		buildTransformMatrixRecur(childB);
	}
}



float skeleton::getVolume()
{
	arrayBone_p allBones;
	getSortedBoneArray(allBones);
	float vol = 0;
	for (auto b:allBones)
	{
		vol += b->getVolumef();
	}

	return vol;
}

void skeleton::drawGroup(int mode)
{
	if (m_root)
	{
		colorIndex = 0;
		drawGroupRecur(m_root, mode);
	}
}

void skeleton::drawGroupRecur(bone* node, int mode, bool mirror /*= false*/)
{
	if (node == nullptr)
		return;

	glPushMatrix();
	glTranslatef(node->m_posCoord[0], node->m_posCoord[1], node->m_posCoord[2]);
	// Rotate global x-y-z
	// In GL, we do invert
	glRotatef(node->m_angle[2], 0, 0, 1);// z
	glRotatef(node->m_angle[1], 0, 1, 0);// y
	glRotatef(node->m_angle[0], 1, 0, 0);// x

// 	static arrayVec3f color = Util_w::randColor(30);
// 	glColor3fv(color[colorIndex++].data());
	node->draw(mode, meshScale*node->groupShrink(), mirror);

	if (node == m_root)
	{
		for (size_t i = 0; i < node->child.size(); i++)
		{
			drawGroupRecur(node->child[i], mode, mirror);

			if (node == m_root && node->child[i]->m_type == TYPE_SIDE_BONE)
			{
				sideBoneDrawFlag = true;
				glPushMatrix();
				glScalef(1, -1, 1);
				drawGroupRecur(node->child[i], mode, true);
				glPopMatrix();
				sideBoneDrawFlag = false;
			}
		}
	}

	glPopMatrix();
}

void bone::draw(int mode, float scale, bool mirror)
{
	glLineWidth(mirror ? 1.0 : 2.0);

// 	// Scale
// 	Vec3f center = (leftDownf + rightUpf) / 2.0;
// 	Vec3f diag = (rightUpf - leftDownf) / 2.0;
// 
// 	Vec3f ldf = center - diag*scale;
// 	Vec3f ruf = center + diag* scale;

	if (mode & SKE_DRAW_BOX_WIRE)
	{
		Util_w::drawBoxWireFrame(leftDownf, rightUpf);
		drawCoord();
	}

	if (mode & SKE_DRAW_BOX_SOLID)
	{
		
		Util_w::drawBoxSurface(leftDownf, rightUpf);
	}

	glLineWidth(1.0);

// 	if (child.size() == 0)
// 	{ // Draw joint
// 		GLUquadricObj *qobj = 0;
// 		qobj = gluNewQuadric();
// 		gluSphere(qobj, min(m_size[0], m_size[1])/2, 5, 5);
// 	}
}

void bone::initOther()
{
	leftDownf = Vec3f(-m_sizef[0] / 2, -m_sizef[1] / 2, 0);
	rightUpf = Vec3f(m_sizef[0] / 2, m_sizef[1] / 2, m_sizef[2]);
	m_volumef = m_sizef[0] * m_sizef[1] * m_sizef[2];
}


bone::~bone()
{
	for (size_t i = 0; i < child.size(); i++)
	{
		delete child[i];
	}

	if (mesh)
	{
		delete mesh;
	}
}

bone::bone()
{
	mesh = nullptr;
	indexOfMeshBox = -1;
	bIsGroup = false;
	parent = nullptr;
}

BOOL bone::isLarger(bone* a)
{
	if (a->m_type == TYPE_CENTER_BONE && a->m_type == TYPE_SIDE_BONE)
	{
		return true;
	}

	return m_volumef > a->m_volumef;
}

void bone::drawCoord()
{
	glPushAttrib(GL_COLOR);

	glBegin(GL_LINES);
	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(m_sizef[0]/2, 0, 0);

	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, m_sizef[1]/2, 0);

	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, m_sizef[2]/2);
	glEnd();

	glPopAttrib();
}

void bone::drawMesh(float scale)
{
	if (!mesh)
	{
		return;
	}
	glPushMatrix();
	Vec3f mid = (leftDownf+rightUpf)/2;
	glTranslatef(mid[0], mid[1], mid[2]);
	glScalef(scale, scale, scale);
	MeshCutting mC;
	mC.drawPolygonFace(mesh);

	glPopMatrix();
}

Vec3i normalizedVector(Vec3i sizei)
{
	int lengthOrder;
	// orientation
	int idxL, idxS, L = MIN, S = MAX;
	for (int i = 0; i < 3; i++)
	{
		if (L < sizei[i])
		{
			idxL = i; L = sizei[i];
		}
		if (S > sizei[i])
		{
			idxS = i; S = sizei[i];
		}
	}

	if (L == S) // Cube; original is OK
	{
		return Vec3i(1,2,4);
	}
	else
	{
		int idxM = -1;
		for (int i = 0; i < 3; i++)
		{
			if ((sizei[i] != L && i!=idxS)
				|| (sizei[i] != S && i!=idxL))
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

Vec3i normalizedVector(Vec3f sizef)
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

	if (L == S) // Cube; original is OK
	{
		return Vec3i(1,2,4);
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

#define matEYEi Mat3x3f(Vec3f(1,0,0), Vec3f(0,1,0), Vec3f(0,0,1))
#define To3(a) (a[0], a[1], a[2])
void bone::getMeshFromOriginBox(Vec3f leftDown, Vec3f rightUp)
{
	// order of size of box
	Vec3i meshBound = normalizedVector(rightUp-leftDown);
	Vec3i boneBound = normalizedVector(m_sizef);

	Mat3x3f rotMat;
	Vec3i row[3] = {Vec3i(1,0,0), Vec3i(0,1,0), Vec3i(0,0,1)};
	Vec3f rowf[3] = {Vec3f(1,0,0), Vec3f(0,1,0), Vec3f(0,0,1)};

	// boneBound = rotMat*meshBound
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (meshBound*row[j] == boneBound[i])
			{
				rotMat(i,0) = row[j][0];
				rotMat(i,1) = row[j][1];
				rotMat(i,2) = row[j][2];
			}
		}
	}

	// Transform the mesh
	Vec3f meshCenterPos = (leftDown+rightUp)/2;
	std::vector<cVertex>& vertices = mesh->vertices;
	for (int i = 0; i < vertices.size(); i++)
	{
		Vec3f curP = Vec3f(vertices[i].v[0], vertices[i].v[1], vertices[i].v[2]);
		curP = rotMat*(curP - meshCenterPos);
		vertices[i] = carve::geom::VECTOR(curP[0], curP[1], curP[2]);
	}
}

int bone::nbCenterNeighbor()
{
	int count = 0;
	for (int i = 0; i < child.size(); i++)
	{
		if (child[i]->m_type == CENTER_BONE)
		{
			count++;
		}
	}
	return count;
}

int bone::nbSideNeighbor()
{
	int count = 0;
	for (int i = 0; i < child.size(); i++)
	{
		if (child[i]->m_type == SIDE_BONE)
		{
			count++;
		}
	}
	return count;
}

int bone::nbNeighbor() const
{
	int nbN = child.size();
	if (parent)
	{
		nbN++;
	}

	return nbN;
}

bool bone::isLeaf()
{
	return child.size() == 0;
}

float bone::getVolumef()
{
	if (bIsGroup)
	{
		return m_groupVolumef;
	}
	else
		return m_volumef;
}

float& bone::volumeRatio()
{
	if (bIsGroup)
	{
		return m_groupVolumeRatio;
	}
	else
		return m_volumeRatio;
}

char* bone::getTypeString()
{
	if (m_type == CENTER_BONE)
	{
		return "center_bone";
	}
	else if (m_type == SIDE_BONE)
	{
		return "side_bone";
	}
	ASSERT(0);
	return "";
}

void bone::setBoneType(std::string typeString)
{
	if (typeString == "center_bone")
	{
		m_type = CENTER_BONE;
	}
	else if (typeString == "side_bone")
	{
		m_type = SIDE_BONE;
	}
}

Mat4x4f bone::getLocalTransMat()
{
	Mat4x4f a;
	a = transformUtil::translationMat(m_posCoord);
	a = a*transformUtil::rot_Z_Mat(m_angle[2]);
	a = a*transformUtil::rot_Y_Mat(m_angle[1]);
	a = a*transformUtil::rot_X_Mat(m_angle[0]);

	return a;
}

float bone::groupShrink()
{
	if (bIsGroup)
	{
		return m_groupVolumef / m_volumef;
	}
	
	return 1;
}
