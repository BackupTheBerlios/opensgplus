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

#ifndef MYMESH_H
#define MYMESH_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/Types/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/Types/TriMesh_ArrayKernelT.hh>
#include "OSGSubSurfaceDef.h"
#include "OSGDynamicSubdivisionCC.h"
#include "OSGDynamicSubdivisionLP.h"

namespace OpenMesh {

   inline OpenMesh::Vec4f operator % (const OpenMesh::Vec4f& a, const OpenMesh::Vec4f& b) {
      return OpenMesh::Vec4f(a[0]*b[1]-a[1]*b[0], 
           a[2]*b[0]-a[0]*b[2],
           a[1]*b[2]-a[2]*b[1],
           0);
   }

}

OSG_BEGIN_NAMESPACE

/*! OpenMesh Traits */
struct MyTraitsPoint3 : public OpenMesh::DefaultTraits
{
   FaceAttributes (OpenMesh::Attributes::Normal);
   VertexAttributes (OpenMesh::Attributes::Normal);
   HalfedgeAttributes (OpenMesh::Attributes::PrevHalfedge);
};
/*! OpenMesh Traits */
struct MyTraitsPoint4 : public OpenMesh::DefaultTraits
{
   typedef OpenMesh::Vec4f Point;
   FaceAttributes (OpenMesh::Attributes::Normal);
   VertexAttributes (OpenMesh::Attributes::Normal);
   HalfedgeAttributes (OpenMesh::Attributes::PrevHalfedge);
};

template <>
struct MESH2MTYPE<OpenMesh::PolyMesh_ArrayKernelT<MyTraitsPoint3> >
{ 
   enum { MType = QUAD };
};
template <>
struct MESH2MTYPE<OpenMesh::PolyMesh_ArrayKernelT<MyTraitsPoint4> >
{ 
   enum { MType = QUAD };
};
template <>
struct MESH2MTYPE<OpenMesh::TriMesh_ArrayKernelT<MyTraitsPoint3> >
{ 
   enum { MType = TRIANGLE };
};

typedef OpenMesh::PolyMesh_ArrayKernelT<MyTraitsPoint3> MyPolyMesh;
typedef DynamicSubdivisionCC<MyPolyMesh>                MyDynamicSubdivisionCC;
typedef FCPtr<GroupPtr, MyDynamicSubdivisionCC>         MyDynamicSubdivisionCCPtr;
typedef OpenMesh::PolyMesh_ArrayKernelT<MyTraitsPoint4> MyPolyMesh4;
typedef DynamicSubdivisionCC<MyPolyMesh4>               MyDynamicSubdivisionCC4;
typedef FCPtr<GroupPtr, MyDynamicSubdivisionCC4>        MyDynamicSubdivisionCC4Ptr;

typedef OpenMesh::TriMesh_ArrayKernelT<MyTraitsPoint3>  MyTriMesh;
typedef DynamicSubdivisionLP<MyTriMesh>                 MyDynamicSubdivisionLP;
typedef FCPtr<GroupPtr, MyDynamicSubdivisionLP>         MyDynamicSubdivisionLPPtr;

OSG_END_NAMESPACE

#endif 
