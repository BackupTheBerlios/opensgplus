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

#ifndef _DGRAMSOCKET_H_
#define _DGRAMSOCKET_H_
#ifdef __sgi
#pragma once
#endif

//---------------------------------------------------------------------------
//  Includes
//---------------------------------------------------------------------------

#include <OSGConfig.h>
#include <OSGSocketConfig.h>
#include <OSGSocketException.h>
#include <OSGAddress.h>
#include <OSGSocket.h>

OSG_BEGIN_NAMESPACE

class OSG_SOCKET_DLLMAPPING DgramSocket:public Socket
{
    /*==========================  PUBLIC  =================================*/
  public:

    /*---------------------------------------------------------------------*/
    /*! \name                   Constructors                               */
    /*! \{                                                                 */

    DgramSocket();
    DgramSocket(const DgramSocket &source);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Socket functionaliy                        */
    /*! \{                                                                 */

    virtual void open             ();
    int          sendTo           (const void *buf,int size,const Address &to);
    int          recvFrom         (void *buf,int size,Address &from);
    int          peekFrom         (void *buf,int size,Address &from);
    void         join             (const Address &group,
                                   const Address &interf=AnyAddress());
    void         leave            (const Address &group,
                                   const Address &interf=AnyAddress());
    void         setTTL           (unsigned char ttl);
    int          sendTo           (SocketMessage &msg,const Address &to);
    int          recvFrom         (SocketMessage &msg,Address &from);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   assignment                                 */
    /*! \{                                                                 */

	const DgramSocket & operator =(const DgramSocket &source);

    /*! \}                                                                 */

    /*=========================  PROTECTED  ===============================*/
  protected:

    /*==========================  PRIVATE  ================================*/
  private:

    typedef Socket Inherited;
	static char cvsid[];
};

OSG_END_NAMESPACE

#endif
