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

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#define OSG_COMPILESOCKETLIB
#include <OSGSocketConfig.h>
#include <OSGSocket.h>
#include <OSGSocketException.h>

OSG_BEGIN_NAMESPACE

/** \brief Constructor
 */
SocketException::SocketException():
    Exception()
{
    _what+="SocketLib: ";
}

/** \brief Constructor
 */
SocketError::SocketError(const char *function):SocketException()
{
    char estr[6];
	const char *s;
	
    sprintf(estr,"%5d",Socket::getError());
    _what=_what + function + " " + 
          estr;
	if ( s = Socket::getErrorStr() )
		_what = _what + " " + s; 
}

/** \brief Constructor
 */
SocketHostError::SocketHostError(const char *function):SocketException()
{
    char estr[6];
	const char *s;

    sprintf(estr,"%5d",Socket::getHostError());
    _what=_what + function + " " + 
          estr + " " +
          Socket::getHostErrorStr(); 
	if ( s = Socket::getHostErrorStr() )
		_what = _what + " " + s; 
}

/** \brief Constructor
 */
SocketInUse::SocketInUse(const char *function):
    SocketError(function)
{
}

/** \brief Constructor
 */
SocketConnReset::SocketConnReset(const char *function):
    SocketError(function)
{
}
OSG_END_NAMESPACE

