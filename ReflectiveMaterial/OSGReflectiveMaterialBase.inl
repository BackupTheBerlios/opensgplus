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
 *   Initial coding by Polina Kondratieva                                    *
 *                                                                           *
 *   contact:    PolinaKondratieva@gmx.de     (supervisor Jan Kautz)         *
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
 **     class ReflectiveMaterial!
 **                                                                         **
 *****************************************************************************
\*****************************************************************************/

#include <OSGConfig.h>

OSG_BEGIN_NAMESPACE


//! access the type of the class
inline
OSG::FieldContainerType &ReflectiveMaterialBase::getClassType(void)
{
    return _type; 
} 

//! access the numerical type of the class
inline
OSG::UInt32 ReflectiveMaterialBase::getClassTypeId(void) 
{
    return _type.getId(); 
} 

//! create a new instance of the class
inline
ReflectiveMaterialPtr ReflectiveMaterialBase::create(void) 
{
    ReflectiveMaterialPtr fc; 

    if(getClassType().getPrototype() != OSG::NullFC) 
    {
        fc = ReflectiveMaterialPtr::dcast(
            getClassType().getPrototype()-> shallowCopy()); 
    }
    
    return fc; 
}

//! create an empty new instance of the class, do not copy the prototype
inline
ReflectiveMaterialPtr ReflectiveMaterialBase::createEmpty(void) 
{ 
    ReflectiveMaterialPtr returnValue; 
    
    newPtr(returnValue); 

    return returnValue; 
}


/*------------------------------ get -----------------------------------*/

//! Get the ReflectiveMaterial::_sfDiffuse field.
inline
SFColor3f *ReflectiveMaterialBase::getSFDiffuse(void)
{
    return &_sfDiffuse;
}

//! Get the ReflectiveMaterial::_sfFresnel field.
inline
SFColor3f *ReflectiveMaterialBase::getSFFresnel(void)
{
    return &_sfFresnel;
}

//! Get the ReflectiveMaterial::_sfTransparency field.
inline
SFReal32 *ReflectiveMaterialBase::getSFTransparency(void)
{
    return &_sfTransparency;
}

//! Get the ReflectiveMaterial::_sfImageDiffuse field.
inline
SFImagePtr *ReflectiveMaterialBase::getSFImageDiffuse(void)
{
    return &_sfImageDiffuse;
}

//! Get the ReflectiveMaterial::_sfImagePosX field.
inline
SFImagePtr *ReflectiveMaterialBase::getSFImagePosX(void)
{
    return &_sfImagePosX;
}

//! Get the ReflectiveMaterial::_sfImageNegX field.
inline
SFImagePtr *ReflectiveMaterialBase::getSFImageNegX(void)
{
    return &_sfImageNegX;
}

//! Get the ReflectiveMaterial::_sfImagePosY field.
inline
SFImagePtr *ReflectiveMaterialBase::getSFImagePosY(void)
{
    return &_sfImagePosY;
}

//! Get the ReflectiveMaterial::_sfImageNegY field.
inline
SFImagePtr *ReflectiveMaterialBase::getSFImageNegY(void)
{
    return &_sfImageNegY;
}

//! Get the ReflectiveMaterial::_sfImagePosZ field.
inline
SFImagePtr *ReflectiveMaterialBase::getSFImagePosZ(void)
{
    return &_sfImagePosZ;
}

//! Get the ReflectiveMaterial::_sfImageNegZ field.
inline
SFImagePtr *ReflectiveMaterialBase::getSFImageNegZ(void)
{
    return &_sfImageNegZ;
}

//! Get the ReflectiveMaterial::_sfLightPos field.
inline
SFVec3f *ReflectiveMaterialBase::getSFLightPos(void)
{
    return &_sfLightPos;
}

//! Get the ReflectiveMaterial::_sfViewPos field.
inline
SFVec3f *ReflectiveMaterialBase::getSFViewPos(void)
{
    return &_sfViewPos;
}

//! Get the ReflectiveMaterial::_sfCompTexCoord field.
inline
SFBool *ReflectiveMaterialBase::getSFCompTexCoord(void)
{
    return &_sfCompTexCoord;
}

//! Get the ReflectiveMaterial::_sfHalfBBoxObj field.
inline
SFReal32 *ReflectiveMaterialBase::getSFHalfBBoxObj(void)
{
    return &_sfHalfBBoxObj;
}

//! Get the value of the ReflectiveMaterial::_sfDiffuse field.
inline
Color3f &ReflectiveMaterialBase::getDiffuse(void)
{
    return _sfDiffuse.getValue();
}

//! Get the value of the ReflectiveMaterial::_sfDiffuse field.
inline
const Color3f &ReflectiveMaterialBase::getDiffuse(void) const
{
    return _sfDiffuse.getValue();
}

//! Set the value of the ReflectiveMaterial::_sfDiffuse field.
inline
void ReflectiveMaterialBase::setDiffuse(const Color3f &value)
{
    _sfDiffuse.setValue(value);
}

//! Get the value of the ReflectiveMaterial::_sfFresnel field.
inline
Color3f &ReflectiveMaterialBase::getFresnel(void)
{
    return _sfFresnel.getValue();
}

//! Get the value of the ReflectiveMaterial::_sfFresnel field.
inline
const Color3f &ReflectiveMaterialBase::getFresnel(void) const
{
    return _sfFresnel.getValue();
}

//! Set the value of the ReflectiveMaterial::_sfFresnel field.
inline
void ReflectiveMaterialBase::setFresnel(const Color3f &value)
{
    _sfFresnel.setValue(value);
}

//! Get the value of the ReflectiveMaterial::_sfTransparency field.
inline
Real32 &ReflectiveMaterialBase::getTransparency(void)
{
    return _sfTransparency.getValue();
}

//! Get the value of the ReflectiveMaterial::_sfTransparency field.
inline
const Real32 &ReflectiveMaterialBase::getTransparency(void) const
{
    return _sfTransparency.getValue();
}

//! Set the value of the ReflectiveMaterial::_sfTransparency field.
inline
void ReflectiveMaterialBase::setTransparency(const Real32 &value)
{
    _sfTransparency.setValue(value);
}

//! Get the value of the ReflectiveMaterial::_sfImageDiffuse field.
inline
ImagePtr &ReflectiveMaterialBase::getImageDiffuse(void)
{
    return _sfImageDiffuse.getValue();
}

//! Get the value of the ReflectiveMaterial::_sfImageDiffuse field.
inline
const ImagePtr &ReflectiveMaterialBase::getImageDiffuse(void) const
{
    return _sfImageDiffuse.getValue();
}

//! Set the value of the ReflectiveMaterial::_sfImageDiffuse field.
inline
void ReflectiveMaterialBase::setImageDiffuse(const ImagePtr &value)
{
    _sfImageDiffuse.setValue(value);
}

//! Get the value of the ReflectiveMaterial::_sfImagePosX field.
inline
ImagePtr &ReflectiveMaterialBase::getImagePosX(void)
{
    return _sfImagePosX.getValue();
}

//! Get the value of the ReflectiveMaterial::_sfImagePosX field.
inline
const ImagePtr &ReflectiveMaterialBase::getImagePosX(void) const
{
    return _sfImagePosX.getValue();
}

//! Set the value of the ReflectiveMaterial::_sfImagePosX field.
inline
void ReflectiveMaterialBase::setImagePosX(const ImagePtr &value)
{
    _sfImagePosX.setValue(value);
}

//! Get the value of the ReflectiveMaterial::_sfImageNegX field.
inline
ImagePtr &ReflectiveMaterialBase::getImageNegX(void)
{
    return _sfImageNegX.getValue();
}

//! Get the value of the ReflectiveMaterial::_sfImageNegX field.
inline
const ImagePtr &ReflectiveMaterialBase::getImageNegX(void) const
{
    return _sfImageNegX.getValue();
}

//! Set the value of the ReflectiveMaterial::_sfImageNegX field.
inline
void ReflectiveMaterialBase::setImageNegX(const ImagePtr &value)
{
    _sfImageNegX.setValue(value);
}

//! Get the value of the ReflectiveMaterial::_sfImagePosY field.
inline
ImagePtr &ReflectiveMaterialBase::getImagePosY(void)
{
    return _sfImagePosY.getValue();
}

//! Get the value of the ReflectiveMaterial::_sfImagePosY field.
inline
const ImagePtr &ReflectiveMaterialBase::getImagePosY(void) const
{
    return _sfImagePosY.getValue();
}

//! Set the value of the ReflectiveMaterial::_sfImagePosY field.
inline
void ReflectiveMaterialBase::setImagePosY(const ImagePtr &value)
{
    _sfImagePosY.setValue(value);
}

//! Get the value of the ReflectiveMaterial::_sfImageNegY field.
inline
ImagePtr &ReflectiveMaterialBase::getImageNegY(void)
{
    return _sfImageNegY.getValue();
}

//! Get the value of the ReflectiveMaterial::_sfImageNegY field.
inline
const ImagePtr &ReflectiveMaterialBase::getImageNegY(void) const
{
    return _sfImageNegY.getValue();
}

//! Set the value of the ReflectiveMaterial::_sfImageNegY field.
inline
void ReflectiveMaterialBase::setImageNegY(const ImagePtr &value)
{
    _sfImageNegY.setValue(value);
}

//! Get the value of the ReflectiveMaterial::_sfImagePosZ field.
inline
ImagePtr &ReflectiveMaterialBase::getImagePosZ(void)
{
    return _sfImagePosZ.getValue();
}

//! Get the value of the ReflectiveMaterial::_sfImagePosZ field.
inline
const ImagePtr &ReflectiveMaterialBase::getImagePosZ(void) const
{
    return _sfImagePosZ.getValue();
}

//! Set the value of the ReflectiveMaterial::_sfImagePosZ field.
inline
void ReflectiveMaterialBase::setImagePosZ(const ImagePtr &value)
{
    _sfImagePosZ.setValue(value);
}

//! Get the value of the ReflectiveMaterial::_sfImageNegZ field.
inline
ImagePtr &ReflectiveMaterialBase::getImageNegZ(void)
{
    return _sfImageNegZ.getValue();
}

//! Get the value of the ReflectiveMaterial::_sfImageNegZ field.
inline
const ImagePtr &ReflectiveMaterialBase::getImageNegZ(void) const
{
    return _sfImageNegZ.getValue();
}

//! Set the value of the ReflectiveMaterial::_sfImageNegZ field.
inline
void ReflectiveMaterialBase::setImageNegZ(const ImagePtr &value)
{
    _sfImageNegZ.setValue(value);
}

//! Get the value of the ReflectiveMaterial::_sfLightPos field.
inline
Vec3f &ReflectiveMaterialBase::getLightPos(void)
{
    return _sfLightPos.getValue();
}

//! Get the value of the ReflectiveMaterial::_sfLightPos field.
inline
const Vec3f &ReflectiveMaterialBase::getLightPos(void) const
{
    return _sfLightPos.getValue();
}

//! Set the value of the ReflectiveMaterial::_sfLightPos field.
inline
void ReflectiveMaterialBase::setLightPos(const Vec3f &value)
{
    _sfLightPos.setValue(value);
}

//! Get the value of the ReflectiveMaterial::_sfViewPos field.
inline
Vec3f &ReflectiveMaterialBase::getViewPos(void)
{
    return _sfViewPos.getValue();
}

//! Get the value of the ReflectiveMaterial::_sfViewPos field.
inline
const Vec3f &ReflectiveMaterialBase::getViewPos(void) const
{
    return _sfViewPos.getValue();
}

//! Set the value of the ReflectiveMaterial::_sfViewPos field.
inline
void ReflectiveMaterialBase::setViewPos(const Vec3f &value)
{
    _sfViewPos.setValue(value);
}

//! Get the value of the ReflectiveMaterial::_sfCompTexCoord field.
inline
bool &ReflectiveMaterialBase::getCompTexCoord(void)
{
    return _sfCompTexCoord.getValue();
}

//! Get the value of the ReflectiveMaterial::_sfCompTexCoord field.
inline
const bool &ReflectiveMaterialBase::getCompTexCoord(void) const
{
    return _sfCompTexCoord.getValue();
}

//! Set the value of the ReflectiveMaterial::_sfCompTexCoord field.
inline
void ReflectiveMaterialBase::setCompTexCoord(const bool &value)
{
    _sfCompTexCoord.setValue(value);
}

//! Get the value of the ReflectiveMaterial::_sfHalfBBoxObj field.
inline
Real32 &ReflectiveMaterialBase::getHalfBBoxObj(void)
{
    return _sfHalfBBoxObj.getValue();
}

//! Get the value of the ReflectiveMaterial::_sfHalfBBoxObj field.
inline
const Real32 &ReflectiveMaterialBase::getHalfBBoxObj(void) const
{
    return _sfHalfBBoxObj.getValue();
}

//! Set the value of the ReflectiveMaterial::_sfHalfBBoxObj field.
inline
void ReflectiveMaterialBase::setHalfBBoxObj(const Real32 &value)
{
    _sfHalfBBoxObj.setValue(value);
}


OSG_END_NAMESPACE

#define OSGREFLECTIVEMATERIALBASE_INLINE_CVSID "@(#)$Id: OSGReflectiveMaterialBase.inl,v 1.1 2003/11/03 19:13:00 jnkautz Exp $"
