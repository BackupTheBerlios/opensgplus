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
 **     class DynamicSubdivisionLP
 **                                                                         **
 *****************************************************************************
\*****************************************************************************/


#ifndef _OSGDYNAMICSUBDIVISIONLPBASE_H_
#define _OSGDYNAMICSUBDIVISIONLPBASE_H_
#ifdef __sgi
#pragma once
#endif


#include "OSGConfig.h"
#include "OSGSubSurfaceDef.h"

#include "OSGBaseTypes.h"

#include "OSGGroup.h" // Parent

#include "OSGUInt16Fields.h" // MinDepth type
#include "OSGReal32Fields.h" // MinProjSize type
#include "OSGBoolFields.h" // BackfaceCulling type
#include "OSGOpenMeshPFields.h" // Mesh type
#include "OSGOpenMeshTesselatorPFields.h" // Tesselator type

#include "OSGDynamicSubdivisionLPFields.h"

OSG_BEGIN_NAMESPACE

template <class MESH> class DynamicSubdivisionLP;
class BinaryDataHandler;

//! \brief DynamicSubdivisionLP Base Class.

template <class MESH>
class OSG_SUBSURFACELIB_DLLMAPPING DynamicSubdivisionLPBase : public Group
{
  private:
    typedef Group Inherited;

  public:
    //enum                                  { MType = MESH2MTYPE<MESH>::MType };
    enum                                    { MType = TRIANGLE };
    typedef MESH                            OpenMesh;
    typedef OpenMesh*                       OpenMeshP;
    typedef SField<OpenMeshP>               SFOpenMeshP;
    typedef WSDmain<Vec3f,OpenMesh,MType>   OpenMeshTesselator;
    typedef OpenMeshTesselator*             OpenMeshTesselatorP;
    typedef SField<OpenMeshTesselatorP>     SFOpenMeshTesselatorP;

    /*==========================  PUBLIC  =================================*/
    enum
    {
        MinProjSizeFieldId        = Inherited::NextFieldId,
        MaxProjSizeFieldId        = MinProjSizeFieldId        + 1,
        VertexClassifierFieldId   = MaxProjSizeFieldId        + 1,
        NormalConeApertureFieldId = VertexClassifierFieldId   + 1,
        MinDepthFieldId           = NormalConeApertureFieldId + 1,
        MaxDepthFieldId           = MinDepthFieldId           + 1,
        BackfaceCullingFieldId    = MaxDepthFieldId           + 1,
        MeshFieldId               = BackfaceCullingFieldId    + 1,
        TesselatorFieldId         = MeshFieldId               + 1,
        NextFieldId               = TesselatorFieldId         + 1
    };

#ifdef OSG_WIN32_ICL
    static const OSG_SUBSURFACELIB_DLLMAPPING OSG::BitVector MinProjSizeFieldMask;
    static const OSG_SUBSURFACELIB_DLLMAPPING OSG::BitVector MaxProjSizeFieldMask;
    static const OSG_SUBSURFACELIB_DLLMAPPING OSG::BitVector VertexClassifierFieldMask;
    static const OSG_SUBSURFACELIB_DLLMAPPING OSG::BitVector NormalConeApertureFieldMask;
    static const OSG_SUBSURFACELIB_DLLMAPPING OSG::BitVector MinDepthFieldMask;
    static const OSG_SUBSURFACELIB_DLLMAPPING OSG::BitVector MaxDepthFieldMask;
    static const OSG_SUBSURFACELIB_DLLMAPPING OSG::BitVector BackfaceCullingFieldMask;
    static const OSG_SUBSURFACELIB_DLLMAPPING OSG::BitVector MeshFieldMask;
    static const OSG_SUBSURFACELIB_DLLMAPPING OSG::BitVector TesselatorFieldMask;
#else
    static const OSG::BitVector MinProjSizeFieldMask;
    static const OSG::BitVector MaxProjSizeFieldMask;
    static const OSG::BitVector VertexClassifierFieldMask;
    static const OSG::BitVector NormalConeApertureFieldMask;
    static const OSG::BitVector MinDepthFieldMask;
    static const OSG::BitVector MaxDepthFieldMask;
    static const OSG::BitVector BackfaceCullingFieldMask;
    static const OSG::BitVector MeshFieldMask;
    static const OSG::BitVector TesselatorFieldMask;
#endif

    /*---------------------------------------------------------------------*/
    /*! \name                    Class Get                                 */
    /*! \{                                                                 */

    inline static        FieldContainerType &getClassType    (void); 
    inline static        UInt32              getClassTypeId  (void); 
    inline static        const Char8*        getName         (void);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                FieldContainer Get                            */
    /*! \{                                                                 */

    virtual       FieldContainerType &getType  (void); 
    virtual const FieldContainerType &getType  (void) const; 

    virtual       UInt32              getContainerSize(void) const;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Field Get                                 */
    /*! \{                                                                 */

     inline SFReal32            *getSFMinProjSize       (void);
     inline SFReal32            *getSFMaxProjSize       (void);
     inline SFReal32            *getSFVertexClassifier  (void);
     inline SFReal32            *getSFNormalConeAperture(void); 
     inline SFUInt16            *getSFMinDepth          (void);
     inline SFUInt16            *getSFMaxDepth          (void);
     inline SFBool              *getSFBackfaceCulling   (void);
     inline SFOpenMeshP         *getSFMesh              (void);
     inline SFOpenMeshTesselatorP *getSFTesselator      (void);

     inline       Real32              &getMinProjSize         (void);
     inline const Real32              &getMinProjSize         (void) const;
     inline       Real32              &getMaxProjSize         (void);
     inline const Real32              &getMaxProjSize         (void) const;
     inline       Real32              &getVertexClassifier    (void);
     inline const Real32              &getVertexClassifier    (void) const;
     inline       Real32              &getNormalConeAperture  (void); 
     inline const Real32              &getNormalConeAperture  (void) const; 
     inline       UInt16              &getMinDepth            (void);
     inline const UInt16              &getMinDepth            (void) const;
     inline       UInt16              &getMaxDepth            (void);
     inline const UInt16              &getMaxDepth            (void) const;
     inline       bool                &getBackfaceCulling     (void);
     inline const bool                &getBackfaceCulling     (void) const;
     inline       OpenMeshP           &getMesh                (void);
     inline const OpenMeshP           &getMesh                (void) const;
     inline       OpenMeshTesselatorP &getTesselator          (void);
     inline const OpenMeshTesselatorP &getTesselator          (void) const;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Field Set                                 */
    /*! \{                                                                 */

     inline void setMinProjSize       ( const Real32 &value );
     inline void setMaxProjSize       ( const Real32 &value );
     inline void setVertexClassifier  ( const Real32 &value );
     inline void setNormalConeAperture( const Real32 &value );
     inline void setMinDepth          ( const UInt16 &value );
     inline void setMaxDepth          ( const UInt16 &value );
     inline void setBackfaceCulling   ( const bool &value );
     inline void setMesh              ( const OpenMeshP &value );
     inline void setTesselator        ( const OpenMeshTesselatorP &value );

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                       Sync                                   */
    /*! \{                                                                 */

    virtual void   executeSync(      FieldContainer    &other,
                               const BitVector         &whichField);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Binary Access                              */
    /*! \{                                                                 */

    virtual UInt32 getBinSize (const BitVector         &whichField);
    virtual void   copyToBin  (      BinaryDataHandler &pMem,
                               const BitVector         &whichField);
    virtual void   copyFromBin(      BinaryDataHandler &pMem,
                               const BitVector         &whichField);


    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Construction                               */
    /*! \{                                                                 */

    inline static  FCPtr<GroupPtr, DynamicSubdivisionLP<MESH> >      create          (void); 
    inline static  FCPtr<GroupPtr, DynamicSubdivisionLP<MESH> >      createEmpty     (void); 

    /*! \}                                                                 */

    /*---------------------------------------------------------------------*/
    /*! \name                       Copy                                   */
    /*! \{                                                                 */

    virtual FieldContainerPtr     shallowCopy     (void) const; 

    /*! \}                                                                 */

    /*=========================  PROTECTED  ===============================*/
  protected:

    /*---------------------------------------------------------------------*/
    /*! \name                      Fields                                  */
    /*! \{                                                                 */

    SFReal32            _sfMinProjSize;
    SFReal32            _sfMaxProjSize;
    SFReal32            _sfVertexClassifier;
    SFReal32            _sfNormalConeAperture;
    SFUInt16            _sfMinDepth;
    SFUInt16            _sfMaxDepth;
    SFBool              _sfBackfaceCulling;
    SFOpenMeshP         _sfMesh;
    SFOpenMeshTesselatorP   _sfTesselator;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Constructors                               */
    /*! \{                                                                 */

    DynamicSubdivisionLPBase(void);
    DynamicSubdivisionLPBase(const DynamicSubdivisionLPBase &source);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructors                                */
    /*! \{                                                                 */

    virtual ~DynamicSubdivisionLPBase(void); 

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                       Sync                                   */
    /*! \{                                                                 */

    void executeSyncImpl(      DynamicSubdivisionLPBase *pOther,
                         const BitVector         &whichField);

    /*! \}                                                                 */
    /*==========================  PRIVATE  ================================*/
  private:

    friend class FieldContainer;

#ifdef OSG_WIN32_ICL
    static OSG_SUBSURFACELIB_DLLMAPPING FieldDescription   *_desc[];
    static OSG_SUBSURFACELIB_DLLMAPPING FieldContainerType  _type;
#else
    static FieldDescription   *_desc[];
    static FieldContainerType  _type;
#endif

    // prohibit default functions (move to 'public' if you need one)
    void operator =(const DynamicSubdivisionLPBase &source);
};

//---------------------------------------------------------------------------
//   Exported Types
//---------------------------------------------------------------------------

OSG_END_NAMESPACE

#define OSGDYNAMICSUBDIVISIONLPBASE_HEADER_CVSID "@(#)$Id: OSGDynamicSubdivisionLPBase.h,v 1.1 2003/12/23 18:47:27 fuenfzig Exp $"

#endif /* _OSGDYNAMICSUBDIVISIONLPBASE_H_ */