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
#include <sys/ioctl.h>
#endif
#include <errno.h>
#include <stdio.h>
#include <math.h>
#include <map>
#include <OSGSocketConfig.h>
#include <OSGAddress.h>
#include <OSGSocket.h>
#include <OSGSelection.h>

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

int Socket::getError()
{
#ifdef WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
}

int Socket::getHostError()
{
#ifdef WIN32
    return WSAGetLastError();
#else
    return h_errno;
#endif
}

const char *Socket::getErrorStr()
{
#ifdef WIN32
    switch(getError())
    {
        case WSAEINTR: return "WSAEINTR"; 
        case WSAEBADF: return "WSAEBADF"; 
        case WSEACCES: return "WSEACCES"; 
        case WSAEFAULT: return "WSAEFAULT"; 
        case WSAEINVAL: return "WSAEINVAL"; 
        case WSAEMFILE: return "WSAEMFILE"; 
        case WSAEWOULDBLOCK: return "WSAEWOULDBLOCK"; 
        case WSAEINPROGRESS: return "WSAEINPROGRESS"; 
        case WSAEALREADY: return "WSAEALREADY"; 
        case WSAENOTSOCK: return "WSAENOTSOCK"; 
        case WSAEDESTADDRREQ: return "WSAEDESTADDRREQ"; 
        case WSAEMSGSIZE: return "WSAEMSGSIZE"; 
        case WSAEPROTOTYPE: return "WSAEPROTOTYPE"; 
        case WSAENOPROTOOPT: return "WSAENOPROTOOPT"; 
        case WSAEPROTONOSUPPORT: return "WSAEPROTONOSUPPORT"; 
        case WSAESOCKTNOSUPPORT: return "WSAESOCKTNOSUPPORT"; 
        case WSAEOPNOTSUPP: return "WSAEOPNOTSUPP"; 
        case WSAEPFNOSUPPORT: return "WSAEPFNOSUPPORT"; 
        case WSAEAFNOSUPPORT: return "WSAEAFNOSUPPORT"; 
        case WSAEADDRINUSE: return "WSAEADDRINUSE"; 
        case WSAEADDRNOTAVAIL: return "WSAEADDRNOTAVAIL"; 
        case WSAENETDOWN: return "WSAENETDOWN"; 
        case WSAENETUNREACH: return "WSAENETUNREACH"; 
        case WSAENETRESET: return "WSAENETRESET"; 
        case WSAECONNABORTED: return "WSAECONNABORTED"; 
        case WSAECONNRESET: return "WSAECONNRESET"; 
        case WSAENOBUFS: return "WSAENOBUFS"; 
        case WSAEISCONN: return "WSAEISCONN"; 
        case WSAENOTCONN: return "WSAENOTCONN"; 
        case WSAESHUTDOWN: return "WSAESHUTDOWN"; 
        case WSAETOOMANYREFS: return "WSAETOOMANYREFS"; 
        case WSAETIMEDOUT: return "WSAETIMEDOUT"; 
        case WSAECONNREFUSED: return "WSAECONNREFUSED"; 
        case WSAELOOP: return "WSAELOOP"; 
        case WSAENAMETOOLONG: return "WSAENAMETOOLONG"; 
        case WSAEHOSTDOWN: return "WSAEHOSTDOWN"; 
        case WSAEHOSTUNREACH: return "WSAEHOSTUNREACH"; 
        case WSASYSNOTREADY: return "WSASYSNOTREADY"; 
        case WSAVERNOTSUPPORTED: return "WSAVERNOTSUPPORTED"; 
        case WSANOTINITIALISED: return "WSANOTINITIALISED"; 
        case WSAHOST_NOT_FOUND: return "WSAHOST_NOT_FOUND"; 
        case WSATRY_AGAIN: return "WSATRY_AGAIN"; 
        case WSANO_RECOVERY: return "WSANO_RECOVERY"; 
        case WSANO_DATA: return "WSANO_DATA"; 
    }
#endif
    return strerror(getError());
}

const char *Socket::getHostErrorStr()
{
#ifdef WIN32
    return strerror(getHostError());
#else
    return hstrerror(getHostError());
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
            throw SocketError("WSAStartup()");
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
#ifdef WIN32
    ::closesocket(_sd);
#else
    ::close(_sd);
#endif
}

int Socket::recvAvailable(void *buf,int size)
{
    int len;

    len=::recv(_sd,(char*)buf,size,0);
    if(len==-1)
    {
#if defined WIN32
        if(getError()==WSAECONNRESET)
        {
            throw SocketConnReset("recvAvailable()");
        }
#endif
        throw SocketError("recv()");
    }
    return len;
}

int Socket::recv(void *buf,int size)
{
    int readSize;
    int pos=0;

    while(size)
    {
        readSize=::recv(_sd,((char*)buf)+pos,size,0);
        if(readSize<0)
        {
#if defined WIN32
            if(getError()==WSAECONNRESET)
            {
                throw SocketConnReset("recv");
            }
#endif
            throw SocketError("recv()");
        }
        if(readSize==0)
        {
            return 0;
        }
        size-=readSize;
        pos +=readSize;
    }
    return pos;
}

int Socket::peek(void *buf,int size)
{
    int readSize;
    int pos=0;

    do
    {
        readSize=::recv(_sd,((char*)buf)+pos,size,MSG_PEEK);
        if(readSize<0)
        {
#if defined WIN32
            if(getError()==WSAECONNRESET)
            {
                throw SocketConnReset("peek");
            }
#endif
            throw SocketError("peek");
        }
        if(readSize==0)
        {
            return 0;
        }
    }
    while(readSize!=size);
    return readSize;
}

int Socket::send(const void *buf,int size)
{
    int writeSize;
    int pos=0;
    while(size)
    {
        writeSize=::send(_sd,((const char*)buf)+pos,size,0);
        if(writeSize==-1)
        {
            throw SocketError("send()");
        }
        if(writeSize==0)
        {
            return 0;
        }
        size-=writeSize;
        pos+=writeSize;
    }
    return pos;
}

void Socket::bind(const Address &address)
{
    Address result=address;
    
    if( ::bind(_sd,
               result.getSockAddr(),
               result.getSockAddrSize()) < 0)
    {
        if(getError()==
#if defined WIN32
            WSAEADDRINUSE 
#else
			EADDRINUSE
#endif
		)
        {
            throw SocketInUse("bind()");
        }
        else
        {
            throw SocketError("bind()");
        }
    }
}

void Socket::listen(int maxPending)
{
    if(::listen(_sd,maxPending)<0)
    {
        throw SocketError("listen()");
    }
}

void Socket::connect(const Address &address)
{
    if( ::connect(_sd,
                  address.getSockAddr(),
                  address.getSockAddrSize()) )
    {
        throw SocketError("connect()");
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
        throw SocketError("fcntl()");
    }    
    val|=O_NDELAY;
    if(value)
    {
        val^=O_NDELAY;
    }
    if (fcntl(_sd, F_SETFL, val) < 0) 
    {
        throw SocketError("fcntl()");
    }    
#else
    u_long ulVal = !value;
    if( (ioctlsocket(_sd, FIONBIO, &ulVal)) != 0) 
    {
        throw SocketError("ioctlsocket()");
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
        throw SocketError("getsockname()");
    }
    return result;
}

void Socket::setReadBufferSize(int size)
{
    int v=(int)size;
    ::setsockopt(_sd,SOL_SOCKET,SO_RCVBUF,(SocketOptT*)&v,sizeof(v));
}

void Socket::setWriteBufferSize(int size)
{
    int v=(int)size;
    ::setsockopt(_sd,SOL_SOCKET,SO_SNDBUF,(SocketOptT*)&v,sizeof(v));
}

int Socket::getReadBufferSize() 
{
    int v;
    SocketLenT len=sizeof(v);
    ::getsockopt(_sd,SOL_SOCKET,SO_RCVBUF,(SocketOptT*)&v,&len);
    return v;
}

int Socket::getWriteBufferSize() 
{
    int v;
    SocketLenT len=sizeof(v);
    ::getsockopt(_sd,SOL_SOCKET,SO_SNDBUF,(SocketOptT*)&v,&len);
    return v;
}

int Socket::getAvailable(void)
{
#ifndef WIN32
    int value;
    if(::ioctl(_sd, FIONREAD, &value)<0)
    {    
        throw SocketError("ioctl()");
    }
    return value;
#else
    u_long ulVal;
    if( (ioctlsocket(_sd, FIONREAD, &ulVal)) != 0) 
    {    
        throw SocketError("ioctlsocket()");
    }
    return (int)ulVal;
#endif
}

Bool Socket::waitReadable(double duration)
{
    Selection selection;
    selection.setRead(*this);
    if(selection.select(duration)==1)
        return true;
    else
        return false;
}

Bool Socket::waitWritable(double duration)
{
    Selection selection;
    selection.setWrite(*this);
    if(selection.select(duration)==1)
        return true;
    else
        return false;
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
