#include "StdAfx.h"
#include "drawUtil.h"

#include "geom_draw.hpp"
#include "GL\GLU.h"
#include "mirrorDraw.h"

typedef void(__stdcall *GLUTessCallback)();
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
	gluTessCallback(tess, GLU_TESS_END_DATA, (GLUTessCallback)_faceEnd);

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

void drawUtil::drawPolygonFace(Polyhedron *p)
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

void drawUtil::drawPolygonFacePtr(PolyhedronPtr p)
{
	drawPolygonFace(p.get());
}
