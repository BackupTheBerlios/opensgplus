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

//---------------------------------------------------------------------------
//  Includes
//---------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>

#include <OSGConfig.h>
#include <OSGWindow.h>

#include "OSGDVRLookupTable.h"
#include "OSGDVRSimpleShader.h"
#include "OSGDVRVolumeTexture.h"
#include "OSGDVRVolume.h"

#include "OSGDVRSimpleLUTShader.h"

OSG_USING_NAMESPACE

#ifdef __sgi
#pragma set woff 1174
#endif

namespace
{
    static char cvsid_cpp[] = "@(#)$Id: OSGDVRSimpleLUTShader.cpp,v 1.1 2002/10/10 11:11:26 weiler Exp $";
    static char cvsid_hpp[] = OSGDVRSIMPLELUTSHADER_HEADER_CVSID;
    static char cvsid_inl[] = OSGDVRSIMPLELUTSHADER_INLINE_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

/*! \class osg::DVRSimpleLUTShader
Shader for color volume rendering with transfer functions. Is intended to be stateless - NO FIELDS!!
*/

/*----------------------- constructors & destructors ----------------------*/

//! Constructor

DVRSimpleLUTShader::DVRSimpleLUTShader(void) :
    Inherited()
{
}

//! Copy Constructor

DVRSimpleLUTShader::DVRSimpleLUTShader(const DVRSimpleLUTShader &source) :
    Inherited(source)
{
}

//! Destructor

DVRSimpleLUTShader::~DVRSimpleLUTShader(void)
{
}

/*----------------------------- class specific ----------------------------*/

//! returns suitable palette format and texture formate for graphics adapter

void DVRSimpleLUTShader::getPaletteFormat( DrawActionBase *action,
					   GLenum & internalFormat,
					   GLenum & externalFormat,
					   UInt8  & texturePaletteMode )
{

    // default values
    internalFormat = GL_INTENSITY; 
    externalFormat = GL_LUMINANCE; 
    texturePaletteMode  = NO_PALETTE; 

    
    ////////////////////////////////////////////////////////////////////////////// 
    //  Check for Open GL Support for paletted textures 
    // 

#if defined GL_SGI_texture_color_table
    if (action->getWindow()->hasExtension(_sgiTexColorTable) &&
	(0 != strncmp("IMPACT", (char *) glGetString(GL_RENDERER), 6))) {
			
        FDEBUG(("Using SGI postshading palette....\n"));
	texturePaletteMode = PALETTE_SGI; 
	internalFormat = GL_INTENSITY; 
	externalFormat = GL_LUMINANCE; 
	return;
    }
#endif
	
#if defined GL_EXT_paletted_texture && defined GL_EXT_shared_texture_palette
    if (action->getWindow()->hasExtension(_extPalettedTexture) &&
	action->getWindow()->hasExtension(_extSharedPalettedTexture)) {
	  
        FDEBUG(("Using preshading palette....\n"));
	texturePaletteMode = PALETTE_EXT; 
	internalFormat = GL_COLOR_INDEX8_EXT; 
	externalFormat = GL_COLOR_INDEX; 
	return;
    }
#endif
	
    FDEBUG(("Using palette reload ....\n"));
    texturePaletteMode = PALETTE_RELOAD; 
    internalFormat = GL_RGBA;  
    externalFormat = GL_COLOR_INDEX; 
}

//! enables lut

void DVRSimpleLUTShader::enablePalette()
{

  
}

//! diasables lut

void DVRSimpleLUTShader::disablePalette()
{

}

//! initialize the static features of the class, e.g. action callbacks

void DVRSimpleLUTShader::initMethod (void)
{
}

//! react to field changes

void DVRSimpleLUTShader::changed(BitVector, UInt32)
{
}

//! output the instance for debug purposes

void DVRSimpleLUTShader::dump(      UInt32    , 
                         const BitVector ) const
{
    SLOG << "Dump DVRSimpleLUTShader NI" << std::endl;
}


//! Callback to set up shader - register textures here
bool DVRSimpleLUTShader::initialize     (DVRVolume *volume, DrawActionBase *action)
{
    FDEBUG(("DVRSimpleLUTShader::initialize\n"));

    GLenum nInternalFormat = GL_INTENSITY; 
    GLenum nExternalFormat = GL_LUMINANCE; 
    m_nTexturePaletteMode  = NO_PALETTE; 
    m_nTextureId           = -1; 

    getPaletteFormat( action, nInternalFormat, nExternalFormat, m_nTexturePaletteMode );

    DVRLookupTablePtr   lut = DVRVOLUME_PARAMETER(volume, DVRLookupTable);
    DVRVolumeTexturePtr vol = DVRVOLUME_PARAMETER(volume, DVRVolumeTexture);

    if ((volume == NULL) || (vol == NullFC)) {
        SWARNING << "NO Volume..." << std::endl;
        return false;
    }

    m_nTextureId = volume->getTextureManager().registerTexture(
                                                 vol->getImage(), // image
						 nInternalFormat, // internalFormat
						 nExternalFormat, // externalFormat
						 1,               // doBricking
						 0,               // textureStage0
						 -1,              // textureStage1
						 0);              // doCopy
    
    if (m_nTextureId == -1) {
        SWARNING << "Error registering textures ..." << std::endl;
        return false;
    }
    
    if (lut != NullFC)
      lut->setTouched( true ); // force initialization in first render pass

    return true;
}

#include <GL/glu.h>

#define CHECK_GL_ERROR(msg) {const GLenum e = glGetError(); if ((e != 0) && (e != GL_NO_ERROR)) {cout\
   << std::endl << "OpenGL-ERROR <0x" << setfill('0') << hex << setw(4) << e << dec\
   << "> in " __FILE__ "(" << __LINE__ << ")!" << std::endl << gluErrorString(e) << msg << std::endl;}}

//! Callback before any slice is rendered - setup per volume
void DVRSimpleLUTShader::activate       (DVRVolume *volume, DrawActionBase *action)
{
//    FDEBUG(("DVRSimpleLUTShader::activate - v1.2 (%d)\n", (int) this));

    Window * win = action->getWindow();

#if defined GL_SGI_texture_color_table
    void (*ColorTableSGI) (GLenum, GLenum, GLsizei, GLenum, GLenum, const GLvoid *) = NULL;
    ColorTableSGI = (void (*) (GLenum, GLenum, GLsizei, GLenum, GLenum, const GLvoid *)) win->getFunction(_funcColorTableSGI);
#endif
    
#if defined GL_EXT_paletted_texture && defined GL_EXT_shared_texture_palette
    void (*ColorTableEXT) (GLenum, GLenum, GLsizei, GLenum, GLenum, const GLvoid *) = NULL;
    ColorTableEXT = (void (*) (GLenum, GLenum, GLsizei, GLenum, GLenum, const GLvoid *)) win->getFunction(_funcColorTableEXT);
#endif

    
    DVRSimpleShader::activate(volume, action);


    DVRLookupTablePtr   lut = DVRVOLUME_PARAMETER(volume, DVRLookupTable);
    DVRVolumeTexturePtr vol = DVRVOLUME_PARAMETER(volume, DVRVolumeTexture);

    if ((volume == NULL) || (vol == NullFC)) {
        SWARNING << "NO Volume ..." << std::endl;
        return;
    }


    // enable lookup table
    if (lut != NullFC)
    {
        switch(m_nTexturePaletteMode) {
	    case PALETTE_SGI:
#if defined GL_SGI_texture_color_table
	        glEnable(GL_TEXTURE_COLOR_TABLE_SGI);
#endif
		break;
	    case PALETTE_EXT:
#if defined GL_EXT_paletted_texture && defined GL_EXT_shared_texture_palette
	        glEnable(GL_SHARED_TEXTURE_PALETTE_EXT);
#endif
		break;
	  default:
	        break;
	}
    }


    // reload lookup table if neccessary
    if ((lut != NullFC) && (lut->getTouched())) {
        int colorTableSize = lut->getSize(0);
	unsigned char * data = &(lut->getData(0));

	lut->setTouched( false );
	
	switch(m_nTexturePaletteMode) {
	    case PALETTE_SGI:
	        FDEBUG(("Using SGI postshading palette....\n"));
#if defined GL_SGI_texture_color_table
		glEnable(GL_TEXTURE_COLOR_TABLE_SGI); 
		ColorTableSGI(GL_TEXTURE_COLOR_TABLE_SGI, // GLenum target,
			      GL_RGBA,                    // GLenum internalformat, 
			      colorTableSize,             // GLsizei width, 
			      GL_RGBA,                    // GLenum format, 
			      GL_UNSIGNED_BYTE,           // GLenum type, 
			      data);                      // const GLvoid *table 
#endif // GL_SGI_texture_color_table	 
		break;

	    case PALETTE_EXT:
	        FDEBUG(("Using preshading palette....\n"));
#if defined GL_EXT_paletted_texture && defined GL_EXT_shared_texture_palette
	        glEnable(GL_SHARED_TEXTURE_PALETTE_EXT);
		ColorTableEXT(GL_SHARED_TEXTURE_PALETTE_EXT, // GLenum target, 
			      GL_RGBA,                       // GLenum internalformat, 
			      colorTableSize,                // GLsizei width, 
			      GL_RGBA,                       // GLenum format, 
			      GL_UNSIGNED_BYTE,              // GLenum type, 
			      data);                         // const GLvoid *table  
#endif	
		break;
	
	    case PALETTE_RELOAD: {
	        FDEBUG(("Using palette reload ....\n"));
		glPixelTransferi(GL_MAP_COLOR, GL_TRUE);
		glPixelMapfv(GL_PIXEL_MAP_I_TO_R, colorTableSize, &(lut->getDataR(0)));
		glPixelMapfv(GL_PIXEL_MAP_I_TO_G, colorTableSize, &(lut->getDataG(0)));
		glPixelMapfv(GL_PIXEL_MAP_I_TO_B, colorTableSize, &(lut->getDataB(0)));
		glPixelMapfv(GL_PIXEL_MAP_I_TO_A, colorTableSize, &(lut->getDataA(0)));
		    
		volume->getTextureManager().reloadTexture(m_nTextureId, action);
		glPixelTransferi(GL_MAP_COLOR, GL_FALSE);
		break;
	    }

	    case NO_PALETTE:
	        FDEBUG(("Using NO palette reload ....\n"));
		break;
	}
    }
}

//! Callback before any brick - state setup per brick
void DVRSimpleLUTShader::brickActivate  (DVRVolume *volume, DrawActionBase *action, UInt8 brickId)
{
//    FDEBUG(("DVRSimpleLUTShader::brickActivate %d\n", (int) this));

    DVRSimpleShader::brickActivate( volume, action, brickId );
  
}

//! Callback after all rendering of the volume is done
void DVRSimpleLUTShader::deactivate     (DVRVolume *volume, DrawActionBase *action)
{
    FDEBUG(("DVRSimpleLUTShader::deactivate\n"));
    
    if (m_nTexturePaletteMode == PALETTE_RELOAD) {
        glPixelTransferi(GL_MAP_COLOR, GL_FALSE);
    } 

    DVRSimpleShader::deactivate( volume, action );
}


UInt32 DVRSimpleLUTShader::_sgiTexColorTable         = Window::registerExtension( "GL_SGI_texture_color_table" );
UInt32 DVRSimpleLUTShader::_extPalettedTexture       = Window::registerExtension( "GL_EXT_paletted_texture" );
UInt32 DVRSimpleLUTShader::_extSharedPalettedTexture = Window::registerExtension( "GL_EXT_shared_texture_palette");

UInt32 DVRSimpleLUTShader::_funcColorTableSGI        = Window::registerFunction ( OSG_DLSYM_UNDERSCORE"glColorTableSGI" );
UInt32 DVRSimpleLUTShader::_funcColorTableEXT        = Window::registerFunction ( OSG_DLSYM_UNDERSCORE"glColorTableEXT" );
