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
	Inherited(),
    _receivers(),
    _seqNumber(1),
    _maxWaitForAck(40),
    _waitForAck(0.04),
    _maxWaitForSync(0.5),
    _socket(),
    _address("224.0.0.50",6546),
    _aliveThread(NULL),
    _stopAliveThread(false)
{
    UInt32 dataPos=sizeof(UDPHeader);
    UInt32 i;
    // no zero copy
    _zeroCopyThreshold=0;
    // create buffers
    _udpBuffers.resize(32 );
    for(UInt32 i=0;
        i<_udpBuffers.size();
        i++)
    {
        _udpBuffers[i].resize(3000);
    }
    for(UInt32 i=0;
        i<(_udpBuffers.size()-1);
        i++)
    {
        _buffers.push_back(
            MemoryBlock(&_udpBuffers[i][dataPos],
                        _udpBuffers[i].size()-dataPos));
    }
    reset();
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
    client.read(ip,100);
    client.read(&multicastport,sizeof(UInt32));
    
    cout << "Multicast group:" << ip << endl;

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

    socket.write(ip,100);
    socket.write(&multicastport,sizeof(UInt32));

    _receivers.push_back(Address(host.c_str(),multicastport));

    cout << "Connected to " << ip << " " << multicastport << endl;
    if(!_aliveThread)
        startAliveThread();
}

/** wait for sync
 *
 **/
void MulticastConnection::wait()
{
    Selection selection;
    UDPHeader sync;
    UInt32 tag;

    // read sync tag;
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
void MulticastConnection::signal()
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

void MulticastConnection::printStatistics()
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

BinaryDataHandler::BuffersT::iterator MulticastConnection::read()
{
    UDPBuffersT::iterator currentBuffer=_udpBuffers.begin(); 
    vector<UDPBufferInfo> dgrams;
    vector<UDPBufferInfo>::iterator d;
    BuffersT::iterator b;
    UDPBufferInfo info;
    UDPBuffer responseAck;
    UInt32 pos,seq;
    BuffersT::iterator result;
    Address from;
    Selection selection;

#   ifdef MULTICAST_STATISTICS
    Time readStartTime=getSystemTime();
#   endif

    for(;;)
    {
        selection.setRead(_socket);
        if(selection.select(2)<=0)
        {
            throw ReadError("Timeout");
        }
        info.buffer=(UDPBuffer*)&(*currentBuffer)[0];
        info.size  =_socket.recvFrom(info.buffer,currentBuffer->size(),from);

        if(info.buffer->header.type == ACK)
        {
            responseAck.header.type = ACK;
            responseAck.header.seqNumber=info.buffer->header.seqNumber;
            responseAck.nack.size=0;
            for(seq=_seqNumber;
                seq<info.buffer->header.seqNumber;
                seq++)
            {
                pos=seq - _seqNumber;
                if(pos>=dgrams.size() || dgrams[pos].size==0)
                {
#   ifdef MULTICAST_STATISTICS
                    _statPckDrop++;
#   endif
                    responseAck.nack.seqNumber[responseAck.nack.size++]=seq;
                }
            }
            // send ack
            _socket.sendTo(&responseAck,
                           (UInt8*)(&responseAck.nack.seqNumber
                                    [responseAck.nack.size])-
                           (UInt8*)(&responseAck),
                           from);
            if(responseAck.nack.size==0 &&
               _seqNumber < info.buffer->header.seqNumber)
            {
                // ok we got all packages
                _seqNumber = info.buffer->header.seqNumber+1;
                break;
            }
        }
        if(info.buffer->header.type == DATA)
        {
            // ignore old packages
            if(info.buffer->header.seqNumber < _seqNumber)
                continue;
            pos=info.buffer->header.seqNumber - _seqNumber;
            while(dgrams.size()<=pos)
            {
                UDPBufferInfo missing;
                missing.size=0;
                dgrams.push_back(missing);
            }
            // ignore retransmitted packages
            if(dgrams[pos].size==0)
            {
#   ifdef MULTICAST_STATISTICS
                _statBytesRead+=info.size;
#   endif
                dgrams[pos]=info;
                currentBuffer++;
            }
        }
    }
    // reorder handler buffers;
    for(d=dgrams.begin(),b=_buffers.begin();
        d!=dgrams.end();
        d++,b++)
    {
        b->dataSize=d->size - sizeof(UDPHeader);
        b->mem=((UInt8*)d->buffer) + sizeof(UDPHeader);
    }
    result=b;
    for(;currentBuffer!=_udpBuffers.end();currentBuffer++,b++)
    {
        b->dataSize=0;
        b->mem=((UInt8*)&(*currentBuffer)[sizeof(UDPHeader)]);
    }
#   ifdef MULTICAST_STATISTICS
    Time readEndTime=getSystemTime();
    _statTimeRead+=readEndTime - readStartTime;
#   endif

    return result;
}    

/** Write buffer
 *
 * @param writeEnd  iterator points behind the last buffer containing data
 *
 **/
void MulticastConnection::write(BuffersT::iterator writeEnd)
{
    BuffersT::iterator b;
    vector<UDPBufferInfo>::iterator d;
    vector<UDPBufferInfo> dgrams;
    UDPBuffer ackRequest;
    set<Address> receivers(_receivers.begin(),_receivers.end());
    set<Address> missingAcks;
    Selection selection;
    vector<UInt8> responseBuffer;
    UDPBuffer *responseAck;
    Time waitTime,maxWaitTime,t0,t1;
    Address from;

#   ifdef MULTICAST_STATISTICS
    Time writeStartTime=getSystemTime();
#   endif

    // prepare buffers
    for(b =_buffers.begin();
        b!=writeEnd; 
        ++b)
    {
        UDPBufferInfo info;
        // dgram information
        info.size           = b->dataSize + sizeof(UDPHeader);
        info.send           = true;
        info.buffer         = (UDPBuffer*)(b->mem - sizeof(UDPHeader));
        // set sequence numbers
        info.buffer->header.seqNumber = _seqNumber++;
        // data package
        info.buffer->header.type       = DATA;
        dgrams.push_back(info);
#   ifdef MULTICAST_STATISTICS
        _statBytesWrite+=info.size;
#   endif
    }

    // prepate ackRequest
    ackRequest.header.seqNumber       = _seqNumber++;
    ackRequest.header.type            = ACK;

    // prepare response buffer. This is the max size for 
    // ack response. longer packages will be truncated
    responseBuffer.resize( (UInt8*)&ackRequest.nack.seqNumber[dgrams.size()]-
                           (UInt8*)&ackRequest.header);

    // loop as long as one receiver needs some data
    while(!receivers.empty())
    {   
        // send packages as fast as possible
        for(d =dgrams.begin();
            d!=dgrams.end();
            d++)
        {
            if(d->send)
            {
                _socket.sendTo(d->buffer,d->size,_address);
                d->send=false;
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
            _socket.sendTo(&ackRequest.header,
                           sizeof(UDPHeader),_address);
            // wait for acknolages. Max _waitForAck seconds.
            for(waitTime=_waitForAck,t1=OSG::getSystemTime();
                waitTime>0.001 && (!missingAcks.empty());
                waitTime=_waitForAck-(OSG::getSystemTime()-t1))
            {
                selection.setRead(_socket);
                if(selection.select(waitTime)>0)
                {
                    _socket.recvFrom(&responseBuffer[0],
                                     responseBuffer.size(),from);
                    responseAck=(UDPBuffer*)&responseBuffer[0];
                    // ignore if we are not waiting for this ack
                    if(missingAcks.find(from)==missingAcks.end())
                    {
                        printf("Unexpected ACK from %s,%d\n",
                               from.getHost().c_str(),
                               from.getPort());
                        continue;
                    }
                    // ignore if no ack or old package
                    if(responseAck->header.type == DATA ||
                       responseAck->header.seqNumber != 
                       ackRequest.header.seqNumber)
                        continue;
                    // we got it, so we do not longer wait for this
                    missingAcks.erase(from);
                    if(responseAck->nack.size==0)
                    {
                        // receiver has got all packages, so we can remove 
                        // from list of receivers for this transmission
                        receivers.erase(from);
                    }
                    else
                    {
                        // mark packages for retransmission
                        for(UInt32 i=0;i<responseAck->nack.size;i++) 
                        {
#   ifdef MULTICAST_STATISTICS
                            _statPckDrop++;
#   endif
                            /*
                            printf("Missing package %d %s:%d\n",
                                   responseAck->nack.seqNumber[i],
                                   from.getHost().c_str(),
                                   from.getPort());
                            */
                            for(d =dgrams.begin();d!=dgrams.end();d++)
                            {
                                if(responseAck->nack.seqNumber[i] == 
                                   d->buffer->header.seqNumber)
                                    d->send=true;
                            }
                        }
                    }
                }
            }
        }
        // not all ack after maxWaitForAck seconds
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


