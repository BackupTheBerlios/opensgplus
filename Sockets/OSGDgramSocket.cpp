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
#define OSG_COMPILESOCKETLIB

#include <sys/types.h>
#ifdef WIN32
#include <windows.h>
#include <WS2TCPIP.h>
#include <io.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#endif
#include <errno.h>
#include <stdio.h>
#include <math.h>
#include <map>
#include <OSGSocketConfig.h>
#include <OSGAddress.h>
#include <OSGDgramSocket.h>

OSG_BEGIN_NAMESPACE

/***************************************************************************\
 *                               Types                                     *
\***************************************************************************/

/***************************************************************************\
 *                           Class variables                               *
\***************************************************************************/

char DgramSocket::cvsid[] = "@(#)$Id:$";

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
DgramSocket::DgramSocket():
    Socket()
{
}

DgramSocket::DgramSocket(const DgramSocket &source):
    Socket(source)
{
}

/** \brief Destructor
 */

/*------------------------------ access -----------------------------------*/

/*---------------------------- properties ---------------------------------*/

/*-------------------------- your_category---------------------------------*/

void DgramSocket::open()
{
    _sd = socket(AF_INET, SOCK_DGRAM, 0);
    if(_sd<0)
    {
        throw SocketError("socket()");
    }
    // all dgram sockets are allowed to send broadcast
    int on = 1;
    if(::setsockopt(_sd, 
                    SOL_SOCKET, SO_BROADCAST, 
                    (SocketOptT*)&on, sizeof(on)) < 0)
    {
        throw SocketError("setsockopt(,SOL_SOCKET,SO_BROADCAST)");
    }
    // by default, multicast only in local network
    setTTL(1);
}

int DgramSocket::recvFrom(void *buf,int size,Address &from)
{
    int len;
    SocketLenT addrLen=from.getSockAddrSize();

    len=recvfrom(_sd,
                 (char*)buf,
                 size,
                 0,
                 from.getSockAddr(),
                 &addrLen);
    if(len==-1)
    {
#if defined WIN32
        if(getError()==WSAECONNRESET)
        {
            throw SocketConnReset("recvfrom");
        }
#endif
        throw SocketError("recvfrom()");
    }
    return len;
}

int DgramSocket::peekFrom(void *buf,int size,Address &from)
{
    int len;
    SocketLenT addrLen=from.getSockAddrSize();

    len=recvfrom(_sd,
                 (char*)buf,
                 size,
                 MSG_PEEK,
                 from.getSockAddr(),
                 &addrLen);
    if(len==-1)
    {
#if defined WIN32
        if(getError()==WSAECONNRESET)
        {
            throw SocketConnReset("recvfrom");
        }
#endif
        throw SocketError("recvfrom()");
    }
    return len;
}

int DgramSocket::sendTo(const void *buf,int size,const Address &to)
{
    int len;

    // send Request
    len=sendto(_sd,
               (const char*)buf,size,
               0,
               to.getSockAddr(),
               to.getSockAddrSize());
    if(len == -1)
    {
        throw SocketError("sendto()");
    }
    return len;
}

void DgramSocket::join(const Address &group,const Address &interf)
{
    struct ip_mreq joinAddr;
    int rc;

    // group to join
    joinAddr.imr_multiaddr.s_addr
    = ((sockaddr_in*)group.getSockAddr())->sin_addr.s_addr;
    // interface that joins. (equal to bind address)
    joinAddr.imr_interface
    = ((struct sockaddr_in*)interf.getSockAddr())->sin_addr;
    rc=setsockopt(_sd,
                  IPPROTO_IP,
                  IP_ADD_MEMBERSHIP,
                  (SocketOptT*)&joinAddr,
                  sizeof(joinAddr));
    if(rc < 0)
    {
        throw SocketError("setsockopt(IPPROTO_IP,IP_ADD_MEMBERSHIP)");
    }
}

void DgramSocket::leave(const Address &group,const Address &interf)
{
    struct ip_mreq joinAddr;
    int rc;

    // group to join
    joinAddr.imr_multiaddr.s_addr
    = ((sockaddr_in*)group.getSockAddr())->sin_addr.s_addr;
    // interface that joins. (equal to bind address)
    joinAddr.imr_interface
    = ((sockaddr_in*)interf.getSockAddr())->sin_addr;
    rc=setsockopt(_sd,
                  IPPROTO_IP,
                  IP_DROP_MEMBERSHIP,
                  (SocketOptT*)&joinAddr,
                  sizeof(joinAddr));
    if(rc < 0)
    {
        throw SocketError("setsockopt(IPPROTO_IP,IP_DROP_MEMBERSHIP)");
    }
}

void DgramSocket::setTTL(unsigned char ttl)
{
    int rc=setsockopt(_sd, IPPROTO_IP,IP_MULTICAST_TTL,
                      (SocketOptT*)&ttl,sizeof(ttl));
    if(rc<0)
    {
        throw SocketError("setsockopt(IPPROTO_IP,IP_MULTICAST_TTL)");
    }
}

/*-------------------------- assignment -----------------------------------*/

/** \brief assignment
 */
const DgramSocket & DgramSocket::operator =(const DgramSocket &source)
{
    _sd=source._sd;
    return *this;
}

/*-------------------------- comparison -----------------------------------*/

/*-------------------------------------------------------------------------*\
 -  protected                                                              -
\*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*\
 -  private                                                                -
\*-------------------------------------------------------------------------*/

OSG_END_NAMESPACE
