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
#include <sys/ioctl.h>
#endif

#include <errno.h>
#include <stdio.h>
#include <math.h>
#include <map>
#include <OSGSocketConfig.h>
#include <OSGAddress.h>
#include <OSGStreamSocket.h>

OSG_BEGIN_NAMESPACE

/***************************************************************************\
 *                               Types                                     *
\***************************************************************************/

/***************************************************************************\
 *                           Class variables                               *
\***************************************************************************/

char StreamSocket::cvsid[] = "@(#)$Id:$";

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
StreamSocket::StreamSocket():
    Socket()
{
}

StreamSocket::StreamSocket(const StreamSocket &source):
    Socket(source)
{
}

/** \brief Destructor
 */

/*------------------------------ access -----------------------------------*/

/*---------------------------- properties ---------------------------------*/

/*-------------------------- your_category---------------------------------*/

void StreamSocket::open()
{
    _sd = ::socket(AF_INET, SOCK_STREAM, 0);
    if(_sd<0)
    {
        throw SocketError("socket()");
    }
}

StreamSocket StreamSocket::acceptFrom(Address &address)
{
    StreamSocket client;
    SocketLenT len;
    
    len=address.getSockAddrSize();
    client._sd=::accept(_sd,
                        address.getSockAddr(),
                        &len);
    if(client._sd<0)
    {
        throw SocketError("accept()");
    }
    return client;
}

StreamSocket StreamSocket::accept()
{
    Address addr;
    return acceptFrom(addr);
}

void StreamSocket::setDelay(bool value)
{
    int rc,on;
    on=!value;
    rc=setsockopt(_sd, IPPROTO_TCP, TCP_NODELAY, 
                  (SocketOptT*)&on, sizeof(on));
    if(rc<0)
    {
        throw SocketError("setsockopt(,SOCK_STREAM,TCP_NODELAY)");
    }
}

int StreamSocket::read(void *buf,int size)
{
    int readSize;
    int pos=0;

    while(size)
    {
        readSize=::read(_sd,((char*)buf)+pos,size);
        if(readSize<=0)
        {
            throw SocketError("read()");
        }
        size-=readSize;
        pos +=readSize;
    }
    return pos;
}

int StreamSocket::write(const void *buf,int size)
{
    int writeSize;
    int pos=0;

    while(size)
    {
        writeSize=::write(_sd,(const char*)buf+pos,size);
        if(writeSize<=0)
        {
            throw SocketError("send()");
        }
        size-=writeSize;
        pos+=writeSize;
    }
    return pos;
}

int StreamSocket::getNReadBytes(void)
{
    int value;
    if(::ioctl(_sd, FIONREAD, &value)<0)
    {    
        throw SocketError("ioctl()");
    }
    return value;
}

/*-------------------------- assignment -----------------------------------*/

/** \brief assignment
 */
const StreamSocket & StreamSocket::operator =(const StreamSocket &source)
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
