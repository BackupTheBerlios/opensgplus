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

#include <OSGImage.h>

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
    static char cvsid_cpp[] = "@(#)$Id: OSGDVRSimpleLUTShader.cpp,v 1.2 2003/10/07 15:26:37 weiler Exp $";
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
    m_nTextureMode      = -1; 
    m_pFragProg         = NullFC;
    m_pDepTexture       = NullFC;
}

//! Copy Constructor

DVRSimpleLUTShader::DVRSimpleLUTShader(const DVRSimpleLUTShader &source) :
    Inherited(source)
{
    m_nTextureMode      = -1; 
    m_pFragProg         = NullFC;
    m_pDepTexture       = NullFC;
}

//! Destructor

DVRSimpleLUTShader::~DVRSimpleLUTShader(void)
{
    if (m_pFragProg != NullFC) {
        subRefCP(m_pFragProg);
	m_pFragProg = NullFC;
    }

    destroyDependentTexture();
}

/*----------------------------- class specific ----------------------------*/

//! returns texture formate for selected lookup table mode

void DVRSimpleLUTShader::getPaletteFormat( DrawActionBase */*action*/,
					   UInt8 mode,
					   GLenum & internalFormat,
					   GLenum & externalFormat )
{
    switch (mode)
    {
        case LM_TABLE_SGI:
	    internalFormat = GL_INTENSITY; 
	    externalFormat = GL_LUMINANCE; 
	    break;

#if defined GL_EXT_paletted_texture && defined GL_EXT_shared_texture_palette
        case LM_PALETTE_EXT:
	    internalFormat = GL_COLOR_INDEX8_EXT; 
	    externalFormat = GL_COLOR_INDEX; 
	    break;
#endif
	
        case LM_DEPENDENT:
	    internalFormat = GL_INTENSITY;
	    externalFormat = GL_LUMINANCE;
	    break;

        case LM_FRAGPROG:
	    internalFormat = GL_INTENSITY;
	    externalFormat = GL_LUMINANCE;
	    break;
	
        case LM_RELOAD:
	    internalFormat = GL_RGBA;  
	    externalFormat = GL_COLOR_INDEX; 
	    break;

        case LM_NO:
        default:
	    internalFormat = GL_INTENSITY; 
	    externalFormat = GL_LUMINANCE;
    }
    
    return;
}


//! Automatically select a lookup table mode
UInt8 DVRSimpleLUTShader::selectMode( DrawActionBase *action )
{
    
    if (isModeSupported( action, LM_TABLE_SGI ))
    {
        FDEBUG(("Using SGI postshading palette....\n"));
        FFATAL(("Using SGI postshading palette....\n"));
	return LM_TABLE_SGI;
    }

    if (isModeSupported( action, LM_FRAGPROG ))
    {
        FDEBUG(("Using fragment program postshading palette....\n"));
	FFATAL(("Using fragment program postshading palette....\n"));
	return LM_FRAGPROG;
    }

    if (isModeSupported( action, LM_DEPENDENT ))
    { 
        FDEBUG(("Using dependent texture postshading palette....\n"));
	FFATAL(("Using dependent texture postshading palette....\n"));
	return LM_DEPENDENT;
    }

    if (isModeSupported( action, LM_PALETTE_EXT ))
    {
        FDEBUG(("Using preshading palette....\n"));
        FFATAL(("Using preshading palette....\n"));
	return LM_PALETTE_EXT;
    }

    FDEBUG(("Using palette reload ....\n"));
    FFATAL(("Using palette reload ....\n"));
    return LM_RELOAD;
}

//! Checks whether the selected mode is supported
bool DVRSimpleLUTShader::isModeSupported( DrawActionBase *action, UInt8 mode )
{
    bool result = false;
  
    switch (mode) {
      case LM_TABLE_SGI:
	result = (action->getWindow()->hasExtension(_sgiTexColorTable) &&
		  (0 != strncmp("IMPACT", (char *) glGetString(GL_RENDERER), 6)));
	break;

      case LM_PALETTE_EXT:
	result = (action->getWindow()->hasExtension(_extPalettedTexture) &&
		  action->getWindow()->hasExtension(_extSharedPalettedTexture));
	break;
	
      case LM_DEPENDENT:
	result = (action->getWindow()->hasExtension(_arbMultitexture) &&
		  action->getWindow()->hasExtension(_nvTextureShader2));
	break;

      case LM_FRAGPROG:
	result = (action->getWindow()->hasExtension(_arbMultitexture) &&
		  action->getWindow()->hasExtension(_arbFragmentProgram));
	break;
	
      case LM_RELOAD:
	result = true;
	break;

      case LM_NO:
	result = true;
	break;

      default: 
	result = false;
    }
    
    return result;
}

//! enables lut

void DVRSimpleLUTShader::enablePalette()
{

  
}

//! diasables lut

void DVRSimpleLUTShader::disablePalette()
{

}

//! create dependent texture

void DVRSimpleLUTShader::initDependentTexture(int size)
{
    FDEBUG(("Initializing dependent texture"));

    ImagePtr img = Image::create();

    // we create a dummy 2D texture here since there is only a dependent
    // 2D texture lookup with texture shaders
    beginEditCP(img);
    img->set(Image::OSG_RGBA_PF, size, 2);
    img->clear(128);
    endEditCP(img);

    m_pDepTexture = TextureChunk::create();
    addRefCP( m_pDepTexture );
		    
    TextureChunkPtr & chunk = m_pDepTexture;
    beginEditCP( chunk );
    chunk->setImage( img );
    chunk->setInternalFormat( GL_RGBA );
    chunk->setExternalFormat( GL_RGBA );
		    
    chunk->setWrapS( GL_CLAMP_TO_EDGE );
    chunk->setWrapT( GL_CLAMP_TO_EDGE );
    chunk->setWrapR( GL_CLAMP_TO_EDGE );
	    
    chunk->setMinFilter( GL_LINEAR );
    chunk->setMagFilter( GL_LINEAR );
    
    chunk->setEnvMode(GL_REPLACE);
    
    endEditCP(chunk);

    FDEBUG((" - DONE\n"));
}

//! update dependent texture

void DVRSimpleLUTShader::updateDependentTexture(int size, const UInt8 * data)
{
    FDEBUG(("Updating dependent texture\n"));
    ImagePtr img = m_pDepTexture->getImage();

    beginEditCP(img);
    img->setSubData(0, 0, 0, size, 1, 1, data);
    img->setSubData(0, 1, 0, size, 1, 1, data);
    endEditCP(img);
		
    m_pDepTexture->imageContentChanged(); // makes begin/end edit unneccessary
    FDEBUG((" - DONE\n"));
}

//! destroy dependent texture

void DVRSimpleLUTShader::destroyDependentTexture()
{
    if (m_pDepTexture != NullFC) {
        subRefCP(m_pDepTexture);
	m_pDepTexture = NullFC;
    }
}

//! initialize the static features of the class, e.g. action callbacks

void DVRSimpleLUTShader::initMethod (void)
{
}

//! react to field changes

void DVRSimpleLUTShader::changed(BitVector whichField, UInt32 origin)
{
    FDEBUG(("DVRSimpleLUTShader::changed\n"));
    FWARNING(("DVRSimpleLUTShader::changed\n"));

    // trigger re-initialization
    if (whichField & LutModeFieldMask)
        setActiveLutMode(LM_AUTO);

    Inherited::changed(whichField, origin);
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
    FWARNING(("DVRSimpleLUTShader::initialize\n"));

    GLenum nInternalFormat = GL_INTENSITY; 
    GLenum nExternalFormat = GL_LUMINANCE; 
    m_nTexturePaletteMode  = LM_NO;
    m_nTextureId           = -1; 


    // Determine lookup table mechanism
    if (getLutMode() != LM_AUTO)
    {
        // A certain mode has been selected
        if (isModeSupported( action, getLutMode() ))
	{
	    SWARNING << "DVRSimpleLUTShader - User specified lookup table mode "
		     << int(getLutMode()) << std::endl;
	    m_nTexturePaletteMode =  getLutMode();
	}
	else
	{
	    SWARNING << "DVRSimpleLUTShader - Unsupported lookup table mode "
		     << int(getLutMode()) << " disabling LUT" << std::endl;
	    m_nTexturePaletteMode = LM_NO;
	}
    }
    else {
        // Use automatic mode selection
        m_nTexturePaletteMode = selectMode( action );
    }
    setActiveLutMode( m_nTexturePaletteMode );
    
      
    getPaletteFormat( action, m_nTexturePaletteMode, nInternalFormat, nExternalFormat );

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
						 -1);             // textureStage1
    
    if (m_nTextureId == -1) {
        SWARNING << "Error registering textures ..." << std::endl;
        return false;
    }
    else
    {
        SWARNING << "New texture Id: " << m_nTextureId << std::endl;
    }
    
    if (lut != NullFC)
    {
        beginEditCP(lut, DVRLookupTable::TouchedFieldMask);
	lut->setTouched( true ); // force initialization in first render pass
	endEditCP(lut, DVRLookupTable::TouchedFieldMask);
    }
    
    return true;
}

#include <OSGGLU.h>

#define CHECK_GL_ERROR(msg) {const GLenum e = glGetError(); if ((e != 0) && (e != GL_NO_ERROR)) {cout\
   << std::endl << "OpenGL-ERROR <0x" << setfill('0') << hex << setw(4) << e << dec\
   << "> in " __FILE__ "(" << __LINE__ << ")!" << std::endl << gluErrorString(e) << msg << std::endl;}}

//! Callback before any slice is rendered - setup per volume
void DVRSimpleLUTShader::activate       (DVRVolume *volume, DrawActionBase *action)
{
//    FDEBUG(("DVRSimpleLUTShader::activate - v1.2 (%d)\n", (int) this));

    Window * win = action->getWindow();

#if defined GL_NV_texture_shader2 // remove compiler warnings for unused variable
    void (*ActiveTextureARB)(GLenum target) = (void (*)(GLenum target)) win->getFunction(_funcActiveTextureARB);
#endif
    
    void (*ColorTableSGI) (GLenum, GLenum, GLsizei, GLenum, GLenum, const GLvoid *) = NULL;
    ColorTableSGI = (void (*) (GLenum, GLenum, GLsizei, GLenum, GLenum, const GLvoid *)) win->getFunction(_funcColorTableSGI);

    void (*ColorTableEXT) (GLenum, GLenum, GLsizei, GLenum, GLenum, const GLvoid *) = NULL;
    ColorTableEXT = (void (*) (GLenum, GLenum, GLsizei, GLenum, GLenum, const GLvoid *)) win->getFunction(_funcColorTableEXT);

    
    DVRSimpleShader::activate(volume, action);


    DVRLookupTablePtr   lut = DVRVOLUME_PARAMETER(volume, DVRLookupTable);
    DVRVolumeTexturePtr vol = DVRVOLUME_PARAMETER(volume, DVRVolumeTexture);

    if ((volume == NULL) || (vol == NullFC)) {
        SWARNING << "NO Volume ..." << std::endl;
        return;
    }

    
    // reinitialize if hardware mode has not yet been chosen or mode has changed
    if (getActiveLutMode() == LM_AUTO) {
        SNOTICE << "Reinitializing DVRSimpleLUTShader" << std::endl;
        cleanup(volume, action);
        initialize(volume, action);
	// notify volume about shader changed
	beginEditCP(DVRVolumePtr(volume), DVRVolume::ShaderFieldMask);
	endEditCP(DVRVolumePtr(volume), DVRVolume::ShaderFieldMask);
    }


    // reload lookup table if neccessary
    if ((lut != NullFC) && (lut->getTouched())) {
        int colorTableSize = lut->getSize(0);
	unsigned char * data = &(lut->getData(0));

	lut->setTouched( false );
	
	switch(m_nTexturePaletteMode) {
	    case LM_TABLE_SGI:
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

	    case LM_PALETTE_EXT:
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
	
	    case LM_RELOAD: {
	        FDEBUG(("Using palette reload ....\n"));
	        FFATAL(("Using palette reload ....\n"));
		glPixelTransferi(GL_MAP_COLOR, GL_TRUE);
		glPixelMapfv(GL_PIXEL_MAP_I_TO_R, colorTableSize, &(lut->getDataR(0)));
		glPixelMapfv(GL_PIXEL_MAP_I_TO_G, colorTableSize, &(lut->getDataG(0)));
		glPixelMapfv(GL_PIXEL_MAP_I_TO_B, colorTableSize, &(lut->getDataB(0)));
		glPixelMapfv(GL_PIXEL_MAP_I_TO_A, colorTableSize, &(lut->getDataA(0)));
		    
		volume->getTextureManager().reloadTexture(m_nTextureId, action);
		glPixelTransferi(GL_MAP_COLOR, GL_FALSE);
		break;
	    }

	    case LM_DEPENDENT: {
	        // Initialize dependent texture
	        if (m_pDepTexture == NullFC)
		    initDependentTexture(lut->getSize(0));

		// Setup the correct texture shader mode
		if (volume->getTextureMode(win) != m_nTextureMode) {
		    switch (volume->getTextureMode(win)) {
		      case TextureManager::TM_3D:
			  SLOG << "Setup texture shader for 3D" << std::endl;
			  break;
		      case TextureManager::TM_2D:
			  SLOG << "Setup texture shader for 2D" << std::endl;
			  break;
		      default:
			  FFATAL(( "Texture mode for dependent textures"
				   " not supported by DVRSimpleLUTShader" ));
			  return;
		    }
		}

		// Update dependent texture if neccessary
		updateDependentTexture(lut->getSize(0), &(lut->getData(0)));
		break;
	    }
		
	    case LM_FRAGPROG: {
	        // Initialize dependent texture
	        if (m_pDepTexture == NullFC)
		    initDependentTexture(lut->getSize(0));

		// Initialize fragment program
		if (m_pFragProg == NullFC) {
		    FDEBUG(("Initializing fragment program "));
		    m_pFragProg = FragmentProgramChunk::create();
		    addRefCP( m_pFragProg );

		    FDEBUG((" - DONE\n"));
		}

		// Load the appropriate program
		if (volume->getTextureMode(win) != m_nTextureMode) {
		    switch (volume->getTextureMode(win)) {
		      case TextureManager::TM_3D:
			  // SLOG << "Loading ... lutFragProg3D.asm" << std::endl;
			  beginEditCP(m_pFragProg);
		          m_pFragProg->setProgram(_fragProg3D);
			  //m_pFragProg->read( "lutFragProg3D.asm" );
			  endEditCP(m_pFragProg);
			  break;
		      case TextureManager::TM_2D:
			  // SLOG << "Loading ... lutFragProg2D.asm" << std::endl;
			  beginEditCP(m_pFragProg);
		          m_pFragProg->setProgram(_fragProg2D);
			  // m_pFragProg->read( "lutFragProg2D.asm" );
			  endEditCP(m_pFragProg);
			  break;
		      case TextureManager::TM_2D_Multi:
			  // SLOG << "Loading ... lutFragProg2DMulti.asm" << std::endl;
			  beginEditCP(m_pFragProg);
		          m_pFragProg->setProgram(_fragProg2DMulti);
			  //m_pFragProg->read( "lutFragProg2DMulti.asm" );
			  endEditCP(m_pFragProg);
			  break;
		      default:
			  FFATAL(( "Texture mode for fragment programs"
				   " not supported by DVRSimpleLUTShader" ));
			  return;
		    }
		    m_nTextureMode = volume->getTextureMode(win);
		}
		
		// Update dependent texture if neccessary
		updateDependentTexture(lut->getSize(0), &(lut->getData(0)));
		break;
	    }
		 
	    case LM_NO:
	        FDEBUG(("Using NO palette reload ....\n"));
		break;
	}
    }


    // enable lookup table
    if (lut != NullFC)
    {
        switch(m_nTexturePaletteMode) {
	    case LM_TABLE_SGI:
#if defined GL_SGI_texture_color_table
	        glEnable(GL_TEXTURE_COLOR_TABLE_SGI);
#endif
		break;
	    case LM_PALETTE_EXT:
#if defined GL_EXT_paletted_texture && defined GL_EXT_shared_texture_palette
	        glEnable(GL_SHARED_TEXTURE_PALETTE_EXT);
#endif
		break;

	    case LM_DEPENDENT:
#if defined GL_NV_texture_shader2
	        m_pDepTexture->activate(action, 1);
		
		//!! Texture Stage 1 - dependent lookup
		glEnable(GL_TEXTURE_SHADER_NV);
		ActiveTextureARB(GL_TEXTURE1_ARB);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_DEPENDENT_AR_TEXTURE_2D_NV);

		//!! Texture Stage 0 - ordinary lookup
	        ActiveTextureARB(GL_TEXTURE0_ARB);
		glEnable(GL_TEXTURE_SHADER_NV);

		switch (volume->getTextureMode(win)) {
		    case TextureManager::TM_3D:
		        glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_TEXTURE_3D);
			break;
		    case TextureManager::TM_2D:
		        glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_TEXTURE_2D);
			break;
		    default:
//		        FFATAL(( "Texture mode for dependent texture not supported by DVRSimpleLUTShader" ));
			break;
		}
		
	        break;
#endif

	    case LM_FRAGPROG:
	        // activate fragment program chunk
		m_pDepTexture->activate( action, 2 );
	        m_pFragProg->activate( action );
	        break;

	  default:
	        break;
	}
    }

}

//! Callback before any brick - state setup per brick
void DVRSimpleLUTShader::brickActivate  (DVRVolume *volume, DrawActionBase *action, Brick *brick)
{
//    FDEBUG(("DVRSimpleLUTShader::brickActivate %d\n", (int) this));

    DVRSimpleShader::brickActivate( volume, action, brick );

}

//! Callback after all rendering of the volume is done
void DVRSimpleLUTShader::deactivate     (DVRVolume *volume, DrawActionBase *action)
{
    FDEBUG(("DVRSimpleLUTShader::deactivate\n"));

#if defined GL_NV_texture_shader2 // remove compiler warnings for unused variable
    void (*ActiveTextureARB)(GLenum target) =
      (void (*)(GLenum target)) action->getWindow()->getFunction(_funcActiveTextureARB);
#endif
    
    switch(m_nTexturePaletteMode) {
        case LM_TABLE_SGI:
#if defined GL_SGI_texture_color_table
	    glDisable(GL_TEXTURE_COLOR_TABLE_SGI);
#endif
	    break;

        case LM_PALETTE_EXT:
#if defined GL_EXT_paletted_texture && defined GL_EXT_shared_texture_palette
	    glDisable(GL_SHARED_TEXTURE_PALETTE_EXT);
#endif
	    break;
	    
        case LM_DEPENDENT:
#if defined GL_NV_texture_shader2
	    ActiveTextureARB(GL_TEXTURE1_ARB);
	    glDisable(GL_TEXTURE_SHADER_NV);

	    ActiveTextureARB(GL_TEXTURE0_ARB);
	    glDisable(GL_TEXTURE_SHADER_NV);

	    m_pDepTexture->deactivate( action, 1 );
	    break;
#endif

        case LM_FRAGPROG:
	    // de-activate fragment program chunk
	    m_pFragProg->deactivate( action );
	    m_pDepTexture->deactivate( action, 2 );
	    break;
	    
        case LM_RELOAD:
	    glPixelTransferi(GL_MAP_COLOR, GL_FALSE);
	    break;

        default:
	    break;
    }
    
    
    DVRSimpleShader::deactivate( volume, action );
}

//! Callback to clean up shader resources
void DVRSimpleLUTShader::cleanup        (DVRVolume *volume, DrawActionBase *)
{
    if (volume != NULL)
    {
        if (m_nTextureId != -1)
	    volume->getTextureManager().unregisterTexture(m_nTextureId);
    }

    destroyDependentTexture();
}


char DVRSimpleLUTShader::_fragProg2D[] =
         "!!ARBfp1.0 \n"
         "TEMP volume; \n"	
	 "PARAM bias = {0.001953125, 0.0, 0.0, 0.0};\n"
	 "PARAM scale = {0.99609375, 0.99609375, 0.99609375}; # 255/256\n"
	 "PARAM const = {0.0, 0.0, 0.0, 0.0};\n"
	 "TEMP depCoor;\n"

	 "TEX volume, fragment.texcoord[0], texture[0], 2D;\n"
	 "MOV depCoor, const;\n"
	 "MOV depCoor.x, volume;\n"
	 "MAD depCoor, depCoor, scale, bias;\n"
	 "TEX result.color, depCoor, texture[2], 2D;\n"
	 "END\n";


char DVRSimpleLUTShader::_fragProg3D[] =
	 "!!ARBfp1.0 \n"
	 "TEMP volume; \n"	
	 "PARAM bias = {0.001953125, 0.0, 0.0, 0.0};\n"
	 "PARAM scale = {0.99609375, 0.99609375, 0.99609375}; # 255/256\n"
	 "PARAM const = {0.0, 0.0, 0.0, 0.0};\n"
	 "TEMP depCoor;\n"

	 "TEX volume, fragment.texcoord[0], texture[0], 3D;\n"
	 "MOV depCoor, const;\n"
	 "MOV depCoor.x, volume;\n"
	 "MAD depCoor, depCoor, scale, bias;\n"
	 "TEX result.color, depCoor, texture[2], 2D;\n"
	 "END\n";

char DVRSimpleLUTShader::_fragProg2DMulti[] =
	 "!!ARBfp1.0 \n"
	 "TEMP volume; \n"
	 "TEMP stage0; \n"
	 "TEMP stage1; \n"
	 "PARAM bias = {0.001953125, 0.0, 0.0, 0.0};\n"
	 "PARAM scale = {0.99609375, 0.99609375, 0.99609375}; # 255/256\n"
	 "PARAM const = {0.0, 0.0, 0.0, 0.0};\n"
	 "TEMP depCoor;\n"

	 "TEX stage0, fragment.texcoord[0], texture[0], 2D;\n"
	 "TEX stage1, fragment.texcoord[1], texture[1], 2D;\n"
         "LRP volume, state.fog.color.a, stage1, stage0;\n"
	 "MOV depCoor, const;\n"
	 "MOV depCoor.x, volume;\n"
	 "MAD depCoor, depCoor, scale, bias;\n"
	 "TEX result.color, depCoor, texture[2], 2D;\n"
	 "END\n";

UInt32 DVRSimpleLUTShader::_sgiTexColorTable         = Window::registerExtension( "GL_SGI_texture_color_table" );
UInt32 DVRSimpleLUTShader::_extPalettedTexture       = Window::registerExtension( "GL_EXT_paletted_texture" );
UInt32 DVRSimpleLUTShader::_extSharedPalettedTexture = Window::registerExtension( "GL_EXT_shared_texture_palette" );
UInt32 DVRSimpleLUTShader::_arbMultitexture          = Window::registerExtension( "GL_ARB_multitexture" );
UInt32 DVRSimpleLUTShader::_nvTextureShader2         = Window::registerExtension( "GL_NV_texture_shader2" );
UInt32 DVRSimpleLUTShader::_arbFragmentProgram       = Window::registerExtension( "GL_ARB_fragment_program" );

UInt32 DVRSimpleLUTShader::_funcColorTableSGI        = Window::registerFunction ( OSG_DLSYM_UNDERSCORE"glColorTableSGI" );
UInt32 DVRSimpleLUTShader::_funcColorTableEXT        = Window::registerFunction ( OSG_DLSYM_UNDERSCORE"glColorTableEXT" );
UInt32 DVRSimpleLUTShader::_funcActiveTextureARB     = Window::registerFunction ( OSG_DLSYM_UNDERSCORE"glActiveTextureARB" );
