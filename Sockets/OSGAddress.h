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

#ifndef _ADDRESS_H_
#define _ADDRESS_H_
#ifdef __sgi
#pragma once
#endif

#include <OSGConfig.h>

#ifndef WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

//---------------------------------------------------------------------------
//  Includes
//---------------------------------------------------------------------------
#include <string>
#include "OSGSocketConfig.h"

OSG_BEGIN_NAMESPACE

class OSG_SOCKET_DLLMAPPING Address {
    /*==========================  PUBLIC  =================================*/
  public:

#if defined WIN32
    typedef char              SocketAddrT;
#else
    typedef struct socketaddr SocketAddrT;
#endif

    static const char *getClassname(void) { return "Address"; }
 
    /*---------------------------------------------------------------------*/
    /*! \name                   Constructors                               */
    /*! \{                                                                 */

    Address(const char *host=0,int port=0);
    Address(const Address &source);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructor                                 */
    /*! \{                                                                 */

    virtual ~Address();

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Get/Set                                    */
    /*! \{                                                                 */

    sockaddr       *getSockAddr    (                       ) const;
    int             getSockAddrSize(                       ) const;
    void            setPort        (int port               );
    void            setHost        (const std::string &host);
    int             getPort        (                       ) const;
    std::string     getHost        (                       ) const;
    std::string     getHostByName  (                       ) const;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Comparision                                */
    /*! \{                                                                 */

    bool operator == (const Address &other) const;
    bool operator != (const Address &other) const;
    bool operator < (const Address &other) const;

    /*! \}                                                                 */
  protected:
    /*---------------------------------------------------------------------*/
    /*! \name                   member                                     */
    /*! \{                                                                 */

    struct sockaddr_in _sockaddr;

    /*! \}                                                                 */
  private:
	static char cvsid[];
};

class OSG_SOCKET_DLLMAPPING BroadcastAddress:public Address {
public:
    /*---------------------------------------------------------------------*/
    /*! \name                   Constructors                               */
    /*! \{                                                                 */

    BroadcastAddress(int port);

    /*! \}                                                                 */
private:
	static char cvsid[];
};

class OSG_SOCKET_DLLMAPPING AnyAddress:public Address {
public:
    /*---------------------------------------------------------------------*/
    /*! \name                   Constructors                               */
    /*! \{                                                                 */

    AnyAddress(int port=0);

    /*! \}                                                                 */
private:
	static char cvsid[];
};

OSG_END_NAMESPACE

#endif
