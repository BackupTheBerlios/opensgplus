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
 *   Initial coding by Polina Kondratieva                                    *
 *                                                                           *
 *   contact:    PolinaKondratieva@gmx.de     (supervisor Jan Kautz)         *
 *                                                                           *
 *                                                                           *
\*---------------------------------------------------------------------------*/

#ifndef _OSGREFLECTIVEMATERIAL_H_
#define _OSGREFLECTIVEMATERIAL_H_
#ifdef __sgi
#pragma once
#endif

#include <OSGConfig.h>
#include <OSGMaterialChunk.h>
#include <OSGImage.h>
#include <OSGTextureChunk.h>
#include <OSGVertexProgramChunk.h>
#include <OSGRegisterCombinersChunk.h>

//#include <OSGGLUTWindow.h>
#include <OSGViewport.h>
#include <OSGPerspectiveCamera.h>
#include <OSGMatrixCamera.h>
#include <OSGMatrixCameraDecorator.h>
#include <OSGTextureGrabForeground.h>
#include <OSGSolidBackground.h>

#include <OSGCubeTextureChunk.h>
#include <OSGTexGenChunk.h>
#include <OSGTextureTransformChunk.h>

//#include <OSGReflectiveMaterialBase.h>
#include "OSGReflectiveMaterialBase.h"

OSG_BEGIN_NAMESPACE

/*! \brief ReflectiveMaterial class. See \ref
           PageSystemReflectiveMaterial for a description.

     This material represents the reflectivity property
     (environment mapping type: CubeMap). It possible to vary
     the reflectivity from the ideal mirror level to the zero
     level. This property achieved by the use of spherical
     harmonics BRDF approximation.
*/

class OSG_SYSTEMLIB_DLLMAPPING ReflectiveMaterial : public ReflectiveMaterialBase
{
  private:

    typedef ReflectiveMaterialBase Inherited;

    /*==========================  PUBLIC  =================================*/
  public:

    /*---------------------------------------------------------------------*/
    /*! \name                      Sync                                    */
    /*! \{                                                                 */

    virtual void changed(BitVector  whichField, 
                         UInt32     origin    );

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                     Output                                   */
    /*! \{                                                                 */

    virtual void dump(      UInt32     uiIndent = 0, 
                      const BitVector  bvFlags  = 0) const;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Rendering                                  */
    /*! \{                                                                 */

    virtual StatePtr   makeState         (void);

    virtual void       rebuildState      (void);

    // Obligatory function!!! It have to be called in the test program,
    // otherwise no CubeMapping will be done.
    virtual void MakeCubeMap(PerspectiveCameraPtr &cam_env, WindowPtr &win, NodePtr &environment);

    /*! \}                                                                 */

    /*=========================  PROTECTED  ===============================*/
  protected:

    // Variables should all be in ReflectiveMaterialBase.

    /*---------------------------------------------------------------------*/
    /*! \name                  Constructors                                */
    /*! \{                                                                 */

    ReflectiveMaterial(void);
    ReflectiveMaterial(const ReflectiveMaterial &source);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructors                                */
    /*! \{                                                                 */

    virtual ~ReflectiveMaterial(void); 

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                     Helper                                   */
    /*! \{                                                                 */

    void prepareLocalChunks(void);

    /*! \}                                                                 */

    /*==========================  PRIVATE  ================================*/
  private:

    friend class FieldContainer;
    friend class ReflectiveMaterialBase;

    static void initMethod(void);

    // these chunks are used for rendering the material
    //---------------------------------------------------------------------
    // The base of material
    MaterialChunkPtr          _materialChunk;

    // Texture for diffuse term
    TextureChunkPtr           _textureChunkDiffuse;

    // Texture which contains the result of CubeMapping
    CubeTextureChunkPtr       _cubeTextureChunk;

    // Compute spherical harmonics fresnel term, texture coordinates
    VertexProgramChunkPtr     _vpChunk;

    // Assemble all together (textures and spherical harmonics result)
    RegisterCombinersChunkPtr _rcChunk;

    // Viewport for grabbing the scene into the texture
    ViewportPtr viewPort[6];

    // Texture which contains temporary images grabbed from the scene
    TextureGrabForegroundPtr texGrabForeground[6];

    // Render the scene from the of one camera postions
    void make_env_map(int index, PerspectiveCameraPtr &cam_env, WindowPtr &win, NodePtr &environment);

    // Get coefficients for spherical harmonics
    void *getSHCoefficients(ImagePtr imfg[6]);
    Vec3f Clm[25];   // Spherical harmonics coefficients

    // assembly all parts of vertex program together
    std::string makeVP(bool compTexCoord);

    // prohibit default functions (move to 'public' if you need one)
    void operator =(const ReflectiveMaterial &source);
};

typedef ReflectiveMaterial *ReflectiveMaterialP;

OSG_END_NAMESPACE

#include <OSGReflectiveMaterialBase.inl>
#include <OSGReflectiveMaterial.inl>

#define OSGREFLECTIVEMATERIAL_HEADER_CVSID "@(#)$Id: OSGReflectiveMaterial.h,v 1.1 2003/11/03 19:13:00 jnkautz Exp $"

#endif /* _OSGREFLECTIVEMATERIAL_H_ */
