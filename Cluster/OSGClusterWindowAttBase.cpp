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
 **     class ClusterWindowAtt!
 **                                                                         **
 *****************************************************************************
\*****************************************************************************/


#define OSG_COMPILECLUSTERWINDOWATTINST

#include <stdlib.h>
#include <stdio.h>

#include <OSGConfig.h>

#include "OSGClusterWindowAttBase.h"
#include "OSGClusterWindowAtt.h"


OSG_BEGIN_NAMESPACE

DataType FieldDataTraits<ClusterWindowAttPtr>::_type("ClusterWindowAttPtr", "AttachmentPtr", true);

#if defined(__sgi)

#pragma instantiate SField<ClusterWindowAttPtr>::_fieldType
#pragma instantiate MField<ClusterWindowAttPtr>::_fieldType

#else

OSG_DLLEXPORT_DEF1(SField, ClusterWindowAttPtr, OSG_CLUSTERLIB_DLLTMPLMAPPING)
OSG_DLLEXPORT_DEF1(MField, ClusterWindowAttPtr, OSG_CLUSTERLIB_DLLTMPLMAPPING)

#endif

OSG_END_NAMESPACE

OSG_USING_NAMESPACE

#ifdef __sgi
#pragma set woff 1174
#endif

namespace
{
    static char cvsid_cpp       [] = "@(#)$Id: $";
    static char cvsid_hpp       [] = OSGCLUSTERWINDOWATTBASE_HEADER_CVSID;
    static char cvsid_inl       [] = OSGCLUSTERWINDOWATTBASE_INLINE_CVSID;

    static char cvsid_fields_hpp[] = OSGCLUSTERWINDOWATTFIELDS_HEADER_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

const OSG::BitVector  ClusterWindowAttBase::ServerIdFieldMask = 
    (1 << ClusterWindowAttBase::ServerIdFieldId);

const OSG::BitVector  ClusterWindowAttBase::CompositeFieldMask = 
    (1 << ClusterWindowAttBase::CompositeFieldId);

const OSG::BitVector  ClusterWindowAttBase::ImageTransTypeFieldMask = 
    (1 << ClusterWindowAttBase::ImageTransTypeFieldId);

const OSG::BitVector  ClusterWindowAttBase::SubTileSizeFieldMask = 
    (1 << ClusterWindowAttBase::SubTileSizeFieldId);



// Field descriptions

/*! \var UInt32          ClusterWindowAttBase::_sfServerId
    Which server is responsible for the attached Window
*/
/*! \var Bool            ClusterWindowAttBase::_sfComposite
    Should the server send the rendered image back to the client
*/
/*! \var string          ClusterWindowAttBase::_sfImageTransType
    Use this type for image transmission
*/
/*! \var UInt32          ClusterWindowAttBase::_sfSubTileSize
    Transfer images in subtiles of this size
*/
//! ClusterWindowAtt description

FieldDescription *ClusterWindowAttBase::_desc[] = 
{
    new FieldDescription(SFUInt32::getClassType(), 
                     "serverId", 
                     ServerIdFieldId, ServerIdFieldMask,
                     false,
                     (FieldAccessMethod) &ClusterWindowAttBase::getSFServerId),
    new FieldDescription(SFBool::getClassType(), 
                     "composite", 
                     CompositeFieldId, CompositeFieldMask,
                     false,
                     (FieldAccessMethod) &ClusterWindowAttBase::getSFComposite),
    new FieldDescription(SFString::getClassType(), 
                     "imageTransType", 
                     ImageTransTypeFieldId, ImageTransTypeFieldMask,
                     false,
                     (FieldAccessMethod) &ClusterWindowAttBase::getSFImageTransType),
    new FieldDescription(SFUInt32::getClassType(), 
                     "subTileSize", 
                     SubTileSizeFieldId, SubTileSizeFieldMask,
                     false,
                     (FieldAccessMethod) &ClusterWindowAttBase::getSFSubTileSize)
};

//! ClusterWindowAtt type

FieldContainerType ClusterWindowAttBase::_type(
    "ClusterWindowAtt",
    "Attachment",
    NULL,
    (PrototypeCreateF) &ClusterWindowAttBase::createEmpty,
    ClusterWindowAtt::initMethod,
    _desc,
    sizeof(_desc));

//OSG_FIELD_CONTAINER_DEF(ClusterWindowAttBase, ClusterWindowAttPtr)

/*------------------------------ get -----------------------------------*/

FieldContainerType &ClusterWindowAttBase::getType(void) 
{
    return _type; 
} 

const FieldContainerType &ClusterWindowAttBase::getType(void) const 
{
    return _type;
} 
/*! \}                                                                 */

FieldContainerPtr ClusterWindowAttBase::shallowCopy(void) const 
{ 
    ClusterWindowAttPtr returnValue; 

    newPtr(returnValue, dynamic_cast<const ClusterWindowAtt *>(this)); 

    return returnValue; 
}

UInt32 ClusterWindowAttBase::getContainerSize(void) const 
{ 
    return sizeof(ClusterWindowAttBase); 
}


void ClusterWindowAttBase::executeSync(      FieldContainer &other,
                                    const BitVector      &whichField)
{
    this->executeSyncImpl((ClusterWindowAttBase *) &other, whichField);
}

/*------------------------- constructors ----------------------------------*/

//! Constructor

#ifdef OSG_WIN32_ICL
#pragma warning (disable : 383)
#endif

ClusterWindowAttBase::ClusterWindowAttBase(void) :
    _sfServerId               (), 
    _sfComposite              (), 
    _sfImageTransType         (), 
    _sfSubTileSize            (), 
    Inherited() 
{
}

#ifdef OSG_WIN32_ICL
#pragma warning (default : 383)
#endif

//! Copy Constructor

ClusterWindowAttBase::ClusterWindowAttBase(const ClusterWindowAttBase &source) :
    _sfServerId               (source._sfServerId               ), 
    _sfComposite              (source._sfComposite              ), 
    _sfImageTransType         (source._sfImageTransType         ), 
    _sfSubTileSize            (source._sfSubTileSize            ), 
    Inherited                 (source)
{
}

/*-------------------------- destructors ----------------------------------*/

//! Destructor

ClusterWindowAttBase::~ClusterWindowAttBase(void)
{
}

/*------------------------------ access -----------------------------------*/

UInt32 ClusterWindowAttBase::getBinSize(const BitVector &whichField)
{
    UInt32 returnValue = Inherited::getBinSize(whichField);

    if(FieldBits::NoField != (ServerIdFieldMask & whichField))
    {
        returnValue += _sfServerId.getBinSize();
    }

    if(FieldBits::NoField != (CompositeFieldMask & whichField))
    {
        returnValue += _sfComposite.getBinSize();
    }

    if(FieldBits::NoField != (ImageTransTypeFieldMask & whichField))
    {
        returnValue += _sfImageTransType.getBinSize();
    }

    if(FieldBits::NoField != (SubTileSizeFieldMask & whichField))
    {
        returnValue += _sfSubTileSize.getBinSize();
    }


    return returnValue;
}

void ClusterWindowAttBase::copyToBin(      BinaryDataHandler &pMem,
                                  const BitVector         &whichField)
{
    Inherited::copyToBin(pMem, whichField);

    if(FieldBits::NoField != (ServerIdFieldMask & whichField))
    {
        _sfServerId.copyToBin(pMem);
    }

    if(FieldBits::NoField != (CompositeFieldMask & whichField))
    {
        _sfComposite.copyToBin(pMem);
    }

    if(FieldBits::NoField != (ImageTransTypeFieldMask & whichField))
    {
        _sfImageTransType.copyToBin(pMem);
    }

    if(FieldBits::NoField != (SubTileSizeFieldMask & whichField))
    {
        _sfSubTileSize.copyToBin(pMem);
    }


}

void ClusterWindowAttBase::copyFromBin(      BinaryDataHandler &pMem,
                                    const BitVector    &whichField)
{
    Inherited::copyFromBin(pMem, whichField);

    if(FieldBits::NoField != (ServerIdFieldMask & whichField))
    {
        _sfServerId.copyFromBin(pMem);
    }

    if(FieldBits::NoField != (CompositeFieldMask & whichField))
    {
        _sfComposite.copyFromBin(pMem);
    }

    if(FieldBits::NoField != (ImageTransTypeFieldMask & whichField))
    {
        _sfImageTransType.copyFromBin(pMem);
    }

    if(FieldBits::NoField != (SubTileSizeFieldMask & whichField))
    {
        _sfSubTileSize.copyFromBin(pMem);
    }


}

void ClusterWindowAttBase::executeSyncImpl(      ClusterWindowAttBase *pOther,
                                        const BitVector         &whichField)
{

    Inherited::executeSyncImpl(pOther, whichField);

    if(FieldBits::NoField != (ServerIdFieldMask & whichField))
        _sfServerId.syncWith(pOther->_sfServerId);

    if(FieldBits::NoField != (CompositeFieldMask & whichField))
        _sfComposite.syncWith(pOther->_sfComposite);

    if(FieldBits::NoField != (ImageTransTypeFieldMask & whichField))
        _sfImageTransType.syncWith(pOther->_sfImageTransType);

    if(FieldBits::NoField != (SubTileSizeFieldMask & whichField))
        _sfSubTileSize.syncWith(pOther->_sfSubTileSize);


}

