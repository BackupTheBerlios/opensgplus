/*---------------------------------------------------------------------------*\
 *                                OpenSG                                     *
 *                                                                           *
 *                                                                           *
 *                     Copyright 2000,2001 by OpenSG Forum                   *
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

#include <OSGConfig.h>

OSG_BEGIN_NAMESPACE


//! access the type of the class
inline OSG::FieldContainerType &ClusterWindowBase::getClassType(void)
{
    return _type; 
} 

//! access the numerical type of the class
inline OSG::UInt32 ClusterWindowBase::getClassTypeId(void) 
{
    return _type.getId(); 
} 

//! create a new instance of the class
inline ClusterWindowPtr ClusterWindowBase::create(void) 
{
    ClusterWindowPtr fc; 

    if(getClassType().getPrototype() != osg::NullFC) 
    {
        fc = ClusterWindowPtr::dcast(
            getClassType().getPrototype()-> shallowCopy()); 
    }
    
    return fc; 
}

//! create an empty new instance of the class, do not copy the prototype
inline ClusterWindowPtr ClusterWindowBase::createEmpty(void) 
{ 
    ClusterWindowPtr returnValue; 
    
    newPtr(returnValue); 

    return returnValue; 
}


/*------------------------------ get -----------------------------------*/

OSG_CLUSTERLIB_DLLMAPPING
MFString *ClusterWindowBase::getMFServers(void)
{
    return &_mfServers;
}

OSG_CLUSTERLIB_DLLMAPPING
SFString *ClusterWindowBase::getSFConnectionType(void)
{
    return &_sfConnectionType;
}

OSG_CLUSTERLIB_DLLMAPPING
SFWindowPtr *ClusterWindowBase::getSFClientWindow(void)
{
    return &_sfClientWindow;
}

OSG_CLUSTERLIB_DLLMAPPING
SFUInt32 *ClusterWindowBase::getSFServicePort(void)
{
    return &_sfServicePort;
}


OSG_CLUSTERLIB_DLLMAPPING
string &ClusterWindowBase::getConnectionType(void)
{
    return _sfConnectionType.getValue();
}

OSG_CLUSTERLIB_DLLMAPPING
const string &ClusterWindowBase::getConnectionType(void) const
{
    return _sfConnectionType.getValue();
}

OSG_CLUSTERLIB_DLLMAPPING
void ClusterWindowBase::setConnectionType(const string &value)
{
    _sfConnectionType.setValue(value);
}

OSG_CLUSTERLIB_DLLMAPPING
WindowPtr &ClusterWindowBase::getClientWindow(void)
{
    return _sfClientWindow.getValue();
}

OSG_CLUSTERLIB_DLLMAPPING
const WindowPtr &ClusterWindowBase::getClientWindow(void) const
{
    return _sfClientWindow.getValue();
}

OSG_CLUSTERLIB_DLLMAPPING
void ClusterWindowBase::setClientWindow(const WindowPtr &value)
{
    _sfClientWindow.setValue(value);
}

OSG_CLUSTERLIB_DLLMAPPING
UInt32 &ClusterWindowBase::getServicePort(void)
{
    return _sfServicePort.getValue();
}

OSG_CLUSTERLIB_DLLMAPPING
const UInt32 &ClusterWindowBase::getServicePort(void) const
{
    return _sfServicePort.getValue();
}

OSG_CLUSTERLIB_DLLMAPPING
void ClusterWindowBase::setServicePort(const UInt32 &value)
{
    _sfServicePort.setValue(value);
}


OSG_CLUSTERLIB_DLLMAPPING
string &ClusterWindowBase::getServers(UInt32 index)
{
    return _mfServers[index];
}

MFString &ClusterWindowBase::getServers(void)
{
    return _mfServers;
}

const MFString &ClusterWindowBase::getServers(void) const
{
    return _mfServers;
}


OSG_END_NAMESPACE

#define OSGCLUSTERWINDOWBASE_INLINE_CVSID "@(#)$Id: $"

