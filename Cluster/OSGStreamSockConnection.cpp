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
    Inherited(16000),
    _sockets()
{
    _socketReadBuffer.resize(64000);
    _socketWriteBuffer.resize( _socketReadBuffer.size() );
    // reserve first bytes for buffer size
    readBufAdd (&_socketReadBuffer [sizeof(SocketBufferHeader)],
                _socketReadBuffer.size() -sizeof(SocketBufferHeader));
    writeBufAdd(&_socketWriteBuffer[sizeof(SocketBufferHeader)],
                _socketWriteBuffer.size()-sizeof(SocketBufferHeader));
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
 * Read data form the current read socket. The read socket is that
 * socket, that was selectet in selectChannel.
 *
 **/

void StreamSockConnection::read(MemoryHandle mem,UInt32 size)
{
    int len;

    // read data
    len=_readSocket.read(mem,size);
    if(len==0)
    {
        cout << size << endl;
        throw ReadError("read got 0 bytes!");
    }
}

/** Read next data block
 *
 * The stream connection uses only BinaryDataHandler buffer. If more
 * then one buffer is present, then this methode must be changed!
 *
 */

void StreamSockConnection::readBuffer()
{
    BuffersT::iterator buffer;
    int size;
    int len;

    // read buffer header
    len=_readSocket.read(&_socketReadBuffer[0],sizeof(SocketBufferHeader));
    if(len==0)
        throw ReadError("peek got 0 bytes!");
    // read remaining data
    size=((SocketBufferHeader*)&_socketReadBuffer[0])->size;
    len=_readSocket.read(&_socketReadBuffer[sizeof(SocketBufferHeader)],size);
    if(len==0)
        throw ReadError("read got 0 bytes!");
    readBufBegin()->setDataSize(size);
}    

/** Write data to all destinations
 *
 **/

void StreamSockConnection::write(MemoryHandle mem,UInt32 size)
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
 * Write blocksize and data.
 *
 **/
void StreamSockConnection::writeBuffer(void)
{
    UInt32 size = writeBufBegin()->getDataSize();
    // write size to header
    ((SocketBufferHeader*)&_socketWriteBuffer[0])->size=size;
    // write data to all sockets
    for(SocketsT::iterator socket =_sockets.begin();
        socket!=_sockets.end();
        ++socket)
    {
        // write whole block
        socket->write(&_socketWriteBuffer[0],size+sizeof(SocketBufferHeader));
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
    StreamSocket socket,from;

    interpreteAddress(address,host,port);
    socket.open();
    socket.setReusePort(true);
    socket.bind(AnyAddress(port));
    socket.listen();
    from=socket.accept();
    from.setDelay(false);
    _sockets.push_back(from);
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
    socket.setDelay(false);
    socket.connect(Address(host.c_str(),port));
    _sockets.push_back(socket);
}

/** wait for sync
 *
 **/
void StreamSockConnection::wait(void)
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
void StreamSockConnection::signal(void)
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

/** get number of links
 *
 **/
UInt32 StreamSockConnection::getChannelCount(void)
{
    return _sockets.size();
}

/** select channel for read
 *
 * A connection can have n links from which data can be read. So we
 * need to select one channel for exclusive read. 
 *
 **/
Bool StreamSockConnection::selectChannel(void)
{
    Int32 maxnread=0,nread;
    SocketsT::iterator socket;
    Selection selection,result;
    bool ready=false;

    // only one socket?
    if(_sockets.size()==1)
    {
        _readSocket=_sockets[0];
        return true;
    }

    // select socket with most data
    for(socket=_sockets.begin();
        socket!=_sockets.end();
        socket++)
    {
        nread=socket->getNReadBytes();
        if(maxnread < nread)
        {
            maxnread = nread;
            _readSocket=*socket;
        }
    }
    if(maxnread)
    {
        return true;
    }

    // wait for first socket to deliver data
    for(socket=_sockets.begin();
        socket!=_sockets.end();
        socket++)
        selection.setRead(*socket);

    // select ok ?
    if(!selection.select(-1,result))
    {
        throw ReadError("no socket readable");
    }

    // find readable socket
    for(socket=_sockets.begin();!result.isSetRead(*socket);socket++);
    _readSocket=*socket;
    return true;
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

