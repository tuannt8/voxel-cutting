#include "StdAfx.h"
#include "skeleton.h"
#include "Utility_wrap.h"
#include "rapidxml_utils.hpp"
#include "myXML.h"

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
	scale = Vec3f(1, -1, 1);
}


skeleton::~skeleton(void)
{
	if (m_root)
		delete m_root;
}

void skeleton::initTest()
{

} 

void skeleton::draw(int mode)
{
	if (m_root != nullptr)
	{
		colorIndex = 0;
		drawBoneRecursive(m_root, mode);
	}

}

void skeleton::drawBoneRecursive(bone* node, int mode)
{
	if (node == nullptr)
		return;

	glPushMatrix();
	Vec3f trans = node->m_posCoord + node->localTranslatef;
	glTranslatef(trans[0], trans[1], trans[2]);
	glRotatef(node->m_angle[0], 0, 0, 1);// z
	glRotatef(node->m_angle[1], 1, 0, 0);// x
	glRotatef(node->m_angle[2], 0, 0, 1);// z

	static arrayVec3f color = Util_w::randColor(20);
	glColor3fv(color[node->colorIndex].data());

	node->draw(mode, meshScale);


	for (size_t i = 0; i < node->child.size(); i++)
	{
		drawBoneRecursive(node->child[i], mode);

		if (node == m_root && node->child[i]->m_type == SIDE_BONE)
		{
			glScalef(-1, 1, 1);
			glRotatef(180, 0, 0, 1);
			drawBoneRecursive(node->child[i], mode);
			glScalef(1, 1, 1);
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

void skeleton::writeToFile(const char* filePath)
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

void skeleton::loadFromFile(const char *filePath)
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

void skeleton::writeToFileWithModification(const char* filePath)
{
	myXML * doc = new myXML;
	myXMLNode * node = doc->addNode(BONE_KEY);
	writeBoneToXMLWithModify(doc, node, m_root);

	doc->save(filePath);
	delete doc;
}

void skeleton::writeBoneToXMLWithModify(myXML * doc, myXMLNode * node, bone* boneNode)
{
	// Write properties
	myXMLNode * pNode = doc->addNodeToNode(node, PROPERTIES_KEY);
	doc->addElementToNode(pNode, NAME_KEY, std::string(CStringA(boneNode->m_name)));
	Vec3f newSize = boneNode->meshRightUp - boneNode->meshLeftDown;
	doc->addVectorDatafToNode(pNode, BONE_SIZE_KEY, newSize);
	doc->addVectorDatafToNode(pNode, JOINT_PARENT_KEY, boneNode->m_jointBegin + boneNode->localTranslatef);
	doc->addVectorDatafToNode(pNode, JOINT_CHILD_KEY, boneNode->m_posCoord + boneNode->localTranslatef);
	doc->addElementToNode(pNode, BONE_TYPE_KEY, boneNode->getTypeString());
	doc->addVectorDatafToNode(pNode, ROTATION_ANGLE_KEY, boneNode->m_angle);

	// Now children
	if (boneNode->child.size() > 0)
	{
		myXMLNode * childNode = doc->addNodeToNode(node, CHILD_KEY);
		for (int i = 0; i < boneNode->child.size(); i++)
		{
			myXMLNode * newChild = doc->addNodeToNode(childNode, BONE_KEY);
			writeBoneToXMLWithModify(doc, newChild, boneNode->child[i]);
		}
	}
}

void bone::draw(int mode, float scale)
{
	if (mode & SKE_DRAW_BOX_WIRE)
	{
		Util_w::drawBoxWireFrame(leftDownf, rightUpf);
	//	drawCoord();
	}

	if (mode & SKE_DRAW_BOX_SOLID)
	{
		Util_w::drawBoxSurface(leftDownf, rightUpf);
	}

	if (mode & SKE_DRAW_MESH)
	{
		drawMesh();
	}

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
}

bone::bone()
{
	indexOfMeshBox = -1;
	bIsGroup = false;

	static int idx = 0;
	colorIndex = idx;
	idx++;
}

BOOL bone::isLarger(bone* a)
{
	if (a->m_type == CENTER_BONE && a->m_type == SIDE_BONE)
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

	Util_w::drawBoxWireFrame(meshLeftDown, meshRightUp);
	drawUtil::drawPolygonFacePtr(mesh);

//	drawCoord();

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


void bone::getMeshFromOriginBox(Vec3f leftDown, Vec3f rightUp)
{

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

void bone::scale(bone* origin, float ratio)
{
	m_jointBegin = origin->m_jointBegin*ratio;
	m_posCoord = origin->m_posCoord*ratio;
	m_sizef = origin->m_sizef*ratio;

	initOther();
}
