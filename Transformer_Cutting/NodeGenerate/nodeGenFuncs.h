#pragma once
//#include "stdafx.h"
#include "Graphics/Surfaceobj.h"
#include <memory>

#undef X Y Z
#define X 0
#define Y 1
#define Z 2

#define element(table, pt) table[pt[0]][pt[1]][pt[2]]

class nodeGeneration
{
public:
	nodeGeneration();
	~nodeGeneration(){};
// 
// 		// Visualization
// 		/*Draw surface
// 		mode = 1 - solid surface; 2 - Wireframe; 3 - Wireframe and solid surface
// 		*/
	void drawSurface(int mode = 1);
 	void drawNode(float radius=0);

	// Load surface information from STL
	// Note: Require correct normal vector of triangle
	BOOL loadSurfaceSTL(char *filePath);
	BOOL loadSurfaceSTLNoAABB(char *filePath);

	void discretizeFloatPoints();
	void voxelFillObject(float voxelSize);
	void lineFillObject();
	void floodFillOutside(Vec3i pt);

	// Generate uniform nodes
	BOOL generateUniformNode(int res);
	void drawBVH();
	void addSurfaceIntersectionVoxel();

	// Save point info
	void savePoint(char* filePath);
	void drawDebug(int yIdx, int zIdx);
	bool voxelAndTriIntersect(std::vector<Vec3f> * points, Vec3i cTri, Vec3i tri, float voxelSize);
public:
	// Surface object
	std::shared_ptr<SurfaceObj> m_surObj;

	// Node position
	std::vector<Vec3f> m_nodePos;
	float m_voxelSize;

	// Our new
	int getTableAt(int i, int j, int k);
	void setTableAt(int i, int j, int k, int val);
	
	int *** vTable;
	Vec3i leftDowni;
	Vec3i rightUpi;
	Vec3i sizei;
};

typedef std::shared_ptr<nodeGeneration> nodeGenPtr;
