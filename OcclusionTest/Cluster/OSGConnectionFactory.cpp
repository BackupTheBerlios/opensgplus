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

//---------------------------------------------------------------------------
//  Includes
//---------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>

#include "OSGConfig.h"
#include "OSGLog.h"
#include "OSGConnection.h"
#include "OSGConnectionType.h"
#include "OSGConnectionFactory.h"

OSG_USING_NAMESPACE

#ifdef __sgi
#pragma set woff 1174
#endif

namespace
{
    static Char8 cvsid_cpp[] = "@(#)$Id: OSGConnectionFactory.cpp,v 1.1 2001/11/21 17:04:29 jsux Exp $";
    static Char8 cvsid_hpp[] = OSG_CONNECTIONFACTORYHEADER_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

ConnectionFactory *ConnectionFactory::_the;

/*! \class osg::ConnectionFactory
    ConnectionFactory documentation,
 */


/*-------------------------------------------------------------------------*/
/*                            Constructors                                 */

/*! Constructor documentation
 */
ConnectionFactory::ConnectionFactory(void) :
    _map()
{
}

/*-------------------------------------------------------------------------*/
/*                             Destructor                                  */

/*! Destructor documentation
 */
ConnectionFactory::~ConnectionFactory(void)
{
}

/*-------------------------------------------------------------------------*/
/*                      public interface                                   */

/*! create connection by name
 */

Connection *ConnectionFactory::create(const string &name)
{
    TypeMap::iterator i=_map.find(name);
    if(i==_map.end())
       return NULL;
    else
        return i->second->create();
}

/*! get factory 
 */

ConnectionFactory &ConnectionFactory::the(void)
{
    if(_the==NULL)
        _the=new ConnectionFactory();
    return *_the;
}

/*-------------------------------------------------------------------------*/
/*                      protected interface                                */

/*! add new type
 */

void ConnectionFactory::addType(ConnectionType *type)
{
    _map[type->getName()]=type;
    SINFO << "Connection type " << type->getName() << " added" << endl;
}

/*! remove type
 */

void ConnectionFactory::subType(ConnectionType *type)
{
    TypeMap::iterator i=_map.find(type->getName());
    if(i!=_map.end() && i->second==type)
        _map.erase(i);
    SINFO << "Connection type " << type->getName() << " removed" << endl;
}










