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

#ifndef _OSGDVRSIMPLELUTSHADER_H_
#define _OSGDVRSIMPLELUTSHADER_H_
#ifdef __sgi
#pragma once
#endif

#include <OSGConfig.h>

#include <OSGDVRSimpleLUTShaderBase.h>

OSG_BEGIN_NAMESPACE

/*! \brief *put brief class description here* 
 */

class OSG_VOLRENLIB_DLLMAPPING DVRSimpleLUTShader : public DVRSimpleLUTShaderBase
{
  private:

    typedef DVRSimpleLUTShaderBase Inherited;

    /*==========================  PUBLIC  =================================*/
  public:

    /*---------------------------------------------------------------------*/
    /*! \name                      Sync                                    */
    /*! \{                                                                 */

    virtual void changed(BitVector  whichField, 
                         UInt32     from);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                     Output                                   */
    /*! \{                                                                 */

    virtual void dump(      UInt32     uiIndent = 0, 
                      const BitVector  bvFlags  = 0) const;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                 Volume Rendering                             */
    /*! \{                                                                 */

    // Callback to set up shader - register textures here
    virtual bool initialize     (DVRVolume *volume, DrawActionBase *action);
    
    // Callback before any slice is rendered - setup per volume
    virtual void activate       (DVRVolume *volume, DrawActionBase *action);

    // Callback before any brick - state setup per brick
    virtual void brickActivate  (DVRVolume *volume, DrawActionBase *action, UInt8 brickId);

    // Callback after all rendering of the volume is done
    virtual void deactivate     (DVRVolume *volume, DrawActionBase *action);

    /*! \}                                                                 */
    /*=========================  PROTECTED  ===============================*/
  protected:

    // Variables should all be in DVRSimpleLUTShaderBase.

    UInt8 m_nTexturePaletteMode;
    
    /*---------------------------------------------------------------------*/
    /*! \name                  Constructors                                */
    /*! \{                                                                 */

    DVRSimpleLUTShader(void);
    DVRSimpleLUTShader(const DVRSimpleLUTShader &source);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructors                                */
    /*! \{                                                                 */

    virtual ~DVRSimpleLUTShader(void); 

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                 Volume Rendering                             */
    /*! \{                                                                 */

    // Returns suitable palette format and texture formate for graphics adapter
    void getPaletteFormat( DrawActionBase *action,
			   GLenum & internalFormat,
			   GLenum & externalFormat,
			   UInt8  & texturePaletteMode );
    // Enable/disable palette
    void enablePalette();
    void disablePalette();
    
    /*! \}                                                                 */
    
    /*==========================  PRIVATE  ================================*/
  private:
    enum PaletteMode {
        NO_PALETTE,
	PALETTE_SGI,
	PALETTE_EXT,
	PALETTE_RELOAD
    };

    // extension indices for used extensions;
    static UInt32 _sgiTexColorTable;
    static UInt32 _extPalettedTexture;
    static UInt32 _extSharedPalettedTexture;

    // extension indices for used fucntions;
    static UInt32 _funcColorTableSGI;
    static UInt32 _funcColorTableEXT;
    
  private:

    friend class FieldContainer;
    friend class DVRSimpleLUTShaderBase;

    static void initMethod(void);

    // prohibit default functions (move to 'public' if you need one)

    void operator =(const DVRSimpleLUTShader &source);
};

typedef DVRSimpleLUTShader *DVRSimpleLUTShaderP;

OSG_END_NAMESPACE

#include <OSGDVRSimpleLUTShader.inl>
#include <OSGDVRSimpleLUTShaderBase.inl>

#define OSGDVRSIMPLELUTSHADER_HEADER_CVSID "@(#)$Id: OSGDVRSimpleLUTShader.h,v 1.1 2002/10/10 11:11:26 weiler Exp $"

#endif /* _OSGDVRSIMPLELUTSHADER_H_ */
