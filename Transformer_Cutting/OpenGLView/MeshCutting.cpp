#include "StdAfx.h"
#include "MeshCutting.h"
#include "Utility_wrap.h"
#include "Modules\SurfaceCuttingManager.h"

#include "geom_draw.hpp"
#include "carve\csg.hpp"

#include "voxelObject.h"
#include "skeleton.h"
#include "bvhVoxel.h"
#include "voxelSurfaceContruction.h"
#include "tranformCoord.h"
#include "log.h"


MeshCutting::MeshCutting(void)
{
	m_polyHedron = nullptr;
}


MeshCutting::~MeshCutting(void)
{
	if (m_polyHedron)
	{
		delete m_polyHedron;
	}

	for (int i = 0; i < m_cutSurface.size(); i++)
	{
		delete m_cutSurface[i];
	}
	m_cutSurface.clear();
// 
// 	for (int i = 0; i < m_cutPieces.size(); i++)
// 	{
// 		delete m_cutPieces[i];
// 	}
// 	m_cutPieces.clear();
}


Polyhedron* MeshCutting::boxCut(Vec3f leftDown, Vec3f rightUp)
{
	Polyhedron* boxP = makeCube(leftDown[0], leftDown[1], leftDown[2], rightUp[0], rightUp[1], rightUp[2]);
	carve::csg::CSG testt;
	Polyhedron* resultP = testt.compute(m_polyHedron, boxP, carve::csg::CSG::INTERSECTION);

	delete boxP;
	return resultP;
}

_inline Vec3f Vec3fL(Vec3f a, Vec3f b, Vec3f c){
	return Vec3f(a[0], b[1], c[2]);
}

void MeshCutting::triangleBox(Vec3f leftDown, Vec3f rightUp, std::vector<Vec3f> &_points, std::vector<Vec3i> & _faces)
{
	Vec3f U = rightUp, o = leftDown;

	_points.clear(); _faces.clear();
	_points.push_back(Vec3fL(o,o,o));
	_points.push_back(Vec3fL(o,U,o));
	_points.push_back(Vec3fL(U,U,o));
	_points.push_back(Vec3fL(U,o,o));
	_points.push_back(Vec3fL(o,o,U));
	_points.push_back(Vec3fL(o,U,U));
	_points.push_back(Vec3fL(U,U,U));
	_points.push_back(Vec3fL(U,o,U));

	_faces.push_back(Vec3i(0,1,2));
	_faces.push_back(Vec3i(0,2,3));
	_faces.push_back(Vec3i(3,7,4));
	_faces.push_back(Vec3i(3,4,0));
	_faces.push_back(Vec3i(3,6,7));
	_faces.push_back(Vec3i(3,2,6));
	_faces.push_back(Vec3i(6,2,5));
	_faces.push_back(Vec3i(5,2,1));
	_faces.push_back(Vec3i(4,5,0));
	_faces.push_back(Vec3i(5,1,0));
	_faces.push_back(Vec3i(4,6,5));
	_faces.push_back(Vec3i(4,7,6));
}

carve::poly::Polyhedron * MeshCutting::makeCubeTriangular(double minX, double minY, double minZ, double maxX, double maxY, double maxZ)
{
	arrayVec3f pt;
	arrayVec3i f;
	triangleBox(Vec3f(minX, minY, minY), Vec3f(maxX, maxY, maxZ), pt, f);

	std::vector<carve::geom3d::Vector> vertices;
	for (int i = 0; i < pt.size(); i++)
	{
		vertices.push_back(carve::geom::VECTOR(pt[i][0], pt[i][1], pt[i][2]));
	}

	std::vector<int> fpoly;
	int numfaces = f.size();
	for (int i = 0; i < numfaces; i++)
	{
		fpoly.push_back(3);
		fpoly.push_back(f[i][0]);
		fpoly.push_back(f[i][1]);
		fpoly.push_back(f[i][2]);
	}

	return new Polyhedron(vertices, numfaces, fpoly);
}

carve::poly::Polyhedron * MeshCutting::makeCube(double minX, double minY, double minZ, double maxX, double maxY, double maxZ)
{
	std::vector<carve::geom3d::Vector> vertices;

	vertices.push_back(carve::geom::VECTOR(maxX, maxY, maxZ));
	vertices.push_back(carve::geom::VECTOR(minX, maxY, maxZ));
	vertices.push_back(carve::geom::VECTOR(minX, minY, maxZ));
	vertices.push_back(carve::geom::VECTOR(maxX, minY, maxZ));
	vertices.push_back(carve::geom::VECTOR(maxX, maxY, minZ));
	vertices.push_back(carve::geom::VECTOR(minX, maxY, minZ));
	vertices.push_back(carve::geom::VECTOR(minX, minY, minZ));
	vertices.push_back(carve::geom::VECTOR(maxX, minY, minZ));

	std::vector<int> f;
	int numfaces = 6;

	f.push_back(4); // Vertex count
	f.push_back(0);
	f.push_back(1);
	f.push_back(2);
	f.push_back(3);

	f.push_back(4);
	f.push_back(7);
	f.push_back(6);
	f.push_back(5);
	f.push_back(4);

	f.push_back(4);
	f.push_back(0);
	f.push_back(4);
	f.push_back(5);
	f.push_back(1);

	f.push_back(4);
	f.push_back(1);
	f.push_back(5);
	f.push_back(6);
	f.push_back(2);

	f.push_back(4);
	f.push_back(2);
	f.push_back(6);
	f.push_back(7);
	f.push_back(3);

	f.push_back(4);
	f.push_back(3);
	f.push_back(7);
	f.push_back(4);
	f.push_back(0);

	return new carve::poly::Polyhedron(vertices, numfaces, f);
}

void MeshCutting::draw(BOOL displayMode[10])
{
	static arrayVec3f color = Util_w::randColor(30);
	if (displayMode[5])
	{
		if (m_polyHedron)
		{
			drawPolygon(m_polyHedron);
		}
	}

	if (displayMode[6])
	{
		for (int i = 0; i < m_cutSurface.size(); i++)
		{
			glColor3fv(color[i + 1].data());
			drawPolygonEdge(m_cutSurface[i]);

			glColor3fv(color[i].data());
			drawPolygonFace(m_cutSurface[i]);

		}
	}

	if (displayMode[7])
	{
		for (int i = 0; i < m_cutPieces.size(); i++)
		{
			glColor3fv(color[i].data());
			drawPolygonFace(m_cutPieces[i]);

		}
	}
}

void MeshCutting::init()
{
	std::vector<Vec3f> *meshPt = s_surObj->point();
	std::vector<Vec3i> *faces = s_surObj->face();
	m_polyHedron = convertTriangularToPolygonMesh(meshPt, faces);

	// Update bone index, if need

	// Construct cut surface
	for (int i = 0; i < s_meshBox->size(); i++)
	{
		arrayInt vIdxs = s_meshBox->at(i)->voxelIdxs;
		arrayVec3f pts;
		arrayVec3i tris;
		voxelSurfaceContruction vC;
		vC.getSurface(s_voxelObj, vIdxs, pts, tris);
		Polyhedron * cutf = convertTriangularToPolygonMesh(&pts, &tris);
		m_cutSurface.push_back(cutf);
	}
}

void MeshCutting::initFromMesh(SurfaceObj *mesh)
{

}

typedef void (__stdcall *GLUTessCallback)();
void __stdcall _faceBegin(GLenum type, void *data) {
	carve::poly::Face<3> *face = static_cast<carve::poly::Face<3> *>(data);
	glBegin(type);
	glNormal3f(face->plane_eqn.N.x, face->plane_eqn.N.y, face->plane_eqn.N.z);
}

void __stdcall _faceVertex(void *vertex_data, void *data) {
 	std::pair<carve::geom3d::Vector, cRGBA> &vd(*static_cast<std::pair<carve::geom3d::Vector, cRGBA> *>(vertex_data));
// 	glColor4f(vd.second.r, vd.second.g, vd.second.b, vd.second.a);
	glVertex3f(vd.first.x, vd.first.y, vd.first.z);
}

void __stdcall _faceEnd(void *data) {
	glEnd();
}

void drawFace(carve::poly::Face<3> *face, cRGBA fc, bool offset) {

	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	GLUtesselator *tess = gluNewTess();

	gluTessCallback(tess, GLU_TESS_BEGIN_DATA, (GLUTessCallback)_faceBegin);
	gluTessCallback(tess, GLU_TESS_VERTEX_DATA, (GLUTessCallback)_faceVertex);
	gluTessCallback(tess,  GLU_TESS_END_DATA, (GLUTessCallback)_faceEnd);

	gluTessBeginPolygon(tess, (void *)face);
	gluTessBeginContour(tess);

	carve::geom3d::Vector g_translation;
	double g_scale = 1.0;

	std::vector<std::pair<carve::geom3d::Vector, cRGBA> > v;
	v.resize(face->nVertices());
	for (size_t i = 0, l = face->nVertices(); i != l; ++i) {
		v[i] = std::make_pair(g_scale * (face->vertex(i)->v + g_translation), fc);
		gluTessVertex(tess, (GLdouble *)v[i].first.v, (GLvoid *)&v[i]);
	}

	gluTessEndContour(tess);
	gluTessEndPolygon(tess);

	gluDeleteTess(tess);

}

void MeshCutting::drawPolygon(Polyhedron * p)
{
//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_TRIANGLES);

	glColor4f(0.3, 0.6, 0.7, 0.8);

	for (size_t i = 0, l = p->faces.size(); i != l; ++i) {
		carve::poly::Face<3> &f = p->faces[i];
		if (f.nVertices() == 3) {
			glNormal3dv(f.plane_eqn.N.v);
			glVertex3f(f.vertex(0)->v[0], f.vertex(0)->v[1], f.vertex(0)->v[2]);
			glVertex3f(f.vertex(1)->v[0], f.vertex(1)->v[1], f.vertex(1)->v[2]);
			glVertex3f(f.vertex(2)->v[0], f.vertex(2)->v[1], f.vertex(2)->v[2]);
		}
	}
	glEnd();

	glColor3f(0,0,1);
	glBegin(GL_LINES);
	for (size_t i = 0, l = p->faces.size(); i != l; ++i) {
		carve::poly::Face<3> &f = p->faces[i];
		for (int j = 0; j < f.nVertices()-1; j++)
		{
			glVertex3f(f.vertex(j)->v[0], f.vertex(j)->v[1], f.vertex(j)->v[2]);
			glVertex3f(f.vertex(j+1)->v[0], f.vertex(j+1)->v[1], f.vertex(j+1)->v[2]);
		}
	}
	glEnd();

	for (size_t i = 0, l = p->faces.size(); i != l; ++i) {
		carve::poly::Face<3> &f = p->faces[i];

		if (f.nVertices() != 3) {
			drawFace(&p->faces[i], cRGBA(0.3, 0.6, 0.7, 1.0), false);
		}
	}


}

SurfaceObj* MeshCutting::triangulatePolygon(Polyhedron * p)
{
	SurfaceObj* obj = new SurfaceObj();
	return obj;
}

Polyhedron * MeshCutting::convertTriangularToPolygonMesh(arrayVec3f * pts, arrayVec3i * faces)
{
	std::vector<cVector> points;
	int faceNum = faces->size();
	arrayInt f;

	for (int i = 0; i < pts->size(); i++)
	{
		Vec3f pt = pts->at(i);
		points.push_back(carve::geom::VECTOR(pt[0], pt[1], pt[2]));
	}

	for (int i = 0; i < faceNum; i++)
	{
		f.push_back(3);
		f.push_back((*faces)[i][0]);
		f.push_back((*faces)[i][1]);
		f.push_back((*faces)[i][2]);
	}

	return new Polyhedron(points, faceNum, f);
}

void MeshCutting::init2(std::vector<arrayInt> meshIdx, std::vector<bone*> boneArray_i)
{
	boneArray = boneArray_i;
	meshVoxelIdxs = meshIdx;
	//
	std::vector<Vec3f> *meshPt = s_surObj->point();
	std::vector<Vec3i> *faces = s_surObj->face();
	m_polyHedron = convertTriangularToPolygonMesh(meshPt, faces);

	// Update bone index, if need
	std::vector<voxelBox> *s_boxes = &s_voxelObj->m_boxes;
	for (int i = 0; i < meshIdx.size(); i++)
	{
		arrayInt idxs = meshIdx[i];
		for (int j = 0; j < idxs.size(); j++)
		{
			s_boxes->at(idxs[j]).boneIndex = i;
		}
	}

	// Construct cut surface
	CTimeTick time;
	time.SetStart();
	for (int i = 0; i < meshIdx.size(); i++)
	{
		arrayInt vIdxs = meshIdx[i];
		arrayVec3f pts;
		arrayVec3i tris;
		voxelSurfaceContruction vC;
		vC.getSurface(s_voxelObj, vIdxs, pts, tris);
		vC.shrinkPoint(s_voxelObj, pts, 0.2);
		Polyhedron * cutf = convertTriangularToPolygonMesh(&pts, &tris);
		m_cutSurface.push_back(cutf);
	}
	time.SetEnd();
	command::print("Mesh generation time: %f", time.GetTick());
}

void MeshCutting::drawPolygonFace(Polyhedron *p)
{
	glBegin(GL_TRIANGLES);
	for (size_t i = 0, l = p->faces.size(); i != l; ++i) {
		carve::poly::Face<3> &f = p->faces[i];
		if (f.nVertices() == 3) {
			glNormal3dv(f.plane_eqn.N.v);
			glVertex3f(f.vertex(0)->v[0], f.vertex(0)->v[1], f.vertex(0)->v[2]);
			glVertex3f(f.vertex(1)->v[0], f.vertex(1)->v[1], f.vertex(1)->v[2]);
			glVertex3f(f.vertex(2)->v[0], f.vertex(2)->v[1], f.vertex(2)->v[2]);
		}
	}
	glEnd();

	for (size_t i = 0, l = p->faces.size(); i != l; ++i) {
		carve::poly::Face<3> &f = p->faces[i];

		if (f.nVertices() != 3) {
			drawFace(&p->faces[i], cRGBA(0.3, 0.6, 0.7, 1.0), false);
		}
	}
}

void MeshCutting::drawPolygonEdge(Polyhedron * p)
{
	glBegin(GL_LINES);
	for (size_t i = 0, l = p->faces.size(); i != l; ++i) {
		carve::poly::Face<3> &f = p->faces[i];
		for (int j = 0; j < f.nVertices() - 1; j++)
		{
			glVertex3f(f.vertex(j)->v[0], f.vertex(j)->v[1], f.vertex(j)->v[2]);
			glVertex3f(f.vertex(j + 1)->v[0], f.vertex(j + 1)->v[1], f.vertex(j + 1)->v[2]);
		}
	}
	glEnd();
}

void MeshCutting::cutTheMesh()
{
	CTimeTick time;
	time.SetStart();
	for (int i = 0; i < m_cutSurface.size(); i++)
	{
		carve::csg::CSG testt;
		Polyhedron* resultP = testt.compute(m_polyHedron, m_cutSurface[i], carve::csg::CSG::INTERSECTION);
		m_cutPieces.push_back(resultP);
	}
	time.SetEnd();
	command::print("Cut mesh time: %f", time.GetTick());
}

void MeshCutting::transformMesh()
{
	// Including translation and rotation
	for (int i = 0; i < m_cutPieces.size(); i++)
	{
		Polyhedron* curP = m_cutPieces[i];

		tranformCoord tc;
		tc.m_coord = coords[i];
		tc.m_tranf = getCenterBox(meshVoxelIdxs[i]);

		std::vector<cVertex> * vertices = &curP->vertices;
		for (int j = 0; j < vertices->size(); j++)
		{
			cVertex curV = vertices->at(j);
			Vec3f curP(curV.v[0], curV.v[1], curV.v[2]);
			Vec3f tP = tc.tranfrom(curP);

			vertices->at(j) = carve::geom::VECTOR(tP[0], tP[1], tP[2]);
		}
	}
}
Box combineBox(Box box1, Box box2)
{
	Box newBox;
	for (int i = 0; i < 3; i++)
	{
		newBox.leftDown[i] = Util::min_(box1.leftDown[i], box2.leftDown[i]);
		newBox.rightUp[i] = Util::max_(box1.rightUp[i], box2.rightUp[i]);
	}

	return newBox;
}

Vec3f MeshCutting::getCenterBox(arrayInt voxelIdxs)
{
	std::vector<voxelBox> * boxes = &s_voxelObj->m_boxes;
	// Get current bounding box
	Vec3f LD(MAX, MAX, MAX);
	Vec3f RU(MIN, MIN, MIN);
	Box b(LD, RU);

	for (int i = 0; i < voxelIdxs.size(); i++)
	{
		voxelBox curB = boxes->at(voxelIdxs[i]);
		b = combineBox(b, Box(curB.leftDown, curB.rightUp));
	}

	return (b.leftDown + b.rightUp)/2.0;
}

void MeshCutting::CopyMeshToBone()
{
	for (int i = 0; i < m_cutPieces.size(); i++)
	{
		boneArray[i]->mesh = m_cutPieces[i];
	}
}

void MeshCutting::updateScale(float scaleR)
{
	
}
