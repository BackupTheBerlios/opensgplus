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

#include <algorithm>

#include <OSGConfig.h>
#include <OSGLog.h>
#include <OSGStreamSockConnection.h>
#include <OSGSelection.h>

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

char StreamSockConnection::cvsid[] = "@(#)$Id:$";

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
    _sockets()
{
    _socketBuffer.resize(16000);
    _zeroCopyThreshold=200;
    // reserve first bytes for buffer size
    _buffers.push_back(
        MemoryBlock(&_socketBuffer[sizeof(SocketBufferHeader)],
                    _socketBuffer.size()-sizeof(SocketBufferHeader)) );
    _socketBufferHeader=(SocketBufferHeader*)&_socketBuffer[0];
    reset();
}

/** \brief Destructor
 */

StreamSockConnection::~StreamSockConnection(void)
{
}

/*------------------------------ access -----------------------------------*/

/*---------------------------- properties ---------------------------------*/

/*-------------------------- your_category---------------------------------*/

/** Read data into given memory
 *
 * The first socket that provides data is used for read.
 *
 * @bug It is not possible to read sync from multible sources 
 *      with a StreamSockConnection. This is currently not a
 *      real problem. You have to switch off zero copy to get this work
 *      MR
 **/

void StreamSockConnection::read(MemoryHandle mem,int size)
{
    Selection selection;
    SocketsT::iterator i;
    int len;

    // wait for first socket to deliver data
    for(i=_sockets.begin();i!=_sockets.end();i++)
        selection.setRead(*i);
    // select ok ?
    if(!selection.select(-1))
    {
        throw ReadError("no socket selectable");
    }
    // get readable socket
    for(i=_sockets.begin();!selection.isSetRead(*i);i++)
        if(i==_sockets.end())
            throw ReadError("no socket readable");
    StreamSocket socket=*i;
    // read data
    len=socket.read(mem,size);
    if(len==0)
    {
        throw ReadError("read got 0 bytes!");
    }
}

/** Read next data block
 *
 * The first socket that provides data is used for read. 
 *
 * @return buffer iterator points behind the last buffer containing data
 */

BinaryDataHandler::BuffersT::iterator StreamSockConnection::read()
{
    SocketsT::iterator i;
    Selection selection;
    int readSize;
    int len;

    // wait for first socket to deliver data
    for(i=_sockets.begin();
        i!=_sockets.end();
        i++)
        selection.setRead(*i);
    // select ok ?
    if(!selection.select(-1))
    {
        throw ReadError("no socket selectable");
    }
    // get readable socket
    for(i=_sockets.begin();!selection.isSetRead(*i);i++)
        if(i==_sockets.end())
            throw ReadError("no socket readable");
    StreamSocket socket=*i;
    // read buffer header
    len=socket.read(&_socketBuffer[0],sizeof(SocketBufferHeader));
    if(len==0)
        throw ReadError("peek got 0 bytes!");
    // read remaining data
    len=socket.read(_buffers[0].mem,
                    _socketBufferHeader->size);
    if(len==0)
        throw ReadError("read got 0 bytes!");
    _buffers[0].dataSize = _socketBufferHeader->size;
    return _buffers.end();
}    

/** Write data to all destinations
 *
 **/

void StreamSockConnection::write(MemoryHandle mem,int size)
{
    SocketsT::iterator socket;

    // write to all connected sockets
    for(socket =_sockets.begin();
        socket!=_sockets.end();
        socket++)
    {
        socket->write(mem,size);
    }
}

/** Write buffer
 *
 * @param writeEnd  iterator points behind the last buffer containing data
 *
 **/
void StreamSockConnection::write(BuffersT::iterator writeEnd)
{
    UInt32 size=0;
    SocketsT::iterator socket;
    BuffersT::iterator i;

    // calculate blocklen
    for(i =_buffers.begin(); i!=writeEnd; ++i)
    {
        size+=i->dataSize;
    }
    // write size to header
    _socketBufferHeader->size=size;
    // write data to all sockets
    for(socket =_sockets.begin();
        socket!=_sockets.end();
        socket++)
    {
        // write whole block
        socket->write(&_socketBuffer[0],size+sizeof(SocketBufferHeader));
    }
}

/** Wait for incommint connections on the given address
 *
 * @param address    Port number
 *
 **/
void StreamSockConnection::accept( const string &address )
{
    string host;
    UInt32 port;
    StreamSocket socket;

    interpreteAddress(address,host,port);
    socket.open();
    socket.setReusePort(true);
    socket.bind(AnyAddress(port));
    socket.listen();
    _sockets.push_back(socket.accept());
    socket.close();
}

/** connect a connection at the given address
 *
 * @param address    Host:Port
 *
 **/
void StreamSockConnection::connect( const string &address )
{
    string host;
    UInt32 port;
    StreamSocket socket;

    interpreteAddress(address,host,port);
    socket.open();
    socket.connect(Address(host.c_str(),port));
    _sockets.push_back(socket);
}

/** wait for sync
 *
 **/
void StreamSockConnection::wait()
{
    SocketsT::iterator   i;
    UInt8                trigger;

    for(i =_sockets.begin();
        i!=_sockets.end();
        i++)
    {
        // tell receiver wait entered
        i->write(&trigger,sizeof(UInt8));
    }
    for(i =_sockets.begin();
        i!=_sockets.end();
        i++)
    {
        // wait for signal
        i->read(&trigger,sizeof(UInt8));
    }
}

/** send sync
 *
 **/
void StreamSockConnection::signal()
{
    SocketsT::iterator   i;
    UInt8                trigger;

    for(i =_sockets.begin();
        i!=_sockets.end();
        i++)
    {
        // wait for all links to enter wait
        i->read(&trigger,sizeof(UInt8));
    }
    for(i =_sockets.begin();
        i!=_sockets.end();
        i++)
    {
        // send signal to all links
        i->write(&trigger,sizeof(UInt8));
    }
}

/*-------------------------- assignment -----------------------------------*/

/** \brief assignment
 */

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

void StreamSockConnection::interpreteAddress(const string &address,
                                             std::string  &host,
                                             UInt32       &port)
{
    UInt32 pos=address.find(':',0);
    
    if(pos>0)
    {
        host = address.substr(0,pos);
        port = atoi(address.substr(pos+1).c_str());
    }
    else
    {
        string::const_iterator i;
        for(i =address.begin();
            i!=address.end() && isdigit(*i);
            i++);
        if(i==address.end())
        {
            host="";
            port=atoi(address.c_str());
        }
        else
        {
            host=address;
            port=0;
        }
    }
}

/*-------------------------------------------------------------------------*\
 -  private                                                                -
\*-------------------------------------------------------------------------*/

