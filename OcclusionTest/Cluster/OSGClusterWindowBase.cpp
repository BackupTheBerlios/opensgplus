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
 **     class ClusterWindow!
 **                                                                         **
 *****************************************************************************
\*****************************************************************************/


#define OSG_COMPILECLUSTERWINDOWINST

#include <stdlib.h>
#include <stdio.h>

#include <OSGConfig.h>

#include "OSGClusterWindowBase.h"
#include "OSGClusterWindow.h"


OSG_BEGIN_NAMESPACE

DataType FieldDataTraits<ClusterWindowPtr>::_type("ClusterWindowPtr", "WindowPtr");

#if defined(__sgi)

#pragma instantiate SField<ClusterWindowPtr>::_fieldType
#pragma instantiate MField<ClusterWindowPtr>::_fieldType

#else

OSG_DLLEXPORT_DEF1(SField, ClusterWindowPtr, OSG_CLUSTERLIB_DLLTMPLMAPPING)
OSG_DLLEXPORT_DEF1(MField, ClusterWindowPtr, OSG_CLUSTERLIB_DLLTMPLMAPPING)

#endif

OSG_END_NAMESPACE

OSG_USING_NAMESPACE

#ifdef __sgi
#pragma set woff 1174
#endif

namespace
{
    static Char8 cvsid_cpp       [] = "@(#)$Id: OSGClusterWindowBase.cpp,v 1.1 2002/02/11 05:45:54 jsux Exp $";
    static Char8 cvsid_hpp       [] = OSGCLUSTERWINDOWBASE_HEADER_CVSID;
    static Char8 cvsid_inl       [] = OSGCLUSTERWINDOWBASE_INLINE_CVSID;

    static Char8 cvsid_fields_hpp[] = OSGCLUSTERWINDOWFIELDS_HEADER_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

const OSG::BitVector  ClusterWindowBase::ServersFieldMask = 
    (1 << ClusterWindowBase::ServersFieldId);

const OSG::BitVector  ClusterWindowBase::ConnectionTypeFieldMask = 
    (1 << ClusterWindowBase::ConnectionTypeFieldId);

const OSG::BitVector  ClusterWindowBase::ClientWindowFieldMask = 
    (1 << ClusterWindowBase::ClientWindowFieldId);

const OSG::BitVector  ClusterWindowBase::ServicePortFieldMask = 
    (1 << ClusterWindowBase::ServicePortFieldId);



// Field descriptions

/*! \var string          ClusterWindowBase::_mfServers
    List of all symbolic server names
*/
/*! \var string          ClusterWindowBase::_sfConnectionType
    How to connect to the servers
*/
/*! \var WindowPtr       ClusterWindowBase::_sfClientWindow
    Window for client rendering
*/
/*! \var UInt32          ClusterWindowBase::_sfServicePort
    Broadcastport used for server search
*/
//! ClusterWindow description

FieldDescription *ClusterWindowBase::_desc[] = 
{
    new FieldDescription(MFString::getClassType(), 
                     "servers", 
                     ServersFieldId, ServersFieldMask,
                     false,
                     (FieldAccessMethod) &ClusterWindowBase::getMFServers),
    new FieldDescription(SFString::getClassType(), 
                     "connectionType", 
                     ConnectionTypeFieldId, ConnectionTypeFieldMask,
                     false,
                     (FieldAccessMethod) &ClusterWindowBase::getSFConnectionType),
    new FieldDescription(SFWindowPtr::getClassType(), 
                     "clientWindow", 
                     ClientWindowFieldId, ClientWindowFieldMask,
                     false,
                     (FieldAccessMethod) &ClusterWindowBase::getSFClientWindow),
    new FieldDescription(SFUInt32::getClassType(), 
                     "servicePort", 
                     ServicePortFieldId, ServicePortFieldMask,
                     false,
                     (FieldAccessMethod) &ClusterWindowBase::getSFServicePort)
};

//! ClusterWindow type

FieldContainerType ClusterWindowBase::_type(
    "ClusterWindow",
    "Window",
    NULL,
    (PrototypeCreateF) &ClusterWindowBase::createEmpty,
    ClusterWindow::initMethod,
    _desc,
    sizeof(_desc));

//OSG_FIELD_CONTAINER_DEF(ClusterWindowBase, ClusterWindowPtr)

/*------------------------------ get -----------------------------------*/

FieldContainerType &ClusterWindowBase::getType(void) 
{
    return _type; 
} 

const FieldContainerType &ClusterWindowBase::getType(void) const 
{
    return _type;
} 


FieldContainerPtr ClusterWindowBase::shallowCopy(void) const 
{ 
    ClusterWindowPtr returnValue; 

    newPtr(returnValue, dynamic_cast<const ClusterWindow *>(this)); 

    return returnValue; 
}

UInt32 ClusterWindowBase::getContainerSize(void) const 
{ 
    return sizeof(ClusterWindowBase); 
}


void ClusterWindowBase::executeSync(      FieldContainer &other,
                                    const BitVector      &whichField)
{
    this->executeSyncImpl((ClusterWindowBase *) &other, whichField);
}

/*------------------------- constructors ----------------------------------*/

//! Constructor

#ifdef OSG_WIN32_ICL
#pragma warning (disable : 383)
#endif

ClusterWindowBase::ClusterWindowBase(void) :
    _mfServers                (), 
    _sfConnectionType         (), 
    _sfClientWindow           (), 
    _sfServicePort            (UInt32(8437)), 
    Inherited() 
{
}

#ifdef OSG_WIN32_ICL
#pragma warning (default : 383)
#endif

//! Copy Constructor

ClusterWindowBase::ClusterWindowBase(const ClusterWindowBase &source) :
    _mfServers                (source._mfServers                ), 
    _sfConnectionType         (source._sfConnectionType         ), 
    _sfClientWindow           (source._sfClientWindow           ), 
    _sfServicePort            (source._sfServicePort            ), 
    Inherited                 (source)
{
}

/*-------------------------- destructors ----------------------------------*/

//! Destructor

ClusterWindowBase::~ClusterWindowBase(void)
{
}

/*------------------------------ access -----------------------------------*/

UInt32 ClusterWindowBase::getBinSize(const BitVector &whichField)
{
    UInt32 returnValue = Inherited::getBinSize(whichField);

    if(FieldBits::NoField != (ServersFieldMask & whichField))
    {
        returnValue += _mfServers.getBinSize();
    }

    if(FieldBits::NoField != (ConnectionTypeFieldMask & whichField))
    {
        returnValue += _sfConnectionType.getBinSize();
    }

    if(FieldBits::NoField != (ClientWindowFieldMask & whichField))
    {
        returnValue += _sfClientWindow.getBinSize();
    }

    if(FieldBits::NoField != (ServicePortFieldMask & whichField))
    {
        returnValue += _sfServicePort.getBinSize();
    }


    return returnValue;
}

void ClusterWindowBase::copyToBin(      BinaryDataHandler &pMem,
                                  const BitVector         &whichField)
{
    Inherited::copyToBin(pMem, whichField);

    if(FieldBits::NoField != (ServersFieldMask & whichField))
    {
        _mfServers.copyToBin(pMem);
    }

    if(FieldBits::NoField != (ConnectionTypeFieldMask & whichField))
    {
        _sfConnectionType.copyToBin(pMem);
    }

    if(FieldBits::NoField != (ClientWindowFieldMask & whichField))
    {
        _sfClientWindow.copyToBin(pMem);
    }

    if(FieldBits::NoField != (ServicePortFieldMask & whichField))
    {
        _sfServicePort.copyToBin(pMem);
    }


}

void ClusterWindowBase::copyFromBin(      BinaryDataHandler &pMem,
                                    const BitVector    &whichField)
{
    Inherited::copyFromBin(pMem, whichField);

    if(FieldBits::NoField != (ServersFieldMask & whichField))
    {
        _mfServers.copyFromBin(pMem);
    }

    if(FieldBits::NoField != (ConnectionTypeFieldMask & whichField))
    {
        _sfConnectionType.copyFromBin(pMem);
    }

    if(FieldBits::NoField != (ClientWindowFieldMask & whichField))
    {
        _sfClientWindow.copyFromBin(pMem);
    }

    if(FieldBits::NoField != (ServicePortFieldMask & whichField))
    {
        _sfServicePort.copyFromBin(pMem);
    }


}

void ClusterWindowBase::executeSyncImpl(      ClusterWindowBase *pOther,
                                        const BitVector         &whichField)
{

    Inherited::executeSyncImpl(pOther, whichField);

    if(FieldBits::NoField != (ServersFieldMask & whichField))
        _mfServers.syncWith(pOther->_mfServers);

    if(FieldBits::NoField != (ConnectionTypeFieldMask & whichField))
        _sfConnectionType.syncWith(pOther->_sfConnectionType);

    if(FieldBits::NoField != (ClientWindowFieldMask & whichField))
        _sfClientWindow.syncWith(pOther->_sfClientWindow);

    if(FieldBits::NoField != (ServicePortFieldMask & whichField))
        _sfServicePort.syncWith(pOther->_sfServicePort);


}
