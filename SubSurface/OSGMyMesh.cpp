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
#include "OSGOpenMeshPFields.cpp"
#include "OSGDynamicSubdivisionLPBase.cpp"
#include "OSGDynamicSubdivisionLP.cpp"
#include "OSGDynamicSubdivisionCCBase.cpp"
#include "OSGDynamicSubdivisionCC.cpp"

typedef WSDmain<OSG::Vec3f, MyTriMesh,   TRIANGLE> MyTriTesselator;
typedef WSDmain<OSG::Vec3f, MyPolyMesh,  QUAD>     MyPolyTesselator;
typedef WSDmain<OSG::Vec4f, MyPolyMesh4, QUAD>     MyPolyTesselator4;

//  SField
OSG_DLLEXPORT_SFIELD_DEF1(MyTriTesselator*,   OSG_SUBSURFACELIB_DLLTMPLMAPPING);
OSG_DLLEXPORT_SFIELD_DEF1(MyPolyTesselator*,  OSG_SUBSURFACELIB_DLLTMPLMAPPING);
OSG_DLLEXPORT_SFIELD_DEF1(MyPolyTesselator4*, OSG_SUBSURFACELIB_DLLTMPLMAPPING);
OSG_DLLEXPORT_SFIELD_DEF1(MyTriMesh*,         OSG_SUBSURFACELIB_DLLTMPLMAPPING);
OSG_DLLEXPORT_SFIELD_DEF1(MyPolyMesh*,        OSG_SUBSURFACELIB_DLLTMPLMAPPING);
OSG_DLLEXPORT_SFIELD_DEF1(MyPolyMesh4*,       OSG_SUBSURFACELIB_DLLTMPLMAPPING);
//  FieldContainer
template class OSG_SUBSURFACELIB_DLLMAPPING DynamicSubdivisionLP<MyTriMesh>;
template class OSG_SUBSURFACELIB_DLLMAPPING DynamicSubdivisionLPBase<MyTriMesh>;
template class OSG_SUBSURFACELIB_DLLMAPPING DynamicSubdivisionCC<MyPolyMesh>;
template class OSG_SUBSURFACELIB_DLLMAPPING DynamicSubdivisionCCBase<MyPolyMesh>;
template class OSG_SUBSURFACELIB_DLLMAPPING DynamicSubdivisionCC<MyPolyMesh4>;
template class OSG_SUBSURFACELIB_DLLMAPPING DynamicSubdivisionCCBase<MyPolyMesh4>;

