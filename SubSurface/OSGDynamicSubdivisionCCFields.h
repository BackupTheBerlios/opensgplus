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


#ifndef _OSGDYNAMICSUBDIVISIONCCFIELDS_H_
#define _OSGDYNAMICSUBDIVISIONCCFIELDS_H_
#ifdef __sgi
#pragma once
#endif

#include "OSGConfig.h"

#include "OSGFieldContainerPtr.h"
#include "OSGNodeCoreFieldDataType.h"
#include "OSGSubSurfaceDef.h"

#include "OSGGroupFields.h"

OSG_BEGIN_NAMESPACE

template <class MESH> class DynamicSubdivisionCC;

#if !defined(OSG_DO_DOC)   // created as a dummy class, remove to prevent doubles
//! DynamicSubdivisionCCPtr

//typedef FCPtr<GroupPtr, DynamicSubdivisionCC> DynamicSubdivisionCCPtr;

#endif

#if !defined(OSG_DO_DOC) || defined(OSG_DOC_DEV)
/*! \brief DynamicSubdivisionCCPtr field traits 
*/

template <class MESH>
struct FieldDataTraits<FCPtr<GroupPtr, DynamicSubdivisionCC<MESH> > > : 
    public FieldTraitsRecurseMapper<FCPtr<GroupPtr, DynamicSubdivisionCC<MESH> >, true>
{
   static DataType             _type;                       

   static char* getName(void) { 
      static char sname[1000];
      sprintf(sname, "DynamicSubdivisionCCPtr%d", MeshIdentifier<MESH>::id);
      return sname; 
   }

   enum                        { StringConvertable = 0x00 };
   enum                        { bHasParent        = 0x01 };
};
#endif             // exclude from doc


OSG_END_NAMESPACE

#define OSGDYNAMICSUBDIVISIONCCFIELDS_HEADER_CVSID "@(#)$Id: OSGDynamicSubdivisionCCFields.h,v 1.1 2003/07/11 14:46:51 fuenfzig Exp $"

#endif /* _OSGDYNAMICSUBDIVISIONCCFIELDS_H_ */
