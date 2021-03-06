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

#ifndef _MULTICASTCONNECTION_H_
#define _MULTICASTCONNECTION_H_
#ifdef __sgi
#pragma once
#endif

#include <OSGBaseTypes.h>
#include <OSGTime.h>
#include <OSGClusterDef.h>
#include <OSGConnection.h>
#include <OSGDgramSocket.h>
#include <OSGAddress.h>
#include <OSGConnectionType.h>

#define MULTICAST_STATISTICS

OSG_BEGIN_NAMESPACE

class Thread;

#define MULTICASTCONNECTION_MAX_WINDOW_SIZE 64
#define MULTICAST_BUFFER_COUNT 12
#define MULTICAST_BUFFER_SIZE  4000

class OSG_CLUSTERLIB_DLLMAPPING MulticastConnection : public Connection
{
    /*==========================  PUBLIC  =================================*/
  public:

    /*---------------------------------------------------------------------*/
    /*! \name                   Constructors                               */
    /*! \{                                                                 */
 
    MulticastConnection(int port=0);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructor                                 */
    /*! \{                                                                 */

    virtual ~MulticastConnection(void); 

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                      Get                                     */
    /*! \{                                                                 */

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                      Set                                     */
    /*! \{                                                                 */

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   your_category                              */
    /*! \{                                                                 */

    string                bind            ( const string &address );
    void                  accept          ( void                  );
    void                  connect         ( const string &address );
    void                  wait            ( void                  );
    void                  signal          ( void                  );
    UInt32                getChannelCount ( void                  );
    void                  selectChannel   ( void                  );
    const ConnectionType *getType         ( void                  );

    void   printStatistics( void );
    void   clearStatistics( void );

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   create                                     */
    /*! \{                                                                 */

    static Connection *create(void);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   your_operators                             */
    /*! \{                                                                 */

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Assignment                                */
    /*! \{                                                                 */

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Comparison                                */
    /*! \{                                                                 */

    //OSGbool operator < (const MulticastConnection &other) const;
    
	//OSGbool operator == (const MulticastConnection &other) const;
	//OSGbool operator != (const MulticastConnection &other) const;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                        Dump                                  */
    /*! \{                                                                 */

    /*! \}                                                                 */
    /*=========================  PROTECTED  ===============================*/
  protected:

    enum UDPHeaderType {
        ACK_REQUEST,
        ACK,
        DATA,
        ALIVE,
        SYNC,
        CONNECT,
        CLOSED
    };

    typedef std::vector<std::vector<UInt8> > UDPBuffersT;
    struct UDPHeader
    {
        UInt32 seqNumber;
        UInt8  type;
    };
    struct UDPBuffer
    {
        UDPHeader  header;
        union 
        {
            UInt8 data[1];
            UInt32 member;
            struct 
            {
                UInt32 size;
                UInt32 missing[MULTICASTCONNECTION_MAX_WINDOW_SIZE];
            } nack;
        };
    };

    /*---------------------------------------------------------------------*/
    /*! \name                  read/write                                  */
    /*! \{                                                                 */

    virtual void      readBuffer  ( void                        );
    virtual void      writeBuffer ( void                        );

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                  address handling                            */
    /*! \{                                                                 */
    void interpreteAddress(const string  &address,
                                 string  &group,
                                 UInt32       &port,
                                 UInt32       &member);
    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                      Member                                  */
    /*! \{                                                                 */

    std::vector<Address>              _channelAddress;
    std::vector<UInt32>               _channelSeqNumber;
    UInt32                            _channel;
    UInt32                            _seqNumber;
    UDPBuffersT                       _udpReadBuffers;
    UDPBuffersT                       _udpWriteBuffers;
    Time                              _maxWaitForAck;
    Time                              _waitForAck;
    Time                              _maxWaitForSync;
    DgramSocket                       _socket;
    DgramSocket                       _inSocket;
    DgramSocket                       _groupSocket;
    DgramSocket                       _aliveSocket;
    Address                           _destination;
    UInt32                            _member;
    Time                              _aliveTime;

    /*! \}                                                                 */
#   ifdef MULTICAST_STATISTICS
    /*---------------------------------------------------------------------*/
    /*! \name                      Statistics                              */
    /*! \{                                                                 */
    UInt32                            _statBytesRead;
    UInt32                            _statBytesWrite;
    Time                              _statTimeRead;
    Time                              _statTimeWrite;
    UInt32                            _statPckDrop;
    UInt32                            _statAckRetransmit;
    /*! \}                                                                 */
#   endif
    /*==========================  PRIVATE  ================================*/
  private:

    /*---------------------------------------------------------------------*/
    /*! \name                   Class Variable                             */
    /*! \{                                                                 */

    static ConnectionType _type;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                 Alive thread handling                        */
    /*! \{                                                                 */
    Thread *_aliveThread;
    bool _stopAliveThread;
    void startAliveThread();
    void stopAliveThread();
    static void *aliveProc(void *arg);
    /*! \}                                                                 */

    typedef Connection Inherited;

	/*!\brief prohibit default function (move to 'public' if needed) */

    MulticastConnection(const MulticastConnection &source);
    void operator =(const MulticastConnection &source);
};

//---------------------------------------------------------------------------
//   Exported Types
//---------------------------------------------------------------------------

// class pointer

typedef MulticastConnection *MulticastConnectionP;

OSG_END_NAMESPACE

#define OSG_MULTICASTCONNECTIONHEADER_CVSID "@(#)$Id:$"

#endif /* _MULTICASTCONNECTION_H_ */
