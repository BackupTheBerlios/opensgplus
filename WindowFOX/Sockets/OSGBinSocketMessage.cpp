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

#include "OSGBinSocketMessage.h"

OSG_USING_NAMESPACE

#ifdef __sgi
#pragma set woff 1174
#endif

namespace
{
    static Char8 cvsid_cpp[] = "@(#)$Id: OSGBinSocketMessage.cpp,v 1.1 2002/05/24 15:50:49 fuenfzig Exp $";
    static Char8 cvsid_hpp[] = OSG_BINSOCKETMESSAGEHEADER_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

/** \class osg::BinSocketMessage
 *  \ingroup SocketsLib
 *  \brief Little-, Big endian independent message buffer
 *
 * Little-, Big endian independent message buffer.
 * 
 * Example:
 * <PRE> 
 * // send
 * BinSockMessage msg;
 * msg.clear();              // if not already empty
 * msg.putUInt32(220);
 * msg.putInt32 (221);
 * msg.putUInt16(222);
 * msg.putInt16 (223);
 * msg.putUInt8 (224);
 * msg.putInt8  (225);
 * msg.putReal32(226.0);
 * msg.putString("227");
 * socket.send(msg);
 * // receive
 * string str;
 * socket.recv(msg);
 * str = msg.getString();
 * msg.getString(str);        // avoid one copy
 * </PRE> 
 * 
 **/

/*-------------------------------------------------------------------------*/
/*                            Constructors                                 */

/*! Constructor 
 */
BinSocketMessage::BinSocketMessage(void):
    SocketMessage(),
    _buffer(),
    _pos(sizeof(Header))
{
    clear();
}

/** \brief Copy constructor
 */
BinSocketMessage::BinSocketMessage(const BinSocketMessage &source):
    SocketMessage(source),
    _buffer(source._buffer),
    _pos(source._pos)
{
}

/*-------------------------------------------------------------------------*/
/*                             Destructor                                  */

/** \brief Destructor
 */
BinSocketMessage::~BinSocketMessage(void)
{
}

/*-------------------------------------------------------------------------*/
/*                             Assignment                                  */

/*! assignment
 */
BinSocketMessage& BinSocketMessage::operator = (const BinSocketMessage &source)
{
    if(this == &source)
        return *this;

    // copy parts inherited from parent
    *(static_cast<Inherited *>(this)) = source;

    // free mem alloced by members of 'this'

    // alloc new mem for members

    // copy
    _buffer=source._buffer;
    _pos   =source._pos;
    return *this;
}

/*-------------------------------------------------------------------------*/
/*                             Misc                                        */

/** \brief Set message size 
 *
 * Set message size. This is called by the socket to get enough space
 * to read a message.
 *
 * \param size   New message size
 */
void BinSocketMessage::setSize(UInt32 size)
{
    _buffer.resize(size);
    reset();
}

/** \brief Clear message buffer
 */
void BinSocketMessage::clear(void)
{
    _buffer.resize(sizeof(Header));
}

/** \brief Reset buffer for reading
 *
 * Reset readpointer to the beginn of the buffer
 */
void BinSocketMessage::reset(void)
{
    _pos=sizeof(Header);
}

/*-------------------------------------------------------------------------*/
/*                             Get                                         */

/** \brief Get message size
 *
 * \return message size in bytes
 */
UInt32 BinSocketMessage::getSize(void) 
{
    return _buffer.size();
}

/** \brief Get buffer address
 *
 * \return pointer to the begin of the buffer
 */
MemoryHandle BinSocketMessage::getBuffer(void)
{
    if(_buffer.size())
        return static_cast<MemoryHandle>(&_buffer[0]);
    else
        return 0;
}







