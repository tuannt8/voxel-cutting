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

typedef std::shared_ptr<Polyhedron> PolyhedronPtr;

namespace drawUtil
{ 
	void drawPolygonFace(Polyhedron *p);
	void drawPolygonFacePtr(PolyhedronPtr p);
}