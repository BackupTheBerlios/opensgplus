/*---------------------------------------------------------------------------*\
 *                                OpenSG                                     *
 *                                                                           *
 *                                                                           *
 *             Copyright (C) 2000,2001 by the OpenSG Forum                   *
 *                                                                           *
 *                            www.opensg.org                                 *
 *                                                                           *
 *   contact: dirk@opensg.org, gerrit.voss@vossg.org, jbehr@zgdv.de          *
 *                                                                           *
\*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*\
 *                                License                                    *
 *                                                                           *
 * This library is free software; you can redistribute it and/or modify it   *
 * under the terms of the GNU Library General Public License as published    *
 * by the Free Software Foundation, version 2.                               *
 *                                                                           *
 * This library is distributed in the hope that it will be useful, but       *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 * Library General Public License for more details.                          *
 *                                                                           *
 * You should have received a copy of the GNU Library General Public         *
 * License along with this library; if not, write to the Free Software       *
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 *
 *                                                                           *
\*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*\
 *                                Changes                                    *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
\*---------------------------------------------------------------------------*/

#ifndef OSGMESHTYPE_H
#define OSGMESHTYPE_H

#include <OpenMesh/Core/Utils/GenProg.hh>
#include "OSGConfig.h"
#include "OSGSubSurfaceDef.h"
#include "OSGVector.h"

// forward declaration for OpenMesh
namespace OpenMesh {

template <class Kernel> class TriMeshT;
template <class Kernel> class PolyMeshT;

}

OSG_BEGIN_NAMESPACE

// mesh type
enum MeshType { TRIANGLE, QUAD }; 


// Derive vector type from the point type of MESH
#define MESH2WSDVECTOR \
  VectorInterface<Real32, typename OpenMesh::GenProg::IF<MESH::Point::size_ == 3, VecStorage3<Real32>, VecStorage4<Real32> >::Result>
// Derive mesh type (TRIANGLE, QUAD) from MESH
template <class MESH>
struct MESH2MTYPE
{ 
   enum { MType = TRIANGLE };
};
// partial specializations
template <class MeshKernel>
struct MESH2MTYPE<OpenMesh::TriMeshT<MeshKernel> >
{ 
   enum { MType = TRIANGLE };
};
template <class MeshKernel>
struct MESH2MTYPE<OpenMesh::PolyMeshT<MeshKernel> >
{ 
   enum { MType = QUAD };
};

/*
#define MESH2MTYPE \
  OpenMesh::GenProg::IF<MESH::Mesh == OpenMesh::TriMeshT<MESH::MeshKernel>, OpenMesh::GenProg::Int2Type<TRIANGLE>, OpenMesh::GenProg::Int2Type<QUAD> >
*/

OSG_END_NAMESPACE

#endif
