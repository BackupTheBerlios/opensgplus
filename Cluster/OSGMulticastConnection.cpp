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

char MulticastConnection::cvsid[] = "@(#)$Id:$";
ConnectionType MulticastConnection::_type(&MulticastConnection::create,
                                          "Multicast");

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

MulticastConnection::MulticastConnection(int ) :
	Inherited(0),
    _seqNumber(1),
    _udpReadBuffers(32),
    _udpWriteBuffers(32),
    _maxWaitForAck(40),
    _waitForAck(0.04),
    _maxWaitForSync(0.5),
    _socket(),
    _aliveThread(NULL),
    _stopAliveThread(false)
{
    UInt32 i;

    // create read buffers
    _udpReadBuffers.resize( MULTICAST_BUFFER_COUNT );
    for(i=0;i<(_udpReadBuffers.size()-1);i++)
    {
        _udpReadBuffers[i].resize(MULTICAST_BUFFER_SIZE);
        readBufAdd(&_udpReadBuffers[i][sizeof(UDPHeader)],
                   _udpReadBuffers[i].size()-sizeof(UDPHeader));
    }
    _udpReadBuffers[i].resize(MULTICAST_BUFFER_SIZE);

    // create write buffers
    _udpWriteBuffers.resize( MULTICAST_BUFFER_COUNT );
    for(i=0;i<(_udpWriteBuffers.size()-1);i++)
    {
        _udpWriteBuffers[i].resize(MULTICAST_BUFFER_SIZE);
        writeBufAdd(&_udpWriteBuffers[i][sizeof(UDPHeader)],
                     _udpWriteBuffers[i].size()-sizeof(UDPHeader));
    }
    _udpWriteBuffers[i].resize(MULTICAST_BUFFER_SIZE);

    _socket.open();
    _groupSocket.open();
    startAliveThread();

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
    _groupSocket.close();
#   ifdef MULTICAST_STATISTICS
    printStatistics();
#   endif
}

/*------------------------------ access -----------------------------------*/

/*---------------------------- properties ---------------------------------*/

/*-------------------------- your_category---------------------------------*/

/** \brief create conneciton
 */

Connection *MulticastConnection::create(void)
{
    return new MulticastConnection();
}

/** Bind connection to the givven address
 *
 * @param address    Port number
 *
 **/
string MulticastConnection::bind( const string &address )
{
    char bound[256];
    string group;
    UInt32 port;
    UInt32 member;
    
    interpreteAddress(address,group,port,member);
    if(group.empty())
        group     ="224.0.0.50";
    if(port==0)
        port =6546;

    // prepare socket
    _groupSocket.setReusePort(true);
    _groupSocket.bind(AnyAddress(port));
    _groupSocket.join(Address(group.c_str()));
    _inSocket=_groupSocket;
    _socket.bind(AnyAddress(0));

    if(member==0)
    {
        member=(UInt32)(getSystemTime()*(1<<30));
        member&=0x7fff0000;
        member|=_socket.getAddress().getPort();
    }
    _member=member;
    sprintf(bound,"%.120s:%d:%d",group.c_str(),port,member);
    SINFO << "Multicast bound to " << bound << endl;
    return bound;
}

/** Wait for incommint connections on the given address
 *
 **/
void MulticastConnection::accept( void )
{
    UDPBuffer alive;
    Address destination;

    do
    {
        _inSocket.recvFrom(&alive,sizeof(alive),destination);
    }
    while(alive.header.type != ALIVE);
    _destination=destination;
    _channelAddress.push_back(destination);
    _channelSeqNumber.push_back(1);
    SINFO << "Connection accepted from " << destination.getHost()
          << ":" << destination.getPort() << endl;
}

/** connect a connection at the given address
 *
 * @param address    Host:Port
 *
 **/
void MulticastConnection::connect( const string &address )
{
    Address from;
    UDPBuffer alive;
    string group;
    UInt32 port;
    UInt32 member;

    interpreteAddress(address,group,port,member);
    _destination=Address(group.c_str(),port);
    if(member==0)
    {
        SFATAL << "Connect to member and no member is given" << endl;
        return;
    }
    if(group.empty())
    {
        SFATAL << "No group given to connect" << endl;
        return;
    }
    // wait for alive signal
    do
    {
        _socket.recvFrom(&alive,sizeof(alive),from);
    }
    while(alive.header.type != ALIVE ||
          alive.member != member);
    _channelAddress.push_back(from);
    _channelSeqNumber.push_back(1);
    _inSocket=_socket;
    SINFO << "Connected to " << from.getHost() 
          << ":" << from.getPort();
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
        SINFO << "Sync timeout" << endl;
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
    _socket.sendTo(&sync,sizeof(sync),_destination);
}

/** get number of links
 *
 **/
UInt32 MulticastConnection::getChannelCount(void)
{
    return _channelAddress.size();
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
        if(!_inSocket.waitReadable(4))
        {
            return false;
        }
        size=_inSocket.peekFrom(&header,sizeof(header),from);
        // wait for data or ack request of unread data
        if(size>=sizeof(header))
        {
            for(UInt32 i=0;i<_channelAddress.size();i++)
            {
                if(_channelAddress[i]==from &&
                   ( header.type == DATA ||
                     (header.type == ACK_REQUEST && 
                      header.seqNumber > _channelSeqNumber[i])))
                {
                    _channel=i;
                    return true;
                }
            }
        }
        _inSocket.recv(&header,sizeof(header));
    }
}

void MulticastConnection::printStatistics(void)
{
#ifdef MULTICAST_STATISTICS
    printf("Bytes read ...............%12d\n",_statBytesRead);
    printf("Bytes write ..............%12d\n",_statBytesWrite);
    if(_statTimeWrite>0)
        printf("Write kbytes/s ...........%12.2f\n",
               (_statBytesWrite/_statTimeWrite)/1024.0);
    if(_statTimeRead>0)
        printf("Read kbytes/s ............%12.2f\n",
               (_statBytesRead/_statTimeRead)/1024.0);
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
        selection.setRead(_inSocket);
        if(selection.select(2)<=0)
        {
            throw ReadError("Timeout");
        }
        dataSize = _inSocket.recvFrom(&(*currentBuffer)[0],
                                      currentBuffer->size(),
                                      from);
        if(from != _channelAddress[_channel])
            continue;
        header   = (UDPHeader*)&(*currentBuffer)[0];
        if(header->type == ACK_REQUEST)
        {
            responseAck.header.type      = ACK;
            responseAck.header.seqNumber = header->seqNumber;
            responseAck.nack.size        = 0;
            if(header->seqNumber > _channelSeqNumber[_channel])
            {
                for(pos = 0 , buffer=readBufBegin();
                    pos < (header->seqNumber - _channelSeqNumber[_channel]);
                    pos++   , buffer++)
                {
                    if(buffer->getDataSize()==0)
                    {
#   ifdef MULTICAST_STATISTICS
                        _statPckDrop++;
#   endif
                        SINFO << "missing" << pos << " " 
                              << _channelSeqNumber[_channel] << " "
                              << header->seqNumber << endl;
                        responseAck.nack.missing[responseAck.nack.size++]=pos;
                    }
                }
            }
            // send ack
            _socket.sendTo(&responseAck,
                             (MemoryHandle)(&responseAck.nack.missing
                                            [responseAck.nack.size])-
                             (MemoryHandle)(&responseAck),
                             from);
            if(responseAck.nack.size==0 && 
               _channelSeqNumber[_channel] < header->seqNumber)
            {
                // ok we got all packages
                _channelSeqNumber[_channel] = header->seqNumber+1;
                break;
            }
        }
        if(header->type == DATA)
        {
            // ignore old packages
            if(header->seqNumber < _channelSeqNumber[_channel])
            {
                continue;
            }
            buffer=readBufBegin() + (header->seqNumber -
                                     _channelSeqNumber[_channel]);
            // ignore retransmitted packages
            if(buffer->getDataSize()>0)
            {
                continue;
            }
#   ifdef MULTICAST_STATISTICS
            _statBytesRead+=dataSize - sizeof(UDPHeader);
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
    set<Address>           receivers(_channelAddress.begin(),
                                     _channelAddress.end());
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
                               _destination);
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
            _socket.sendTo(&ackRequest,sizeof(UDPHeader),_destination);
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
    UDPBuffer alive;

    while(!connection->_stopAliveThread)
    {
        if(connection->_destination.getPort()!=0)
        {
            // send ALIVE package
            // receivers should ignore this
            alive.header.seqNumber = 0;
            alive.header.type      = ALIVE;
            alive.member           = connection->_member;
            connection->_socket.sendTo(
                &alive,
                sizeof(UDPHeader)+sizeof(alive.member),
                connection->_destination);
        }
#if defined WIN32
        Sleep(2000);
#else
        sleep(2);
#endif
    }
    return NULL;
}

/** nterprete address
 *
 *   multicastgroup:port:client
 *
 **/

void MulticastConnection::interpreteAddress(const string &address,
                                                  string &group,
                                                  UInt32 &port,
                                                  UInt32 &member)
{
    Int32 pos1=address.find(':',0);
    Int32 pos2;
    
    group.erase();
    port=0;
    member=0;
    if(address.empty())
    {
        return;
    }
    if(pos1>0)
    {
        group = address.substr(0,pos1);
        pos2=address.find(':',pos1+1);
        if(pos2>0)
        {
            port = atoi(address.substr(pos1+1,pos2).c_str());
            member = atoi(address.substr(pos2+1).c_str());
        }
        else
        {
            port = atoi(address.substr(pos1+1).c_str());
        }
    }
    else
    {
        string::const_iterator i;
        for(i =address.begin();
            i!=address.end() && isdigit(*i);
            i++);
        if(i==address.end())
        {
            port=atoi(address.c_str());
        }
        else
        {
            group=address;
            port=0;
        }
    }
}


