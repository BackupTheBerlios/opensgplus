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
 **     class DynamicSubdivisionLP!
 **                                                                         **
 *****************************************************************************
\*****************************************************************************/

#include "OSGConfig.h"

OSG_BEGIN_NAMESPACE


//! access the type of the class
template <class MESH>
inline
FieldContainerType &DynamicSubdivisionLPBase<MESH>::getClassType(void)
{
    return _type; 
} 

//! access the numerical type of the class
template <class MESH>
inline
UInt32 DynamicSubdivisionLPBase<MESH>::getClassTypeId(void) 
{
    return _type.getId(); 
} 

//! access the class name
template <class MESH>
inline
const Char8* DynamicSubdivisionLPBase<MESH>::getName (void) 
{
   static std::string sname;
   sname = "DynamicSubdivisionLP";
   sname.append(MeshIdentifier<MESH>::getIdString());
   return sname.c_str(); 
}

//! create a new instance of the class
template <class MESH>
inline
FCPtr<GroupPtr, DynamicSubdivisionLP<MESH> > DynamicSubdivisionLPBase<MESH>::create(void) 
{
    FCPtr<GroupPtr, DynamicSubdivisionLP<MESH> > fc; 

    if(getClassType().getPrototype() != NullFC) 
    {
        fc = FCPtr<GroupPtr, DynamicSubdivisionLP<MESH> >::dcast(
            getClassType().getPrototype()-> shallowCopy()); 
    }    
    
    return fc; 
}

//! create an empty new instance of the class, do not copy the prototype
template <class MESH>
inline
FCPtr<GroupPtr, DynamicSubdivisionLP<MESH> > DynamicSubdivisionLPBase<MESH>::createEmpty(void) 
{ 
    FCPtr<GroupPtr, DynamicSubdivisionLP<MESH> > returnValue; 
    
    newPtr(returnValue); 

    return returnValue; 
}


/*------------------------------ get -----------------------------------*/

template <class MESH>
inline
SFReal32 *DynamicSubdivisionLPBase<MESH>::getSFMinProjSize(void)
{
    return &_sfMinProjSize;
}

template <class MESH>
inline
SFReal32 *DynamicSubdivisionLPBase<MESH>::getSFMaxProjSize(void)
{
    return &_sfMaxProjSize;
}

template <class MESH>
inline
SFReal32 *DynamicSubdivisionLPBase<MESH>::getSFVertexClassifier(void)
{
    return &_sfVertexClassifier;
}

template <class MESH>
inline
SFReal32 *DynamicSubdivisionLPBase<MESH>::getSFNormalConeAperture(void)
{
    return &_sfNormalConeAperture;
}

template <class MESH>
inline
SFUInt16 *DynamicSubdivisionLPBase<MESH>::getSFMinDepth(void)
{
    return &_sfMinDepth;
}

template <class MESH>
inline
SFUInt16 *DynamicSubdivisionLPBase<MESH>::getSFMaxDepth(void)
{
    return &_sfMaxDepth;
}

template <class MESH>
inline
SFBool *DynamicSubdivisionLPBase<MESH>::getSFBackfaceCulling(void)
{
    return &_sfBackfaceCulling;
}

template <class MESH>
inline
typename DynamicSubdivisionLPBase<MESH>::SFOpenMeshP*
DynamicSubdivisionLPBase<MESH>::getSFMesh(void)
{
    return &_sfMesh;
}

template <class MESH>
inline
typename DynamicSubdivisionLPBase<MESH>::SFOpenMeshTesselatorP*
DynamicSubdivisionLPBase<MESH>::getSFTesselator(void)
{
    return &_sfTesselator;
}

template <class MESH>
inline
SFBool *DynamicSubdivisionLPBase<MESH>::getSFAutoUpdate(void)
{
    return &_sfAutoUpdate;
}


template <class MESH>
inline
Real32 &DynamicSubdivisionLPBase<MESH>::getMinProjSize(void)
{
    return _sfMinProjSize.getValue();
}

template <class MESH>
inline
const Real32 &DynamicSubdivisionLPBase<MESH>::getMinProjSize(void) const
{
    return _sfMinProjSize.getValue();
}

template <class MESH>
inline
void DynamicSubdivisionLPBase<MESH>::setMinProjSize(const Real32 &value)
{
    _sfMinProjSize.setValue(value);
}

template <class MESH>
inline
Real32 &DynamicSubdivisionLPBase<MESH>::getMaxProjSize(void)
{
    return _sfMaxProjSize.getValue();
}

template <class MESH>
inline
const Real32 &DynamicSubdivisionLPBase<MESH>::getMaxProjSize(void) const
{
    return _sfMaxProjSize.getValue();
}

template <class MESH>
inline
void DynamicSubdivisionLPBase<MESH>::setMaxProjSize(const Real32 &value)
{
    _sfMaxProjSize.setValue(value);
}

template <class MESH>
inline
Real32 &DynamicSubdivisionLPBase<MESH>::getVertexClassifier(void)
{
    return _sfVertexClassifier.getValue();
}

template <class MESH>
inline
const Real32 &DynamicSubdivisionLPBase<MESH>::getVertexClassifier(void) const
{
    return _sfVertexClassifier.getValue();
}

template <class MESH>
inline
void DynamicSubdivisionLPBase<MESH>::setVertexClassifier(const Real32 &value)
{
    _sfVertexClassifier.setValue(value);
}

template <class MESH>
inline
Real32 &DynamicSubdivisionLPBase<MESH>::getNormalConeAperture(void)
{
    return _sfNormalConeAperture.getValue();
}

template <class MESH>
inline
const Real32 &DynamicSubdivisionLPBase<MESH>::getNormalConeAperture(void) const
{
    return _sfNormalConeAperture.getValue();
}

template <class MESH>
inline
void DynamicSubdivisionLPBase<MESH>::setNormalConeAperture(const Real32 &value)
{
    _sfNormalConeAperture.setValue(value);
}

template <class MESH>
inline
UInt16 &DynamicSubdivisionLPBase<MESH>::getMinDepth(void)
{
    return _sfMinDepth.getValue();
}

template <class MESH>
inline
const UInt16 &DynamicSubdivisionLPBase<MESH>::getMinDepth(void) const
{
    return _sfMinDepth.getValue();
}

template <class MESH>
inline
void DynamicSubdivisionLPBase<MESH>::setMinDepth(const UInt16 &value)
{
    _sfMinDepth.setValue(value);
}

template <class MESH>
inline
UInt16 &DynamicSubdivisionLPBase<MESH>::getMaxDepth(void)
{
    return _sfMaxDepth.getValue();
}

template <class MESH>
inline
const UInt16 &DynamicSubdivisionLPBase<MESH>::getMaxDepth(void) const
{
    return _sfMaxDepth.getValue();
}

template <class MESH>
inline
void DynamicSubdivisionLPBase<MESH>::setMaxDepth(const UInt16 &value)
{
    _sfMaxDepth.setValue(value);
}

template <class MESH>
inline
bool &DynamicSubdivisionLPBase<MESH>::getBackfaceCulling(void)
{
    return _sfBackfaceCulling.getValue();
}

template <class MESH>
inline
const bool &DynamicSubdivisionLPBase<MESH>::getBackfaceCulling(void) const
{
    return _sfBackfaceCulling.getValue();
}

template <class MESH>
inline
void DynamicSubdivisionLPBase<MESH>::setBackfaceCulling(const bool &value)
{
    _sfBackfaceCulling.setValue(value);
}

template <class MESH>
inline
typename DynamicSubdivisionLPBase<MESH>::OpenMeshP &
DynamicSubdivisionLPBase<MESH>::getMesh(void)
{
    return _sfMesh.getValue();
}

template <class MESH>
inline
const typename DynamicSubdivisionLPBase<MESH>::OpenMeshP &
DynamicSubdivisionLPBase<MESH>::getMesh(void) const
{
    return _sfMesh.getValue();
}

template <class MESH>
inline
void DynamicSubdivisionLPBase<MESH>::setMesh(const OpenMeshP &value)
{
    _sfMesh.setValue(value);
}

template <class MESH>
inline
typename DynamicSubdivisionLPBase<MESH>::OpenMeshTesselatorP &
DynamicSubdivisionLPBase<MESH>::getTesselator(void)
{
    return _sfTesselator.getValue();
}

template <class MESH>
inline
const typename DynamicSubdivisionLPBase<MESH>::OpenMeshTesselatorP &
DynamicSubdivisionLPBase<MESH>::getTesselator(void) const
{
    return _sfTesselator.getValue();
}

template <class MESH>
inline
void DynamicSubdivisionLPBase<MESH>::setTesselator(const OpenMeshTesselatorP &value)
{
    _sfTesselator.setValue(value);
}


template <class MESH>
inline
bool &DynamicSubdivisionLPBase<MESH>::getAutoUpdate(void)
{
    return _sfAutoUpdate.getValue();
}

template <class MESH>
inline
const bool &DynamicSubdivisionLPBase<MESH>::getAutoUpdate(void) const
{
    return _sfAutoUpdate.getValue();
}


template <class MESH>
inline
void DynamicSubdivisionLPBase<MESH>::setAutoUpdate(const bool &value)
{
    _sfAutoUpdate.setValue(value);
}


OSG_END_NAMESPACE

#define OSGDYNAMICSUBDIVISIONLPBASE_INLINE_CVSID "@(#)$Id: OSGDynamicSubdivisionLPBase.inl,v 1.8 2004/07/09 09:00:18 fuenfzig Exp $"

