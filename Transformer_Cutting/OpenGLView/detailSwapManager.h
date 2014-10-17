#pragma once
#include "stdafx.h"
#include "DataTypes\vec.h"
#include <map>
#include "skeleton.h"
#include "boneAbstract.h"
#include "octreeSolid.h"
#include "bvhVoxel.h"
#include "cutSurfTreeMngr2.h"
#include "boneTransform.h"
//#include "coordAssignManager.h"

extern class coordAssignManager;

enum errorCompareFlag
{
	COMPARE_VOLUME = 0x0001,
	COMPARE_ASPECT = 0x0010,
	COMPARE_FILL = 0x0100,
};

class detailSwapManager
{
public:
	detailSwapManager(void);
	~detailSwapManager(void);

	void draw();
	void draw2(BOOL displayMode[10]);
	void updateDisplay(int idx1, int idx2);

	// 1. Load
	void initTest();
	void getInfoFromCutTree(cutSurfTreeMngr2* testCut);
	void initFromCutTree2(cutSurfTreeMngr2* m_cutSurface); // Quick swap
	void initFromAssignCoord(coordAssignManager * m_coordAssign);

	void swapVoxel2();
	void swapVoxelTotal();

	// 1.1. Mesh box
	void loadMeshBox(char *filePath);
	void loadMeshBoxFromCutTree(cutSurfTreeMngr2* testCut);
	void loadMeshBoxFromCutTreeWithPose(cutSurfTreeMngr2* testCut);
	void loadMeshBoxFromCutTreeWithPose2(cutSurfTreeMngr2* testCut);
	void loadMeshBoxFromCoordAssign(coordAssignManager * coordAssign);

	// 1.2. Construct hash table of all voxel
	void constructVolxeHash();
	void constructVolxeHashFromCutTree(cutSurfTreeMngr2* testCut);
	void constructVolxeHashFromCutTree2(cutSurfTreeMngr2* testCut = nullptr);

	// 1.3. BVH of voxel of object
	// But it is actually low performance. Does not use in new code
	void constructBVHOfMeshBoxes();
	void constructBVHOfMeshBoxesFromCutTree(cutSurfTreeMngr2* testCut);
	void constructBVHOfMeshBoxesFromCutTree2(cutSurfTreeMngr2* testCut);
	void constructBVHOfMeshBoxesAndHashVoxel();
	// 
	void computeOtherForCutBoxCase();

	// Test
	// Group of bone that does not have aspect ratio
	void manualAssignBoneGroup();

	// 1.4. Assign coordinate manually by user
	void manuallySetupCoord();
	void computeAspectRatio();

	// 2. swap one voxel
	// 2.1
	void swapPossibleLayer();
	void swapOneVoxel();

	// 2.1.a With group bone, which does not have aspect ratio 
	void swapPossibleLayer2();
	bool swapPossibleLayerWithVolume(); // Only consider volume ratio
	bool swapPossibleLayerWithVolumeAndAspect();

	void cutCenterMeshByHalf();
	void restoreCenterMesh();
	void growSwap();

	//2.2
	void swapOneBestVoxel();
	void swapOnePossibleVoxel();
	void swapOnePossibleVoxel2();

	//3.1 Save information for group cutting
	void saveMeshBox(char* filePath);
	void initFromSaveFile();
public:
	std::vector<voxelBox> boxes; // all voxel pixel box
	hashVoxel hashTable;

	Vec3f leftDownVoxel, rightUpVoxel;
	float voxelSize;
	Vec3i NumXYZ;

	// Skeleton
	skeleton m_skeleton;
	octreeSolid m_octree;// Octree of the total object

	std::vector<bone*> boneArray; // Sorted by order from cutTree
	std::vector<bvhVoxel*> meshBox; // box to cut the mesh; sorted by correct order of bone
	std::vector<std::vector<int>> meshNeighbor;
	std::vector<Vec3i> coords;// map x-y-z bone on world coord

	// direct neighbor
	std::vector<arrayInt> neighborVoxel;
	int *tempMark;
	int *voxelOccupy;
	int m_idx1, m_idx2;

	// Group bone cut
	SurfaceObj *s_obj;
	skeleton *s_skeleton;
	voxelObject *s_voxelObj;
	octreeSolid *s_octree;
	std::vector<voxelBox>* s_boxes; // all voxel pixel box
	hashVoxel *s_hashTable;
	std::vector<arrayInt>* s_boxShareFaceWithBox;

	bool m_bStopThread;
private:
	void setVoxelArray();

	float errorReduceAssumeWrap(AABBNode* voxelAABB, int boneSourceIdx, int boneDestIdx);
	void wrapVoxel(AABBNode* node, int sourceBoneIdx, int desBoneIdx);
	std::vector<float> getMeshBoxPerimeter();
	float getPerimeterError(std::vector<float> perimeter);
	bool isObjectIfRemove(int boneIndex, int boxIdxToRemove);
	bool isObjectIfRemove(int boneIndex, arrayInt boxIdxsToRemove);

	bool isObjectIfRemoveVoxel(int boneIndex, arrayInt boxIdxsToRemove);
	bool isNeighborMaintainIfRemoveBox(int meshBoxIndex, int ABNodeIdx);
	bool shouldSwap(AABBNode* voxelAABB, int boneSourceIdx, int boneDestIdx);

	bool shouldSwap(std::vector<AABBNode*> voxelAABBs, int boneSourceIdx, int boneDestIdx);
	bool swapBoxLayer(int meshBoxIdx1, int meshBoxIdx2);

	bool swapBoxLayer2(int meshBoxIdx1, int meshBoxIdx2);
	bool swapBoxLayerVolume(int meshBoxIdx1, int meshBoxIdx2);
	bool swapBoxLayerVolumeAndRatio(int meshBoxIdx1, int meshBoxIdx2);

	bool isBoxInContact(Box b1, Box b2);
	void getSwapableVoxel(int xyzDirect, float cPos, int meshBoxIdx1, int meshBoxIdx2, arrayInt &swapableIdx1, arrayInt &swapableIdx2, arrayInt &remainIdx1, arrayInt &remainIdx2);
	void getSwapableVoxel2(int xyzDirect, float cPos, int meshBoxIdx1, int meshBoxIdx2, arrayInt &swapableIdx1, arrayInt &swapableIdx2, arrayInt &remainIdx1, arrayInt &remainIdx2);
	void getTwoOtherIndex(int xyzDirect, int &xyz1, int &xyz2);
	bool swapVoxel(int sourceMeshIdx, int destMeshIdx, arrayInt voxelIdxsInSource, arrayInt voxelRemainInLayer, int xyzDirect);
	bool swapVoxel2(int sourceMeshIdx, int destMeshIdx, arrayInt voxelIdxsInSource, arrayInt voxelRemainInLayer, int xyzDirect);
	bool swapVoxelVolume(int sourceMeshIdx, int destMeshIdx, arrayInt voxelIdxsInSource, arrayInt voxelRemainInLayer, int xyzDirect);
	bool swapVoxelVolumeAndRatio(int sourceMeshIdx, int destMeshIdx, arrayInt voxelIdxsInSource, arrayInt voxelRemainInLayer, int xyzDirect);
	AABBNode* swapNode;

	// We don't need BVH of the mesh piece
public:
	bool shouldSwap(int sourceMeshIdx, int destMeshIdx, arrayInt voxelIdx);
	bool shouldSwapVolume(int sourceMeshIdx, int destMeshIdx, arrayInt voxelIdx);
	bool shouldSwapVolumeProgress(int sourceMeshIdx, int destMeshIdx, arrayInt voxelIdx, arrayFloat &errorSource, arrayFloat &errorDesh);
	bool shouldSwapVolumeAndRatio(int sourceMeshIdx, int destMeshIdx, arrayInt voxelIdx);
	bool shouldSwapVolumeAndRatioProgress(int sourceMeshIdx, int destMeshIdx, arrayInt voxelIdx, arrayFloat &errorSource, arrayFloat &errorDesh);
	bool shouldSwap2(int sourceMeshIdx, int destMeshIdx, arrayInt voxelIdx);
	bool shouldSwap(int sourceMeshIdx, int destMeshIdx, arrayInt voxelIdxRemoveFromSource, arrayInt voxelIdxAddToDest);
	void swapLayer(int sourceMeshIdx, int destMeshIdx, arrayInt voxelIdxsInSource);

	void getTheVoxelWeShouldSwap(int sourceMeshIdx, int destMeshIdx, arrayInt voxelIdxsInSource, arrayInt &sourceIdx, arrayInt &destIdx);

	void swapVoxels(int sourceMeshIdx, int destMeshIdx, arrayInt voxelIdxsInSource, arrayInt voxelAddToDest);

	void getSwapVoxelList(int sourceMeshIdx, int destMeshIdx, arrayInt voxelIdxsInSource, arrayInt &voxelToRemove, arrayInt &voxelToAdd);

	bool swapBetweenBox(int sourceIdx, int destIdx);
	bool swapBetweenBox1by1(int sourceIdx, int destIdx);
	int findPossibleSwapVoxel(int sourceIdx, int destIdx);
	bool canSwap(int sourceIdx, int destIdx, const arrayInt &temp);
	arrayFloat getTotalError(float ws, float wd, arrayFloat & eSource, arrayFloat & eDest);
	float getErrorSum(arrayFloat weight, arrayFloat eOrigin);
	arrayInt getIdxsShoudSwap(int sourceIdx, int destIdx, arrayInt temp1);
	std::vector<arrayInt> getIndependentGroup(std::vector<voxelBox>* boxes, arrayInt idxs);
	bool swap1Voxel(int meshIdx, int sourceIdx);
	float getBenefit(arrayFloat weight, arrayFloat before, arrayFloat after);
	bool isBenefitVolume(int sourceIdx, int destIdx);
	float benefitWithoutVolume(int sourceIdx, int destIdx, arrayInt idxs);
	void initGroupVoxelFromSaveFile(char* filePath);
};

