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
 **     class MultiDisplayWindow!
 **                                                                         **
 *****************************************************************************
\*****************************************************************************/


#define OSG_COMPILEMULTIDISPLAYWINDOWINST

#include <stdlib.h>
#include <stdio.h>

#include <OSGConfig.h>

#include "OSGMultiDisplayWindowBase.h"
#include "OSGMultiDisplayWindow.h"



OSG_USING_NAMESPACE

#ifdef __sgi
#pragma set woff 1174
#endif

namespace
{
    static char cvsid_cpp       [] = "@(#)$Id: $";
    static char cvsid_hpp       [] = OSGMULTIDISPLAYWINDOWBASE_HEADER_CVSID;
    static char cvsid_inl       [] = OSGMULTIDISPLAYWINDOWBASE_INLINE_CVSID;

    static char cvsid_fields_hpp[] = OSGMULTIDISPLAYWINDOWFIELDS_HEADER_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

const OSG::BitVector  MultiDisplayWindowBase::HServersFieldMask = 
    (1 << MultiDisplayWindowBase::HServersFieldId);

const OSG::BitVector  MultiDisplayWindowBase::VServersFieldMask = 
    (1 << MultiDisplayWindowBase::VServersFieldId);



// Field descriptions

/*! \var UInt32          MultiDisplayWindowBase::_sfHServers
    Number of horizontal servers
*/
/*! \var UInt32          MultiDisplayWindowBase::_sfVServers
    Number of vertical servers
*/
//! MultiDisplayWindow description

FieldDescription *MultiDisplayWindowBase::_desc[] = 
{
    new FieldDescription(SFUInt32::getClassType(), 
                     "hServers", 
                     HServersFieldId, HServersFieldMask,
                     false,
                     (FieldAccessMethod) &MultiDisplayWindowBase::getSFHServers),
    new FieldDescription(SFUInt32::getClassType(), 
                     "vServers", 
                     VServersFieldId, VServersFieldMask,
                     false,
                     (FieldAccessMethod) &MultiDisplayWindowBase::getSFVServers)
};

//! MultiDisplayWindow type

FieldContainerType MultiDisplayWindowBase::_type(
    "MultiDisplayWindow",
    "ClusterWindow",
    NULL,
    (PrototypeCreateF) &MultiDisplayWindowBase::createEmpty,
    MultiDisplayWindow::initMethod,
    _desc,
    sizeof(_desc));

//OSG_FIELD_CONTAINER_DEF(MultiDisplayWindowBase, MultiDisplayWindowPtr)

/*------------------------------ get -----------------------------------*/

FieldContainerType &MultiDisplayWindowBase::getType(void) 
{
    return _type; 
} 

const FieldContainerType &MultiDisplayWindowBase::getType(void) const 
{
    return _type;
} 


FieldContainerPtr MultiDisplayWindowBase::shallowCopy(void) const 
{ 
    MultiDisplayWindowPtr returnValue; 

    newPtr(returnValue, dynamic_cast<const MultiDisplayWindow *>(this)); 

    return returnValue; 
}

UInt32 MultiDisplayWindowBase::getContainerSize(void) const 
{ 
    return sizeof(MultiDisplayWindowBase); 
}


void MultiDisplayWindowBase::executeSync(      FieldContainer &other,
                                    const BitVector      &whichField)
{
    this->executeSyncImpl((MultiDisplayWindowBase *) &other, whichField);
}

/*------------------------- constructors ----------------------------------*/

//! Constructor

#ifdef OSG_WIN32_ICL
#pragma warning (disable : 383)
#endif

MultiDisplayWindowBase::MultiDisplayWindowBase(void) :
    _sfHServers               (), 
    _sfVServers               (), 
    Inherited() 
{
}

#ifdef OSG_WIN32_ICL
#pragma warning (default : 383)
#endif

//! Copy Constructor

MultiDisplayWindowBase::MultiDisplayWindowBase(const MultiDisplayWindowBase &source) :
    _sfHServers               (source._sfHServers               ), 
    _sfVServers               (source._sfVServers               ), 
    Inherited                 (source)
{
}

/*-------------------------- destructors ----------------------------------*/

//! Destructor

MultiDisplayWindowBase::~MultiDisplayWindowBase(void)
{
}

/*------------------------------ access -----------------------------------*/

UInt32 MultiDisplayWindowBase::getBinSize(const BitVector &whichField)
{
    UInt32 returnValue = Inherited::getBinSize(whichField);

    if(FieldBits::NoField != (HServersFieldMask & whichField))
    {
        returnValue += _sfHServers.getBinSize();
    }

    if(FieldBits::NoField != (VServersFieldMask & whichField))
    {
        returnValue += _sfVServers.getBinSize();
    }


    return returnValue;
}

void MultiDisplayWindowBase::copyToBin(      BinaryDataHandler &pMem,
                                  const BitVector         &whichField)
{
    Inherited::copyToBin(pMem, whichField);

    if(FieldBits::NoField != (HServersFieldMask & whichField))
    {
        _sfHServers.copyToBin(pMem);
    }

    if(FieldBits::NoField != (VServersFieldMask & whichField))
    {
        _sfVServers.copyToBin(pMem);
    }


}

void MultiDisplayWindowBase::copyFromBin(      BinaryDataHandler &pMem,
                                    const BitVector    &whichField)
{
    Inherited::copyFromBin(pMem, whichField);

    if(FieldBits::NoField != (HServersFieldMask & whichField))
    {
        _sfHServers.copyFromBin(pMem);
    }

    if(FieldBits::NoField != (VServersFieldMask & whichField))
    {
        _sfVServers.copyFromBin(pMem);
    }


}

void MultiDisplayWindowBase::executeSyncImpl(      MultiDisplayWindowBase *pOther,
                                        const BitVector         &whichField)
{

    Inherited::executeSyncImpl(pOther, whichField);

    if(FieldBits::NoField != (HServersFieldMask & whichField))
        _sfHServers.syncWith(pOther->_sfHServers);

    if(FieldBits::NoField != (VServersFieldMask & whichField))
        _sfVServers.syncWith(pOther->_sfVServers);


}

