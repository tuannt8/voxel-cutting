// Begin License:
// Copyright (C) 2006-2008 Tobias Sargeant (tobias.sargeant@gmail.com).
// All rights reserved.
//
// This file is part of the Carve CSG Library (http://carve-csg.com/)
//
// This file may be used under the terms of the GNU General Public
// License version 2.0 as published by the Free Software Foundation
// and appearing in the file LICENSE.GPL2 included in the packaging of
// this file.
//
// This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
// INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE.
// End:


#if defined(HAVE_CONFIG_H)
#  include <carve_config.h>
#endif

#include <algorithm>

#include <carve/carve.hpp>
#include <carve/poly.hpp>
#include <carve/timing.hpp>
#include <carve/intersection.hpp>



void carve::csg::Intersections::collect(const IObj &obj,
                                        std::vector<const carve::poly::Polyhedron::vertex_t *> *collect_v,
                                        std::vector<const carve::poly::Polyhedron::edge_t *> *collect_e,
                                        std::vector<const carve::poly::Polyhedron::face_t *> *collect_f) const {
  carve::csg::Intersections::const_iterator i = find(obj);
  if (i != end()) {
    Intersections::mapped_type::const_iterator a, b;
    for (a = (*i).second.begin(), b = (*i).second.end(); a != b; ++a) {
      switch ((*a).first.obtype) {
      case carve::csg::IObj::OBTYPE_VERTEX:
        if (collect_v) collect_v->push_back((*a).first.vertex);
        break;
      case carve::csg::IObj::OBTYPE_EDGE:
        if (collect_e) collect_e->push_back((*a).first.edge);
        break;
      case carve::csg::IObj::OBTYPE_FACE:
        if (collect_f) collect_f->push_back((*a).first.face);
        break;
      default:
        throw carve::exception("should not happen " __FILE__ ":" XSTR(__LINE__));
      }
    }
  }
}



bool carve::csg::Intersections::intersectsFace(const carve::poly::Polyhedron::vertex_t *v, const carve::poly::Polyhedron::face_t *f) const {
  const_iterator i = find(v);
  if (i != end()) {
    mapped_type::const_iterator a, b;

    for (a = (*i).second.begin(), b = (*i).second.end(); a != b; ++a) {
      switch ((*a).first.obtype) {
      case IObj::OBTYPE_VERTEX: {
        for (size_t j = 0; j < f->nVertices(); ++j) if (f->vertex(j) == (*a).first.vertex) return true;
        break;
      }
      case carve::csg::IObj::OBTYPE_EDGE: {
        for (size_t j = 0; j < f->nEdges(); ++j) if (f->edge(j) == (*a).first.edge) return true;
        break;
      }
      case carve::csg::IObj::OBTYPE_FACE: {
        if ((*a).first.face == f) return true;
        break;
      }
      default:
        throw carve::exception("should not happen " __FILE__ ":" XSTR(__LINE__));
      }
    }
  }
  return false;
}
