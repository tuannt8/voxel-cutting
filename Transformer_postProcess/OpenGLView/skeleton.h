#pragma once
#include "stdafx.h"
#include "DataTypes\vec.h"
#include "drawUtil.h"
#include "myXML.h"

#include <memory>


#define MAX_CHILD 10

enum
{
	SKE_DRAW_LINE = 1,
	SKE_DRAW_BOX_WIRE = 2,
	SKE_DRAW_BOX_SOLID = 4,
	SKE_DRAW_MESH = 8,
};

enum neighhorType
{
	NEIGHBOR_SERIAL,
	NEIGHBOR_SERIAL_MIRROR,
	NEIGHBOR_SIDE
};

#define	CENTER_BONE 0
#define SIDE_BONE 1


class bone
{
public:
	bone();
	~bone();

	void draw(int mode, float scale = 1.0);
	void drawCoord();
	void drawMesh(float scale = 1.0);
	void initOther();

	BOOL isLarger(bone* a);
	void getMeshFromOriginBox(Vec3f LeftDown, Vec3f rightUp);

	bool isLeaf();
	float getVolumef();
	float& volumeRatio();

	char* getTypeString();

	void scale(bone* origin, float ratio);
public:
	// Tree hierarchy
	bone* parent;
	neighhorType neighborType;
	std::vector<bone*> child;

	// Coordinate information relative to parent
	Vec3f m_jointBegin;	// The beginning of the joint
	Vec3f m_posCoord; // Original coordinate relative to parent
						// This is also end of the joint 
	Vec3f m_angle;	// Rotation angle by z-x-z sequence (Euler), degree

	// Bone information
	Vec3f m_sizef;
	CString m_name;
	int m_type;

	// Temporary var
	float m_volumef;
	Vec3f leftDownf, rightUpf;
	float m_volumeRatio;

	// For group bone algorithm
	bool bIsGroup;
	float m_groupVolumef;
	float m_groupVolumeRatio;


	// mesh information
	PolyhedronPtr mesh;
	Vec3f meshLeftDown;
	Vec3f meshRightUp;
	Vec3i meshRot; // z-x-z 90 rotation, Euler
	Vec3f localTranslatef;

	int colorIndex;
public:
	// for neighbor check; temp var
	int indexOfMeshBox;
	int nbCenterNeighbor();
	int nbSideNeighbor();
	int nbNeighbor() const;
	void setBoneType(std::string typeString);
};

typedef std::vector<bone*> arrayBone_p;
typedef std::vector<bone> arrayBone;

class skeleton
{
public:
	skeleton(void);
	~skeleton(void);

	void loadFromFile(const char *filePath);
	void writeToFile(const char* filePath);
	void writeToFileWithModification(const char* filePath);

	void draw(int mode=SKE_DRAW_BOX_WIRE);
	void drawBoneRecursive(bone* node, int mode);
	
	void initTest(); // Manually for testing
	void computeTempVar();

	// For group bone algorithm
	void groupBone();
	void getBoneGroupAndNeighborInfo(std::vector<bone*> &sorted, std::vector<std::pair<int, int>> &neighborPair);
	void getSortedGroupBoneArray(std::vector<bone*> &sortedArray);

	void getBoneAndNeighborInfo(std::vector<bone*> &boneArray, std::vector<std::pair<int,int>> &neighborA);

	void getSortedBoneArray(std::vector<bone*> &sortedArray);

	void getGroupBone(bone* node, std::vector<bone*> &groupBone); // Get root of group bone
	void getBoneInGroup(bone* node, std::vector<bone*> &boneInGroup); // Get all bone in this group
	void getNeighborPair(bone* node, std::vector<Vec2i> &neighbor, std::vector<bone*> boneArray);
private:
	void getSortedBoneArrayRecur(bone* node, std::vector<bone*> &sortedArray);
	void getBoneAndNeighborInfoRecur(bone* node, int parentIdx, std::vector<bone*> &boneArray, std::vector<std::pair<int,int>> &neighborA);

private:
	// For group bone algorithm
	void groupChildren(bone* node);
	float volumeOfGroupBone(bone* node);
	float volumeRatioOfGroupBone(bone* node);
	void getBoneGroupAndNeighborInfoRecur(bone* node, int parentIdx, std::vector<bone*> & boneArray, std::vector<std::pair<int, int>> & neighborA);
	void getSortedBoneGroupArrayRecur(bone* node, std::vector<bone*> & sortedArray);
	void writeBoneToXML(myXML * doc, myXMLNode * node, bone* boneNode);
	void writeBoneToXMLWithModify(myXML * doc, myXMLNode * node, bone* boneNode);
	void loadBoneData(myXML * doc, myXMLNode * xmlNode, bone* boneNode);
public:
	bone* m_root;
	float meshScale;
	int colorIndex;
	Vec3f scale;
};

typedef std::shared_ptr<skeleton> skeletonPtr;
