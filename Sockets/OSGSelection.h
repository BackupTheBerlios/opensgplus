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

#ifndef _SELECTION_H_
#define _SELECTION_H_
#ifdef __sgi
#pragma once
#endif

//---------------------------------------------------------------------------
//  Includes
//---------------------------------------------------------------------------

#include <OSGConfig.h>

#ifdef WIN32
#include <windows.h>
#else
#include <sys/types.h>
#endif
#include <string>
#include "OSGSocketConfig.h"

OSG_BEGIN_NAMESPACE

//---------------------------------------------------------------------------
//  Forward References
//---------------------------------------------------------------------------
class Socket;

class OSG_SOCKET_DLLMAPPING Selection
{
    /*==========================  PUBLIC  =================================*/
  public:

    /*---------------------------------------------------------------------*/
    /*! \name                   Constructors                               */
    /*! \{                                                                 */

    Selection();
    Selection(const Selection &source);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructor                                 */
    /*! \{                                                                 */

    ~Selection();

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Selection functions                        */
    /*! \{                                                                 */

    void clear();
    void clearRead (const Socket &soc);
    void clearWrite(const Socket &soc);
    void setRead   (const Socket &soc);
    void setWrite  (const Socket &soc);
    bool isSetRead (const Socket &soc);
    bool isSetWrite(const Socket &soc);
    int  select    (double time);
    int  select    (double time,Selection &result) const;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   assignment                                 */
    /*! \{                                                                 */

	const Selection & operator =(const Selection &source);

    /*! \}                                                                 */

    /*=========================  PROTECTED  ===============================*/
  protected:
    fd_set _fdSetRead;
    fd_set _fdSetWrite;

    /*==========================  PRIVATE  ================================*/
  private:
	static char cvsid[];
};

OSG_END_NAMESPACE

#endif

