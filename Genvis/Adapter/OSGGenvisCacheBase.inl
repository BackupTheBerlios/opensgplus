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
 **     class GenvisCache!
 **                                                                         **
 *****************************************************************************
\*****************************************************************************/

#include <OSGConfig.h>

OSG_BEGIN_NAMESPACE


//! access the type of the class
inline
OSG::FieldContainerType &GenvisCacheBase::getClassType(void)
{
    return _type; 
} 

//! access the numerical type of the class
inline
OSG::UInt32 GenvisCacheBase::getClassTypeId(void) 
{
    return _type.getId(); 
} 

//! create a new instance of the class
inline
GenvisCachePtr GenvisCacheBase::create(void) 
{
    GenvisCachePtr fc; 

    if(getClassType().getPrototype() != osg::NullFC) 
    {
        fc = GenvisCachePtr::dcast(
            getClassType().getPrototype()-> shallowCopy()); 
    }
    
    return fc; 
}

//! create an empty new instance of the class, do not copy the prototype
inline
GenvisCachePtr GenvisCacheBase::createEmpty(void) 
{ 
    GenvisCachePtr returnValue; 
    
    newPtr(returnValue); 

    return returnValue; 
}


/*------------------------------ get -----------------------------------*/

inline
MFMatrix *GenvisCacheBase::getMFAdapterMatrix(void)
{
    return &_mfAdapterMatrix;
}

inline
MFBVolAdapterBaseP *GenvisCacheBase::getMFBVolAdapter(void)
{
    return &_mfBVolAdapter;
}
inline
MFBVolAdapterBaseP *GenvisCacheBase::getMFAdapter2(void)
{
    return &_mfAdapter2;
}
inline
MFBVolAdapterBaseP *GenvisCacheBase::getMFAdapter3(void)
{
    return &_mfAdapter3;
}



inline
Matrix &GenvisCacheBase::getAdapterMatrix(const UInt32 index)
{
    return _mfAdapterMatrix[index];
}

inline
MFMatrix &GenvisCacheBase::getAdapterMatrix(void)
{
    return _mfAdapterMatrix;
}

inline
const MFMatrix &GenvisCacheBase::getAdapterMatrix(void) const
{
    return _mfAdapterMatrix;
}

inline
SFMatrix *GenvisCacheBase::getSFFrameMatrix(void)
{
    return &_sfFrameMatrix;
}

inline
Matrix &GenvisCacheBase::getFrameMatrix(void)
{
    return _sfFrameMatrix.getValue();
}

inline
const Matrix &GenvisCacheBase::getFrameMatrix(void) const
{
    return _sfFrameMatrix.getValue();
}

inline
BVolAdapterBaseP &GenvisCacheBase::getBVolAdapter(const UInt32 index)
{
    return _mfBVolAdapter[index];
}

inline
MFBVolAdapterBaseP &GenvisCacheBase::getBVolAdapter(void)
{
    return _mfBVolAdapter;
}

inline
const MFBVolAdapterBaseP &GenvisCacheBase::getBVolAdapter(void) const
{
    return _mfBVolAdapter;
}

inline
BVolAdapterBaseP &GenvisCacheBase::getAdapter2(const UInt32 index)
{
    return _mfAdapter2[index];
}

inline
MFBVolAdapterBaseP &GenvisCacheBase::getAdapter2(void)
{
    return _mfAdapter2;
}

inline
const MFBVolAdapterBaseP &GenvisCacheBase::getAdapter2(void) const
{
    return _mfAdapter2;
}

inline
BVolAdapterBaseP &GenvisCacheBase::getAdapter3(const UInt32 index)
{
    return _mfAdapter3[index];
}

inline
MFBVolAdapterBaseP &GenvisCacheBase::getAdapter3(void)
{
    return _mfAdapter3;
}

inline
const MFBVolAdapterBaseP &GenvisCacheBase::getAdapter3(void) const
{
    return _mfAdapter3;
}

inline
GenvisCacheBase::ColCacheContainer& GenvisCacheBase::getColCache ()
{
    return _collisionCache;
}
inline
GenvisCacheBase::AdapterContainer& 
GenvisCacheBase::getColCache (Self& data)
{
    return _collisionCache[&data];
}
inline
const GenvisCacheBase::AdapterContainer& 
GenvisCacheBase::getColCache (Self& data) const
{
    SelfP             nonconstThis = (SelfP)this;
    AdapterContainer& result = nonconstThis->_collisionCache[&data];
    return result;
}

inline
void GenvisCacheBase::setFrameMatrix(const Matrix &value)
{
    _sfFrameMatrix.setValue(value);
}

OSG_END_NAMESPACE

#define OSGGENVISCACHEBASE_INLINE_CVSID "@(#)$Id: OSGGenvisCacheBase.inl,v 1.3 2004/12/20 15:56:36 fuenfzig Exp $"

