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

#define OSG_COMPILECLUSTER

#include <stdlib.h>
#include <stdio.h>

#include <vector>
#include <algorithm>
#include <set>

#include "OSGConfig.h"
#include "OSGThread.h"
#include "OSGThreadManager.h"
#include "OSGSelection.h"
#include "OSGMulticastConnection.h"
#include "OSGStreamSocket.h"

using std::vector;
using std::set;

OSG_USING_NAMESPACE

namespace 
{
    char cvsid_cpp[] = "@(#)$Id:$";
    char cvsid_hpp[] = OSG_MULTICASTCONNECTIONHEADER_CVSID;
}

/** \enum OSGVecBase::VectorSizeE
 *  \brief 
 */

/** \var OSGVecBase::VectorSizeE OSGVecBase::_iSize
 * 
 */

/** \fn const char *OSGVecBase::getClassname(void)
 *  \brief Classname
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

MulticastConnection::MulticastConnection(int port) :
	Inherited(0),
    _receivers(),
    _seqNumber(1),
    _udpReadBuffers(32),
    _udpWriteBuffers(32),
    _maxWaitForAck(40),
    _waitForAck(0.04),
    _maxWaitForSync(0.5),
    _socket(),
    _address("224.0.0.50",6546),
//    _address("198.111.111.111",6546),
    _aliveThread(NULL),
    _stopAliveThread(false)
{
    UInt32 i;

    // create read buffers
    _udpReadBuffers.resize( 32 );
    for(i=0;i<(_udpReadBuffers.size()-1);i++)
    {
        _udpReadBuffers[i].resize(3000);
        readBufAdd(&_udpReadBuffers[i][sizeof(UDPHeader)],
                   _udpReadBuffers[i].size()-sizeof(UDPHeader));
    }
    _udpReadBuffers[i].resize(3000);

    // create read buffers
    _udpWriteBuffers.resize( 32 );
    for(i=0;i<(_udpWriteBuffers.size()-1);i++)
    {
        _udpWriteBuffers[i].resize(3000);
        writeBufAdd(&_udpWriteBuffers[i][sizeof(UDPHeader)],
                     _udpWriteBuffers[i].size()-sizeof(UDPHeader));
    }
    _udpWriteBuffers[i].resize(3000);

    _socket.open();
#   ifdef MULTICAST_STATISTICS
    clearStatistics();
#   endif
}

/** \brief Destructor
 */

MulticastConnection::~MulticastConnection(void)
{
    stopAliveThread();
    _socket.close();
}

/*------------------------------ access -----------------------------------*/

/*---------------------------- properties ---------------------------------*/

/*-------------------------- your_category---------------------------------*/

/** Wait for incommint connections on the given address
 *
 * @param address    Port number
 *
 **/
void MulticastConnection::accept( const string &address )
{
    string host;
    UInt32 port;
    StreamSocket socket,client;
    char ip[100];
    UInt32 multicastport;

    interpreteAddress(address,host,port);

    // connection is done with the use of stream sockets. 
    socket.open();
    socket.setReusePort(true);
    socket.bind(AnyAddress(port));
    socket.listen();
    client=socket.accept();
    cout << "client connected" << endl;
    client.recv(ip,100);
    client.recv(&multicastport,sizeof(UInt32));
    
    cout << "Multicast group:" << ip << " " << multicastport << endl;

    // prepare multicast socket
    _socket.setReusePort(true);
    _socket.bind(AnyAddress(multicastport));
    _socket.join(ip);

    client.close();
    socket.close();
}

/** connect a connection at the given address
 *
 * @param address    Host:Port
 *
 **/
void MulticastConnection::connect( const string &address )
{
    string host;
    UInt32 port;
    StreamSocket socket;
    char ip[100];
    UInt32 multicastport;

    interpreteAddress(address,host,port);
    socket.open();
    socket.connect(Address(host.c_str(),port));

    strcpy(ip,_address.getHost().c_str());
    multicastport=_address.getPort();

    socket.send(ip,100);
    socket.send(&multicastport,sizeof(UInt32));

    _receivers.push_back(Address(host.c_str(),multicastport));

    cout << "Connected to " << ip << " " << multicastport << endl;
    if(!_aliveThread)
        startAliveThread();
}

/** wait for sync
 *
 **/
void MulticastConnection::wait(void)
{
    Selection selection;
    UDPHeader sync;
    UInt32 tag;

    // read sync tag;
    selectChannel();
    getUInt32(tag);

    selection.setRead(_socket);
    if(selection.select(_maxWaitForSync)<=0)
    {
        cout << "no sync... who cares!!" << endl;
        return;
    }
    _socket.recv(&sync,sizeof(sync));
}

/** send sync
 *
 **/
void MulticastConnection::signal(void)
{
    UDPHeader sync;

    // write sync tag
    putUInt32(0);
    flush();

    // write some sync packages
    sync.seqNumber=0;
    sync.type     =SYNC;

    // send
    _socket.sendTo(&sync,sizeof(sync),_address);
}

/** get number of links
 *
 **/
UInt32 MulticastConnection::getChannelCount(void)
{
    return _receivers.size();
}

/** select channel for read
 *
 * A connection can have n links from which data can be read. So we
 * need to select one channel for exclusive read. 
 *
 **/
Bool MulticastConnection::selectChannel()
{
    UDPHeader header;
    Address from;
    Selection selection;
    UInt32 size;

    for(;;)
    {
        selection.setRead(_socket);
        if(selection.select(2)<=0)
        {
            return false;
        }
        size=_socket.peekFrom(&header,sizeof(header),from);
        // wait for data or ack request of unread data
        if(size>=sizeof(header) &&
           ( header.type == DATA ||
             (header.type == ACK_REQUEST && header.seqNumber > _seqNumber)))
        {
            _readAddress=from;
            return true;
        }
        else
        {
            _socket.recv(&header,sizeof(header));
        }
    }
    return false;
}

void MulticastConnection::printStatistics(void)
{
#ifdef MULTICAST_STATISTICS
    printf("Bytes read ...............%12d\n",_statBytesRead);
    printf("Bytes write ..............%12d\n",_statBytesWrite);
    if(_statTimeWrite>0)
        printf("Write kbytes/s ...........%12.2f\n",
               (_statBytesWrite/_statTimeWrite)/1000.0);
    if(_statTimeRead>0)
        printf("Read kbytes/s ............%12.2f\n",
               (_statBytesRead/_statTimeRead)/1000.0);
    printf("Package loss .............%12d\n",_statPckDrop);
    printf("Ack losss ................%12d\n",_statAckRetransmit);
#else
    printf("No statistics available\n");
#endif
}

void MulticastConnection::clearStatistics()
{
#ifdef MULTICAST_STATISTICS
    _statBytesRead=0;
    _statBytesWrite=0;
    _statTimeRead=0;
    _statTimeWrite=0;
    _statPckDrop=0;
    _statAckRetransmit=0;
#endif
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

void MulticastConnection::readBuffer()
{
    UDPBuffersT::iterator currentBuffer=_udpReadBuffers.begin(); 
    BuffersT::iterator    buffer;
    UDPHeader            *header;
    UDPBuffer             responseAck;
    UInt32                pos;
    Address               from;
    Selection             selection;
    UInt32                dataSize;

#   ifdef MULTICAST_STATISTICS
    Time readStartTime=getSystemTime();
#   endif

    // clear read buffers
    for(buffer=readBufBegin();buffer!=readBufEnd();++buffer)
        buffer->setDataSize(0);

    for(;;)
    {
        selection.setRead(_socket);
        if(selection.select(2)<=0)
        {
            throw ReadError("Timeout");
        }
        dataSize = _socket.recvFrom(&(*currentBuffer)[0],
                                    currentBuffer->size(),
                                    from);
        if(from != _readAddress)
            continue;
        header   = (UDPHeader*)&(*currentBuffer)[0];
        if(header->type == ACK_REQUEST)
        {
            responseAck.header.type      = ACK;
            responseAck.header.seqNumber = header->seqNumber;
            responseAck.nack.size        = 0;
            for(pos = 0 , buffer=readBufBegin();
                pos < (header->seqNumber - _seqNumber);
                pos++   , buffer++)
            {
                if(buffer->getDataSize()==0)
                {
#   ifdef MULTICAST_STATISTICS
                    _statPckDrop++;
#   endif
                    cout << "missing" << pos << " " << pos+_seqNumber << endl;
                    responseAck.nack.missing[responseAck.nack.size++]=pos;
                }
            }
            // send ack
            _socket.sendTo(&responseAck,
                           (MemoryHandle)(&responseAck.nack.missing
                                         [responseAck.nack.size])-
                           (MemoryHandle)(&responseAck),
                           from);
            if(responseAck.nack.size==0 && _seqNumber < header->seqNumber)
            {
                // ok we got all packages
                _seqNumber = header->seqNumber+1;
                break;
            }
        }
        if(header->type == DATA)
        {
            // ignore old packages
            if(header->seqNumber < _seqNumber)
            {
                continue;
            }
            buffer=readBufBegin() + (header->seqNumber - _seqNumber);
            // ignore retransmitted packages
            if(buffer->getDataSize()>0)
            {
                continue;
            }
#   ifdef MULTICAST_STATISTICS
            _statBytesRead+=dataSize;
#   endif
            buffer->setMem ( (MemoryHandle)&(*currentBuffer)[sizeof(UDPHeader)] );
            buffer->setDataSize ( dataSize - sizeof(UDPHeader) );
            buffer->setSize ( currentBuffer->size() );
            currentBuffer++;
        }
    }
#   ifdef MULTICAST_STATISTICS
    Time readEndTime=getSystemTime();
    _statTimeRead+=readEndTime - readStartTime;
#   endif
}    

/** Write buffer
 *
 **/
void MulticastConnection::writeBuffer(void)
{
    vector<int>            send;
    vector<int>::iterator  sendI;
    BuffersT::iterator     bufferI;
    UDPHeader              ackRequest;
    set<Address>           receivers(_receivers.begin(),_receivers.end());
    set<Address>           missingAcks;
    Selection              selection;
    UDPBuffer              responseAck;
    Time                   waitTime,t0,t1;
    Address                from;
    UDPHeader             *header;

#   ifdef MULTICAST_STATISTICS
    Time writeStartTime=getSystemTime();
#   endif

    for(bufferI=writeBufBegin() ; 
        bufferI!=writeBufEnd() && bufferI->getDataSize()>0 ;
        bufferI++)
    {
        header = (UDPHeader*)(bufferI->getMem()-sizeof(UDPHeader));
        header->type      = DATA;
        header->seqNumber = _seqNumber++;
        send.push_back(true);
#   ifdef MULTICAST_STATISTICS
        _statBytesWrite+=bufferI->getDataSize();
#   endif
    }

    // prepate ackRequest
    ackRequest.seqNumber       = _seqNumber++;
    ackRequest.type            = ACK_REQUEST;

    // loop as long as one receiver needs some data
    while(!receivers.empty())
    {   
        // send packages as fast as possible
        for(sendI =  send.begin()       , bufferI=writeBufBegin();
            sendI != send.end();
            sendI++                     , bufferI++)
        {
            if(*sendI == true)
            {
                _socket.sendTo(bufferI->getMem()      - sizeof(UDPHeader),
                               bufferI->getDataSize() + sizeof(UDPHeader),
                               _address);
                *sendI=false;
            }
        }
        missingAcks=receivers;
        // cancle transmission if maxWaitForAck reached
        t0=OSG::getSystemTime();
#   ifdef MULTICAST_STATISTICS
            _statAckRetransmit--;
#   endif
        while((!missingAcks.empty()) && 
              (_maxWaitForAck-(OSG::getSystemTime()-t0))>0.001)
        {
#   ifdef MULTICAST_STATISTICS
            _statAckRetransmit++;
#   endif
            // send acknolage request
            _socket.sendTo(&ackRequest,sizeof(UDPHeader),_address);
            // wait for acknolages. Max _waitForAck seconds.
            for(waitTime=_waitForAck,t1=OSG::getSystemTime();
                waitTime>0.001 && (!missingAcks.empty());
                waitTime=_waitForAck-(OSG::getSystemTime()-t1))
            {
                selection.setRead(_socket);
                if(selection.select(waitTime)>0)
                {
                    _socket.recvFrom(&responseAck,sizeof(responseAck),from);
                    // ignore if we are not waiting for this ack
                    if(missingAcks.find(from)==missingAcks.end())
                    {
                        printf("Unexpected ACK from %s,%d\n",
                               from.getHost().c_str(),
                               from.getPort());
                        continue;
                    }
                    // ignore if no ack or old package
                    if(responseAck.header.type != ACK ||
                       responseAck.header.seqNumber != 
                       ackRequest.seqNumber)
                        continue;
                    // we got it, so we do not longer wait for this
                    missingAcks.erase(from);
                    if(responseAck.nack.size==0)
                    {
                        // receiver has got all packages, so we can remove 
                        // from list of receivers for this transmission
                        receivers.erase(from);
                    }
                    else
                    {
                        // mark packages for retransmission
                        for(UInt32 i=0;i<responseAck.nack.size;i++) 
                        {
#   ifdef MULTICAST_STATISTICS
                            _statPckDrop++;
#   endif
                            printf("Missing package %d %s:%d\n",
                                   responseAck.nack.missing[i],
                                   from.getHost().c_str(),
                                   from.getPort());
                            send[responseAck.nack.missing[i]]=true;
                        }
                    }
                }
            }
        }
        // not all acks received after maxWaitForAck seconds
        if(!missingAcks.empty())
        {
            throw WriteError("ACK Timeout");
        }
    }
#   ifdef MULTICAST_STATISTICS
    Time writeEndTime=getSystemTime();
    _statTimeWrite+=writeEndTime - writeStartTime;
#   endif
}

/*-------------------------------------------------------------------------*\
 -  private                                                                -
\*-------------------------------------------------------------------------*/

void MulticastConnection::startAliveThread()
{
    stopAliveThread();
    _stopAliveThread=false;
    _aliveThread=dynamic_cast<Thread*>(ThreadManager::the()->getThread(NULL));
    _aliveThread->run( aliveProc, 0, (void *) (this) );
}

void MulticastConnection::stopAliveThread()
{
    if(_aliveThread)
    {
        _stopAliveThread=true;
        Thread::join( _aliveThread );
//!!        ThreadManager::the()->removeThread(_aliveThread);
        _aliveThread=0;
    }
}

void *MulticastConnection::aliveProc(void *arg) 
{ 
    MulticastConnection *connection=static_cast<MulticastConnection *>(arg);
    UDPBuffer ackRequest;

    while(!connection->_stopAliveThread)
    {
        // send acknolage request for old package
        // clients should ignore this
        ackRequest.header.seqNumber       = 0;
        ackRequest.header.type            = ALIVE;
        connection->_socket.sendTo(&ackRequest.header,
                                   sizeof(UDPHeader),connection->_address);
        sleep(1);
    }
    return NULL;
}

void MulticastConnection::interpreteAddress(const string &address,
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


