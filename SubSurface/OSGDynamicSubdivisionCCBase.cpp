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
 **     class DynamicSubdivisionCC!
 **                                                                         **
 *****************************************************************************
\*****************************************************************************/


#define OSG_COMPILEDYNAMICSUBDIVISIONCCINST

#include <stdlib.h>
#include <stdio.h>
#include <OpenMesh/Core/Utils/Property.hh>

#include "OSGConfig.h"
#include "OSGDynamicSubdivisionCCBase.h"
#include "OSGDynamicSubdivisionCC.h"


OSG_USING_NAMESPACE

template <class MESH>
const OSG::BitVector  DynamicSubdivisionCCBase<MESH>::MinProjSizeFieldMask = 
    (1 << DynamicSubdivisionCCBase<MESH>::MinProjSizeFieldId);

template <class MESH>
const OSG::BitVector  DynamicSubdivisionCCBase<MESH>::MaxProjSizeFieldMask = 
    (1 << DynamicSubdivisionCCBase<MESH>::MaxProjSizeFieldId);

template <class MESH>
const OSG::BitVector  DynamicSubdivisionCCBase<MESH>::VertexClassifierFieldMask = 
    (1 << DynamicSubdivisionCCBase<MESH>::VertexClassifierFieldId);

template <class MESH>
const OSG::BitVector  DynamicSubdivisionCCBase<MESH>::NormalConeApertureFieldMask = 
    (1 << DynamicSubdivisionCCBase<MESH>::NormalConeApertureFieldId);

template <class MESH>
const OSG::BitVector  DynamicSubdivisionCCBase<MESH>::MinDepthFieldMask = 
    (1 << DynamicSubdivisionCCBase<MESH>::MinDepthFieldId);

template <class MESH>
const OSG::BitVector  DynamicSubdivisionCCBase<MESH>::MaxDepthFieldMask = 
    (1 << DynamicSubdivisionCCBase<MESH>::MaxDepthFieldId);

template <class MESH>
const OSG::BitVector  DynamicSubdivisionCCBase<MESH>::BackfaceCullingFieldMask = 
    (1 << DynamicSubdivisionCCBase<MESH>::BackfaceCullingFieldId);

template <class MESH>
const OSG::BitVector  DynamicSubdivisionCCBase<MESH>::MeshFieldMask = 
    (1 << DynamicSubdivisionCCBase<MESH>::MeshFieldId);

template <class MESH>
const OSG::BitVector  DynamicSubdivisionCCBase<MESH>::TesselatorFieldMask = 
    (1 << DynamicSubdivisionCCBase<MESH>::TesselatorFieldId);

template <class MESH>
const OSG::BitVector  DynamicSubdivisionCCBase<MESH>::AutoUpdateFieldMask = 
    (1 << DynamicSubdivisionCCBase<MESH>::AutoUpdateFieldId);



// Field descriptions

/*! \var Real32          DynamicSubdivisionCCBase::_sfMinProjSize
    minimum pixel size
*/
/*! \var Real32          DynamicSubdivisionCCBase::_sfMaxProjSize
    maximum pixel size
*/
/*! \var Real32          DynamicSubdivisionCCBase::_sfVertexClassifier
    silhouette epsilon
*/
/*! \var Real32          DynamicSubdivisionCCBase::_sfNormalConeAperture
    curvature calc. 
*/
/*! \var UInt16          DynamicSubdivisionCCBase::_sfMinDepth
    minimum subdivision depth
*/
/*! \var UInt16          DynamicSubdivisionCCBase::_sfMaxDepth
    maximum subdivision depth
*/
/*! \var bool            DynamicSubdivisionCCBase::_sfBackfaceCulling
    switch for backface culling
*/
/*! \var OpenMeshP       DynamicSubdivisionCCBase::_sfMesh
    base mesh used for subdivision
*/
/*! \var OpenMeshTesselatorP DynamicSubdivisionCCBase::_sfTesselator
    tesselator implementing the two-table algorithm
*/

//! DynamicSubdivisionCC description

template <class MESH>
FieldDescription *DynamicSubdivisionCCBase<MESH>::_desc[] = 
{
    new FieldDescription(SFReal32::getClassType(), 
                     "MinProjSize", 
                     MinProjSizeFieldId, MinProjSizeFieldMask,
                     false,
                     (FieldAccessMethod) &DynamicSubdivisionCCBase::getSFMinProjSize),
    new FieldDescription(SFReal32::getClassType(), 
                     "MaxProjSize", 
                     MaxProjSizeFieldId, MaxProjSizeFieldMask,
                     false,
                     (FieldAccessMethod) &DynamicSubdivisionCCBase::getSFMaxProjSize),
    new FieldDescription(SFReal32::getClassType(), 
                     "VertexClassifier", 
                     VertexClassifierFieldId, VertexClassifierFieldMask,
                     false,
                     (FieldAccessMethod) &DynamicSubdivisionCCBase::getSFVertexClassifier),
    new FieldDescription(SFReal32::getClassType(), 
                     "NormalConeAperture", 
                     NormalConeApertureFieldId, NormalConeApertureFieldMask,
                     false,
                     (FieldAccessMethod) &DynamicSubdivisionCCBase::getSFNormalConeAperture),
    new FieldDescription(SFUInt16::getClassType(), 
                     "MinDepth", 
                     MinDepthFieldId, MinDepthFieldMask,
                     false,
                     (FieldAccessMethod) &DynamicSubdivisionCCBase::getSFMinDepth),
    new FieldDescription(SFUInt16::getClassType(), 
                     "MaxDepth", 
                     MaxDepthFieldId, MaxDepthFieldMask,
                     false,
                     (FieldAccessMethod) &DynamicSubdivisionCCBase::getSFMaxDepth),
    new FieldDescription(SFBool::getClassType(), 
                     "BackfaceCulling", 
                     BackfaceCullingFieldId, BackfaceCullingFieldMask,
                     false,
                     (FieldAccessMethod) &DynamicSubdivisionCCBase::getSFBackfaceCulling),
    new FieldDescription(SFOpenMeshP::getClassType(), 
                     "Mesh", 
                     MeshFieldId, MeshFieldMask,
                     false,
                     (FieldAccessMethod) &DynamicSubdivisionCCBase::getSFMesh),
    new FieldDescription(SFOpenMeshTesselatorP::getClassType(), 
                     "Tesselator", 
                     TesselatorFieldId, TesselatorFieldMask,
                     true,
                     (FieldAccessMethod) &DynamicSubdivisionCCBase::getSFTesselator),
    new FieldDescription(SFBool::getClassType(), 
                     "AutoUpdate", 
                     AutoUpdateFieldId, AutoUpdateFieldMask,
                     false,
                     (FieldAccessMethod) &DynamicSubdivisionCCBase::getSFAutoUpdate)
};


template <class MESH>
FieldContainerType DynamicSubdivisionCCBase<MESH>::_type(
    getName(),
    "Group",
    NULL,
    (PrototypeCreateF) &(DynamicSubdivisionCCBase<MESH>::createEmpty),
    DynamicSubdivisionCC<MESH>::initMethod,
    _desc,
    sizeof(_desc));

//OSG_FIELD_CONTAINER_DEF(DynamicSubdivisionCCBase, FCPtr<GroupPtr, DynamicSubdivisionCC<MESH> >)

/*------------------------------ get -----------------------------------*/

template <class MESH>
FieldContainerType &DynamicSubdivisionCCBase<MESH>::getType(void) 
{
    return _type; 
} 

template <class MESH>
const FieldContainerType &DynamicSubdivisionCCBase<MESH>::getType(void) const 
{
    return _type;
} 


template <class MESH>
FieldContainerPtr DynamicSubdivisionCCBase<MESH>::shallowCopy(void) const 
{ 
    FCPtr<GroupPtr, DynamicSubdivisionCC<MESH> > returnValue; 

    newPtr(returnValue, dynamic_cast<const DynamicSubdivisionCC<MESH>*>(this)); 

    return returnValue; 
}

template <class MESH>
UInt32 DynamicSubdivisionCCBase<MESH>::getContainerSize(void) const 
{ 
    return sizeof(DynamicSubdivisionCC<MESH>); 
}


template <class MESH>
void DynamicSubdivisionCCBase<MESH>::executeSync(      FieldContainer &other,
                                    const BitVector      &whichField)
{
    this->executeSyncImpl((DynamicSubdivisionCCBase<MESH>*) &other, whichField);
}

/*------------------------- constructors ----------------------------------*/

#ifdef OSG_WIN32_ICL
#pragma warning (disable : 383)
#endif

template <class MESH>
DynamicSubdivisionCCBase<MESH>::DynamicSubdivisionCCBase(void) :
    _sfMinProjSize            (Real32(5.0)), 
    _sfMaxProjSize            (Real32(15.0)), 
    _sfVertexClassifier       (Real32(0.01)), 
    _sfNormalConeAperture     (Real32(0.01)), 
    _sfMinDepth               (UInt16(0)), 
    _sfMaxDepth               (UInt16(4)), 
    _sfBackfaceCulling        (bool(false)), 
    _sfMesh                   (OpenMeshP(NULL)), 
    _sfTesselator             (OpenMeshTesselatorP(NULL)),     
    _sfAutoUpdate             (bool(true)),     
    Inherited() 
{     
}

#ifdef OSG_WIN32_ICL
#pragma warning (default : 383)
#endif

template <class MESH>
DynamicSubdivisionCCBase<MESH>::DynamicSubdivisionCCBase(const DynamicSubdivisionCCBase &source) :
    _sfMinProjSize            (source._sfMinProjSize            ), 
    _sfMaxProjSize            (source._sfMaxProjSize            ), 
    _sfVertexClassifier       (source._sfVertexClassifier       ), 
    _sfNormalConeAperture     (source._sfNormalConeAperture     ), 
    _sfMinDepth               (source._sfMinDepth               ), 
    _sfMaxDepth               (source._sfMaxDepth               ), 
    _sfBackfaceCulling        (source._sfBackfaceCulling        ), 
    _sfMesh                   (source._sfMesh                   ), 
    _sfTesselator             (source._sfTesselator             ),     
    _sfAutoUpdate             (source._sfAutoUpdate             ),     
    Inherited                 (source)
{   
}

/*-------------------------- destructors ----------------------------------*/

template <class MESH>
DynamicSubdivisionCCBase<MESH>::~DynamicSubdivisionCCBase(void)
{
}

/*------------------------------ access -----------------------------------*/

template <class MESH>
UInt32 DynamicSubdivisionCCBase<MESH>::getBinSize(const BitVector &whichField)
{
    UInt32 returnValue = Inherited::getBinSize(whichField);

    if(FieldBits::NoField != (MinProjSizeFieldMask & whichField))
    {
        returnValue += _sfMinProjSize.getBinSize();
    }

    if(FieldBits::NoField != (MaxProjSizeFieldMask & whichField))
    {
        returnValue += _sfMaxProjSize.getBinSize();
    }

    if(FieldBits::NoField != (VertexClassifierFieldMask & whichField))
    {
        returnValue += _sfVertexClassifier.getBinSize();
    }

    if(FieldBits::NoField != (NormalConeApertureFieldMask & whichField))
    {
        returnValue += _sfNormalConeAperture.getBinSize();
    }

    if(FieldBits::NoField != (MinDepthFieldMask & whichField))
    {
        returnValue += _sfMinDepth.getBinSize();
    }

    if(FieldBits::NoField != (MaxDepthFieldMask & whichField))
    {
        returnValue += _sfMaxDepth.getBinSize();
    }

    if(FieldBits::NoField != (BackfaceCullingFieldMask & whichField))
    {
        returnValue += _sfBackfaceCulling.getBinSize();
    }

#if 0
    if(FieldBits::NoField != (MeshFieldMask & whichField))
    {
        returnValue += _sfMesh.getBinSize();
    }

    if(FieldBits::NoField != (TesselatorFieldMask & whichField))
    {
        returnValue += _sfTesselator.getBinSize();
    }
#endif

    if(FieldBits::NoField != (AutoUpdateFieldMask & whichField))
    {
        returnValue += _sfAutoUpdate.getBinSize();
    }

    return returnValue;
}

template <class MESH>
void DynamicSubdivisionCCBase<MESH>::copyToBin(      BinaryDataHandler &pMem,
                                  const BitVector         &whichField)
{
    Inherited::copyToBin(pMem, whichField);

    if(FieldBits::NoField != (MinProjSizeFieldMask & whichField))
    {
        _sfMinProjSize.copyToBin(pMem);
    }

    if(FieldBits::NoField != (MaxProjSizeFieldMask & whichField))
    {
        _sfMaxProjSize.copyToBin(pMem);
    }

    if(FieldBits::NoField != (VertexClassifierFieldMask & whichField))
    {
        _sfVertexClassifier.copyToBin(pMem);
    }

    if(FieldBits::NoField != (NormalConeApertureFieldMask & whichField))
    {
        _sfNormalConeAperture.copyToBin(pMem);
    }

    if(FieldBits::NoField != (MinDepthFieldMask & whichField))
    {
        _sfMinDepth.copyToBin(pMem);
    }

    if(FieldBits::NoField != (MaxDepthFieldMask & whichField))
    {
        _sfMaxDepth.copyToBin(pMem);
    }

    if(FieldBits::NoField != (BackfaceCullingFieldMask & whichField))
    {
        _sfBackfaceCulling.copyToBin(pMem);
    }


    if(FieldBits::NoField != (MeshFieldMask & whichField))
    {
       // write openmesh
       //_sfMesh.copyToBin(pMem);
       SINFO << "write Mesh" << std::endl;
       OSGMeshIO<MESH> myMeshIO(*getMesh());
       myMeshIO.writeOpenMesh(pMem);
       // fill limit points and normals to maxdepth
       getTesselator()->preprocessInstances();   
    }

#if 0
    if(FieldBits::NoField != (TesselatorFieldMask & whichField))
    {
        _sfTesselator.copyToBin(pMem);
    }
#endif

    if(FieldBits::NoField != (AutoUpdateFieldMask & whichField))
    {
        _sfAutoUpdate.copyToBin(pMem);
    }

}

template <class MESH>
void DynamicSubdivisionCCBase<MESH>::copyFromBin(      BinaryDataHandler &pMem,
                                    const BitVector    &whichField)
{
    Inherited::copyFromBin(pMem, whichField);

    if(FieldBits::NoField != (MinProjSizeFieldMask & whichField))
    {
        _sfMinProjSize.copyFromBin(pMem);
    }

    if(FieldBits::NoField != (MaxProjSizeFieldMask & whichField))
    {
        _sfMaxProjSize.copyFromBin(pMem);
    }

    if(FieldBits::NoField != (VertexClassifierFieldMask & whichField))
    {
        _sfVertexClassifier.copyFromBin(pMem);
    }

    if(FieldBits::NoField != (NormalConeApertureFieldMask & whichField))
    {
        _sfNormalConeAperture.copyFromBin(pMem);
    }

    if(FieldBits::NoField != (MinDepthFieldMask & whichField))
    {
        _sfMinDepth.copyFromBin(pMem);
    }

    if(FieldBits::NoField != (MaxDepthFieldMask & whichField))
    {
        _sfMaxDepth.copyFromBin(pMem);
    }

    if(FieldBits::NoField != (BackfaceCullingFieldMask & whichField))
    {
        _sfBackfaceCulling.copyFromBin(pMem);
    }

    if(FieldBits::NoField != (MeshFieldMask & whichField))
    {
       // new OpenMesh object
       MESH* newmesh = new MESH;
       ::OpenMesh::EPropHandleT<Int32> isCrease;
       newmesh->add_property(isCrease,"isCrease");
       //_sfMesh.copyFromBin(pMem,getMesh());       
       SINFO << "read Mesh" << std::endl;
       OSGMeshIO<MESH> myMeshIO(*newmesh);
       myMeshIO.readOpenMesh(pMem);
       setMesh(newmesh);       
    }

#if 0
    if(FieldBits::NoField != (TesselatorFieldMask & whichField))
    {
        _sfTesselator.copyFromBin(pMem);
    }
#endif

    if(FieldBits::NoField != (AutoUpdateFieldMask & whichField))
    {
        _sfAutoUpdate.copyFromBin(pMem);
    }

}

template <class MESH>
void DynamicSubdivisionCCBase<MESH>::executeSyncImpl(      DynamicSubdivisionCCBase *pOther,
                                        const BitVector         &whichField)
{

    Inherited::executeSyncImpl(pOther, whichField);

    if(FieldBits::NoField != (MinProjSizeFieldMask & whichField))
        _sfMinProjSize.syncWith(pOther->_sfMinProjSize);

    if(FieldBits::NoField != (MaxProjSizeFieldMask & whichField))
        _sfMaxProjSize.syncWith(pOther->_sfMaxProjSize);

    if(FieldBits::NoField != (VertexClassifierFieldMask & whichField))
        _sfVertexClassifier.syncWith(pOther->_sfVertexClassifier);

    if(FieldBits::NoField != (NormalConeApertureFieldMask & whichField))
        _sfNormalConeAperture.syncWith(pOther->_sfNormalConeAperture);

    if(FieldBits::NoField != (MinDepthFieldMask & whichField))
        _sfMinDepth.syncWith(pOther->_sfMinDepth);

    if(FieldBits::NoField != (MaxDepthFieldMask & whichField))
        _sfMaxDepth.syncWith(pOther->_sfMaxDepth);

    if(FieldBits::NoField != (BackfaceCullingFieldMask & whichField))
        _sfBackfaceCulling.syncWith(pOther->_sfBackfaceCulling);

    if(FieldBits::NoField != (MeshFieldMask & whichField))
        _sfMesh.syncWith(pOther->_sfMesh);

    if(FieldBits::NoField != (TesselatorFieldMask & whichField))
        _sfTesselator.syncWith(pOther->_sfTesselator);

    if(FieldBits::NoField != (AutoUpdateFieldMask & whichField))
        _sfAutoUpdate.syncWith(pOther->_sfAutoUpdate);


}


#ifndef FIELD_TYPE_DEF
#define FIELD_TYPE_DEF

#include "OSGSFieldTypeDef.inl"
#include "OSGMFieldTypeDef.inl"

#endif

OSG_BEGIN_NAMESPACE

#if !defined(OSG_DO_DOC) || defined(OSG_DOC_DEV)
template <class MESH>
DataType FieldDataTraits<FCPtr<GroupPtr, DynamicSubdivisionCC<MESH> > >::_type(getName(), "GroupPtr");

//OSG_DLLEXPORT_SFIELD_DEF1(FCPtr<GroupPtr, DynamicSubdivisionCC<MESH> >, OSG_SUBSURFACELIB_DLLMAPPING);
//OSG_DLLEXPORT_MFIELD_DEF1(FCPtr<GroupPtr, DynamicSubdivisionCC<MESH> >, OSG_SUBSURFACELIB_DLLMAPPING);
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

#if 0
namespace
{
    static Char8 cvsid_cpp       [] = "@(#)$Id: OSGDynamicSubdivisionCCBase.cpp,v 1.9 2004/07/09 09:00:18 fuenfzig Exp $";
    static Char8 cvsid_hpp       [] = OSGDYNAMICSUBDIVISIONCCBASE_HEADER_CVSID;
    static Char8 cvsid_inl       [] = OSGDYNAMICSUBDIVISIONCCBASE_INLINE_CVSID;

    static Char8 cvsid_fields_hpp[] = OSGDYNAMICSUBDIVISIONCCFIELDS_HEADER_CVSID;
}
#endif
