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

#ifndef _BINSOCKETMESSAGE_H_
#define _BINSOCKETMESSAGE_H_
#ifdef __sgi
#pragma once
#endif

#include <vector>
#include <OSGSocketConfig.h>
#include <OSGBaseTypes.h>
#include <OSGSocketMessage.h>

OSG_BEGIN_NAMESPACE

/*! \ingroup baselib
 *  \brief Brief
 */

class OSG_SOCKET_DLLMAPPING BinSocketMessage : public SocketMessage
{
    /*==========================  PUBLIC  =================================*/
  public:

    typedef std::vector<UInt8> BufferType;

    /*---------------------------------------------------------------------*/
    /*! \name                   Constructors                               */
    /*! \{                                                                 */

    BinSocketMessage(void);
    BinSocketMessage(const BinSocketMessage &source);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructor                                 */
    /*! \{                                                                 */

    virtual ~BinSocketMessage(void);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                      Get                                     */
    /*! \{                                                                 */

    virtual UInt32       getSize   (void);
    virtual MemoryHandle getBuffer (void);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                      Misc                                    */
    /*! \{                                                                 */

    virtual void setSize           (UInt32 size);
    void clear                     (void);
    void reset                     (void);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name               write message                                  */
    /*! \{                                                                 */

    void putUInt32(const UInt32  value);
    void putInt32 (const Int32   value);
    void putUInt16(const UInt16  value);
    void putInt16 (const Int16   value);
    void putUInt8 (const UInt8   value);
    void putInt8  (const Int8    value);
    void putString(const string &value);
    void putReal32(const Real32  value);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name               read message                                   */
    /*! \{                                                                 */

    void getUInt32(UInt32 &value);
    void getInt32 (Int32  &value);
    void getUInt16(UInt16 &value);
    void getInt16 (Int16  &value);
    void getUInt8 (UInt8  &value);
    void getInt8  (Int8   &value);
    void getString(string &value);
    void getReal32(Real32 &value);

    UInt32 getUInt32(void);
    Int32  getInt32 (void);
    UInt16 getUInt16(void);
    Int16  getInt16 (void);
    UInt8  getUInt8 (void);
    Int8   getInt8  (void);
    string getString(void);
    Real32 getReal32(void);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Assignment                                */
    /*! \{                                                                 */

    BinSocketMessage& operator =(const BinSocketMessage &source);

    /*! \}                                                                 */
    /*=========================  PROTECTED  ===============================*/
  protected:

    /*---------------------------------------------------------------------*/
    /*! \name                      Member                                  */
    /*! \{                                                                 */

    BufferType               _buffer;
    UInt32                   _pos;

    /*! \}                                                                 */
    /*==========================  PRIVATE  ================================*/
  private:

    typedef SocketMessage Inherited;
};

//---------------------------------------------------------------------------
//   Exported Types
//---------------------------------------------------------------------------

// class pointer

typedef BinSocketMessage *BinSocketMessageP;

OSG_END_NAMESPACE

#define OSG_BINSOCKETMESSAGEHEADER_CVSID "@(#)$Id:$"

#include "OSGBinSocketMessage.inl"

#endif /* _BINSOCKETMESSAGE_H_ */
