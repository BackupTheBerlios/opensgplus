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

#include "OSGMyMesh.h"

// explicit template instantiations
#include "OSGDynamicSubdivisionCCBase.cpp"
#include "OSGDynamicSubdivisionCC.cpp"
#include "OSGOpenMeshPFields.cpp"
#include "OSGOpenMeshTesselatorPFields.cpp"

template class OSG_SUBSURFACELIB_DLLMAPPING DynamicSubdivisionCCBase<MyMesh>;
template class OSG_SUBSURFACELIB_DLLMAPPING DynamicSubdivisionCC<MyMesh>;
template class OSG_SUBSURFACELIB_DLLMAPPING FCPtr<GroupPtr, DynamicSubdivisionCC<MyMesh> >;

// explicit template instantiations
#include "OSGwsdmain.cpp"
#include "OSGwsdsubdiv.cpp"
#include "OSGwsdsubdivquad.cpp"
#include "OSGwsddat.cpp"
#include "OSGwsdmesh2dat.cpp"

template class OSG_SUBSURFACELIB_DLLMAPPING WSDmain<Vec4f, MyMesh, QUAD>;
template class OSG_SUBSURFACELIB_DLLMAPPING WSDsubdiv<Vec4f, QUAD>;
template class OSG_SUBSURFACELIB_DLLMAPPING WSDdat<Vec4f, QUAD>;
template class OSG_SUBSURFACELIB_DLLMAPPING WSDmesh2dat<Vec4f, MyMesh, QUAD>;


