#pragma once
#include "voxelObject.h"

class voxelSurfaceContruction
{
	typedef std::map<int, int> mapii;
	typedef std::pair<int, int> pairii;

	class pointHash
	{
	public:
		pointHash(){};
		~pointHash(){};

		int getHash(Vec3f pt);
	public:
		Vec3f basePt;
		float sizef;
		Vec3i NumXYZ;
	};
	enum direction
	{
		X_MINUS = 0,
		X_PLUS,
		Y_MINUS,
		Y_PLUS,
		Z_MINUS,
		Z_PLUS,
		DIRECT_TOTAL
	};
public:
	voxelSurfaceContruction();
	~voxelSurfaceContruction();

	void getSurface(voxelObject *s_obj, arrayInt vIdxs, arrayVec3f &point, arrayVec3i &faces);
	void shrinkPoint(voxelObject * s_voxelObj, arrayVec3f& pts, float shrinkRatio);
private:
	Vec3i getNeighborPos(Vec3i posi, direction d);
	arrayVec3f getQuadFace(Vec3f leftDown, Vec3f rightUp, direction d);
	arrayInt getVoxelAroundPoint(Vec3f pt, hashVoxel * hashTable);
	arrayVec3f pointAroundPoint8(Vec3f pt, double dis);
public:
	hashVoxel *s_hashTable;
	std::vector<voxelBox> *s_boxes;
};

