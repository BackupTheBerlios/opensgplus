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

#define OSG_COMPILECLUTERLIB

#include <stdlib.h>
#include <stdio.h>

#include "OSGConfig.h"
#include "OSGStreamSockConnection.h"

OSG_USING_NAMESPACE

/** \enum OSGVecBase::VectorSizeE
 *  \brief 
 */

/** \var OSGVecBase::VectorSizeE OSGVecBase::_iSize
 * 
 */

/** \fn const char *OSGVecBase::getStreamSockConnection(void)
 *  \brief StreamSockConnection
 */

/** \var OSGValueTypeT OSGVecBase::_values[iSize];
 *  \brief Value store
 */

/***************************************************************************\
 *                               Types                                     *
\***************************************************************************/

/***************************************************************************\
 *                           Class variables                               *
\***************************************************************************/

char StreamSockConnection::cvsid[] = "@(#)$Id: $";

/***************************************************************************\
 *                           Class methods                                 *
\***************************************************************************/



/*-------------------------------------------------------------------------*\
 -  public                                                                 -
\*-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*\
 -  protected                                                              -
\*-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*\
 -  private                                                                -
\*-------------------------------------------------------------------------*/



/***************************************************************************\
 *                           Instance methods                              *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  public                                                                 -
\*-------------------------------------------------------------------------*/

/*------------- constructors & destructors --------------------------------*/

/** \brief Constructor
 */

StreamSockConnection::StreamSockConnection():
    Inherited(),
    _buffer(NULL),
    _sockets()
{
    resizeBuffer(4096);
}


//StreamSockConnection::StreamSockConnection(const StreamSockConnection &source) :
//	Inherited(source),
//	  // TODO: initialize members
//{
//}

/** \brief Destructor
 */

StreamSockConnection::~StreamSockConnection(void)
{
    if(_buffer)
    {
        delete [] _buffer;
    }
}

/*------------------------------ access -----------------------------------*/

/*---------------------------- properties ---------------------------------*/

/*-------------------------- your_category---------------------------------*/

MemoryHandle StreamSockConnection::getBuffer()
{
    return _buffer + sizeof(BlockLenT);
}

void StreamSockConnection::resizeBuffer(int size)
{
    if(_buffer)
    {
        delete [] _buffer;
    }
    _bufferSize = size + sizeof(BlockLenT);
    _buffer = new UInt8[ _bufferSize ];
    _dataSize = 0;
}

int StreamSockConnection::getBufferSize()
{
    return _bufferSize - sizeof(BlockLenT);
}

int StreamSockConnection::getDataSize()
{
    return _dataSize;
}

void StreamSockConnection::setDataSize(int size)
{
    _dataSize=size;
}

void StreamSockConnection::send()
{
    const int MAXWRITE=16000;
    int size;
    int pos;
    SocketsT::iterator i;
    int maxsize;

    size=_dataSize + sizeof(BlockLenT);
    *((BlockLenT*)(_buffer))=htonl(size);
    // send to all receivers
    for(i=_sockets.begin();
        i!=_sockets.end();
        i++)
    {
        maxsize=i->getWriteBufferSize();
        size=_dataSize + sizeof(BlockLenT);
        pos=0;
        while(size>maxsize)
        {
            i->write(_buffer+pos,maxsize);
            pos+=maxsize;
            size-=maxsize;
        }
        i->write(_buffer+pos,size);
    }
    _dataSize=0;
}

int StreamSockConnection::receive()
{
    int size;
    int blocksize;
    const int MAXREAD=16000;
    int pos;
    int len;
    int maxsize;

    _dataSize=0;
    if(_sockets.begin() != _sockets.end())
    {
        maxsize=_sockets.begin()->getReadBufferSize();
        len=_sockets.begin()->peek(_buffer,sizeof(BlockLenT));
        if(len==0)
            return 0;
        blocksize=ntohl(*((BlockLenT*)(_buffer)));
        if(blocksize>_bufferSize)
        {
            resizeBuffer(blocksize);
        }
        pos=0;
        size=blocksize;
        while(size>maxsize)
        {
            len=_sockets.begin()->read(_buffer+pos,maxsize);
            if(len==0)
                return 0;
            pos+=maxsize;
            size-=maxsize;
        }
        len=_sockets.begin()->read(_buffer+pos,size);
        if(len==0)
            return 0;
        _dataSize=blocksize - sizeof(BlockLenT);
    }
    return _dataSize;
}

void StreamSockConnection::flush()
{
}

void StreamSockConnection::addSocket(StreamSocket &sock)
{
    _sockets.push_back(sock);
}

/*-------------------------- assignment -----------------------------------*/

/** \brief assignment
 */

StreamSockConnection& StreamSockConnection::operator = (const StreamSockConnection &source)
{
	if (this == &source)
		return *this;

	// copy parts inherited from parent

	// free mem alloced by members of 'this'

	// alloc new mem for members

	// copy 
    return *this;
}

/*-------------------------- comparison -----------------------------------*/

/** \brief assignment
 */

/** \brief equal
 */

/** \brief unequal
 */

/*-------------------------------------------------------------------------*\
 -  protected                                                              -
\*-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*\
 -  private                                                                -
\*-------------------------------------------------------------------------*/


