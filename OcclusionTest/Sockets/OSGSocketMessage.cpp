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
#include "OSGSocketMessage.h"

OSG_USING_NAMESPACE

#ifdef __sgi
#pragma set woff 1174
#endif

namespace
{
    static Char8 cvsid_cpp[] = "@(#)$Id: OSGSocketMessage.cpp,v 1.1 2002/01/11 14:41:20 jsux Exp $";
    static Char8 cvsid_hpp[] = OSG_SOCKETMESSAGEHEADER_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

/** \class osg::SocketMessage
 *  \ingroup SocketsLib
 *  \brief Abstract socket message buffer
 *
 * This is the base class for socket messages. It provides an Interface
 * for transmission of data buffers.
 * 
 **/

/*-------------------------------------------------------------------------*/
/*                            Constructors                                 */

/*! Constructor
 */
SocketMessage::SocketMessage(void)
{
}

/*! copy Constructor
 */
SocketMessage::SocketMessage(const SocketMessage &)
{
}

/*-------------------------------------------------------------------------*/
/*                             Destructor                                  */

/*! Destructor
 */
SocketMessage::~SocketMessage(void)
{
}

/*-------------------------------------------------------------------------*/
/*                             Assignment                                  */

/*! assignment
 */
SocketMessage& SocketMessage::operator = (const SocketMessage &)
{
    return *this;
}

/** \brief Get message header
 *
 * \return Address to the message header
 */
SocketMessage::Header &SocketMessage::getHeader (void)
{
    return *((Header*)(getBuffer()));
}





