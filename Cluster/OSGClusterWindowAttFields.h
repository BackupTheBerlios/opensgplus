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


#ifndef _OSGCLUSTERWINDOWATTFIELDS_H_
#define _OSGCLUSTERWINDOWATTFIELDS_H_
#ifdef __sgi
#pragma once
#endif

//---------------------------------------------------------------------------
//  Includes
//---------------------------------------------------------------------------

#include <OSGConfig.h>

#include <OSGFieldContainerPtr.h>
#include <OSGNodeCoreFieldDataType.h>
#include <OSGClusterDef.h>

OSG_BEGIN_NAMESPACE

//---------------------------------------------------------------------------
//  Forward References
//---------------------------------------------------------------------------

class ClusterWindowAtt;
/** \brief ClusterWindowAttPtr
 */
typedef FCPtr<AttachmentPtr, ClusterWindowAtt> ClusterWindowAttPtr;

/** \ingroup FieldLib ClusterWindowAtt
 *  \brief ClusterWindowAttPtr field traits 
 */

template <>
struct FieldDataTraits<ClusterWindowAttPtr> : 
    public FieldTraitsRecurseMapper<ClusterWindowAttPtr>
{
    static DataType             _type;                       

    enum                        { StringConvertable = 0x00 };
    enum                        { bHasParent        = 0x01 };

    static DataType &getType (void) { return _type;        }

    static char     *getSName(void) { return "SFClusterWindowAttPtr"; }
    static char     *getMName(void) { return "MFClusterWindowAttPtr"; }
};

/** \brief SFClusterWindowAttPtr
 */
typedef SField<ClusterWindowAttPtr> SFClusterWindowAttPtr;

#ifndef OSG_COMPILECLUSTERWINDOWATTINST
#if defined(__sgi)

#pragma do_not_instantiate SField<ClusterWindowAttPtr>::_fieldType

#else

OSG_DLLEXPORT_DECL1(SField, ClusterWindowAttPtr, OSG_CLUSTERLIB_DLLTMPLMAPPING)

#endif
#endif

/** \brief MFClusterWindowAttPtr
 */
typedef MField<ClusterWindowAttPtr> MFClusterWindowAttPtr;

#ifndef OSG_COMPILECLUSTERWINDOWATTINST
#if defined(__sgi)

#pragma do_not_instantiate MField<ClusterWindowAttPtr>::_fieldType

#else

OSG_DLLEXPORT_DECL1(MField, ClusterWindowAttPtr, OSG_CLUSTERLIB_DLLTMPLMAPPING)

#endif
#endif

OSG_END_NAMESPACE

#endif /* _OSGCLUSTERWINDOWATTFIELDS_H_ */
