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

#ifndef _SOCKET_H_
#define _SOCKET_H_
#ifdef __sgi
#pragma once
#endif

//---------------------------------------------------------------------------
//  Includes
//---------------------------------------------------------------------------

#include "OSGSocketConfig.h"
#include "OSGSocketException.h"
#include "OSGAddress.h"

OSG_BEGIN_NAMESPACE

//---------------------------------------------------------------------------
//  Forward References
//---------------------------------------------------------------------------
class SocketMessage;

//---------------------------------------------------------------------------
//   Types
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  Class
//---------------------------------------------------------------------------

class OSG_SOCKET_DLLMAPPING Socket
{
    /*==========================  PUBLIC  =================================*/
  public:
    /*---------------------------------------------------------------------*/
    /*! \name                   Constructors                               */
    /*! \{                                                                 */

    Socket();
    Socket(const Socket &source);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructor                                 */
    /*! \{                                                                 */

    virtual ~Socket();

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                 Socket access                                */
    /*! \{                                                                 */

    virtual void open              ()=0;
    void         close             (void);
    int          recv              (void *buf,int size);
    int          recvAvailable     (void *buf,int size);
    int          peek              (void *buf,int size);
    int          send              (const void *buf,int size);
    void         bind              (const Address &address=AnyAddress());
    void         listen            (int maxPending=10);
    void         connect           (const Address &address);
    void         setReusePort      (bool value);
    void         setBlocking       (bool value);
    Address      getAddress        (void);
    void         setReadBufferSize (int size);
    void         setWriteBufferSize(int size);
    int          getReadBufferSize (void);
    int          getWriteBufferSize(void);
    int          getAvailable      (void);
    bool         waitReadable      (double duration);
    bool         waitWritable      (double duration);
    int          send              (SocketMessage &msg);
    int          recv              (SocketMessage &msg);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                      Assignment                              */
    /*! \{                                                                 */

	const Socket & operator =(const Socket &source);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                      Error information                       */
    /*! \{                                                                 */

    static int         getError();
    static int         getHostError();
    static string      getErrorStr();
    static string      getHostErrorStr();

    /*! \}                                                                 */
  protected:

    /** Socket option type 
     *  Used to hide the different interface implementations
     **/
#if defined WIN32
    typedef char FAR  SocketOptT;
#else
    typedef void      SocketOptT;
#endif

    /** Socket length type 
     *  Used to hide the different interface implementations
     **/
#if defined __linux
    typedef socklen_t SocketLenT;
#else
    typedef int       SocketLenT;
#endif

    int _sd;

  private:

    friend class Selection;

	static char cvsid[];
    static int  initialized;

};

OSG_END_NAMESPACE

#endif
