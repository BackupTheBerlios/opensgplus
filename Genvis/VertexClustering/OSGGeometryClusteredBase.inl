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
 **     class GeometryClustered!
 **                                                                         **
 *****************************************************************************
\*****************************************************************************/

#include <OSGConfig.h>

OSG_BEGIN_NAMESPACE


//! access the type of the class
inline
OSG::FieldContainerType &GeometryClusteredBase::getClassType(void)
{
    return _type; 
} 

//! access the numerical type of the class
inline
OSG::UInt32 GeometryClusteredBase::getClassTypeId(void) 
{
    return _type.getId(); 
} 

//! create a new instance of the class
inline
GeometryClusteredPtr GeometryClusteredBase::create(void) 
{
    GeometryClusteredPtr fc; 

    if(getClassType().getPrototype() != OSG::NullFC) 
    {
        fc = GeometryClusteredPtr::dcast(
            getClassType().getPrototype()-> shallowCopy()); 
    }
    
    return fc; 
}

//! create an empty new instance of the class, do not copy the prototype
inline
GeometryClusteredPtr GeometryClusteredBase::createEmpty(void) 
{ 
    GeometryClusteredPtr returnValue; 
    
    newPtr(returnValue); 

    return returnValue; 
}


/*------------------------------ get -----------------------------------*/

//! Get the GeometryClustered::_sfNumCells field.
inline
SFReal32 *GeometryClusteredBase::getSFNumCells (void)
{
    return &_sfNumCells;
}

//! Get the value of the GeometryClustered::_sfNumCells field.
inline
Real32 &GeometryClusteredBase::getNumCells(void)
{
    return _sfNumCells.getValue();
}

//! Get the value of the GeometryClustered::_sfNumCells field.
inline
const Real32 &GeometryClusteredBase::getNumCells(void) const
{
    return _sfNumCells.getValue();
}

//! Set the value of the GeometryClustered::_sfNumCells field.
inline
void GeometryClusteredBase::setNumCells(const Real32 &value)
{
    _sfNumCells.setValue(value);
}



OSG_END_NAMESPACE

#define OSGGEOMETRYCLUSTEREDBASE_INLINE_CVSID "@(#)$Id: OSGGeometryClusteredBase.inl,v 1.2 2003/09/17 16:55:59 fuenfzig Exp $"
