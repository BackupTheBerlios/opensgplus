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
 **     class SortFirstWindow!
 **                                                                         **
 *****************************************************************************
\*****************************************************************************/

#include <OSGConfig.h>

OSG_BEGIN_NAMESPACE


//! access the type of the class
inline OSG::FieldContainerType &SortFirstWindowBase::getClassType(void)
{
    return _type; 
} 

//! access the numerical type of the class
inline OSG::UInt32 SortFirstWindowBase::getClassTypeId(void) 
{
    return _type.getId(); 
} 

//! create a new instance of the class
inline SortFirstWindowPtr SortFirstWindowBase::create(void) 
{
    SortFirstWindowPtr fc; 

    if(getClassType().getPrototype() != osg::NullFC) 
    {
        fc = SortFirstWindowPtr::dcast(
            getClassType().getPrototype()-> shallowCopy()); 
    }
    
    return fc; 
}

//! create an empty new instance of the class, do not copy the prototype
inline SortFirstWindowPtr SortFirstWindowBase::createEmpty(void) 
{ 
    SortFirstWindowPtr returnValue; 
    
    newPtr(returnValue); 

    return returnValue; 
}


/*------------------------------ get -----------------------------------*/

OSG_CLUSTERLIB_DLLMAPPING
MFReal32 *SortFirstWindowBase::getMFLeft(void)
{
    return &_mfLeft;
}

OSG_CLUSTERLIB_DLLMAPPING
MFReal32 *SortFirstWindowBase::getMFRight(void)
{
    return &_mfRight;
}

OSG_CLUSTERLIB_DLLMAPPING
MFReal32 *SortFirstWindowBase::getMFBottom(void)
{
    return &_mfBottom;
}

OSG_CLUSTERLIB_DLLMAPPING
MFReal32 *SortFirstWindowBase::getMFTop(void)
{
    return &_mfTop;
}

OSG_CLUSTERLIB_DLLMAPPING
SFString *SortFirstWindowBase::getSFCompression(void)
{
    return &_sfCompression;
}

OSG_CLUSTERLIB_DLLMAPPING
SFUInt32 *SortFirstWindowBase::getSFSubtileSize(void)
{
    return &_sfSubtileSize;
}

OSG_CLUSTERLIB_DLLMAPPING
SFBool *SortFirstWindowBase::getSFCompose(void)
{
    return &_sfCompose;
}


OSG_CLUSTERLIB_DLLMAPPING
string &SortFirstWindowBase::getCompression(void)
{
    return _sfCompression.getValue();
}

OSG_CLUSTERLIB_DLLMAPPING
const string &SortFirstWindowBase::getCompression(void) const
{
    return _sfCompression.getValue();
}

OSG_CLUSTERLIB_DLLMAPPING
void SortFirstWindowBase::setCompression(const string &value)
{
    _sfCompression.setValue(value);
}

OSG_CLUSTERLIB_DLLMAPPING
UInt32 &SortFirstWindowBase::getSubtileSize(void)
{
    return _sfSubtileSize.getValue();
}

OSG_CLUSTERLIB_DLLMAPPING
const UInt32 &SortFirstWindowBase::getSubtileSize(void) const
{
    return _sfSubtileSize.getValue();
}

OSG_CLUSTERLIB_DLLMAPPING
void SortFirstWindowBase::setSubtileSize(const UInt32 &value)
{
    _sfSubtileSize.setValue(value);
}

OSG_CLUSTERLIB_DLLMAPPING
bool &SortFirstWindowBase::getCompose(void)
{
    return _sfCompose.getValue();
}

OSG_CLUSTERLIB_DLLMAPPING
const bool &SortFirstWindowBase::getCompose(void) const
{
    return _sfCompose.getValue();
}

OSG_CLUSTERLIB_DLLMAPPING
void SortFirstWindowBase::setCompose(const bool &value)
{
    _sfCompose.setValue(value);
}


OSG_CLUSTERLIB_DLLMAPPING
Real32 &SortFirstWindowBase::getLeft(UInt32 index)
{
    return _mfLeft.getValue( index );
}

MFReal32 &SortFirstWindowBase::getLeft(void)
{
    return _mfLeft;
}

const MFReal32 &SortFirstWindowBase::getLeft(void) const
{
    return _mfLeft;
}

OSG_CLUSTERLIB_DLLMAPPING
Real32 &SortFirstWindowBase::getRight(UInt32 index)
{
    return _mfRight.getValue( index );
}

MFReal32 &SortFirstWindowBase::getRight(void)
{
    return _mfRight;
}

const MFReal32 &SortFirstWindowBase::getRight(void) const
{
    return _mfRight;
}

OSG_CLUSTERLIB_DLLMAPPING
Real32 &SortFirstWindowBase::getBottom(UInt32 index)
{
    return _mfBottom.getValue( index );
}

MFReal32 &SortFirstWindowBase::getBottom(void)
{
    return _mfBottom;
}

const MFReal32 &SortFirstWindowBase::getBottom(void) const
{
    return _mfBottom;
}

OSG_CLUSTERLIB_DLLMAPPING
Real32 &SortFirstWindowBase::getTop(UInt32 index)
{
    return _mfTop.getValue( index );
}

MFReal32 &SortFirstWindowBase::getTop(void)
{
    return _mfTop;
}

const MFReal32 &SortFirstWindowBase::getTop(void) const
{
    return _mfTop;
}


OSG_END_NAMESPACE

#define OSGSORTFIRSTWINDOWBASE_INLINE_CVSID "@(#)$Id: $"

