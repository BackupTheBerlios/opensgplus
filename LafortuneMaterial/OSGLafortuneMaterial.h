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

#ifndef _OSGLAFORTUNEMATERIAL_H_
#define _OSGLAFORTUNEMATERIAL_H_
#ifdef __sgi
#pragma once
#endif

#include <OSGConfig.h>
#include <OSGMaterialChunk.h>
#include <OSGTextureChunk.h>
#include <OSGVertexProgramChunk.h>
#include <OSGFragmentProgramChunk.h>
#include <OSGRegisterCombinersChunk.h>

#include <rpc/rpc.h>
#include <OSGImage.h>
#include <OSGTexGenChunk.h>
#include <OSGTextureTransformChunk.h>
#include <iostream.h>

#include "OSGLafortuneMaterialBase.h"

OSG_BEGIN_NAMESPACE

/*! \brief LafortuneMaterial class. See \ref 
           PageSystemLafortuneMaterial for a description.
   Material with using of Lafortune reflectance function.
   BRDF is computed as follows:
      f_brdf = Ks*(Cx*(ux*vx+uy*vy)+Cz*uz*vz);
   and the color at the end is:
      if(useWeight = 1)
      {
          if(selector = 1)
             c = tex*(rho + f_brdf)*cosine(light_local, normal_local)
          else
             c = tex*pho + f_brdf
      }
      else   c = pho + f_brdf
*/

class OSG_SYSTEMLIB_DLLMAPPING LafortuneMaterial : public LafortuneMaterialBase
{
  private:

    typedef LafortuneMaterialBase Inherited;

    /*==========================  PUBLIC  =================================*/
  public:

    /*---------------------------------------------------------------------*/
    /*! \name                    Class Get                                 */
    /*! \{                                                                 */

    static const char *getClassname(void) { return "LafortuneMaterial"; };

    /*! \}                                                                 */

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

    virtual bool       isTransparent     (void) const;

    /*! \}                                                                 */
    /*=========================  PROTECTED  ===============================*/
  protected:

    // Variables should all be in LafortuneMaterialBase.

    /*---------------------------------------------------------------------*/
    /*! \name                  Constructors                                */
    /*! \{                                                                 */

    LafortuneMaterial(void);
    LafortuneMaterial(const LafortuneMaterial &source);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructors                                */
    /*! \{                                                                 */

    virtual ~LafortuneMaterial(void); 

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                     Helper                                   */
    /*! \{                                                                 */

    void prepareLocalChunks(void);

    /*! \}                                                                 */

    /*==========================  PRIVATE  ================================*/
  private:

    friend class FieldContainer;
    friend class LafortuneMaterialBase;

    static void initMethod(void);

    // these chunks are used for rendering the material
    //---------------------------------------------------------------------
    MaterialChunkPtr        _materialChunk;  // The base of material
    TextureChunkPtr         _textureChunk;   // Texture for weighting the result
    VertexProgramChunkPtr   _vpChunk;        // Compute specular color, correct
                                             //    cosine, texture coordinats
    FragmentProgramChunkPtr   _fpChunk;      // Add diffuse, ambient terms (use either FP or RC)
    RegisterCombinersChunkPtr _rcChunk;      // Add diffuse, ambient terms

    // prohibit default functions (move to 'public' if you need one)
    void operator =(const LafortuneMaterial &source);
};

typedef LafortuneMaterial *LafortuneMaterialP;

OSG_END_NAMESPACE

#include <OSGLafortuneMaterialBase.inl>
#include <OSGLafortuneMaterial.inl>

#define OSGLAFORTUNEMATERIAL_HEADER_CVSID "@(#)$Id: OSGLafortuneMaterial.h,v 1.1 2003/12/03 21:49:52 jnkautz Exp $"

#endif /* _OSGLAFORTUNEMATERIAL_H_ */
