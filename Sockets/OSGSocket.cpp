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
#include <Winsock2.h>
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
#include <OSGSocket.h>

OSG_BEGIN_NAMESPACE

/***************************************************************************\
 *                               Types                                     *
\***************************************************************************/

/***************************************************************************\
 *                           Class variables                               *
\***************************************************************************/

int Socket::initialized=0;
char Socket::cvsid[] = "@(#)$Id:$";

/***************************************************************************\
 *                           Class methods                                 *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  public                                                                 -
\*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*\
 -  protected                                                              -
\*-------------------------------------------------------------------------*/

int Socket::getLastError()
{
#ifdef WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
}

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
Socket::Socket():
    _sd(-1)
{
    if(!initialized)
    {
        initialized=1;
#ifdef WIN32
        WSADATA wsaData;
        
        WORD wVersionRequested = MAKEWORD( 2, 2 ); 
        if(WSAStartup( wVersionRequested, &wsaData )!=0)
        {
            throw SocketException("WSAStartup()",getLastError());
        }
#endif
    }
}

Socket::Socket(const Socket &source):
    _sd(source._sd)
{
}

/** \brief Destructor
 */
Socket::~Socket()
{
}

/*------------------------------ access -----------------------------------*/

/*---------------------------- properties ---------------------------------*/

/*-------------------------- your_category---------------------------------*/

void Socket::close()
{
    ::close(_sd);
}

int Socket::read(void *buf,int size)
{
    int len;

    len=::recv(_sd,(char*)buf,size,0);
    if(len==-1)
    {
        throw SocketException("recv()",getLastError());
    }
    return len;
}

int Socket::peek(void *buf,int size)
{
    int len;
    //    socklen_t addrLen=from.getSockAddrSize();

    len=recv(_sd,
             (char*)buf,
             size,
             MSG_PEEK);
    if(len==-1)
    {
        throw SocketException("recv()",getLastError());
    }
    return len;
}

int Socket::write(const void *buf,int size)
{
    int len=::send(_sd,(char*)buf,size,0);
    if(len==-1)
    {
        throw SocketException("send()",getLastError());
    }
    return len;
}

void Socket::bind(const Address &address)
{
    Address result=address;
    
    if( ::bind(_sd,
               result.getSockAddr(),
               result.getSockAddrSize()) < 0)
    {
        if(getLastError()==
#if defined WIN32
            WSAEADDRINUSE 
#else
			EADDRINUSE
#endif
		)
        {
            throw SocketInUse("bind():",getLastError());
        }
        else
        {
            throw SocketException("bind()",getLastError());
        }
    }
}

void Socket::listen(int maxPending)
{
    if(::listen(_sd,maxPending)<0)
    {
        throw SocketException("listen()",getLastError());
    }
}

void Socket::connect(const Address &address)
{
    if( ::connect(_sd,
                  address.getSockAddr(),
                  address.getSockAddrSize()) )
    {
        throw SocketException("connect()",getLastError());
    }
}

void Socket::setReusePort(bool value)
{
    int v=(int)value;
    ::setsockopt(_sd,SOL_SOCKET,SO_REUSEADDR,(SocketOptT*)&v,sizeof(v));
}

void Socket::setBlocking(bool value)
{
#ifndef WIN32
    int val=0;
    
    if(value==false)
        val=O_NDELAY;
    if (fcntl(_sd, F_GETFL, &val) < 0) 
    {
        throw SocketException("fcntl()",getLastError());
    }    
    val|=O_NDELAY;
    if(value)
    {
        val^=O_NDELAY;
    }
    if (fcntl(_sd, F_SETFL, val) < 0) 
    {
        throw SocketException("fcntl()",getLastError());
    }    
#else
    u_long ulVal = !value;
    if( (ioctlsocket(_sd, FIONBIO, &ulVal)) != 0) 
    {
        throw SocketException("ioctlsocket()",getLastError());
    }    
#endif
}

Address Socket::getAddress()
{
    Address result;
    SocketLenT len;

    len=result.getSockAddrSize();
    if( ::getsockname(_sd,result.getSockAddr(),&len) < 0)
    {
        throw SocketException("getsockname()",getLastError());
    }
    return result;
}

/*-------------------------- assignment -----------------------------------*/

/** \brief assignment
 */

const Socket & Socket::operator =(const Socket &source)
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
