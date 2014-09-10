#pragma once
#include "skeleton.h"
#include "meshMnger.h"

#define XML_ORIGINAL_MESH_KEY "original_mesh_path_name"
#define XML_SKELETON_MESH_KEY "skeleton_mesh_path_name"
#define XML_MESH_PART			"mesh_cut_part"
#define XML_CUT_MESH_NAME		"polygon_name"
#define XML_BONE_NAME			"bone_name"
#define XML_COORD_MAP			"bone_mesh_coordinate_map"

class myDocument
{
public:
	myDocument();
	~myDocument();

public:
	void init();
	void loadData(const std::string folderPath);

	void draw1(BOOL mode[10]);
	void draw2(bool mode[10]);

	void receiveKey(char k);
	void updateParam();

	void update(int yIdx, int zIdx);
private:
	Polyhedron * loadPolyhedron(std::string cutPartPath);
	void drawCoord(Vec3i coords);
	void saveNewBone();
	void convertPolyHedronToMayaObj(Polyhedron *p, const char* path);
	void writeAllFileToMayaFormat();
public:
	skeletonPtr m_skeleton;
	SurfaceObjPtr m_surObj;
	skeletonPtr m_originalSkeleton;

	meshMngerPtr m_meshMnger;

public:	// Parameters
	float boneScale;

public: // temp
	std::string folderPath;
	std::string temp_skeletonPart;
};

typedef std::shared_ptr<myDocument> myDocumentPtr;