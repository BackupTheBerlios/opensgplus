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

/*****************************************************************************\
 *****************************************************************************
 **                                                                         **
 **                  This file is automatically generated.                  **
 **                                                                         **
 **          Any changes made to this file WILL be lost when it is          **
 **           regenerated, which can become necessary at any time.          **
 **                                                                         **
 *****************************************************************************
\*****************************************************************************/


#ifndef _OSGDVRSHADERFIELDS_H_
#define _OSGDVRSHADERFIELDS_H_
#ifdef __sgi
#pragma once
#endif

#include <OSGConfig.h>

#include <OSGFieldContainerPtr.h>
#include <OSGNodeCoreFieldDataType.h>
#include <OSGVolRenDef.h>

#include <OSGFieldContainerFields.h>

OSG_BEGIN_NAMESPACE

class DVRShader;

//! DVRShaderPtr

typedef FCPtr<FieldContainerPtr, DVRShader> DVRShaderPtr;

/*! \brief DVRShaderPtr field traits 
    \ingroup FieldLib
    \ingroup SingleFields
*/

template <>
struct FieldDataTraits<DVRShaderPtr> : 
#ifdef MW_OSG_1_1
    public FieldTraitsRecurseMapper<DVRShaderPtr>
#else
    public FieldTraitsRecurseMapper<DVRShaderPtr, true>
#endif
{
    static DataType             _type;                       

    enum                        { StringConvertable = 0x00 };
    enum                        { bHasParent        = 0x01 };

    static DataType &getType (void) { return _type;        }

    static char     *getSName(void) { return "SFDVRShaderPtr"; }
};

//! SFDVRShaderPtr
//! \ingroup SingleFields

typedef SField<DVRShaderPtr> SFDVRShaderPtr;

#ifndef OSG_COMPILEDVRSHADERINST
OSG_DLLEXPORT_DECL1(SField, DVRShaderPtr, OSG_VOLRENLIB_DLLTMPLMAPPING)
#endif

OSG_END_NAMESPACE

#define OSGDVRSHADERFIELDS_HEADER_CVSID "@(#)$Id: OSGDVRShaderFields.h,v 1.1 2002/10/10 11:11:26 weiler Exp $"

#endif /* _OSGDVRSHADERFIELDS_H_ */