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

#define OSG_COMPILESETUNIONPFIELDSINST

#include <stdlib.h>
#include <stdio.h>

#include "OSGGeometryPositionCluster.h"
#include "OSGConfig.h"
#include "OSGSFieldTypeDef.inl"
#include "OSGMFieldTypeDef.inl"

OSG_USING_NAMESPACE

UInt32 SetUnion::s_currentStamp = 0;


DataType FieldDataTraits<SetUnionGridP>::_type(getName(), "IntegralType");

#if defined(__sgi)

#pragma instantiate SField<SetUnionGridP>::_fieldType
#pragma instantiate MField<SetUnionGridP>::_fieldType

#else

OSG_DLLEXPORT_SFIELD_DEF1(SetUnionGridP, OSG_GENVISLIB_DLLTMPLMAPPING);
OSG_DLLEXPORT_MFIELD_DEF1(SetUnionGridP, OSG_GENVISLIB_DLLTMPLMAPPING);

#endif


DataType FieldDataTraits<SetUnionPoolP>::_type(getName(), "IntegralType");

#if defined(__sgi)

#pragma instantiate SField<SetUnionPoolP>::_fieldType
#pragma instantiate MField<SetUnionPoolP>::_fieldType

#else

OSG_DLLEXPORT_SFIELD_DEF1(SetUnionPoolP, OSG_GENVISLIB_DLLTMPLMAPPING);
OSG_DLLEXPORT_MFIELD_DEF1(SetUnionPoolP, OSG_GENVISLIB_DLLTMPLMAPPING);

#endif
