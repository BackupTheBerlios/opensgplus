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
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#endif
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "OSGSocketConfig.h"
#include "OSGAddress.h"
#include "OSGSocketException.h"

OSG_BEGIN_NAMESPACE

using namespace std;

/***************************************************************************\
 *                               Types                                     *
\***************************************************************************/

/***************************************************************************\
 *                           Class variables                               *
\***************************************************************************/

char Address::cvsid[] = "@(#)$Id:$";

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
Address::Address(const char *host,int port)
{
    memset(&_sockaddr,0,sizeof(_sockaddr));
    _sockaddr.sin_family = AF_INET;
    if(host)
        setHost(host);
    setPort(port);
}

Address::Address(const Address &source)
{
    _sockaddr = source._sockaddr;
}

/** \brief Destructor
 */
Address::~Address()
{
}

/*------------------------------ access -----------------------------------*/

/*---------------------------- properties ---------------------------------*/

/*-------------------------- your_category---------------------------------*/

void Address::setPort(int port)
{
    _sockaddr.sin_port = htons( port );
}

void Address::setHost(const string &host)
{
    struct hostent *hent;
    char const *c;

    // number or name ?
    for(c=host.c_str();
        *c!='\0' && (isdigit(*c) || *c == '.');
        c++);
    if(! *c )
    {
        // inet_aton(const char *cp, struct in_addr *pin);

        // ip number was given
        _sockaddr.sin_addr.s_addr = inet_addr(host.c_str());
    }
    else
    {
        // get address of host by name
        hent = gethostbyname(host.c_str());
        if(hent == NULL)
        {
            throw SocketHostError("gethostbyname()");
        }
        // set address
        _sockaddr.sin_addr = *(struct in_addr *) hent->h_addr;
    }
}

string Address::getHost() const
{
    struct hostent *hent;
    string result;

    hent=gethostbyaddr((SocketAddrT*)getSockAddr(),
                       getSockAddrSize(),AF_INET);
    if(hent == NULL)
    {
        // if no host assigned or host unknown
        // then return ip address
        result=inet_ntoa(_sockaddr.sin_addr);
    }
    else
    {
        result=hent->h_name;
    }
    return result;
}

sockaddr *Address::getSockAddr() const
{
    return (struct sockaddr *)&_sockaddr;
}

int Address::getSockAddrSize() const
{
    return sizeof(struct sockaddr_in);
}

int Address::getPort() const
{
    return ntohs(_sockaddr.sin_port);
}

/*-------------------------- assignment -----------------------------------*/

/** \brief assignment
 */

/*-------------------------- comparison -----------------------------------*/

bool Address::operator == (const Address &other) const
{
    return _sockaddr.sin_addr.s_addr == other._sockaddr.sin_addr.s_addr &&
           _sockaddr.sin_port        == other._sockaddr.sin_port;
}

bool Address::operator != (const Address &other) const
{
    return ! (*this == other);
}

bool Address::operator < (const Address &other) const
{
    return _sockaddr.sin_addr.s_addr < other._sockaddr.sin_addr.s_addr ||
    (
        _sockaddr.sin_addr.s_addr == other._sockaddr.sin_addr.s_addr &&
        _sockaddr.sin_port        <  other._sockaddr.sin_port
    );
}

/*-------------------------------------------------------------------------*\
 -  protected                                                              -
\*-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*\
 -  private                                                                -
\*-------------------------------------------------------------------------*/


/***************************************************************************\
 *                           Class variables                               *
\***************************************************************************/

char BroadcastAddress::cvsid[] = "@(#)$Id:$";
char AnyAddress::cvsid[] = "@(#)$Id:$";

/***************************************************************************\
 *                           Instance methods                              *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  public                                                                 -
\*-------------------------------------------------------------------------*/

/*------------- constructors & destructors --------------------------------*/

/** \brief Constructor
 */
BroadcastAddress::BroadcastAddress(int port)
{
    _sockaddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    setPort(port);
}

/** \brief Constructor
 */
AnyAddress::AnyAddress(int port)
{
    _sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    setPort(port);
}

OSG_END_NAMESPACE
