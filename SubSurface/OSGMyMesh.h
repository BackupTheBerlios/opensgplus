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

#ifdef WIN32
#pragma warning (disable : 698 47 373 383 171)
#endif

#include <iostream>
#include <OpenMesh/Core/Mesh/Types/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include "OSGSubSurfaceDef.h"
#include "OSGDynamicSubdivisionCC.h"

OSG_BEGIN_NAMESPACE

/*! OpenMesh Traits */
struct MyTraits : public OpenMesh::DefaultTraits
{
  FaceAttributes     (OpenMesh::Attributes::Normal);	
  VertexAttributes   (OpenMesh::Attributes::Normal);	
  HalfedgeAttributes (OpenMesh::Attributes::PrevHalfedge);

  EdgeTraits
  {
     Int32 isCrease;
  };  
};

typedef OpenMesh::PolyMesh_ArrayKernelT<MyTraits> MyMesh;
typedef DynamicSubdivisionCC<MyMesh>              MyDynamicSubdivision;
typedef FCPtr<GroupPtr, MyDynamicSubdivision>     MyDynamicSubdivisionPtr;

OSG_END_NAMESPACE

#endif 
