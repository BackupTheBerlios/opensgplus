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


#ifndef _OSGGVBVOLADAPTERBASEP_H_
#define _OSGGVBVOLADAPTERBASEP_H_

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
#include "OSGGVBase.h"
#include "OSGGVBVolAdapterBase.h"

OSG_BEGIN_NAMESPACE
using genvis::BVolAdapterBaseP;

/** \ingroup FieldLib Genvis
 *  
 */

template <>
struct FieldDataTraits<BVolAdapterBaseP> : 
    public FieldTraitsIntegralRecurseMapper<BVolAdapterBaseP>
{
    static DataType             _type;                       

    enum                        { StringConvertable = 0x00 };
    enum                        { bHasParent        = 0x01 };

    static DataType &getType (void) { return _type;        }

    static char     *getSName(void) { return "SFBVolAdapterBaseP"; }
    static char     *getMName(void) { return "MFBVolAdapterBaseP"; }


    static       UInt32    getBinSize (const BVolAdapterBaseP& )
    {
        return sizeof(BVolAdapterBaseP);
    }
    static       UInt32    getBinSize (const BVolAdapterBaseP*, 
				       UInt32 uiNumObjects)
    {
        return uiNumObjects*sizeof(BVolAdapterBaseP);
    }
    static void   copyToBin  (      BinaryDataHandler &pMem, 
                              const BVolAdapterBaseP  &oObject)
    {
//	not required!
//       Int32 value = (Int32)oObject;
//       pMem.putValue(value);
    }
    static void   copyToBin  (      BinaryDataHandler &pMem, 
                              const BVolAdapterBaseP*  oObject,
				    UInt32 uiNumObjects)
    {
//	not required!
//       Int32 value = (Int32)oObject;
//       pMem.putValue(value);
    }
    static void   copyFromBin(      BinaryDataHandler &pMem, 
                                    BVolAdapterBaseP  &oObject)
    {
//	not required!
//        Int32 value;
//        pMem.getValue(value);
//        oObject = (BVolAdapterBaseP)value;
    }
    static void   copyFromBin(      BinaryDataHandler &pMem, 
                                    BVolAdapterBaseP*  oObject,
				    UInt32 uiNumObjects)
    {
//	not required!
//        Int32 value;
//        pMem.getValue(value);
//        oObject = (BVolAdapterBaseP)value;
    }
};

//! SFBVolAdapterBaseP

typedef SField<BVolAdapterBaseP> SFBVolAdapterBaseP;

#ifndef OSG_COMPILEBVOLADAPTERBASEPFIELDSINST
#if defined(__sgi)

#pragma do_not_instantiate SField<BVolAdapterBaseP>::_fieldType

#else

OSG_DLLEXPORT_DECL1(SField, BVolAdapterBaseP, OSG_GENVISLIB_DLLTMPLMAPPING)

#endif
#endif

//! MFBVolAdapterBaseP

typedef MField<BVolAdapterBaseP> MFBVolAdapterBaseP;

#ifndef OSG_COMPILEBVOLADAPTERBASEPFIELDSINST
#if defined(__sgi)

#pragma do_not_instantiate MField<BVolAdapterBaseP>::_fieldType

#else

OSG_DLLEXPORT_DECL1(MField, BVolAdapterBaseP, OSG_GENVISLIB_DLLTMPLMAPPING)

#endif
#endif

OSG_END_NAMESPACE

#define OSGGVBVOLADAPTERBASEPFIELDS_HEADER_CVSID "@(#)$Id: OSGGVBVolAdapterBasePFields.h,v 1.1 2003/09/11 16:20:29 fuenfzig Exp $"

#endif

