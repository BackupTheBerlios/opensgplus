/*---------------------------------------------------------------------------*\
 *                                OpenSG                                     *
 *                                                                           *
 *                                                                           *
 *               Copyright (C) 2000-2002 by the OpenSG Forum                 *
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

/*****************************************************************************\
 *****************************************************************************
 **                                                                         **
 **                  This file is automatically generated.                  **
 **                                                                         **
 **          Any changes made to this file WILL be lost when it is          **
 **           regenerated, which can become necessary at any time.          **
 **                                                                         **
 **     Do not change this file, changes should be done in the derived      **
 **     class DVRLookupTable!
 **                                                                         **
 *****************************************************************************
\*****************************************************************************/

#include <OSGConfig.h>

OSG_BEGIN_NAMESPACE


//! access the type of the class
inline
OSG::FieldContainerType &DVRLookupTableBase::getClassType(void)
{
    return _type; 
} 

//! access the numerical type of the class
inline
OSG::UInt32 DVRLookupTableBase::getClassTypeId(void) 
{
    return _type.getId(); 
} 

//! create a new instance of the class
inline
DVRLookupTablePtr DVRLookupTableBase::create(void) 
{
    DVRLookupTablePtr fc; 

    if(getClassType().getPrototype() != OSG::NullFC) 
    {
        fc = DVRLookupTablePtr::dcast(
            getClassType().getPrototype()-> shallowCopy()); 
    }
    
    return fc; 
}

//! create an empty new instance of the class, do not copy the prototype
inline
DVRLookupTablePtr DVRLookupTableBase::createEmpty(void) 
{ 
    DVRLookupTablePtr returnValue; 
    
    newPtr(returnValue); 

    return returnValue; 
}


/*------------------------------ get -----------------------------------*/

//! Get the DVRLookupTable::_sfDimension field.
inline
SFUInt8 *DVRLookupTableBase::getSFDimension(void)
{
    return &_sfDimension;
}

//! Get the DVRLookupTable::_mfSize field.
inline
MFUInt32 *DVRLookupTableBase::getMFSize(void)
{
    return &_mfSize;
}

//! Get the DVRLookupTable::_sfChannel field.
inline
SFUInt8 *DVRLookupTableBase::getSFChannel(void)
{
    return &_sfChannel;
}

//! Get the DVRLookupTable::_mfData field.
inline
MFUInt8 *DVRLookupTableBase::getMFData(void)
{
    return &_mfData;
}

//! Get the DVRLookupTable::_mfDataR field.
inline
MFReal32 *DVRLookupTableBase::getMFDataR(void)
{
    return &_mfDataR;
}

//! Get the DVRLookupTable::_mfDataG field.
inline
MFReal32 *DVRLookupTableBase::getMFDataG(void)
{
    return &_mfDataG;
}

//! Get the DVRLookupTable::_mfDataB field.
inline
MFReal32 *DVRLookupTableBase::getMFDataB(void)
{
    return &_mfDataB;
}

//! Get the DVRLookupTable::_mfDataA field.
inline
MFReal32 *DVRLookupTableBase::getMFDataA(void)
{
    return &_mfDataA;
}

//! Get the DVRLookupTable::_sfTouched field.
inline
SFBool *DVRLookupTableBase::getSFTouched(void)
{
    return &_sfTouched;
}


//! Get the value of the DVRLookupTable::_sfDimension field.
inline
UInt8 &DVRLookupTableBase::getDimension(void)
{
    return _sfDimension.getValue();
}

//! Get the value of the DVRLookupTable::_sfDimension field.
inline
const UInt8 &DVRLookupTableBase::getDimension(void) const
{
    return _sfDimension.getValue();
}

//! Set the value of the DVRLookupTable::_sfDimension field.
inline
void DVRLookupTableBase::setDimension(const UInt8 &value)
{
    _sfDimension.setValue(value);
}

//! Get the value of the DVRLookupTable::_sfChannel field.
inline
UInt8 &DVRLookupTableBase::getChannel(void)
{
    return _sfChannel.getValue();
}

//! Get the value of the DVRLookupTable::_sfChannel field.
inline
const UInt8 &DVRLookupTableBase::getChannel(void) const
{
    return _sfChannel.getValue();
}

//! Set the value of the DVRLookupTable::_sfChannel field.
inline
void DVRLookupTableBase::setChannel(const UInt8 &value)
{
    _sfChannel.setValue(value);
}

//! Get the value of the DVRLookupTable::_sfTouched field.
inline
bool &DVRLookupTableBase::getTouched(void)
{
    return _sfTouched.getValue();
}

//! Get the value of the DVRLookupTable::_sfTouched field.
inline
const bool &DVRLookupTableBase::getTouched(void) const
{
    return _sfTouched.getValue();
}

//! Set the value of the DVRLookupTable::_sfTouched field.
inline
void DVRLookupTableBase::setTouched(const bool &value)
{
    _sfTouched.setValue(value);
}


//! Get the value of the \a index element the DVRLookupTable::_mfSize field.
inline
UInt32 &DVRLookupTableBase::getSize(const UInt32 index)
{
    return _mfSize[index];
}

//! Get the DVRLookupTable::_mfSize field.
inline
MFUInt32 &DVRLookupTableBase::getSize(void)
{
    return _mfSize;
}

//! Get the DVRLookupTable::_mfSize field.
inline
const MFUInt32 &DVRLookupTableBase::getSize(void) const
{
    return _mfSize;
}

//! Get the value of the \a index element the DVRLookupTable::_mfData field.
inline
UInt8 &DVRLookupTableBase::getData(const UInt32 index)
{
    return _mfData[index];
}

//! Get the DVRLookupTable::_mfData field.
inline
MFUInt8 &DVRLookupTableBase::getData(void)
{
    return _mfData;
}

//! Get the DVRLookupTable::_mfData field.
inline
const MFUInt8 &DVRLookupTableBase::getData(void) const
{
    return _mfData;
}

//! Get the value of the \a index element the DVRLookupTable::_mfDataR field.
inline
Real32 &DVRLookupTableBase::getDataR(const UInt32 index)
{
    return _mfDataR[index];
}

//! Get the DVRLookupTable::_mfDataR field.
inline
MFReal32 &DVRLookupTableBase::getDataR(void)
{
    return _mfDataR;
}

//! Get the DVRLookupTable::_mfDataR field.
inline
const MFReal32 &DVRLookupTableBase::getDataR(void) const
{
    return _mfDataR;
}

//! Get the value of the \a index element the DVRLookupTable::_mfDataG field.
inline
Real32 &DVRLookupTableBase::getDataG(const UInt32 index)
{
    return _mfDataG[index];
}

//! Get the DVRLookupTable::_mfDataG field.
inline
MFReal32 &DVRLookupTableBase::getDataG(void)
{
    return _mfDataG;
}

//! Get the DVRLookupTable::_mfDataG field.
inline
const MFReal32 &DVRLookupTableBase::getDataG(void) const
{
    return _mfDataG;
}

//! Get the value of the \a index element the DVRLookupTable::_mfDataB field.
inline
Real32 &DVRLookupTableBase::getDataB(const UInt32 index)
{
    return _mfDataB[index];
}

//! Get the DVRLookupTable::_mfDataB field.
inline
MFReal32 &DVRLookupTableBase::getDataB(void)
{
    return _mfDataB;
}

//! Get the DVRLookupTable::_mfDataB field.
inline
const MFReal32 &DVRLookupTableBase::getDataB(void) const
{
    return _mfDataB;
}

//! Get the value of the \a index element the DVRLookupTable::_mfDataA field.
inline
Real32 &DVRLookupTableBase::getDataA(const UInt32 index)
{
    return _mfDataA[index];
}

//! Get the DVRLookupTable::_mfDataA field.
inline
MFReal32 &DVRLookupTableBase::getDataA(void)
{
    return _mfDataA;
}

//! Get the DVRLookupTable::_mfDataA field.
inline
const MFReal32 &DVRLookupTableBase::getDataA(void) const
{
    return _mfDataA;
}

OSG_END_NAMESPACE

#define OSGDVRLOOKUPTABLEBASE_INLINE_CVSID "@(#)$Id: OSGDVRLookupTableBase.inl,v 1.2 2003/10/07 15:26:37 weiler Exp $"

