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
 **     class LinearCombinerGeometry!
 **                                                                         **
 *****************************************************************************
\*****************************************************************************/


#define OSG_COMPILELINEARCOMBINERGEOMETRYINST

#include <stdlib.h>
#include <stdio.h>

#include <OSGConfig.h>

#include "OSGLinearCombinerGeometryBase.h"
#include "OSGLinearCombinerGeometry.h"


OSG_USING_NAMESPACE

const OSG::BitVector  LinearCombinerGeometryBase::WeightsFieldMask = 
    (1 << LinearCombinerGeometryBase::WeightsFieldId);

const OSG::BitVector  LinearCombinerGeometryBase::SrcpositionsFieldMask = 
    (1 << LinearCombinerGeometryBase::SrcpositionsFieldId);

const OSG::BitVector  LinearCombinerGeometryBase::SrcvolumesFieldMask = 
    (1 << LinearCombinerGeometryBase::SrcvolumesFieldId);

const OSG::BitVector  LinearCombinerGeometryBase::RecalconrenderFieldMask = 
    (1 << LinearCombinerGeometryBase::RecalconrenderFieldId);

const OSG::BitVector  LinearCombinerGeometryBase::Allgeometries3fFieldMask = 
    (1 << LinearCombinerGeometryBase::Allgeometries3fFieldId);

const OSG::BitVector  LinearCombinerGeometryBase::PositionsdirtyFieldMask = 
    (1 << LinearCombinerGeometryBase::PositionsdirtyFieldId);



// Field descriptions

/*! \var Real32          LinearCombinerGeometryBase::_mfWeights
    Weights for each of the GeoPositions.
*/
/*! \var GeoPositionsPtr LinearCombinerGeometryBase::_mfSrcpositions
    GeoPositions of the different states.
*/
/*! \var Volume          LinearCombinerGeometryBase::_mfSrcvolumes
    Bounding volumes of the source GeoPositions.
*/
/*! \var bool            LinearCombinerGeometryBase::_sfRecalconrender
    Determines if GeoPositions are recalculated on weight change or on render event.
*/
/*! \var bool            LinearCombinerGeometryBase::_sfAllgeometries3f
    Stores if all GeoPositions in srcpositions are GeoPositions3f as this interface is faster.
*/
/*! \var bool            LinearCombinerGeometryBase::_sfPositionsdirty
    Stores if positions have to be recalculated.
*/

//! LinearCombinerGeometry description

FieldDescription *LinearCombinerGeometryBase::_desc[] = 
{
    new FieldDescription(MFReal32::getClassType(), 
                     "weights", 
                     WeightsFieldId, WeightsFieldMask,
                     false,
                     (FieldAccessMethod) &LinearCombinerGeometryBase::getMFWeights),
    new FieldDescription(MFGeoPositionsPtr::getClassType(), 
                     "srcpositions", 
                     SrcpositionsFieldId, SrcpositionsFieldMask,
                     false,
                     (FieldAccessMethod) &LinearCombinerGeometryBase::getMFSrcpositions),
    new FieldDescription(MFVolume::getClassType(), 
                     "srcvolumes", 
                     SrcvolumesFieldId, SrcvolumesFieldMask,
                     true,
                     (FieldAccessMethod) &LinearCombinerGeometryBase::getMFSrcvolumes),
    new FieldDescription(SFBool::getClassType(), 
                     "recalconrender", 
                     RecalconrenderFieldId, RecalconrenderFieldMask,
                     true,
                     (FieldAccessMethod) &LinearCombinerGeometryBase::getSFRecalconrender),
    new FieldDescription(SFBool::getClassType(), 
                     "allgeometries3f", 
                     Allgeometries3fFieldId, Allgeometries3fFieldMask,
                     true,
                     (FieldAccessMethod) &LinearCombinerGeometryBase::getSFAllgeometries3f),
    new FieldDescription(SFBool::getClassType(), 
                     "positionsdirty", 
                     PositionsdirtyFieldId, PositionsdirtyFieldMask,
                     true,
                     (FieldAccessMethod) &LinearCombinerGeometryBase::getSFPositionsdirty)
};


FieldContainerType LinearCombinerGeometryBase::_type(
    "LinearCombinerGeometry",
    "Geometry",
    NULL,
    (PrototypeCreateF) &LinearCombinerGeometryBase::createEmpty,
    LinearCombinerGeometry::initMethod,
    _desc,
    sizeof(_desc));

//OSG_FIELD_CONTAINER_DEF(LinearCombinerGeometryBase, LinearCombinerGeometryPtr)

/*------------------------------ get -----------------------------------*/

FieldContainerType &LinearCombinerGeometryBase::getType(void) 
{
    return _type; 
} 

const FieldContainerType &LinearCombinerGeometryBase::getType(void) const 
{
    return _type;
} 


FieldContainerPtr LinearCombinerGeometryBase::shallowCopy(void) const 
{ 
    LinearCombinerGeometryPtr returnValue; 

    newPtr(returnValue, dynamic_cast<const LinearCombinerGeometry *>(this)); 

    return returnValue; 
}

UInt32 LinearCombinerGeometryBase::getContainerSize(void) const 
{ 
    return sizeof(LinearCombinerGeometry); 
}


void LinearCombinerGeometryBase::executeSync(      FieldContainer &other,
                                    const BitVector      &whichField)
{
    this->executeSyncImpl((LinearCombinerGeometryBase *) &other, whichField);
}

/*------------------------- constructors ----------------------------------*/

#ifdef OSG_WIN32_ICL
#pragma warning (disable : 383)
#endif

LinearCombinerGeometryBase::LinearCombinerGeometryBase(void) :
    _mfWeights                (), 
    _mfSrcpositions           (), 
    _mfSrcvolumes             (), 
    _sfRecalconrender         (), 
    _sfAllgeometries3f        (), 
    _sfPositionsdirty         (), 
    Inherited() 
{
}

#ifdef OSG_WIN32_ICL
#pragma warning (default : 383)
#endif

LinearCombinerGeometryBase::LinearCombinerGeometryBase(const LinearCombinerGeometryBase &source) :
    _mfWeights                (source._mfWeights                ), 
    _mfSrcpositions           (source._mfSrcpositions           ), 
    _mfSrcvolumes             (source._mfSrcvolumes             ), 
    _sfRecalconrender         (source._sfRecalconrender         ), 
    _sfAllgeometries3f        (source._sfAllgeometries3f        ), 
    _sfPositionsdirty         (source._sfPositionsdirty         ), 
    Inherited                 (source)
{
}

/*-------------------------- destructors ----------------------------------*/

LinearCombinerGeometryBase::~LinearCombinerGeometryBase(void)
{
}

/*------------------------------ access -----------------------------------*/

UInt32 LinearCombinerGeometryBase::getBinSize(const BitVector &whichField)
{
    UInt32 returnValue = Inherited::getBinSize(whichField);

    if(FieldBits::NoField != (WeightsFieldMask & whichField))
    {
        returnValue += _mfWeights.getBinSize();
    }

    if(FieldBits::NoField != (SrcpositionsFieldMask & whichField))
    {
        returnValue += _mfSrcpositions.getBinSize();
    }

    if(FieldBits::NoField != (SrcvolumesFieldMask & whichField))
    {
        returnValue += _mfSrcvolumes.getBinSize();
    }

    if(FieldBits::NoField != (RecalconrenderFieldMask & whichField))
    {
        returnValue += _sfRecalconrender.getBinSize();
    }

    if(FieldBits::NoField != (Allgeometries3fFieldMask & whichField))
    {
        returnValue += _sfAllgeometries3f.getBinSize();
    }

    if(FieldBits::NoField != (PositionsdirtyFieldMask & whichField))
    {
        returnValue += _sfPositionsdirty.getBinSize();
    }


    return returnValue;
}

void LinearCombinerGeometryBase::copyToBin(      BinaryDataHandler &pMem,
                                  const BitVector         &whichField)
{
    Inherited::copyToBin(pMem, whichField);

    if(FieldBits::NoField != (WeightsFieldMask & whichField))
    {
        _mfWeights.copyToBin(pMem);
    }

    if(FieldBits::NoField != (SrcpositionsFieldMask & whichField))
    {
        _mfSrcpositions.copyToBin(pMem);
    }

    if(FieldBits::NoField != (SrcvolumesFieldMask & whichField))
    {
        _mfSrcvolumes.copyToBin(pMem);
    }

    if(FieldBits::NoField != (RecalconrenderFieldMask & whichField))
    {
        _sfRecalconrender.copyToBin(pMem);
    }

    if(FieldBits::NoField != (Allgeometries3fFieldMask & whichField))
    {
        _sfAllgeometries3f.copyToBin(pMem);
    }

    if(FieldBits::NoField != (PositionsdirtyFieldMask & whichField))
    {
        _sfPositionsdirty.copyToBin(pMem);
    }


}

void LinearCombinerGeometryBase::copyFromBin(      BinaryDataHandler &pMem,
                                    const BitVector    &whichField)
{
    Inherited::copyFromBin(pMem, whichField);

    if(FieldBits::NoField != (WeightsFieldMask & whichField))
    {
        _mfWeights.copyFromBin(pMem);
    }

    if(FieldBits::NoField != (SrcpositionsFieldMask & whichField))
    {
        _mfSrcpositions.copyFromBin(pMem);
    }

    if(FieldBits::NoField != (SrcvolumesFieldMask & whichField))
    {
        _mfSrcvolumes.copyFromBin(pMem);
    }

    if(FieldBits::NoField != (RecalconrenderFieldMask & whichField))
    {
        _sfRecalconrender.copyFromBin(pMem);
    }

    if(FieldBits::NoField != (Allgeometries3fFieldMask & whichField))
    {
        _sfAllgeometries3f.copyFromBin(pMem);
    }

    if(FieldBits::NoField != (PositionsdirtyFieldMask & whichField))
    {
        _sfPositionsdirty.copyFromBin(pMem);
    }


}

void LinearCombinerGeometryBase::executeSyncImpl(      LinearCombinerGeometryBase *pOther,
                                        const BitVector         &whichField)
{

    Inherited::executeSyncImpl(pOther, whichField);

    if(FieldBits::NoField != (WeightsFieldMask & whichField))
        _mfWeights.syncWith(pOther->_mfWeights);

    if(FieldBits::NoField != (SrcpositionsFieldMask & whichField))
        _mfSrcpositions.syncWith(pOther->_mfSrcpositions);

    if(FieldBits::NoField != (SrcvolumesFieldMask & whichField))
        _mfSrcvolumes.syncWith(pOther->_mfSrcvolumes);

    if(FieldBits::NoField != (RecalconrenderFieldMask & whichField))
        _sfRecalconrender.syncWith(pOther->_sfRecalconrender);

    if(FieldBits::NoField != (Allgeometries3fFieldMask & whichField))
        _sfAllgeometries3f.syncWith(pOther->_sfAllgeometries3f);

    if(FieldBits::NoField != (PositionsdirtyFieldMask & whichField))
        _sfPositionsdirty.syncWith(pOther->_sfPositionsdirty);


}



OSG_BEGIN_NAMESPACE

#if !defined(OSG_DO_DOC) || defined(OSG_DOC_DEV)
DataType FieldDataTraits<LinearCombinerGeometryPtr>::_type("LinearCombinerGeometryPtr", "GeometryPtr");
#endif


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
    static Char8 cvsid_cpp       [] = "@(#)$Id: OSGLinearCombinerGeometryBase.cpp,v 1.1 2003/04/05 16:17:42 dirk Exp $";
    static Char8 cvsid_hpp       [] = OSGLINEARCOMBINERGEOMETRYBASE_HEADER_CVSID;
    static Char8 cvsid_inl       [] = OSGLINEARCOMBINERGEOMETRYBASE_INLINE_CVSID;

    static Char8 cvsid_fields_hpp[] = OSGLINEARCOMBINERGEOMETRYFIELDS_HEADER_CVSID;
}

