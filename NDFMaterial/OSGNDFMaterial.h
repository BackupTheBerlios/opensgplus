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

#ifndef _OSGNDFMATERIAL_H_
#define _OSGNDFMATERIAL_H_
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

#include "OSGNDFMaterialBase.h"

OSG_BEGIN_NAMESPACE

/*! \brief NDFMaterial class.
     NDF-shading model.
  The ambient and diffuse parts are the same as in the Phong model. The
  highligh is painted on the 2D texture (normal distribution function).
      Lo = ka + kd * Li(l) * n.l/Pi + ks * S(l,v) * p(h) * n.l S(l,v) =
         = Li(l) * G(l,v) * F(h.v)/(Pi * n.l * n.v) G(l,v) =
	 = min{1, n.h * n.v/h.v, n.h * n.l/h.v} F(h.v) =
	 = f + (1 - f)*(1 - h.v)^5

  Additionally here is a possibility to add the texture to the
  diffuse part.
*/

class OSG_SYSTEMLIB_DLLMAPPING NDFMaterial : public NDFMaterialBase
{
  private:

    typedef NDFMaterialBase Inherited;

    /*==========================  PUBLIC  =================================*/
  public:

    /*---------------------------------------------------------------------*/
    /*! \name                    Class Get                                 */
    /*! \{                                                                 */

    static const char *getClassname(void) { return "NDFMaterial"; };

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

    // Variables should all be in NDFMaterialBase.

    /*---------------------------------------------------------------------*/
    /*! \name                  Constructors                                */
    /*! \{                                                                 */

    NDFMaterial(void);
    NDFMaterial(const NDFMaterial &source);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructors                                */
    /*! \{                                                                 */

    virtual ~NDFMaterial(void); 

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                     Helper                                   */
    /*! \{                                                                 */

    void prepareLocalChunks(void);

    /*! \}                                                                 */


    /*==========================  PRIVATE  ================================*/
  private:

    friend class FieldContainer;
    friend class NDFMaterialBase;

    static void initMethod(void);
    // assembly all parts of fragment and vertex program together
    std::string makeFP(bool OnlyDiffuse, bool CorrectCosine);
    std::string makeVP(bool SelfShadowing, bool FresnelTerm, bool CorrectCosine);

    // these chunks are used for rendering the material
    //---------------------------------------------------------------------
    MaterialChunkPtr          _materialChunk;         // The base of material
    TextureChunkPtr           _textureChunkDiffuse;   // Texture for diffuse term
    TextureChunkPtr           _textureChunkSpecular;  // Texture for specular term
    VertexProgramChunkPtr     _vpChunk;         // Compute specular color, correct
                                                // cosine, texture coordinats
    FragmentProgramChunkPtr   _fpChunk;         // Add diffuse, ambient terms
    RegisterCombinersChunkPtr _rcChunk;         // Add diffuse, ambient terms
    // _fpChunk and _rcChunk are alternative (It is possible to use
    //    only one of  them)

    // prohibit default functions (move to 'public' if you need one)
    void operator =(const NDFMaterial &source);
};

typedef NDFMaterial *NDFMaterialP;

OSG_END_NAMESPACE

#include <OSGNDFMaterialBase.inl>
#include <OSGNDFMaterial.inl>

#define OSGNDFMATERIAL_HEADER_CVSID "@(#)$Id: OSGNDFMaterial.h,v 1.1 2003/05/19 12:04:39 jnkautz Exp $"

#endif /* _OSGNDFMATERIAL_H_ */
