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

#ifndef OSGMESHIDENTIFIER_H
#define OSGMESHIDENTIFIER_H

#include <stdio.h>
#include "OSGConfig.h"
#include "OSGSubSurfaceDef.h"
#include "OSGBaseTypes.h"

OSG_BEGIN_NAMESPACE

/*! \brief Generator for running identifier. Use MeshIdentifierBase::getNextId()
    once, if you need a new identifier.
 */
class OSG_SUBSURFACELIB_DLLMAPPING MeshIdentifierBase
{
public:
   static UInt32 getNextId ();
};

/*! \brief Identifier for mesh type.
 */
template <class MESH>
class OSG_SUBSURFACELIB_DLLMAPPING MeshIdentifier
{
public:
   static inline UInt32       getId ();
   static inline const Char8* getIdString ();

protected:
   static UInt32 id;
};

template <class MESH>
inline UInt32       MeshIdentifier<MESH>::getId ()
{
   return id;
}
template <class MESH>
inline const Char8* MeshIdentifier<MESH>::getIdString ()
{
   static Char8  sstore[100];
   static Char8* sname = NULL;
   if (sname == NULL) {
      id = MeshIdentifierBase::getNextId();
      sname = sstore;
      sprintf(sname, "%u", MeshIdentifier<MESH>::id);
   }
   return sname; 
}

template <class MESH>
UInt32 MeshIdentifier<MESH>::id; // = MeshIdentifierBase::getNextId();

OSG_END_NAMESPACE

#endif
