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


#define OSG_COMPILEDVRLOOKUPTABLEINST

#include <stdlib.h>
#include <stdio.h>

#include <OSGConfig.h>

#include "OSGDVRLookupTableBase.h"
#include "OSGDVRLookupTable.h"


OSG_USING_NAMESPACE

const OSG::BitVector  DVRLookupTableBase::DimensionFieldMask = 
    (1 << DVRLookupTableBase::DimensionFieldId);

const OSG::BitVector  DVRLookupTableBase::SizeFieldMask = 
    (1 << DVRLookupTableBase::SizeFieldId);

const OSG::BitVector  DVRLookupTableBase::ChannelFieldMask = 
    (1 << DVRLookupTableBase::ChannelFieldId);

const OSG::BitVector  DVRLookupTableBase::DataFieldMask = 
    (1 << DVRLookupTableBase::DataFieldId);

const OSG::BitVector  DVRLookupTableBase::DataRFieldMask = 
    (1 << DVRLookupTableBase::DataRFieldId);

const OSG::BitVector  DVRLookupTableBase::DataGFieldMask = 
    (1 << DVRLookupTableBase::DataGFieldId);

const OSG::BitVector  DVRLookupTableBase::DataBFieldMask = 
    (1 << DVRLookupTableBase::DataBFieldId);

const OSG::BitVector  DVRLookupTableBase::DataAFieldMask = 
    (1 << DVRLookupTableBase::DataAFieldId);

const OSG::BitVector  DVRLookupTableBase::TouchedFieldMask = 
    (1 << DVRLookupTableBase::TouchedFieldId);



// Field descriptions

/*! \var UInt8           DVRLookupTableBase::_sfDimension
    
*/
/*! \var UInt32          DVRLookupTableBase::_mfSize
    
*/
/*! \var UInt8           DVRLookupTableBase::_sfChannel
    
*/
/*! \var UInt8           DVRLookupTableBase::_mfData
    
*/
/*! \var Real32          DVRLookupTableBase::_mfDataR
    
*/
/*! \var Real32          DVRLookupTableBase::_mfDataG
    
*/
/*! \var Real32          DVRLookupTableBase::_mfDataB
    
*/
/*! \var Real32          DVRLookupTableBase::_mfDataA
    
*/
/*! \var Bool            DVRLookupTableBase::_sfTouched
    
*/

//! DVRLookupTable description

FieldDescription *DVRLookupTableBase::_desc[] = 
{
    new FieldDescription(SFUInt8::getClassType(), 
                     "dimension", 
                     DimensionFieldId, DimensionFieldMask,
                     false,
                     (FieldAccessMethod) &DVRLookupTableBase::getSFDimension),
    new FieldDescription(MFUInt32::getClassType(), 
                     "size", 
                     SizeFieldId, SizeFieldMask,
                     false,
                     (FieldAccessMethod) &DVRLookupTableBase::getMFSize),
    new FieldDescription(SFUInt8::getClassType(), 
                     "channel", 
                     ChannelFieldId, ChannelFieldMask,
                     false,
                     (FieldAccessMethod) &DVRLookupTableBase::getSFChannel),
    new FieldDescription(MFUInt8::getClassType(), 
                     "data", 
                     DataFieldId, DataFieldMask,
                     false,
                     (FieldAccessMethod) &DVRLookupTableBase::getMFData),
    new FieldDescription(MFReal32::getClassType(), 
                     "dataR", 
                     DataRFieldId, DataRFieldMask,
                     false,
                     (FieldAccessMethod) &DVRLookupTableBase::getMFDataR),
    new FieldDescription(MFReal32::getClassType(), 
                     "dataG", 
                     DataGFieldId, DataGFieldMask,
                     false,
                     (FieldAccessMethod) &DVRLookupTableBase::getMFDataG),
    new FieldDescription(MFReal32::getClassType(), 
                     "dataB", 
                     DataBFieldId, DataBFieldMask,
                     false,
                     (FieldAccessMethod) &DVRLookupTableBase::getMFDataB),
    new FieldDescription(MFReal32::getClassType(), 
                     "dataA", 
                     DataAFieldId, DataAFieldMask,
                     false,
                     (FieldAccessMethod) &DVRLookupTableBase::getMFDataA),
    new FieldDescription(SFBool::getClassType(), 
                     "touched", 
                     TouchedFieldId, TouchedFieldMask,
                     false,
                     (FieldAccessMethod) &DVRLookupTableBase::getSFTouched)
};

//! DVRLookupTable type

FieldContainerType DVRLookupTableBase::_type(
    "DVRLookupTable",
    "Attachment",
    NULL,
    (PrototypeCreateF) &DVRLookupTableBase::createEmpty,
    DVRLookupTable::initMethod,
    _desc,
    sizeof(_desc));

//OSG_FIELD_CONTAINER_DEF(DVRLookupTableBase, DVRLookupTablePtr)

/*------------------------------ get -----------------------------------*/

FieldContainerType &DVRLookupTableBase::getType(void) 
{
    return _type; 
} 

const FieldContainerType &DVRLookupTableBase::getType(void) const 
{
    return _type;
} 


FieldContainerPtr DVRLookupTableBase::shallowCopy(void) const 
{ 
    DVRLookupTablePtr returnValue; 

    newPtr(returnValue, dynamic_cast<const DVRLookupTable *>(this)); 

    return returnValue; 
}

UInt32 DVRLookupTableBase::getContainerSize(void) const 
{ 
    return sizeof(DVRLookupTable); 
}


void DVRLookupTableBase::executeSync(      FieldContainer &other,
                                    const BitVector      &whichField)
{
    this->executeSyncImpl((DVRLookupTableBase *) &other, whichField);
}

/*------------------------- constructors ----------------------------------*/

//! Constructor

#ifdef OSG_WIN32_ICL
#pragma warning (disable : 383)
#endif

DVRLookupTableBase::DVRLookupTableBase(void) :
    _sfDimension              (UInt8(1)), 
    _mfSize                   (UInt32(1)), 
    _sfChannel                (UInt8(4)), 
    _mfData                   (), 
    _mfDataR                  (), 
    _mfDataG                  (), 
    _mfDataB                  (), 
    _mfDataA                  (), 
    _sfTouched                (bool(false)), 
    Inherited() 
{
}

#ifdef OSG_WIN32_ICL
#pragma warning (default : 383)
#endif

//! Copy Constructor

DVRLookupTableBase::DVRLookupTableBase(const DVRLookupTableBase &source) :
    _sfDimension              (source._sfDimension              ), 
    _mfSize                   (source._mfSize                   ), 
    _sfChannel                (source._sfChannel                ), 
    _mfData                   (source._mfData                   ), 
    _mfDataR                  (source._mfDataR                  ), 
    _mfDataG                  (source._mfDataG                  ), 
    _mfDataB                  (source._mfDataB                  ), 
    _mfDataA                  (source._mfDataA                  ), 
    _sfTouched                (source._sfTouched                ), 
    Inherited                 (source)
{
}

/*-------------------------- destructors ----------------------------------*/

//! Destructor

DVRLookupTableBase::~DVRLookupTableBase(void)
{
}

/*------------------------------ access -----------------------------------*/

UInt32 DVRLookupTableBase::getBinSize(const BitVector &whichField)
{
    UInt32 returnValue = Inherited::getBinSize(whichField);

    if(FieldBits::NoField != (DimensionFieldMask & whichField))
    {
        returnValue += _sfDimension.getBinSize();
    }

    if(FieldBits::NoField != (SizeFieldMask & whichField))
    {
        returnValue += _mfSize.getBinSize();
    }

    if(FieldBits::NoField != (ChannelFieldMask & whichField))
    {
        returnValue += _sfChannel.getBinSize();
    }

    if(FieldBits::NoField != (DataFieldMask & whichField))
    {
        returnValue += _mfData.getBinSize();
    }

    if(FieldBits::NoField != (DataRFieldMask & whichField))
    {
        returnValue += _mfDataR.getBinSize();
    }

    if(FieldBits::NoField != (DataGFieldMask & whichField))
    {
        returnValue += _mfDataG.getBinSize();
    }

    if(FieldBits::NoField != (DataBFieldMask & whichField))
    {
        returnValue += _mfDataB.getBinSize();
    }

    if(FieldBits::NoField != (DataAFieldMask & whichField))
    {
        returnValue += _mfDataA.getBinSize();
    }

    if(FieldBits::NoField != (TouchedFieldMask & whichField))
    {
        returnValue += _sfTouched.getBinSize();
    }


    return returnValue;
}

void DVRLookupTableBase::copyToBin(      BinaryDataHandler &pMem,
                                  const BitVector         &whichField)
{
    Inherited::copyToBin(pMem, whichField);

    if(FieldBits::NoField != (DimensionFieldMask & whichField))
    {
        _sfDimension.copyToBin(pMem);
    }

    if(FieldBits::NoField != (SizeFieldMask & whichField))
    {
        _mfSize.copyToBin(pMem);
    }

    if(FieldBits::NoField != (ChannelFieldMask & whichField))
    {
        _sfChannel.copyToBin(pMem);
    }

    if(FieldBits::NoField != (DataFieldMask & whichField))
    {
        _mfData.copyToBin(pMem);
    }

    if(FieldBits::NoField != (DataRFieldMask & whichField))
    {
        _mfDataR.copyToBin(pMem);
    }

    if(FieldBits::NoField != (DataGFieldMask & whichField))
    {
        _mfDataG.copyToBin(pMem);
    }

    if(FieldBits::NoField != (DataBFieldMask & whichField))
    {
        _mfDataB.copyToBin(pMem);
    }

    if(FieldBits::NoField != (DataAFieldMask & whichField))
    {
        _mfDataA.copyToBin(pMem);
    }

    if(FieldBits::NoField != (TouchedFieldMask & whichField))
    {
        _sfTouched.copyToBin(pMem);
    }


}

void DVRLookupTableBase::copyFromBin(      BinaryDataHandler &pMem,
                                    const BitVector    &whichField)
{
    Inherited::copyFromBin(pMem, whichField);

    if(FieldBits::NoField != (DimensionFieldMask & whichField))
    {
        _sfDimension.copyFromBin(pMem);
    }

    if(FieldBits::NoField != (SizeFieldMask & whichField))
    {
        _mfSize.copyFromBin(pMem);
    }

    if(FieldBits::NoField != (ChannelFieldMask & whichField))
    {
        _sfChannel.copyFromBin(pMem);
    }

    if(FieldBits::NoField != (DataFieldMask & whichField))
    {
        _mfData.copyFromBin(pMem);
    }

    if(FieldBits::NoField != (DataRFieldMask & whichField))
    {
        _mfDataR.copyFromBin(pMem);
    }

    if(FieldBits::NoField != (DataGFieldMask & whichField))
    {
        _mfDataG.copyFromBin(pMem);
    }

    if(FieldBits::NoField != (DataBFieldMask & whichField))
    {
        _mfDataB.copyFromBin(pMem);
    }

    if(FieldBits::NoField != (DataAFieldMask & whichField))
    {
        _mfDataA.copyFromBin(pMem);
    }

    if(FieldBits::NoField != (TouchedFieldMask & whichField))
    {
        _sfTouched.copyFromBin(pMem);
    }


}

void DVRLookupTableBase::executeSyncImpl(      DVRLookupTableBase *pOther,
                                        const BitVector         &whichField)
{

    Inherited::executeSyncImpl(pOther, whichField);

    if(FieldBits::NoField != (DimensionFieldMask & whichField))
        _sfDimension.syncWith(pOther->_sfDimension);

    if(FieldBits::NoField != (SizeFieldMask & whichField))
        _mfSize.syncWith(pOther->_mfSize);

    if(FieldBits::NoField != (ChannelFieldMask & whichField))
        _sfChannel.syncWith(pOther->_sfChannel);

    if(FieldBits::NoField != (DataFieldMask & whichField))
        _mfData.syncWith(pOther->_mfData);

    if(FieldBits::NoField != (DataRFieldMask & whichField))
        _mfDataR.syncWith(pOther->_mfDataR);

    if(FieldBits::NoField != (DataGFieldMask & whichField))
        _mfDataG.syncWith(pOther->_mfDataG);

    if(FieldBits::NoField != (DataBFieldMask & whichField))
        _mfDataB.syncWith(pOther->_mfDataB);

    if(FieldBits::NoField != (DataAFieldMask & whichField))
        _mfDataA.syncWith(pOther->_mfDataA);

    if(FieldBits::NoField != (TouchedFieldMask & whichField))
        _sfTouched.syncWith(pOther->_sfTouched);


}



OSG_BEGIN_NAMESPACE

DataType FieldDataTraits<DVRLookupTablePtr>::_type("DVRLookupTablePtr", "AttachmentPtr");


OSG_END_NAMESPACE


/*------------------------------------------------------------------------*/
/*                              cvs id's                                  */

#ifdef OSG_SGI_CC
#pragma set woff 1174
#endif

#ifdef OSG_LINUX_ICC
#pragma warning( disable : 177 )
#endif

namespace
{
    static Char8 cvsid_cpp       [] = "@(#)$Id: OSGDVRLookupTableBase.cpp,v 1.1 2002/10/10 11:11:26 weiler Exp $";
    static Char8 cvsid_hpp       [] = OSGDVRLOOKUPTABLEBASE_HEADER_CVSID;
    static Char8 cvsid_inl       [] = OSGDVRLOOKUPTABLEBASE_INLINE_CVSID;

    static Char8 cvsid_fields_hpp[] = OSGDVRLOOKUPTABLEFIELDS_HEADER_CVSID;
}

