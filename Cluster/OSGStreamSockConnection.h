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


#ifndef _STREAMSOCKCONNECTION_H_
#define _STREAMSOCKCONNECTION_H_
#ifdef __sgi
#pragma once
#endif

//---------------------------------------------------------------------------
//  Includes
//---------------------------------------------------------------------------

#include <vector>
#include <OSGClusterDef.h>
#include <OSGBaseTypes.h>
#include <OSGConnection.h>
#include <OSGConnectionType.h>
#include <OSGStreamSocket.h>

OSG_BEGIN_NAMESPACE

class OSG_CLUSTERLIB_DLLMAPPING StreamSockConnection:public Connection
{
    /*==========================  PUBLIC  =================================*/
  public:

    struct SocketBufferHeader {
        UInt32 size;
    };
    typedef Connection Inherited;
    typedef std::vector<StreamSocket> SocketsT;
    typedef std::vector<UInt8>        SocketBufferT;

    /*---------------------------------------------------------------------*/
    /*! \name                   Constructor                                */
    /*! \{                                                                 */

    StreamSockConnection();

    /*! \}                                                                 */

    /*---------------------------------------------------------------------*/
    /*! \name                   Destructor                                 */
    /*! \{                                                                 */

    virtual ~StreamSockConnection(void); 

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Communication                              */
    /*! \{                                                                 */

    string                bind            ( const string &address );
    void                  accept          ( void                  );
    void                  connect         ( const string &address );
    void                  wait            ( void                  );
    void                  signal          ( void                  );
    UInt32                getChannelCount ( void                  );
    void                  selectChannel   ( void                  );
    const ConnectionType *getType         ( void                  );

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   create                                     */
    /*! \{                                                                 */

    static Connection *create(void);

    /*! \}                                                                 */

    /*=========================  PROTECTED  ===============================*/
  protected:

    /*---------------------------------------------------------------------*/
    /*! \name                   IO Implementation                          */
    /*! \{                                                                 */

    virtual void      read(MemoryHandle mem,UInt32 size);
    virtual void      readBuffer();
    virtual void      write(MemoryHandle mem,UInt32 size);
    virtual void      writeBuffer();
    void              interpreteAddress(const string  &address,
                                              string  &host,
                                              UInt32  &port);

    /*! \}                                                                 */

    /*---------------------------------------------------------------------*/
    /*! \name                Instance Variables                            */
    /*! \{                                                                 */

    StreamSocket         _acceptSocket;
    StreamSocket         _readSocket;
    SocketsT             _sockets;
    std::vector<UInt8>   _socketReadBuffer;
    std::vector<UInt8>   _socketWriteBuffer;

    /*! \}                                                                 */

  private:

    /*---------------------------------------------------------------------*/
    /*! \name                   Class Variable                             */
    /*! \{                                                                 */

	static char cvsid[];
    static ConnectionType _type;

    /*! \}                                                                 */

    /*---------------------------------------------------------------------*/
    /*! \name                   Copy                                       */
    /*! \{                                                                 */

	// prohibit default functions (move to 'public' if you need one)
    StreamSockConnection(const StreamSockConnection &source);
    StreamSockConnection& operator =(const StreamSockConnection &source);

    /*! \}                                                                 */
};

//---------------------------------------------------------------------------
//   Exported Types
//---------------------------------------------------------------------------

// class pointer

typedef StreamSockConnection *StreamSockConnectionP;

OSG_END_NAMESPACE

#endif /* _STREAMSOCKCONNECTION_H_ */
