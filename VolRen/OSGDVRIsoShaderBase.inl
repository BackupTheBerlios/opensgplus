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
 **     class DVRIsoShader!
 **                                                                         **
 *****************************************************************************
\*****************************************************************************/

#include <OSGConfig.h>

OSG_BEGIN_NAMESPACE


//! access the type of the class
inline
OSG::FieldContainerType &DVRIsoShaderBase::getClassType(void)
{
    return _type; 
} 

//! access the numerical type of the class
inline
OSG::UInt32 DVRIsoShaderBase::getClassTypeId(void) 
{
    return _type.getId(); 
} 

//! create a new instance of the class
inline
DVRIsoShaderPtr DVRIsoShaderBase::create(void) 
{
    DVRIsoShaderPtr fc; 

    if(getClassType().getPrototype() != OSG::NullFC) 
    {
        fc = DVRIsoShaderPtr::dcast(
            getClassType().getPrototype()-> shallowCopy()); 
    }
    
    return fc; 
}

//! create an empty new instance of the class, do not copy the prototype
inline
DVRIsoShaderPtr DVRIsoShaderBase::createEmpty(void) 
{ 
    DVRIsoShaderPtr returnValue; 
    
    newPtr(returnValue); 

    return returnValue; 
}


/*------------------------------ get -----------------------------------*/

//! Get the DVRIsoShader::_sfShadeMode field.
inline
SFInt8 *DVRIsoShaderBase::getSFShadeMode(void)
{
    return &_sfShadeMode;
}

//! Get the DVRIsoShader::_sfActiveShadeMode field.
inline
SFInt8 *DVRIsoShaderBase::getSFActiveShadeMode(void)
{
    return &_sfActiveShadeMode;
}


//! Get the value of the DVRIsoShader::_sfShadeMode field.
inline
Int8 &DVRIsoShaderBase::getShadeMode(void)
{
    return _sfShadeMode.getValue();
}

//! Get the value of the DVRIsoShader::_sfShadeMode field.
inline
const Int8 &DVRIsoShaderBase::getShadeMode(void) const
{
    return _sfShadeMode.getValue();
}

//! Set the value of the DVRIsoShader::_sfShadeMode field.
inline
void DVRIsoShaderBase::setShadeMode(const Int8 &value)
{
    _sfShadeMode.setValue(value);
}

//! Get the value of the DVRIsoShader::_sfActiveShadeMode field.
inline
Int8 &DVRIsoShaderBase::getActiveShadeMode(void)
{
    return _sfActiveShadeMode.getValue();
}

//! Get the value of the DVRIsoShader::_sfActiveShadeMode field.
inline
const Int8 &DVRIsoShaderBase::getActiveShadeMode(void) const
{
    return _sfActiveShadeMode.getValue();
}

//! Set the value of the DVRIsoShader::_sfActiveShadeMode field.
inline
void DVRIsoShaderBase::setActiveShadeMode(const Int8 &value)
{
    _sfActiveShadeMode.setValue(value);
}


OSG_END_NAMESPACE

#define OSGDVRISOSHADERBASE_INLINE_CVSID "@(#)$Id: OSGDVRIsoShaderBase.inl,v 1.3 2004/01/19 11:22:33 vossg Exp $"

