/*---------------------------------------------------------------------------*\
 *                                OpenSG                                     *
 *                                                                           *
 *                                                                           *
 *               Copyright (C) 2000-2002 by the OpenSG Forum                 *
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
 **     class GeometryClustered!
 **                                                                         **
 *****************************************************************************
\*****************************************************************************/


#define OSG_COMPILEGEOMETRYCLUSTEREDINST

#include <stdlib.h>
#include <stdio.h>

#include <OSGConfig.h>

#include "OSGGeometryClusteredBase.h"
#include "OSGGeometryClustered.h"


OSG_USING_NAMESPACE

const OSG::BitVector  GeometryClusteredBase::NumCellsFieldMask = 
    (1 << GeometryClusteredBase::NumCellsFieldId);

const OSG::BitVector GeometryClusteredBase::MTInfluenceMask = 
    (Inherited::MTInfluenceMask) | 
    (static_cast<BitVector>(0x0) << Inherited::NextFieldId); 

// Field descriptions

/*! \var Real32          GeometryClusteredBase::_sfNumCells
    Number of regular grid cells along shortest axis.
*/

FieldDescription *GeometryClusteredBase::_desc[] = 
{
    new FieldDescription(SFReal32::getClassType(), 
                     "number of cells", 
                     NumCellsFieldId, NumCellsFieldMask,
                     false,
                     (FieldAccessMethod) &GeometryClusteredBase::getSFNumCells)
};

FieldContainerType GeometryClusteredBase::_type(
    "GeometryClustered",
    "Geometry",
    NULL,
    (PrototypeCreateF) &GeometryClusteredBase::createEmpty,
    GeometryClustered::initMethod,
    _desc,
    sizeof(_desc));

//OSG_FIELD_CONTAINER_DEF(GeometryClusteredBase, GeometryClusteredPtr)

/*------------------------------ get -----------------------------------*/

FieldContainerType &GeometryClusteredBase::getType(void) 
{
    return _type; 
} 

const FieldContainerType &GeometryClusteredBase::getType(void) const 
{
    return _type;
} 


FieldContainerPtr GeometryClusteredBase::shallowCopy(void) const 
{ 
    GeometryClusteredPtr returnValue; 

    newPtr(returnValue, dynamic_cast<const GeometryClustered *>(this)); 

    return returnValue; 
}

UInt32 GeometryClusteredBase::getContainerSize(void) const 
{ 
    return sizeof(GeometryClustered); 
}


void GeometryClusteredBase::executeSync(      FieldContainer &other,
                                    const BitVector      &whichField)
{
    this->executeSyncImpl((GeometryClusteredBase *) &other, whichField);
}

/*------------------------- constructors ----------------------------------*/

#ifdef OSG_WIN32_ICL
#pragma warning (disable : 383)
#endif

GeometryClusteredBase::GeometryClusteredBase(void) :
    _sfNumCells                   (Real32(10)), 
    Inherited()
{
}

#ifdef OSG_WIN32_ICL
#pragma warning (default : 383)
#endif

GeometryClusteredBase::GeometryClusteredBase(const GeometryClusteredBase &source) :
    _sfNumCells               (source._sfNumCells), 
    Inherited                 (source)
{
}

/*-------------------------- destructors ----------------------------------*/

GeometryClusteredBase::~GeometryClusteredBase(void)
{
}

/*------------------------------ access -----------------------------------*/

UInt32 GeometryClusteredBase::getBinSize(const BitVector &whichField)
{
    UInt32 returnValue = Inherited::getBinSize(whichField);

    if(FieldBits::NoField != (NumCellsFieldMask & whichField))
    {
        returnValue += _sfNumCells.getBinSize();
    }

    return returnValue;
}

void GeometryClusteredBase::copyToBin(      BinaryDataHandler &pMem,
                                  const BitVector         &whichField)
{
    Inherited::copyToBin(pMem, whichField);

    if(FieldBits::NoField != (NumCellsFieldMask & whichField))
    {
        _sfNumCells.copyToBin(pMem);
    }

}

void GeometryClusteredBase::copyFromBin(      BinaryDataHandler &pMem,
                                    const BitVector    &whichField)
{
    Inherited::copyFromBin(pMem, whichField);

    if(FieldBits::NoField != (NumCellsFieldMask & whichField))
    {
        _sfNumCells.copyFromBin(pMem);
    }

}

void GeometryClusteredBase::executeSyncImpl(      GeometryClusteredBase *pOther,
                                        const BitVector         &whichField)
{

    Inherited::executeSyncImpl(pOther, whichField);

    if(FieldBits::NoField != (NumCellsFieldMask & whichField))
        _sfNumCells.syncWith(pOther->_sfNumCells);

}



OSG_BEGIN_NAMESPACE

#if !defined(OSG_DO_DOC) || defined(OSG_DOC_DEV)
DataType FieldDataTraits<GeometryClusteredPtr>::_type("GeometryClusteredPtr", "GeometryPtr");
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
    static Char8 cvsid_cpp       [] = "@(#)$Id: OSGGeometryClusteredBase.cpp,v 1.2 2003/09/17 16:55:59 fuenfzig Exp $";
    static Char8 cvsid_hpp       [] = OSGGEOMETRYCLUSTEREDBASE_HEADER_CVSID;
    static Char8 cvsid_inl       [] = OSGGEOMETRYCLUSTEREDBASE_INLINE_CVSID;

    static Char8 cvsid_fields_hpp[] = OSGGEOMETRYCLUSTEREDFIELDS_HEADER_CVSID;
}

