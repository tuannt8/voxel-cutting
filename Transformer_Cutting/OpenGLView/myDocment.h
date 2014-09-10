#pragma once
#include "stdafx.h"
#include "voxelObject.h"
#include "Graphics\Surfaceobj.h"
#include "cutSurfTreeMngr2.h"
#include "detailSwapManager.h"
#include "groupCutManager.h"
#include "coordAssignManager.h"
#include "debugInfo.h"
#include "manipulateVoxel.h"

//#include "MeshCutting.h"
#define XML_ORIGINAL_MESH_KEY "original_mesh_path_name"
#define XML_SKELETON_MESH_KEY "skeleton_mesh_path_name"
#define XML_MESH_PART			"mesh_cut_part"
#define XML_CUT_MESH_NAME		"polygon_name"
#define XML_BONE_NAME			"bone_name"
#define XML_COORD_MAP			"bone_mesh_coordinate_map"

typedef enum
{
	MODE_FINDING_CUT_SURFACE,
	MODE_ASSIGN_COORDINATE,
	MODE_SWAP_VOXEL,
	MODE_SPLIT_BONE_GROUP,
	MODE_SWAP_FINAL,
	MODE_FIT_BONE,
	MODE_CUTTING_MESH,
	MODE_NONE,
	MODE_TEST
}appMode;

class myDocment
{
public:
	myDocment();
	~myDocment();

	void draw(BOOL mode[10]);
	void draw2(bool mode[10]);

	void receiveKey(char c);
	void updateIdx(int yIdx, int zIdx);
	void updateRealtime();

private:
	void loadFile(); // Load for cutting
	void loadTestVoxelBitSet();

	void initState();
	void changeState();
	void constructCutTree();
	void changeToWrapMode();
	void changeToCutGroupBone();
	void changeStateBack();
	void changeToCoordAssignMode();
	void getBoneArrayAndMeshBox(std::vector<bone*> &boneFullArray, std::vector<bvhVoxel> &meshBoxFull);
	void changeToSwapFinal();
	void changeToCuttingMeshMode();

	void saveFile();

private:
	void writeMeshBoxStateFinalSwap();
	void startToStateCuttingMesh();
	bool getSavePath(CString &path);
	void writePolygon(Polyhedron* cutPieces, char* path);
	static UINT swapVoxelThread(LPVOID p);
	void loadStateForFinalSwap();
	void loadStateForPostProcess();
	void drawTest(BOOL mode[]);
	void keyPressModeTest(char c);
public:
	// Process
	appMode m_curMode; // Application state
	cutSurfTreeMngr2 m_cutSurface; // kd tree that store configuration

	detailSwapManager *m_swapMngr;
	groupCutManager *m_groupCutMngr;
	coordAssignManager *m_coordAssign;
	detailSwapManager *m_finalSwap;
	manipulateVoxelPtr m_voxelProcess;
	MeshCutting * m_meshCutting;

public:
	// Data
	SurfaceObj *m_surfaceObj;
	voxelObject *m_highResVoxel;
	voxelObject *m_lowResVoxel;

	skeleton *m_skeleton;

	debugInfoPtr m_debug;

public:
	// Debug
	int idx1, idx2;
	int leafIdx;
	int shift;

public:	
	// Test voxel cutting

};

