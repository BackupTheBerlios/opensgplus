/*---------------------------------------------------------------------------*\
 *                                OpenSG                                     *
 *                                                                           *
 *                                                                           *
 *               Copyright (C) 2000-2002 by the OpenSG Forum                 *
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


#ifndef _OSGDVRSIMPLESHADERFIELDS_H_
#define _OSGDVRSIMPLESHADERFIELDS_H_
#ifdef __sgi
#pragma once
#endif

#include <OSGConfig.h>

#include <OSGFieldContainerPtr.h>
#include <OSGNodeCoreFieldDataType.h>
#include <OSGVolRenDef.h>

#include <OSGDVRShaderFields.h>

OSG_BEGIN_NAMESPACE

class DVRSimpleShader;

#if !defined(OSG_DO_DOC) // created as a dummy class, remove to prevent doubles
//! DVRSimpleShaderPtr

typedef FCPtr<DVRShaderPtr, DVRSimpleShader> DVRSimpleShaderPtr;

#endif

#if !defined(OSG_DO_DOC) || (OSG_DOC_LEVEL >= 3)
/*! \ingroup GrpVolRenFieldTraits
 */
#if !defined(OSG_DOC_DEV_TRAITS)
/*! \hideinhierarchy */
#endif

template <>
struct FieldDataTraits<DVRSimpleShaderPtr> : 
    public FieldTraitsRecurseMapper<DVRSimpleShaderPtr, true>
{
    static DataType             _type;                       

    enum                        { StringConvertable = 0x00 };
    enum                        { bHasParent        = 0x01 };
};

#if !defined(OSG_DOC_DEV_TRAITS)
/*! \class  FieldTraitsRecurseMapper<DVRSimpleShaderPtr, true>
    \hideinhierarchy
 */
#endif

#endif // !defined(OSG_DO_DOC) || (OSG_DOC_LEVEL >= 3)



OSG_END_NAMESPACE

#define OSGDVRSIMPLESHADERFIELDS_HEADER_CVSID "@(#)$Id: OSGDVRSimpleShaderFields.h,v 1.3 2004/01/19 11:22:33 vossg Exp $"

#endif /* _OSGDVRSIMPLESHADERFIELDS_H_ */
