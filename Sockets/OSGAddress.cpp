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

#include <sys/types.h>

#ifndef WIN32
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

/** \class Address
 *  \ingroup SocketsLib
 *  \brief Network address
 *
 * The class Address holds an ip-address and a socket port number.
 * It is used to connect, and sendTo Sockets.
 * 
 * Usage:
 * <PRE>
 * Address a("colossus.world.com",3334);
 * socket.bind(a);
 * cout << a.getHost() << endl;
 * cout << a.getPort() << endl;
 * </PRE>
 **/

/** \brief cvsid **/
char Address::cvsid[] = "@(#)$Id:$";

/*------------- constructors & destructors --------------------------------*/

/** \brief Constructor
 *
 * Create a socket address for the given port and host.
 *
 * \param host  Host as hostname or as number 111.222.333.444.
 * \param port  Port id
 */
Address::Address(const char *host,int port)
{
    memset(&_sockaddr,0,sizeof(_sockaddr));
    _sockaddr.sin_family = AF_INET;
    if(host)
        setHost(string(host));
    setPort(port);
}

/** \brief copy Constructor
 */
Address::Address(const Address &source)
{
    _sockaddr = source._sockaddr;
}

/** \brief Destructor
 */
Address::~Address()
{
}

/** \brief set the port number
 *
 * \param port  Port number as an int
 */
void Address::setPort(int port)
{
    _sockaddr.sin_port = htons( port );
}

/** \brief set host name
 *
 * \param host   Host as name or number
 */
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

/** \brief Get host as number string
 *
 * \result   Host as number e.g. 133.33.44.55
 */
string Address::getHost() const
{
    return string(inet_ntoa(_sockaddr.sin_addr));
}

/** \brief Get host as name
 *
 * \result   Hostname if found. Otherwise number string.
 */
string Address::getHostByName() const
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

/** \brief Get a pointer to the sockaddr struct
 *
 * \result   sockaddr struct
 */
sockaddr *Address::getSockAddr() const
{
    return (struct sockaddr *)&_sockaddr;
}

/** \brief Get the size of the sockaddr struct
 *
 * \result   size of sockaddr
 */
int Address::getSockAddrSize() const
{
    return sizeof(struct sockaddr_in);
}

/** \brief Get port number
 *
 * \result   port as int
 */
int Address::getPort() const
{
    return ntohs(_sockaddr.sin_port);
}

/*-------------------------- assignment -----------------------------------*/

/*-------------------------- comparison -----------------------------------*/

/** \brief compare equal
 */
bool Address::operator == (const Address &other) const
{
    return _sockaddr.sin_addr.s_addr == other._sockaddr.sin_addr.s_addr &&
           _sockaddr.sin_port        == other._sockaddr.sin_port;
}

/** \brief compare not equal
 */
bool Address::operator != (const Address &other) const
{
    return ! (*this == other);
}

/** \brief compare less
 */
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


/** \brief cvs id
 */
char BroadcastAddress::cvsid[] = "@(#)$Id:$";

/** \brief cvs id
 */
char AnyAddress::cvsid[] = "@(#)$Id:$";

/***************************************************************************\
 *                           Instance methods                              *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  public                                                                 -
\*-------------------------------------------------------------------------*/

/*------------- constructors & destructors --------------------------------*/

/** \class BroadcastAddress
 *  \ingroup SocketsLib
 *  \brief A BroadcastAddress is an Address used for broadcasting.
 *
 *  The hostfield is set to INADDR_BROADCAST.
 **/

/** \brief Constructor
 */
BroadcastAddress::BroadcastAddress(int port)
{
    _sockaddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    setPort(port);
}

/*! \class AnyAddress
 *  \ingroup SocketsLib
 *  \brief A AnyAddress is an Address with host set to INADDR_ANY
 * 
 * Use This to bind a port to all availabel network interfaces.
 * <PRE>
 * StreamSocket s;
 * s.open();
 * s.bind(AnyAddress(5555));
 * </PRE>
 *
*/

/** \brief Constructor
 */
AnyAddress::AnyAddress(int port)
{
    _sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    setPort(port);
}

OSG_END_NAMESPACE
