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

#define MULTICAST_STATISTICS

OSG_BEGIN_NAMESPACE

class Thread;

/*! \ingroup baselib
 *  \brief Brief
 */
#define MULTICASTCONNECTION_MAX_WINDOW_SIZE 64

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

    void   accept          ( const std::string &address );
    void   connect         ( const std::string &address );
    void   wait            ( void );
    void   signal          ( void );
    UInt32 getChannelCount ( void );
    Bool   selectChannel   ( void );

    void   printStatistics( void );
    void   clearStatistics( void );

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

    //OSGBool operator < (const MulticastConnection &other) const;
    
	//OSGBool operator == (const MulticastConnection &other) const;
	//OSGBool operator != (const MulticastConnection &other) const;

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
        SYNC
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

    virtual void               read  ( void );
    virtual void               write ( void );

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                  address handling                            */
    /*! \{                                                                 */
    void interpreteAddress(const std::string  &address,
                                 std::string  &host,
                                 UInt32       &port);
    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                      Member                                  */
    /*! \{                                                                 */

    std::vector<Address>              _receivers;
    UInt32                            _seqNumber;
    UDPBuffersT                       _udpReadBuffers;
    UDPBuffersT                       _udpWriteBuffers;
    Time                              _maxWaitForAck;
    Time                              _waitForAck;
    Time                              _maxWaitForSync;
    DgramSocket                       _socket;
    Address                           _address;
    Address                           _readAddress;

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
    /*! \name                 Alive thread handling                        */
    /*! \{                                                                 */
    Thread *_aliveThread;
    Bool _stopAliveThread;
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
