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

#ifndef _BINSOCKETMESSAGE_INL_
#define _BINSOCKETMESSAGE_INL_

// Application declarations
// to get ntons/ntohs
#ifdef WIN32
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif

OSG_BEGIN_NAMESPACE

/*---------------------------------------------------------------------*/
/*                      write message                                  */

inline void BinSocketMessage::putUInt32(const UInt32  value)
{
    Int32 net=htonl(value);
    _buffer.insert(_buffer.end(),(UInt8*)(&net),((UInt8*)(&net))+sizeof(net));
}

inline void BinSocketMessage::putInt32 (const Int32  value)
{
    Int32 net=htonl(value);
    _buffer.insert(_buffer.end(),(UInt8*)(&net),((UInt8*)(&net))+sizeof(net));
}

inline void BinSocketMessage::putUInt16(const UInt16  value)
{
    Int16 net=htons(value);
    _buffer.insert(_buffer.end(),(UInt8*)(&net),((UInt8*)(&net))+sizeof(net));
}

inline void BinSocketMessage::putInt16 (const Int16  value)
{
    Int16 net=htons(value);
    _buffer.insert(_buffer.end(),(UInt8*)(&net),((UInt8*)(&net))+sizeof(net));
}

inline void BinSocketMessage::putUInt8 (const UInt8   value)
{
    _buffer.push_back(value);
}

inline void BinSocketMessage::putInt8  (const Int8   value)
{
    UInt8 v=static_cast<UInt8>(value);
    _buffer.push_back(v);
}

inline void BinSocketMessage::putString(const string &value)
{
    putUInt32(value.size());
    if(value.size())
    {
        const UInt8 *s=(const UInt8*)(value.c_str());
        const UInt8 *e=s+value.size();
        _buffer.insert(_buffer.end(),s,e);
    }
}

inline void BinSocketMessage::putReal32(const Real32  value)
{
    putInt32(*((Int32*)(&value)));
}

/*---------------------------------------------------------------------*/
/*                      read message                                   */

inline void BinSocketMessage::getUInt32(UInt32  &value)
{
    Int32 net;
    memcpy(&net,&_buffer[_pos],sizeof(net));
    value=ntohl(net);
    _pos+=sizeof(net);
}

inline void BinSocketMessage::getInt32 (Int32  &value)
{
    Int32 net;
    memcpy(&net,&_buffer[_pos],sizeof(net));
    value=ntohl(net);
    _pos+=sizeof(net);
}

inline void BinSocketMessage::getUInt16(UInt16  &value)
{
    Int16 net=*((Int16 *)( &_buffer[_pos]));
    value=ntohs(net);
    _pos+=sizeof(net);
}

inline void BinSocketMessage::getInt16 (Int16  &value)
{
    Int16 net=*((Int16 *)( &_buffer[_pos]));
    value=ntohs(net);
    _pos+=sizeof(net);
}

inline void BinSocketMessage::getUInt8 (UInt8   &value)
{
    value=_buffer[_pos++];
}

inline void BinSocketMessage::getInt8  (Int8   &value)
{
    value=_buffer[_pos++];
}

inline void BinSocketMessage::getString(string &value)
{
    UInt32 size;
    getUInt32(size);
    if(!value.empty())
        value.erase();
    if(size)
    {
        value.insert(value.begin(),
                     (char*)&_buffer[_pos],
                     (char*)&_buffer[_pos+size]);
        _pos+=size;
    }
}

inline void BinSocketMessage::getReal32(Real32  &value)
{
    getInt32(*((Int32*)(&value)));
}



inline UInt32 BinSocketMessage::getUInt32(void)
{
    UInt32 value;
    getUInt32(value);
    return value;
}

inline Int32  BinSocketMessage::getInt32 (void)
{
    Int32 value;
    getInt32(value);
    return value;
}

inline UInt16 BinSocketMessage::getUInt16(void)
{
    UInt16 value;
    getUInt16(value);
    return value;
}

inline Int16  BinSocketMessage::getInt16 (void)
{
    Int16 value;
    getInt16(value);
    return value;
}

inline UInt8  BinSocketMessage::getUInt8 (void)
{
    UInt8 value;
    getUInt8(value);
    return value;
}

inline Int8   BinSocketMessage::getInt8  (void)
{
    Int8 value;
    getInt8(value);
    return value;
}

inline string BinSocketMessage::getString(void)
{
    string value;
    getString(value);
    return value;
}

inline Real32 BinSocketMessage::getReal32(void)
{
    Real32 value;
    getReal32(value);
    return value;
}

OSG_END_NAMESPACE

#endif







