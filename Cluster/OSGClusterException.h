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


#ifndef _OSGCLUSTEREXCEPTIONS_H_
#define _OSGCLUSTEREXCEPTIONS_H_
#ifdef __sgi
#pragma once
#endif

//---------------------------------------------------------------------------
//  Includes
//---------------------------------------------------------------------------

#include <OSGClusterDef.h>
#include <OSGBaseTypes.h>
#include <OSGException.h>

OSG_BEGIN_NAMESPACE

/** \class ClusterException
 *  \ingroup ClusterLib
 *  \brief Indicate cluster exceptions
 **/

class OSG_CLUSTERLIB_DLLMAPPING ClusterException : public Exception 
{
public:
    ClusterException():Exception()
    {
        _what += "ClusterLib: ";
    }
};

/** \class ClusterException
 *  \ingroup ClusterLib
 *  \brief Indicate close of connecitons
 **/

class OSG_CLUSTERLIB_DLLMAPPING ConnectionClosed : public ClusterException
{
public:
    ConnectionClosed():ClusterException()
    {
        _what += "Connection closed by endpoint. ";
    }
};

OSG_END_NAMESPACE

#endif 


