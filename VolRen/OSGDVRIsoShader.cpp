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
#include <OSGGL.h>
#include <OSGGLEXT.h>
#include <OSGSlicer.h>
#include <OSGGLU.h>

#include "OSGDVRIsoShader.h"

OSG_USING_NAMESPACE

#ifdef __sgi
#pragma set woff 1174
#endif

namespace
{
    static char cvsid_cpp[] = "@(#)$Id: OSGDVRIsoShader.cpp,v 1.1 2002/10/10 11:11:26 weiler Exp $";
    static char cvsid_hpp[] = OSGDVRISOSHADER_HEADER_CVSID;
    static char cvsid_inl[] = OSGDVRISOSHADER_INLINE_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif


// register extensions and initialize extension functions
UInt32 DVRIsoShader::_ARB_multitexture       = Window::registerExtension( "GL_ARB_multitexture" );
UInt32 DVRIsoShader::_EXT_texture3D          = Window::registerExtension( "GL_EXT_texture3D" );
UInt32 DVRIsoShader::_NV_register_combiners  = Window::registerExtension( "GL_NV_register_combiners");
UInt32 DVRIsoShader::_NV_register_combiners2 = Window::registerExtension( "GL_NV_register_combiners2");
UInt32 DVRIsoShader::_SGI_color_matrix       = Window::registerExtension( "GL_SGI_color_matrix");

UInt32 DVRIsoShader::_funcActiveTextureARB            = Window::invalidFunctionID;
UInt32 DVRIsoShader::_funcMultiTexCoord2dARB          = Window::invalidFunctionID;
UInt32 DVRIsoShader::_funcTexImage3DEXT               = Window::invalidFunctionID;
UInt32 DVRIsoShader::_funcCombinerParameteriNV        = Window::invalidFunctionID;
UInt32 DVRIsoShader::_funcCombinerParameterfvNV       = Window::invalidFunctionID;
UInt32 DVRIsoShader::_funcCombinerStageParameterfvNV  = Window::invalidFunctionID;
UInt32 DVRIsoShader::_funcSecondaryColor3fEXT         = Window::invalidFunctionID;
UInt32 DVRIsoShader::_funcSecondaryColor3fvEXT        = Window::invalidFunctionID;
UInt32 DVRIsoShader::_funcCombinerInputNV             = Window::invalidFunctionID;
UInt32 DVRIsoShader::_funcCombinerOutputNV            = Window::invalidFunctionID;
UInt32 DVRIsoShader::_funcFinalCombinerInputNV        = Window::invalidFunctionID;


/*! \class osg::DVRIsoShader
Simple iso surface shader
*/

/*----------------------- constructors & destructors ----------------------*/

//! Constructor

DVRIsoShader::DVRIsoShader(void) :
    Inherited()
{
  _funcActiveTextureARB            = Window::registerFunction( OSG_DLSYM_UNDERSCORE"glActiveTextureARB" );
  _funcMultiTexCoord2dARB          = Window::registerFunction( OSG_DLSYM_UNDERSCORE"glMultiTexCoord2dARB" );
  _funcTexImage3DEXT               = Window::registerFunction ( OSG_DLSYM_UNDERSCORE"glTexImage3DEXT" );
  _funcCombinerParameteriNV        = Window::registerFunction ( OSG_DLSYM_UNDERSCORE"glCombinerParameteriNV" );
  _funcCombinerParameterfvNV       = Window::registerFunction ( OSG_DLSYM_UNDERSCORE"glCombinerParameterfvNV" );
  _funcCombinerStageParameterfvNV  = Window::registerFunction ( OSG_DLSYM_UNDERSCORE"glCombinerStageParameterfvNV" );
  _funcSecondaryColor3fEXT         = Window::registerFunction ( OSG_DLSYM_UNDERSCORE"glSecondaryColor3fEXT" );
  _funcSecondaryColor3fvEXT        = Window::registerFunction ( OSG_DLSYM_UNDERSCORE"glSecondaryColor3fvEXT" );
  _funcCombinerInputNV             = Window::registerFunction ( OSG_DLSYM_UNDERSCORE"glCombinerInputNV" );
  _funcCombinerOutputNV            = Window::registerFunction ( OSG_DLSYM_UNDERSCORE"glCombinerOutputNV" );
  _funcFinalCombinerInputNV        = Window::registerFunction ( OSG_DLSYM_UNDERSCORE"glFinalCombinerInputNV" );

  hasColorMatrix       = false;
  useTexture2D = true;
  hasMultiTexture = false;
  hasRegisterCombiners = false;
  hasPerStageConstants = false;
  maxCombiners = 0;
}

//! Copy Constructor

DVRIsoShader::DVRIsoShader(const DVRIsoShader &source) :
    Inherited(source)
{  
  hasColorMatrix       = false;
  useTexture2D = true;
  hasMultiTexture = false;
  hasRegisterCombiners = false;
  hasPerStageConstants = false;
  maxCombiners = 0;
}

//! Destructor

DVRIsoShader::~DVRIsoShader(void)
{
}

/*----------------------------- class specific ----------------------------*/

//! initialize the static features of the class, e.g. action callbacks

void DVRIsoShader::initMethod (void)
{
}

//! react to field changes

void DVRIsoShader::changed(BitVector, UInt32)
{
}

//! output the instance for debug purposes

void DVRIsoShader::dump(      UInt32    , 
                         const BitVector ) const
{
  SLOG << "Dump DVRIsoShader NI" << std::endl;
}

// Callback to set up shader - register textures here
bool DVRIsoShader::initialize(DVRVolume *volume, DrawActionBase *action)
{  
  hasColorMatrix       = false;
  hasMultiTexture      = false;
  hasRegisterCombiners = false;
  maxCombiners         = 0;
  hasPerStageConstants = false;
  useTexture2D         = true;
    
  DVRVolumeTexturePtr vol = DVRVOLUME_PARAMETER(volume, DVRVolumeTexture);
  
  if ((volume == NULL) || (vol == NullFC)) {
    SWARNING << "DVRIsoShader - NO Volume" << std::endl;
    return false;
  }

  Window * win = action->getWindow();
  
  if(!win){
    SWARNING << "DVRIsoShader - No valid window" << std::endl;
    return false;
  }

  // determine available hardware support
  if(win->hasExtension(_EXT_texture3D)){
    useTexture2D = false;
    FINFO(("DVRIsoShader - found 3D textures\n"));
  }

  if(volume->getTextures2D() == 1){
    useTexture2D = true;
    FINFO(("DVRIsoShader - 2D textures forced\n"));
  }
  else if(volume->getTextures2D() == 0){
    useTexture2D = false;
    FINFO(("DVRIsoShader - 3D textures forced\n"));
  }

  if(win->hasExtension(_NV_register_combiners)){
    glGetIntegerv(GL_MAX_GENERAL_COMBINERS_NV,&maxCombiners);    
    FINFO(("DVRIsoShader - found %d general register combiners\n",maxCombiners));
    if(maxCombiners>0)
      hasRegisterCombiners = true;

    if(win->hasExtension(_NV_register_combiners2)){
      FINFO(("DVRIsoShader - found per (register combiner) stage constants\n"));
      hasPerStageConstants = true;
    }
    
    if(useTexture2D && win->hasExtension(_ARB_multitexture)){
      FINFO(("DVRIsoShader - found multi textures\n"));
      hasMultiTexture = true;
    }
  }
  else{
    if(win->hasExtension(_SGI_color_matrix) || checkGLVersion(1.2)){
      FINFO(("DVRIsoShader - found color matrix support\n"));
      hasColorMatrix = true;
    }
    else{
      SWARNING << "DVRIsoShader - No appropriate OpenGL-Support found" << std::endl;
      //return false;
    }
  }

  // we need a stencil buffer for the color matrix shading
  if(!hasRegisterCombiners && hasColorMatrix){
    GLint numStencil;
    glGetIntegerv(GL_STENCIL_BITS,&numStencil);
    if(numStencil == 0){
      SWARNING << "DVRIsoShader - No Stencil Buffer found" << std::endl;
      return false;
    }
  }

  GLenum nInternalFormat = GL_RGBA;
  GLenum nExternalFormat = GL_RGBA;


  // create gradient texture
  ImageP gradientImage = createGradientImage(vol);
#ifndef MW_OSG_1_1
  gradientImage->addRef();
#endif
  
  // register texture
  Int32 textureId = -1;

  if(useTexture2D && hasMultiTexture && hasRegisterCombiners){
    textureId = volume->getTextureManager().registerTexture(gradientImage, 
                                                            nInternalFormat, 
                                                            nExternalFormat,                      
                                                            true,
                                                            0,
                                                            1,
                                                            true);
  }
  else{
    textureId = volume->getTextureManager().registerTexture(gradientImage, 
                                                            nInternalFormat, 
                                                            nExternalFormat,
                                                            true,
                                                            0,
                                                            -1,
                                                            true);
  }

  if( textureId < 0){
    SWARNING << "DVRIsoShader - Could not register texture: "<< textureId << std::endl; 
    return false;
  }

#ifdef MW_OSG_1_1
  delete gradientImage;
#else
  gradientImage->subRef();
#endif

  return true;
}


// Callback before any slice is rendered - setup per volume
void DVRIsoShader::activate(DVRVolume *volume, DrawActionBase *action)
{
  if(hasRegisterCombiners) {
    activate_NVRegisterCombinerShading(volume,action);
  }
  else{
    if(hasColorMatrix) {
      activate_ColorMatrixShading(volume,action);
    }
  }
}


// Callback before any brick - state setup per brick
void DVRIsoShader::brickActivate(DVRVolume *, DrawActionBase *, UInt8)
{
  FDEBUG(("DVRIsoShader::brickActivate - nothing to do\n"));  
}


// Callback after all rendering of the volume is done
void DVRIsoShader::deactivate(DVRVolume *volume, DrawActionBase *action)
{
  if(hasRegisterCombiners) {
    deactivate_NVRegisterCombinerShading(volume,action);
  }
  else{
    if(hasColorMatrix) {
      deactivate_ColorMatrixShading(volume,action);
    }    
  }
}

// Own function for rendering a slice, only needed for 2D texturing with slice interpolation, 
// otherwise this is done by the volume
void DVRIsoShader::renderSlice(DVRVolume *volume, DrawActionBase *action,
                               Real32 *data, UInt32 vertices, UInt32 values)
{
  if(hasRegisterCombiners)
    renderSlice_NVRegisterCombinerShading(volume,action,data,vertices,values);
}


// Indicate whether a private function for rendering a slice should be used or not
// only needed for 2D texturing with slice interpolation
bool DVRIsoShader::hasRenderCallback () 
{
  return useTexture2D && hasMultiTexture;
}


// Indicate whether slices should be multitextured or not
bool DVRIsoShader::useMTSlabs ()
{
  return useTexture2D && hasMultiTexture;
}


// Compute gradients from a 3D volume
ImageP DVRIsoShader::createGradientImage(DVRVolumeTexturePtr volTex)
{ 
  int resX = (int) volTex->getImage()->getWidth(); 
  int resY = (int) volTex->getImage()->getHeight(); 
  int resZ = (int) volTex->getImage()->getDepth(); 

  int nGradSetSize =   resX * resY * resZ * 4; 
  int zOff         =   resX * resY; 
  int yOff         =   resX;
  
    
  //  Compute the Gradients  
  UChar8 *volData = volTex->getImage()->getData();
  
  UChar8 *gradbuffer = new UChar8[nGradSetSize];  

  Vec3f gradient;
  for(int z = 0 ; z < resZ ; z++) { 
    for(int y = 0 ; y < resY ; y++) { 
      for(int x = 0 ; x < resX ; x++) { 
   
	if (x == 0) {
	  gradient = Vec3f(-1.0, 0.0, 0.0);
	} else if (x == resX-1) {
	  gradient = Vec3f( 1.0, 0.0, 0.0);
	} else if (y == 0) {
	  gradient = Vec3f(0.0, -1.0, 0.0);
	} else if (y == resY-1) {
	  gradient = Vec3f(0.0,  1.0, 0.0);
	} else if (z == 0) {
	  gradient = Vec3f(0.0, 0.0, -1.0);
	} else if (z == resZ-1) {
	  gradient = Vec3f(0.0, 0.0,  1.0);
	} else {
	  
	  UChar8 & dataXl = volData[ z    * zOff +  y    * yOff + x-1]; 
	  UChar8 & dataXr = volData[ z    * zOff +  y    * yOff + x+1]; 
	  UChar8 & dataYb = volData[ z    * zOff + (y-1) * yOff + x  ]; 
	  UChar8 & dataYt = volData[ z    * zOff + (y+1) * yOff + x  ]; 
	  UChar8 & dataZf = volData[(z-1) * zOff +  y    * yOff + x  ]; 
	  UChar8 & dataZn = volData[(z+1) * zOff +  y    * yOff + x  ]; 
					
  	  gradient = Vec3f((float)(dataXl - dataXr), 
                           (float)(dataYb - dataYt), 
                           (float)(dataZf - dataZn));  
          if (gradient.length() != 0.0)
            gradient.normalize(); 

	} 
		
  	gradbuffer[4 * (z * zOff + y * yOff + x)  ] 
          = (UChar8) (127.0 + gradient[0] * 127.0); // R 
  	gradbuffer[4 * (z * zOff + y * yOff + x)+1] 
          = (UChar8) (127.0 + gradient[1] * 127.0); // G 
  	gradbuffer[4 * (z * zOff + y * yOff + x)+2] 
          = (UChar8) (127.0 + gradient[2] * 127.0); // B 
  	gradbuffer[4 * (z * zOff + y * yOff + x)+3] 
          = volData[z * zOff +  y    * yOff + x  ]; // A 

      } // for x 

    } // for y 

  } // for z 

  ImageP gradientImage = new Image(Image::OSG_RGBA_PF, 
                                   resX, resY, resZ, 
                                   1, 1, 0.0, 
                                   gradbuffer, 
                                   true);       

  return gradientImage;

}

// get currently active lightsources from OpenGL state
// should be changed as soon as OpenSG actions can handle this
void DVRIsoShader::getLightSources(DirLightList &diffuseLights,
				   DirLightList &specularLights,
                                   Color4f      &ambientLight   )
{
  ambientLight.clear();
  diffuseLights.clear();
  specularLights.clear();

  int maxNumLights;
  glGetIntegerv(GL_MAX_LIGHTS, &maxNumLights);

  GLfloat  lightPos[4];
  GLfloat  diffuseColor[4];
  GLfloat  specularColor[4];
  GLfloat  ambientColor[4];  

  for(int i = 0; i < maxNumLights; i++) {
    
    if(glIsEnabled(GLenum(GL_LIGHT0 + i))){
      glGetLightfv(GLenum(GL_LIGHT0 + i), GL_POSITION, lightPos);
      glGetLightfv(GLenum(GL_LIGHT0 + i), GL_AMBIENT,  ambientColor);
      glGetLightfv(GLenum(GL_LIGHT0 + i), GL_DIFFUSE,  diffuseColor);
      glGetLightfv(GLenum(GL_LIGHT0 + i), GL_SPECULAR, specularColor);
      
      // check for directional light
      if(1.0 ==  lightPos[3] + 1.0) { // homogenous coordinate = 0 -> infinity !
        DirLight light;

	// set global light direction
	light.dir.setValues(lightPos[0],lightPos[1],lightPos[2]);
	  
        // diffuse
        if(diffuseColor[0] > 1e-6 || diffuseColor[1] > 1e-6 || diffuseColor[2] > 1e-6){
          light.color = Color4f(diffuseColor[0],diffuseColor[1],diffuseColor[2],0.0f);
          diffuseLights.push_back(light);
        }

	// specular
        if(specularColor[0] > 1e-6 || specularColor[1] > 1e-6 || specularColor[2] > 1e-6){
          light.color = Color4f(specularColor[0],specularColor[1],specularColor[2],0.0f);
          specularLights.push_back(light);
        }

        // ambient
        for(unsigned int i = 0; i < 4; i++){
          ambientLight[i] += ambientColor[i];
        }
      }
    }
  }

  // global ambient
  glGetFloatv(GL_LIGHT_MODEL_AMBIENT,ambientColor);
  for(unsigned int i = 0; i < 4; i++){
    ambientLight[i] += ambientColor[i];
  }
  
}

// check if current OpenGL version is greater or equal to the given version
// Note: only the major and the minor version is checked!!
bool DVRIsoShader::checkGLVersion(GLfloat minVersion)
{
  char *versionString = (char*)glGetString(GL_VERSION);

  if(atof(versionString) >= minVersion)
    return true;
  else
    return false;
}

