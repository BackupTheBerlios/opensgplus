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

#include <string.h>
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

#include <OSGSocketConfig.h>
#include <OSGSocketException.h>
#include <OSGSocket.h>
#include <OSGSelection.h>

OSG_BEGIN_NAMESPACE

/***************************************************************************\
 *                               Types                                     *
\***************************************************************************/

/***************************************************************************\
 *                           Class variables                               *
\***************************************************************************/

char Selection::cvsid[] = "@(#)$Id: $";

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

Selection::Selection()
{
    clear();
}

Selection::Selection(const Selection &source):
    _fdSetRead (source._fdSetRead),
    _fdSetWrite(source._fdSetWrite)
{
}

/** \brief Destructor
 */

Selection::~Selection()
{
}

/*------------------------------ access -----------------------------------*/

/*---------------------------- properties ---------------------------------*/

/*-------------------------- your_category---------------------------------*/

void Selection::clear()
{
    FD_ZERO(&_fdSetRead);
    FD_ZERO(&_fdSetWrite);
}

void Selection::clearRead(const Socket &sock)
{
    FD_CLR(sock._sd,&_fdSetRead);
}

void Selection::clearWrite(const Socket &sock)
{
    FD_CLR(sock._sd,&_fdSetWrite);
}

void Selection::setRead(const Socket &sock)
{
    FD_SET(sock._sd,&_fdSetRead);
}

void Selection::setWrite(const Socket &sock)
{
    FD_SET(sock._sd,&_fdSetWrite);
}

int Selection::select(double duration)
{
    timeval tVal,*tValP;
    int count;
    
    if(duration<0)
    {
        tValP=NULL;
    }
    else
    {       
        tVal.tv_sec  = int( duration );
        tVal.tv_usec = int( (duration-tVal.tv_sec)*1000000 );
        tValP=&tVal;
    }
    count=::select(FD_SETSIZE, 
                   &_fdSetRead, 
                   &_fdSetWrite,
                   NULL,
                   tValP);
    if(count < 0)
    {
        throw SocketError("select()");
    }
    return count;
}

int Selection::select(double duration,Selection &result) const
{
    result=*this;
    return result.select(duration);
}

bool Selection::isSetRead(const Socket &sock)
{
    if(FD_ISSET(sock._sd, &_fdSetRead))
        return true;
    else
        return false;
}

bool Selection::isSetWrite(const Socket &sock)
{
    if(FD_ISSET(sock._sd, &_fdSetWrite))
        return true;
    else
        return false;
}

/*-------------------------- assignment -----------------------------------*/

/** \brief assignment
 */

const Selection & Selection::operator =(const Selection &source)
{
    _fdSetRead =source._fdSetRead;
    _fdSetWrite=source._fdSetWrite;
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

