#pragma once
#include "stdafx.h"
#include "Graphics\Surfaceobj.h"
#include <carve/carve.hpp>
#include <carve/poly.hpp>
#include <carve/polyline.hpp>
#include <carve/pointset.hpp>


typedef carve::poly::Polyhedron Polyhedron;
typedef carve::geom3d::Vector cVector;
typedef carve::poly::Vertex<3> cVertex;
typedef carve::poly::Face<3> cFace;

extern class voxelObject;
extern class bvhVoxel;
extern class bone;

class MeshCutting
{
public:
	MeshCutting(void);
	~MeshCutting(void);

	void init();
	void init2(std::vector<arrayInt> meshIdx, std::vector<bone*> boneArray_i);
	void initFromMesh(SurfaceObj *mesh);


	void cutTheMesh();
	void CopyMeshToBone();
	void updateScale(float scaleR);

	Polyhedron* boxCut(Vec3f leftDown, Vec3f rightUp);
	
	void drawPolygon(Polyhedron * p);
	void drawPolygonFace(Polyhedron *p);
	void drawPolygonEdge(Polyhedron * p);
	void draw(BOOL displayMode[10]);

	carve::poly::Polyhedron * makeCube(double minX, double minY, double minZ, double maxX, double maxY, double maxZ);
	void triangleBox(Vec3f leftDown, Vec3f rightUp, std::vector<Vec3f> &points_, std::vector<Vec3i> &faces_);
	carve::poly::Polyhedron * makeCubeTriangular(double minX, double minY, double minZ, double maxX, double maxY, double maxZ);
	SurfaceObj* triangulatePolygon(Polyhedron * testResult);
	arrayVec3f getMeshCoordOrigin();
private:
	Polyhedron * convertTriangularToPolygonMesh(arrayVec3f * pts, arrayVec3i * faces);
	void transformMesh();
	Vec3f getCenterBox(arrayInt voxelIdxs);
	


public:
	Polyhedron *m_polyHedron;
	std::vector<Polyhedron*> m_cutPieces;
	std::vector<Polyhedron*> m_cutSurface;
	std::vector<Polyhedron*> s_meshToBone;
public:	
	SurfaceObj * s_surObj;
	voxelObject * s_voxelObj;

	arrayVec3i coords;
	std::vector<bvhVoxel*> * s_meshBox;
	std::vector<bone*> boneArray;
	std::vector<arrayInt> meshVoxelIdxs;
};

