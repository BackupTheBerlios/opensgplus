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
#ifdef __sgi
#pragma once
#endif

#include "OSGConfig.h"
#include "OSGSubSurfaceDef.h"
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

    static inline DataType& getType (void) { 
       return _type;        
    }

    static inline const Char8* getName(void) { 
       static std::string sname;
       sname = "OpenMeshP";
       sname.append(MeshIdentifier<MESH>::getIdString());
       return sname.c_str(); 
    }
    static inline const Char8* getSName(void) { 
       static std::string sname;
       sname = "SFOpenMeshP";
       sname.append(MeshIdentifier<MESH>::getIdString());
       return sname.c_str(); 
    }
    static inline const Char8* getMName(void) { 
       static std::string sname;
       sname = "MFOpenMeshP";
       sname.append(MeshIdentifier<MESH>::getIdString());
       return sname.c_str(); 
    }

    static inline UInt32 getBinSize (const MESH*& ) {
       return 0;
    }

    static inline void copyToBin  (      BinaryDataHandler& , 
           const MESH*&             ) {
    }

    static inline void copyFromBin (BinaryDataHandler& , 
            MESH*&             oObject) {
       oObject = NULL;
    }
};

OSG_END_NAMESPACE

#define OSGOPENMESHPFIELDS_HEADER_CVSID "@(#)$Id: OSGOpenMeshPFields.h,v 1.4 2004/07/09 09:05:54 fuenfzig Exp $"

#endif

