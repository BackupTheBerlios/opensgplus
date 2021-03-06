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

#define OSG_COMPILEOPENMESHPFIELDSINST

#include <stdlib.h>
#include <stdio.h>
#include "OSGConfig.h"
#include "OSGOpenMeshPFields.h"
#ifndef FIELD_TYPE_DEF
#define FIELD_TYPE_DEF
#include "OSGSFieldTypeDef.inl"
#include "OSGMFieldTypeDef.inl"
#endif

OSG_USING_NAMESPACE

template <class MESH>
DataType FieldDataTraits<MESH*>::_type(getName(), "IntegralType");

#if defined(__sgi)

//#pragma instantiate SField<OpenMeshP>::_fieldType
//#pragma instantiate MField<OpenMeshP>::_fieldType

#else

//OSG_DLLEXPORT_SFIELD_DEF1(OpenMeshP, OSG_SUBSURFACELIB_DLLTMPLMAPPING);
//OSG_DLLEXPORT_MFIELD_DEF1(OpenMeshP, OSG_SUBSURFACELIB_DLLTMPLMAPPING);

#endif


