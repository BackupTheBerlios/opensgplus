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
 **     class DVRShader!
 **                                                                         **
 *****************************************************************************
\*****************************************************************************/


#define OSG_COMPILEDVRSHADERINST

#include <stdlib.h>
#include <stdio.h>

#include <OSGConfig.h>

#include "OSGDVRShaderBase.h"
#include "OSGDVRShader.h"


OSG_USING_NAMESPACE

const OSG::BitVector DVRShaderBase::MTInfluenceMask = 
    (Inherited::MTInfluenceMask) | 
    (static_cast<BitVector>(0x0) << Inherited::NextFieldId); 



FieldContainerType DVRShaderBase::_type(
    "DVRShader",
    "FieldContainer",
    NULL,
    NULL, 
    DVRShader::initMethod,
    NULL,
    0);

//OSG_FIELD_CONTAINER_DEF(DVRShaderBase, DVRShaderPtr)

/*------------------------------ get -----------------------------------*/

FieldContainerType &DVRShaderBase::getType(void) 
{
    return _type; 
} 

const FieldContainerType &DVRShaderBase::getType(void) const 
{
    return _type;
} 


UInt32 DVRShaderBase::getContainerSize(void) const 
{ 
    return sizeof(DVRShader); 
}


void DVRShaderBase::executeSync(      FieldContainer &other,
                                    const BitVector      &whichField)
{
    this->executeSyncImpl((DVRShaderBase *) &other, whichField);
}

/*------------------------- constructors ----------------------------------*/

#ifdef OSG_WIN32_ICL
#pragma warning (disable : 383)
#endif

DVRShaderBase::DVRShaderBase(void) :
    Inherited() 
{
}

#ifdef OSG_WIN32_ICL
#pragma warning (default : 383)
#endif

DVRShaderBase::DVRShaderBase(const DVRShaderBase &source) :
    Inherited                 (source)
{
}

/*-------------------------- destructors ----------------------------------*/

DVRShaderBase::~DVRShaderBase(void)
{
}

/*------------------------------ access -----------------------------------*/

UInt32 DVRShaderBase::getBinSize(const BitVector &whichField)
{
    UInt32 returnValue = Inherited::getBinSize(whichField);


    return returnValue;
}

void DVRShaderBase::copyToBin(      BinaryDataHandler &pMem,
                                  const BitVector         &whichField)
{
    Inherited::copyToBin(pMem, whichField);


}

void DVRShaderBase::copyFromBin(      BinaryDataHandler &pMem,
                                    const BitVector    &whichField)
{
    Inherited::copyFromBin(pMem, whichField);


}

void DVRShaderBase::executeSyncImpl(      DVRShaderBase *pOther,
                                        const BitVector         &whichField)
{

    Inherited::executeSyncImpl(pOther, whichField);


}



#include <OSGSFieldTypeDef.inl>

OSG_BEGIN_NAMESPACE

#if !defined(OSG_DO_DOC) || defined(OSG_DOC_DEV)
DataType FieldDataTraits<DVRShaderPtr>::_type("DVRShaderPtr", "FieldContainerPtr");
#endif

OSG_DLLEXPORT_SFIELD_DEF1(DVRShaderPtr, OSG_VOLRENLIB_DLLTMPLMAPPING);

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
    static Char8 cvsid_cpp       [] = "@(#)$Id: OSGDVRShaderBase.cpp,v 1.2 2003/10/07 15:26:37 weiler Exp $";
    static Char8 cvsid_hpp       [] = OSGDVRSHADERBASE_HEADER_CVSID;
    static Char8 cvsid_inl       [] = OSGDVRSHADERBASE_INLINE_CVSID;

    static Char8 cvsid_fields_hpp[] = OSGDVRSHADERFIELDS_HEADER_CVSID;
}

