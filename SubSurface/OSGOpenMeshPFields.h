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


#ifndef _OSGOPENMESHP_H_
#define _OSGOPENMESHP_H_

#include "OSGSubSurfaceDef.h"

#ifdef __sgi
#pragma once
#endif

//---------------------------------------------------------------------------
//  Includes
//---------------------------------------------------------------------------

#include "OSGConfig.h"

#include "OSGFieldContainerPtr.h"
#include "OSGSField.h"
#include "OSGMField.h"
#include "OSGMeshIdentifier.h"

OSG_BEGIN_NAMESPACE

/** \ingroup FieldLib OpenMesh
 *  OpenMeshPtr field traits 
 */

template <class MESH>
struct FieldDataTraits<MESH*> : 
    public FieldTraitsIntegralRecurseMapper<MESH*>
{
    static DataType             _type;                       

    enum                        { StringConvertable = 0x00 };
    enum                        { bHasParent        = 0x01 };

    static DataType &getType (void) { return _type;        }

    static char     *getName(void) { 
		static char sname[1000];
		sprintf(sname, "OpenMeshP%d", MeshIdentifier<MESH>::id);
		return sname; 
	}
    static char     *getSName(void) { 
		static char sname[1000];
		sprintf(sname, "SFOpenMeshP%d", MeshIdentifier<MESH>::id);
		return sname; 
	}
    static char     *getMName(void) { 
		static char sname[1000];
		sprintf(sname, "MFOpenMeshP%d", MeshIdentifier<MESH>::id);
		return sname; 
	}


    static       UInt32    getBinSize (const MESH*& )
    {
        return sizeof(MESH*);
    }

    static void   copyToBin  (      BinaryDataHandler &pMem, 
                              const MESH*    &oObject)
    {
//	no implementation use
//        Int32 value = (Int32)oObject;
//       pMem.putValue(value);
    }

    static void   copyFromBin(      BinaryDataHandler &pMem, 
                                    MESH*             &oObject)
    {
//	no implementation use
//        Int32 value;
//        pMem.getValue(value);
//        oObject = (OpenMeshP)value;
    }
};

//! SFDAVESoundPtr

//typedef SField<OpenMeshP> SFOpenMeshP;

#ifndef OSG_COMPILEOPENMESHPFIELDSINST
#if defined(__sgi)

//#pragma do_not_instantiate SField<OpenMeshP>::_fieldType

#else

//OSG_DLLEXPORT_DECL1(SField, OpenMeshP, OSG_TRIPSLIB_DLLTMPLMAPPING)

#endif
#endif

//! MFDAVESoundPtr

//typedef MField<OpenMeshP> MFOpenMeshP;

#ifndef OSG_COMPILEOPENMESHPFIELDSINST
#if defined(__sgi)

//#pragma do_not_instantiate MField<OpenMeshP>::_fieldType

#else

//OSG_DLLEXPORT_DECL1(MField, OpenMeshP, OSG_TRIPSLIB_DLLTMPLMAPPING)

#endif
#endif

OSG_END_NAMESPACE

#define OSGOPENMESHPFIELDS_HEADER_CVSID "@(#)$Id: OSGOpenMeshPFields.h,v 1.1 2003/07/11 14:46:51 fuenfzig Exp $"

#endif

