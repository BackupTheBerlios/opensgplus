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
 **     class DVRVolume
 **                                                                         **
 *****************************************************************************
\*****************************************************************************/


#ifndef _OSGDVRVOLUMEBASE_H_
#define _OSGDVRVOLUMEBASE_H_
#ifdef __sgi
#pragma once
#endif


#include <OSGConfig.h>
#include <OSGVolRenDef.h>

#include <OSGBaseTypes.h>

#include <OSGNodeCore.h> // Parent

#include <OSGDVRAppearanceFields.h> // Appearance type
#include <OSGDVRGeometryFields.h> // Geometry type
#include <OSGDVRShader.h> // Shader type
#include <OSGStringFields.h> // FileName type
#include <OSGReal32Fields.h> // Sampling type
#include <OSGReal32Fields.h> // SamplingInteractive type
#include <OSGBoolFields.h> // DoTextures type
#include <OSGUInt32Fields.h> // BrickOverlap type
#include <OSGQBit.h> // Textures2D type
#include <OSGMaterialFields.h> // RenderMaterial type
#include <OSGUInt16Fields.h> // BrickingMode type
#include <OSGUInt16Fields.h> // BrickStaticMemoryMB type
#include <OSGVec3fFields.h> // BrickStaticSubdivision type
#include <OSGVec3fFields.h> // BrickMaxSize type
#include <OSGBoolFields.h> // ShowBricks type
#include <OSGUInt32Fields.h> // DrawStyle type
#include <OSGStringFields.h> // DrawStyleNames type
#include <OSGChunkMaterialFields.h> // TextureStorage type

#include <OSGDVRVolumeFields.h>

OSG_BEGIN_NAMESPACE

class DVRVolume;
class BinaryDataHandler;

//! \brief DVRVolume Base Class.

class OSG_VOLRENLIB_DLLMAPPING DVRVolumeBase : public NodeCore
{
  private:

    typedef NodeCore Inherited;

    /*==========================  PUBLIC  =================================*/
  public:

    enum
    {
        AppearanceFieldId             = Inherited::NextFieldId,
        GeometryFieldId               = AppearanceFieldId             + 1,
        ShaderFieldId                 = GeometryFieldId               + 1,
        FileNameFieldId               = ShaderFieldId                 + 1,
        SamplingFieldId               = FileNameFieldId               + 1,
        SamplingInteractiveFieldId    = SamplingFieldId               + 1,
        DoTexturesFieldId             = SamplingInteractiveFieldId    + 1,
        BrickOverlapFieldId           = DoTexturesFieldId             + 1,
        Textures2DFieldId             = BrickOverlapFieldId           + 1,
        RenderMaterialFieldId         = Textures2DFieldId             + 1,
        BrickingModeFieldId           = RenderMaterialFieldId         + 1,
        BrickStaticMemoryMBFieldId    = BrickingModeFieldId           + 1,
        BrickStaticSubdivisionFieldId = BrickStaticMemoryMBFieldId    + 1,
        BrickMaxSizeFieldId           = BrickStaticSubdivisionFieldId + 1,
        ShowBricksFieldId             = BrickMaxSizeFieldId           + 1,
        DrawStyleFieldId              = ShowBricksFieldId             + 1,
        DrawStyleNamesFieldId         = DrawStyleFieldId              + 1,
        TextureStorageFieldId         = DrawStyleNamesFieldId         + 1,
        NextFieldId                   = TextureStorageFieldId         + 1
    };

    static const OSG::BitVector AppearanceFieldMask;
    static const OSG::BitVector GeometryFieldMask;
    static const OSG::BitVector ShaderFieldMask;
    static const OSG::BitVector FileNameFieldMask;
    static const OSG::BitVector SamplingFieldMask;
    static const OSG::BitVector SamplingInteractiveFieldMask;
    static const OSG::BitVector DoTexturesFieldMask;
    static const OSG::BitVector BrickOverlapFieldMask;
    static const OSG::BitVector Textures2DFieldMask;
    static const OSG::BitVector RenderMaterialFieldMask;
    static const OSG::BitVector BrickingModeFieldMask;
    static const OSG::BitVector BrickStaticMemoryMBFieldMask;
    static const OSG::BitVector BrickStaticSubdivisionFieldMask;
    static const OSG::BitVector BrickMaxSizeFieldMask;
    static const OSG::BitVector ShowBricksFieldMask;
    static const OSG::BitVector DrawStyleFieldMask;
    static const OSG::BitVector DrawStyleNamesFieldMask;
    static const OSG::BitVector TextureStorageFieldMask;


    static const OSG::BitVector MTInfluenceMask;

    /*---------------------------------------------------------------------*/
    /*! \name                    Class Get                                 */
    /*! \{                                                                 */

    static        FieldContainerType &getClassType    (void); 
    static        UInt32              getClassTypeId  (void); 

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

           SFDVRAppearancePtr  *getSFAppearance     (void);
           SFDVRGeometryPtr    *getSFGeometry       (void);
           SFDVRShaderPtr      *getSFShader         (void);
           SFString            *getSFFileName       (void);
           SFReal32            *getSFSampling       (void);
           SFReal32            *getSFSamplingInteractive(void);
           SFBool              *getSFDoTextures     (void);
           SFUInt32            *getSFBrickOverlap   (void);
           SFQBit              *getSFTextures2D     (void);
           SFChunkMaterialPtr  *getSFTextureStorage (void);

           DVRAppearancePtr    &getAppearance     (void);
     const DVRAppearancePtr    &getAppearance     (void) const;
           DVRGeometryPtr      &getGeometry       (void);
     const DVRGeometryPtr      &getGeometry       (void) const;
           DVRShaderPtr        &getShader         (void);
     const DVRShaderPtr        &getShader         (void) const;
           std::string         &getFileName       (void);
     const std::string         &getFileName       (void) const;
           Real32              &getSampling       (void);
     const Real32              &getSampling       (void) const;
           Real32              &getSamplingInteractive(void);
     const Real32              &getSamplingInteractive(void) const;
           bool                &getDoTextures     (void);
     const bool                &getDoTextures     (void) const;
           UInt32              &getBrickOverlap   (void);
     const UInt32              &getBrickOverlap   (void) const;
           QBit                &getTextures2D     (void);
     const QBit                &getTextures2D     (void) const;
           ChunkMaterialPtr    &getTextureStorage (void);
     const ChunkMaterialPtr    &getTextureStorage (void) const;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Field Set                                 */
    /*! \{                                                                 */

     void setAppearance     ( const DVRAppearancePtr &value );
     void setGeometry       ( const DVRGeometryPtr &value );
     void setShader         ( const DVRShaderPtr &value );
     void setFileName       ( const std::string &value );
     void setSampling       ( const Real32 &value );
     void setSamplingInteractive( const Real32 &value );
     void setDoTextures     ( const bool &value );
     void setBrickOverlap   ( const UInt32 &value );
     void setTextures2D     ( const QBit &value );
     void setTextureStorage ( const ChunkMaterialPtr &value );

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

    static  DVRVolumePtr      create          (void); 
    static  DVRVolumePtr      createEmpty     (void); 

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

    SFDVRAppearancePtr   _sfAppearance;
    SFDVRGeometryPtr    _sfGeometry;
    SFDVRShaderPtr      _sfShader;
    SFString            _sfFileName;
    SFReal32            _sfSampling;
    SFReal32            _sfSamplingInteractive;
    SFBool              _sfDoTextures;
    SFUInt32            _sfBrickOverlap;
    SFQBit              _sfTextures2D;
    SFMaterialPtr       _sfRenderMaterial;
    SFUInt16            _sfBrickingMode;
    SFUInt16            _sfBrickStaticMemoryMB;
    SFVec3f             _sfBrickStaticSubdivision;
    SFVec3f             _sfBrickMaxSize;
    SFBool              _sfShowBricks;
    SFUInt32            _sfDrawStyle;
    MFString            _mfDrawStyleNames;
    SFChunkMaterialPtr   _sfTextureStorage;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Constructors                               */
    /*! \{                                                                 */

    DVRVolumeBase(void);
    DVRVolumeBase(const DVRVolumeBase &source);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructors                                */
    /*! \{                                                                 */

    virtual ~DVRVolumeBase(void); 

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Field Get                                 */
    /*! \{                                                                 */

           SFMaterialPtr       *getSFRenderMaterial (void);
           SFUInt16            *getSFBrickingMode   (void);
           SFUInt16            *getSFBrickStaticMemoryMB(void);
           SFVec3f             *getSFBrickStaticSubdivision(void);
           SFVec3f             *getSFBrickMaxSize   (void);
           SFBool              *getSFShowBricks     (void);
           SFUInt32            *getSFDrawStyle      (void);
           MFString            *getMFDrawStyleNames (void);

           MaterialPtr         &getRenderMaterial (void);
     const MaterialPtr         &getRenderMaterial (void) const;
           UInt16              &getBrickingMode   (void);
     const UInt16              &getBrickingMode   (void) const;
           UInt16              &getBrickStaticMemoryMB(void);
     const UInt16              &getBrickStaticMemoryMB(void) const;
           Vec3f               &getBrickStaticSubdivision(void);
     const Vec3f               &getBrickStaticSubdivision(void) const;
           Vec3f               &getBrickMaxSize   (void);
     const Vec3f               &getBrickMaxSize   (void) const;
           bool                &getShowBricks     (void);
     const bool                &getShowBricks     (void) const;
           UInt32              &getDrawStyle      (void);
     const UInt32              &getDrawStyle      (void) const;
           std::string         &getDrawStyleNames (UInt32 index);
           MFString            &getDrawStyleNames (void);
     const MFString            &getDrawStyleNames (void) const;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Field Set                                 */
    /*! \{                                                                 */

     void setRenderMaterial (const MaterialPtr &value);
     void setBrickingMode   (const UInt16 &value);
     void setBrickStaticMemoryMB(const UInt16 &value);
     void setBrickStaticSubdivision(const Vec3f &value);
     void setBrickMaxSize   (const Vec3f &value);
     void setShowBricks     (const bool &value);
     void setDrawStyle      (const UInt32 &value);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                       Sync                                   */
    /*! \{                                                                 */

    void executeSyncImpl(      DVRVolumeBase *pOther,
                         const BitVector         &whichField);

    /*! \}                                                                 */
    /*==========================  PRIVATE  ================================*/
  private:

    friend class FieldContainer;

    static FieldDescription   *_desc[];
    static FieldContainerType  _type;


    // prohibit default functions (move to 'public' if you need one)
    void operator =(const DVRVolumeBase &source);
};

//---------------------------------------------------------------------------
//   Exported Types
//---------------------------------------------------------------------------


typedef DVRVolumeBase *DVRVolumeBaseP;

OSG_END_NAMESPACE

#define OSGDVRVOLUMEBASE_HEADER_CVSID "@(#)$Id: OSGDVRVolumeBase.h,v 1.2 2003/10/07 15:26:37 weiler Exp $"

#endif /* _OSGDVRVOLUMEBASE_H_ */
