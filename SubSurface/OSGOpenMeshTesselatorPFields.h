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


#ifndef _OSGOPENMESHTESSELATORP_H_
#define _OSGOPENMESHTESSELATORP_H_

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
#include "OSGVector.h"
#include "OSGMeshIdentifier.h"

OSG_BEGIN_NAMESPACE

template<class WSDVector, class Mesh, int mtype> class WSDmain;

/** \ingroup FieldLib DAVESound
 *  DAVESoundPtr field traits 
 */

template <class MESH>
struct FieldDataTraits<WSDmain<Vec4f,MESH,1>*> : 
    public FieldTraitsIntegralRecurseMapper<WSDmain<Vec4f,MESH,1>*>
{
    typedef WSDmain<Vec4f,MESH,1> Tesselator;
    static DataType             _type;                       

    enum                        { StringConvertable = 0x00 };
    enum                        { bHasParent        = 0x01 };

    static DataType &getType (void) { return _type;        }

    static char     *getName(void) { 
		static char sname[1000];
		sprintf(sname, "OpenMeshTesselatorP%d", MeshIdentifier<MESH>::id);
		return sname; 
	}
    static char     *getSName(void) { 
		static char sname[1000];
		sprintf(sname, "SFOpenMeshTesselatorP%d", MeshIdentifier<MESH>::id);
		return sname; 
	}
    static char     *getMName(void) { 
		static char sname[1000];
		sprintf(sname, "MFOpenMeshTesselatorP%d", MeshIdentifier<MESH>::id);
		return sname; 
	}


    static       UInt32    getBinSize (const Tesselator*& )
    {
        return sizeof(Tesselator*);
    }

    static void   copyToBin  (      BinaryDataHandler &pMem, 
                              const Tesselator*&oObject)
    {
//	no implementation use
//        Int32 value = (Int32)oObject;
//       pMem.putValue(value);
    }

    static void   copyFromBin(      BinaryDataHandler &pMem, 
                                    Tesselator*&oObject)
    {
//	no implementation use
//        Int32 value;
//        pMem.getValue(value);
//        oObject = (OpenMeshTesselatorP)value;
    }
};

//! SFDAVESoundPtr

//typedef SField<OpenMeshTesselatorP> SFOpenMeshTesselatorP;

#ifndef OSG_COMPILEOPENMESHTESSELATORPFIELDSINST
#if defined(__sgi)

//#pragma do_not_instantiate SField<OpenMeshTesselatorP>::_fieldType

#else

//OSG_DLLEXPORT_DECL1(SField, OpenMeshTesselatorP, OSG_SUBSURFACELIB_DLLTMPLMAPPING )

#endif
#endif

//! MFDAVESoundPtr

//typedef MField<OpenMeshTesselatorP> MFOpenMeshTesselatorP;

#ifndef OSG_COMPILEOPENMESHTESSELATORPFIELDSINST
#if defined(__sgi)

//#pragma do_not_instantiate MField<OpenMeshTesselatorP>::_fieldType

#else

//OSG_DLLEXPORT_DECL1(MField, OpenMeshTesselatorP, OSG_SUBSURFACELIB_DLLTMPLMAPPING )

#endif
#endif

OSG_END_NAMESPACE

#define OSGOPENMESHTESSELATORPFIELDS_HEADER_CVSID "@(#)$Id: OSGOpenMeshTesselatorPFields.h,v 1.1 2003/07/11 14:46:51 fuenfzig Exp $"

#endif

