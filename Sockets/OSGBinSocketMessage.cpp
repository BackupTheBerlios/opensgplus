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
    static Char8 cvsid_cpp[] = "@(#)$Id:$";
    static Char8 cvsid_hpp[] = OSG_BINSOCKETMESSAGEHEADER_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

/*! \class osg::BinSocketMessage
    BinSocketMessage documentation,
 */


/*-------------------------------------------------------------------------*/
/*                            Constructors                                 */

/*! Constructor documentation
 */
BinSocketMessage::BinSocketMessage(void):
    SocketMessage(),
    _buffer(),
    _pos(sizeof(Header))
{
    clear();
}

BinSocketMessage::BinSocketMessage(const BinSocketMessage &source):
    SocketMessage(source),
    _buffer(source._buffer),
    _pos(source._pos)
{
}

/*-------------------------------------------------------------------------*/
/*                             Destructor                                  */

/*! Destructor documentation
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

void BinSocketMessage::setSize(UInt32 size)
{
    _buffer.resize(size);
    reset();
}

void BinSocketMessage::clear(void)
{
    _buffer.resize(sizeof(Header));
}

void BinSocketMessage::reset(void)
{
    _pos=sizeof(Header);
}

/*-------------------------------------------------------------------------*/
/*                             Get                                         */

UInt32 BinSocketMessage::getSize(void) 
{
    return _buffer.size();
}

MemoryHandle BinSocketMessage::getBuffer(void)
{
    if(_buffer.size())
        return static_cast<MemoryHandle>(&_buffer[0]);
    else
        return 0;
}
