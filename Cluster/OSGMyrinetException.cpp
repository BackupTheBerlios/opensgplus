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
#define OSG_COMPILECLUSTERLIB

// do we have Myrinet configured in?
#ifdef OSG_WITH_MYRINET

#include <iostream>
#include <string.h>

#define OSG_COMPILECLUSTERLIB
#include <OSGMyrinetException.h>

OSG_BEGIN_NAMESPACE

/** \brief Constructor
 */
MyrinetException::MyrinetException():
    Exception()
{
    _what += "MyrinetLib: ";
}

/** \brief Constructor
 */
MyrinetError::MyrinetError(const char *function, gm_status_t st):
	MyrinetException()
{
    _what=_what + function + " " + getErrorStr( st ); 
}


const char *MyrinetException::getErrorStr( gm_status_t st)
{
    return gm_strerror( st );
}


OSG_END_NAMESPACE

#endif
