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

#ifndef _SOCKETMESSAGE_H_
#define _SOCKETMESSAGE_H_
#ifdef __sgi
#pragma once
#endif

#include <OSGSocketConfig.h>
#include <OSGBaseTypes.h>

OSG_BEGIN_NAMESPACE

/*! \ingroup socketslib
 *  \brief Brief
 */

class OSG_SOCKET_DLLMAPPING SocketMessage 
{
    /*==========================  PUBLIC  =================================*/
  public:
    
    struct Header
    {
        UInt32 size;
    };

    /*---------------------------------------------------------------------*/
    /*! \name                   Constructors                               */
    /*! \{                                                                 */

    SocketMessage(void);
    SocketMessage(const SocketMessage &source);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructor                                 */
    /*! \{                                                                 */

    virtual ~SocketMessage(void);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                      Get                                     */
    /*! \{                                                                 */

    virtual UInt32        getSize   (void)=0;
    virtual MemoryHandle  getBuffer (void)=0;
            Header       &getHeader (void);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                      Set                                     */
    /*! \{                                                                 */

    virtual void setSize                (UInt32 size)=0;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                      Assignment                              */
    /*! \{                                                                 */

    SocketMessage &operator =(const SocketMessage &source);

    /*! \}                                                                 */
    /*=========================  PROTECTED  ===============================*/
  protected:

    /*---------------------------------------------------------------------*/
    /*! \name                      Member                                  */
    /*! \{                                                                 */

    /*! \}                                                                 */
    /*==========================  PRIVATE  ================================*/
  private:
};

//---------------------------------------------------------------------------
//   Exported Types
//---------------------------------------------------------------------------

// class pointer

typedef SocketMessage *SocketMessageP;

OSG_END_NAMESPACE

#define OSG_SOCKETMESSAGEHEADER_CVSID "@(#)$Id:$"

#endif /* _SOCKETMESSAGE_H_ */
