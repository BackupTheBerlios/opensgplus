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
 **     class LafortuneMaterial
 **                                                                         **
 *****************************************************************************
\*****************************************************************************/


#ifndef _OSGLAFORTUNEMATERIALBASE_H_
#define _OSGLAFORTUNEMATERIALBASE_H_
#ifdef __sgi
#pragma once
#endif


#include <OSGConfig.h>
#include <OSGSystemDef.h>

#include <OSGBaseTypes.h>

#include <OSGChunkMaterial.h> // Parent

#include <OSGColor3fFields.h> // Ambient type
#include <OSGColor3fFields.h> // Diffuse type
#include <OSGReal32Fields.h>  // Ka type
#include <OSGReal32Fields.h>  // Rho_d type
#include <OSGReal32Fields.h>  // Ks type
#include <OSGColor3fFields.h> // Emission type
#include <OSGReal32Fields.h>  // Transparency type
#include <OSGColor3fFields.h> // Cx type
#include <OSGColor3fFields.h> // Cz type
#include <OSGColor3fFields.h> // N type
#include <OSGInt8Fields.h>    // Selector type
#include <OSGPnt3fFields.h>   // LightPos type
#include <OSGBoolFields.h>    // Useweight type
#include <OSGImageFields.h>   // TextureImage type
#include <OSGInt8Fields.h>    // UseFPorRC type
#include <OSGBoolFields.h>    // CompTexCoord type
#include <OSGReal32Fields.h>  // HalfBBoxObj type

#include "OSGLafortuneMaterialFields.h"

OSG_BEGIN_NAMESPACE

class LafortuneMaterial;
class BinaryDataHandler;

//! \brief LafortuneMaterial Base Class.

class OSG_SYSTEMLIB_DLLMAPPING LafortuneMaterialBase : public ChunkMaterial
{
  private:

    typedef ChunkMaterial Inherited;

    /*==========================  PUBLIC  =================================*/
  public:

    enum
    {
        AmbientFieldId      = Inherited::NextFieldId,
        DiffuseFieldId      = AmbientFieldId      + 1,
        KaFieldId           = DiffuseFieldId      + 1,
        Rho_dFieldId        = KaFieldId     + 1,
        KsFieldId           = Rho_dFieldId        + 1,
        EmissionFieldId     = KsFieldId           + 1,
        TransparencyFieldId = EmissionFieldId     + 1,
        CxFieldId           = TransparencyFieldId + 1,
        CzFieldId           = CxFieldId           + 1,
        NFieldId            = CzFieldId           + 1,
        SelectorFieldId     = NFieldId            + 1,
        LightPosFieldId     = SelectorFieldId     + 1,
        UseweightFieldId    = LightPosFieldId     + 1,
        TextureImageFieldId = UseweightFieldId    + 1,
	UseFPorRCFieldId    = TextureImageFieldId + 1,
	CompTexCoordFieldId = UseFPorRCFieldId    + 1,
	HalfBBoxObjFieldId  = CompTexCoordFieldId + 1,
        NextFieldId         = HalfBBoxObjFieldId  + 1
    };

    static const OSG::BitVector AmbientFieldMask;
    static const OSG::BitVector DiffuseFieldMask;
    static const OSG::BitVector KaFieldMask;
    static const OSG::BitVector Rho_dFieldMask;
    static const OSG::BitVector KsFieldMask;
    static const OSG::BitVector EmissionFieldMask;
    static const OSG::BitVector TransparencyFieldMask;
    static const OSG::BitVector CxFieldMask;
    static const OSG::BitVector CzFieldMask;
    static const OSG::BitVector NFieldMask;
    static const OSG::BitVector SelectorFieldMask;
    static const OSG::BitVector LightPosFieldMask;
    static const OSG::BitVector UseweightFieldMask;
    static const OSG::BitVector TextureImageFieldMask;
    static const OSG::BitVector UseFPorRCFieldMask;
    static const OSG::BitVector CompTexCoordFieldMask;
    static const OSG::BitVector HalfBBoxObjFieldMask;


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

           SFColor3f           *getSFAmbient        (void);
           SFColor3f           *getSFDiffuse        (void);
           SFReal32            *getSFKa             (void);
           SFReal32            *getSFRho_d          (void);
           SFReal32            *getSFKs             (void);
           SFColor3f           *getSFEmission       (void);
           SFReal32            *getSFTransparency   (void);
           SFColor3f           *getSFCx             (void);
           SFColor3f           *getSFCz             (void);
           SFColor3f           *getSFN              (void);
           SFInt8              *getSFSelector       (void);
           SFPnt3f             *getSFLightPos       (void);
           SFBool              *getSFUseweight      (void);
           SFImagePtr          *getSFTextureImage   (void);
           SFInt8              *getSFUseFPorRC      (void);
           SFBool              *getSFCompTexCoord   (void);
           SFReal32            *getSFHalfBBoxObj    (void);

           Color3f             &getAmbient        (void);
     const Color3f             &getAmbient        (void) const;
           Color3f             &getDiffuse        (void);
     const Color3f             &getDiffuse        (void) const;
           Real32              &getKa             (void);
     const Real32              &getKa             (void) const;
           Real32              &getRho_d          (void);
     const Real32              &getRho_d          (void) const;
           Real32              &getKs             (void);
     const Real32              &getKs             (void) const;
           Color3f             &getEmission       (void);
     const Color3f             &getEmission       (void) const;
           Real32              &getTransparency   (void);
     const Real32              &getTransparency   (void) const;
           Color3f             &getCx             (void);
     const Color3f             &getCx             (void) const;
           Color3f             &getCz             (void);
     const Color3f             &getCz             (void) const;
           Color3f             &getN              (void);
     const Color3f             &getN              (void) const;
           Int8                &getSelector       (void);
     const Int8                &getSelector       (void) const;
           Pnt3f               &getLightPos       (void);
     const Pnt3f               &getLightPos       (void) const;
           bool                &getUseweight      (void);
     const bool                &getUseweight      (void) const;
           ImagePtr            &getTextureImage   (void);
     const ImagePtr            &getTextureImage   (void) const;
           Int8                &getUseFPorRC      (void);
     const Int8                &getUseFPorRC      (void) const;
           bool                &getCompTexCoord   (void);
     const bool                &getCompTexCoord   (void) const;
           Real32              &getHalfBBoxObj    (void);
     const Real32              &getHalfBBoxObj    (void) const;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Field Set                                 */
    /*! \{                                                                 */

     void setAmbient        ( const Color3f &value );
     void setDiffuse        ( const Color3f &value );
     void setKa             ( const Real32 &value );
     void setRho_d          ( const Real32 &value );
     void setKs             ( const Real32 &value );
     void setEmission       ( const Color3f &value );
     void setTransparency   ( const Real32 &value );
     void setCx             ( const Color3f &value );
     void setCz             ( const Color3f &value );
     void setN              ( const Color3f &value );
     void setSelector       ( const Int8 &value );
     void setLightPos       ( const Pnt3f &value );
     void setUseweight      ( const bool &value );
     void setTextureImage   ( const ImagePtr &value );
     void setUseFPorRC      ( const Int8 &value );
     void setCompTexCoord   ( const bool &value );
     void setHalfBBoxObj    ( const Real32 &value );

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

    static  LafortuneMaterialPtr      create          (void); 
    static  LafortuneMaterialPtr      createEmpty     (void); 

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

    SFColor3f           _sfAmbient;
    SFColor3f           _sfDiffuse;
    SFReal32            _sfKa;
    SFReal32            _sfRho_d;
    SFReal32            _sfKs;
    SFColor3f           _sfEmission;
    SFReal32            _sfTransparency;
    SFColor3f           _sfCx;
    SFColor3f           _sfCz;
    SFColor3f           _sfN;
    SFInt8              _sfSelector;
    SFPnt3f             _sfLightPos;
    SFBool              _sfUseweight;
    SFImagePtr          _sfTextureImage;
    SFInt8              _sfUseFPorRC;
    SFBool              _sfCompTexCoord;
    SFReal32            _sfHalfBBoxObj;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Constructors                               */
    /*! \{                                                                 */

    LafortuneMaterialBase(void);
    LafortuneMaterialBase(const LafortuneMaterialBase &source);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructors                                */
    /*! \{                                                                 */

    virtual ~LafortuneMaterialBase(void); 

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                       Sync                                   */
    /*! \{                                                                 */

    void executeSyncImpl(      LafortuneMaterialBase *pOther,
                         const BitVector         &whichField);

    /*! \}                                                                 */
    /*==========================  PRIVATE  ================================*/
  private:

    friend class FieldContainer;

    static FieldDescription   *_desc[];
    static FieldContainerType  _type;


    // prohibit default functions (move to 'public' if you need one)
    void operator =(const LafortuneMaterialBase &source);
};

//---------------------------------------------------------------------------
//   Exported Types
//---------------------------------------------------------------------------


typedef LafortuneMaterialBase *LafortuneMaterialBaseP;

OSG_END_NAMESPACE

<<<<<<< OSGLafortuneMaterialBase.h
//#define OSGLAFORTUNEMATERIALBASE_HEADER_CVSID "@(#)$Id: OSGLafortuneMaterialBase.h,v 1.1 2003/12/03 21:49:52 jnkautz Exp $"
#define OSGLAFORTUNEMATERIALBASE_HEADER_CVSID "@(#)$Id: OSGLafortuneMaterialBase.h,v 1.1 2003/12/03 21:49:52 jnkautz Exp $"
=======
//#define OSGLAFORTUNEMATERIALBASE_HEADER_CVSID "@(#)$Id: OSGLafortuneMaterialBase.h,v 1.1 2003/12/03 21:49:52 jnkautz Exp $"
#define OSGLAFORTUNEMATERIALBASE_HEADER_CVSID "@(#)$Id: OSGLafortuneMaterialBase.h,v 1.1 2003/12/03 21:49:52 jnkautz Exp $"
>>>>>>> 1.1.1.3

#endif /* _OSGLAFORTUNEMATERIALBASE_H_ */
