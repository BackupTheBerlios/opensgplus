#include <OSGConfig.h>

#include "OSGDVRIsoShader.h"
#include <OSGDVRShader.h>
#include <OSGDrawActionBase.h>
#include <OSGDVRVolume.h>
#include <OSGDVRVolumeTexture.h>
#include <OSGDVRIsoSurface.h>
#include <OSGWindow.h>
#include <OSGSlicer.h>
#include <OSGCamera.h>

#include <OSGGLU.h>

OSG_USING_NAMESPACE

void DVRIsoShader::activate_ColorMatrixShading(DVRVolume *volume, DrawActionBase *)
{
  DVRVolumeTexturePtr vol      = DVRVOLUME_PARAMETER(volume,DVRVolumeTexture);
  DVRIsoSurfacePtr    isoParam = DVRVOLUME_PARAMETER(volume, DVRIsoSurface);

  if ((volume == NULL) || (vol == NullFC)) {
    SWARNING << "NO Volume ..." << std::endl;
    return;
  }

  Real32 isoValue;
  UInt32 alphaMode;

  // get parameters from isosurface attachment if available
  if(isoParam!=NullFC){
    isoValue  = isoParam->getIsoValue();
    alphaMode = isoParam->getAlphaMode();
  }
  else{
    isoValue = 1.0;
    alphaMode = GL_GREATER;
  }

  glPushAttrib(GL_COLOR_BUFFER_BIT | 
               GL_ENABLE_BIT | 
               GL_DEPTH_BUFFER_BIT | 
               GL_STENCIL_BUFFER_BIT | 
               GL_POLYGON_BIT);

  if (volume->getDoTextures()) { 

    // draw color 1 for MODULATE
    glColor4f(1.0,1.0,1.0,1.0);

    // Enable Alpha Test for isosurface
    glEnable(GL_ALPHA_TEST); 
    glAlphaFunc(alphaMode,isoValue);

    // no blending and lighting
    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);
    
    // setup stencil buffer
    // to mask the "volume region"
    glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_FALSE);
    glEnable(GL_STENCIL_TEST);
    glClearStencil(0xFF);
    glStencilMask(0xFF);
    glClear(GL_STENCIL_BUFFER_BIT);
    glStencilFunc(GL_ALWAYS, 0x0, 0x1); // write always
    glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO);
  }

}


void DVRIsoShader::deactivate_ColorMatrixShading(DVRVolume *volume, DrawActionBase *action)
{

  if (volume->getDoTextures()) {

    Vec2s vpSize;
    vpSize[0] = action->getWindow()->getWidth();
    vpSize[1] = action->getWindow()->getHeight();
    
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_ALPHA_TEST);
    glDepthMask(GL_FALSE);
    glDisable(GL_LIGHTING);
    glStencilFunc(GL_EQUAL, 0x0, 0xFF);
    glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);	       

    Color4f diffuseLightColor;
    Vec3f   diffuseDir;

    DirLightList diffuseLights;
    DirLightList specularLights;
    Color4f ambientLight;

    getLightSources(diffuseLights,specularLights,ambientLight);

    // use the first available diffuse lightsource for shading
    if(diffuseLights.empty()){
      diffuseLightColor[0] = 0.0f;
      diffuseLightColor[1] = 0.0f;
      diffuseLightColor[2] = 0.0f;
      diffuseLightColor[3] = 0.0f;
    }
    else{
      diffuseLightColor = diffuseLights.begin()->color;
      Slicer::rotateToLocal(action, diffuseLights.begin()->dir, diffuseDir);
    }

    // store the matrix mode
    GLint oldMatrixMode;
    glGetIntegerv(GL_MATRIX_MODE, &oldMatrixMode);
    
    // store alpha scale and bias
    GLfloat oldAlphaBias;
    GLfloat oldAlphaScale;
    glGetFloatv(GL_ALPHA_BIAS,&oldAlphaBias);
    glGetFloatv(GL_ALPHA_SCALE,&oldAlphaScale);

    // ensure alpha is 1 
    glPixelTransferf(GL_ALPHA_SCALE,0.00001);
    glPixelTransferf(GL_ALPHA_BIAS,0.999999);

    // compute shading

    // switch to color matrix mode to compute diffuse shading
    glMatrixMode(GL_COLOR);
    
    // push actual color matrix
    glPushMatrix();
    
    GLfloat colorMatV[16];

    // set light color
    colorMatV[0] = diffuseLightColor[0];
    colorMatV[1] = 0.0f;
    colorMatV[2] = 0.0f;
    colorMatV[3] = 0.0f;

    colorMatV[4] = 0.0f;
    colorMatV[5] = diffuseLightColor[1];
    colorMatV[6] = 0.0f;
    colorMatV[7] = 0.0f;
    
    colorMatV[8] = 0.0f;
    colorMatV[9] = 0.0f;
    colorMatV[10] = diffuseLightColor[2];
    colorMatV[11] = 0.0f;
	 
    colorMatV[12] = ambientLight[0];
    colorMatV[13] = ambientLight[1];
    colorMatV[14] = ambientLight[2];
    colorMatV[15] = 1.0f;
    
    glLoadMatrixf(colorMatV);	

    // set light direction for computation of cosine-term
    colorMatV[0] = diffuseDir[0];
    colorMatV[1] = diffuseDir[0];
    colorMatV[2] = diffuseDir[0];
    //colorMatV[3] = 0.0f;
    
    colorMatV[4] = diffuseDir[1];
    colorMatV[5] = diffuseDir[1];
    colorMatV[6] = diffuseDir[1];
    //colorMatV[7] = 0.0f;
    
    colorMatV[8] = diffuseDir[2];
    colorMatV[9] = diffuseDir[2];
    colorMatV[10] = diffuseDir[2];
    //colorMatV[11] = 0.0f;
    
    colorMatV[12] = 0.0f;
    colorMatV[13] = 0.0f;
    colorMatV[14] = 0.0f;
    //colorMatV[15] = 1.0f;
    
    glMultMatrixf(colorMatV);		

    // rescale/bias gradients
    colorMatV[0] = 2.0f;
    colorMatV[1] = 0.0f;
    colorMatV[2] = 0.0f;
    
    colorMatV[3] = 0.0f;
    
    colorMatV[4] = 0.0f;
    colorMatV[5] = 2.0f;
    colorMatV[6] = 0.0f;
    
    colorMatV[7] = 0.0f;
	 
    colorMatV[8] = 0.0f;
    colorMatV[9] = 0.0f;
    colorMatV[10] = 2.0f;
    
    colorMatV[11] = 0.0f;
      
    colorMatV[12] = -1.0f;
    colorMatV[13] = -1.0f;
    colorMatV[14] = -1.0f;
    
    colorMatV[15] = 1.0f;

    // set the color matrix on top of the stack
    glMultMatrixf( colorMatV);
    
    // compute shading
    glReadBuffer(GL_BACK);  
    glDrawBuffer(GL_BACK);
    glCopyPixels(0,0,vpSize[0],vpSize[1], GL_COLOR);

    // restore alpha bias and scale
    glPixelTransferf(GL_ALPHA_BIAS, oldAlphaBias);
    glPixelTransferf(GL_ALPHA_SCALE, oldAlphaScale);

    // restore old color matrix
    glPopMatrix();
      
    // restore old matrix mode
    glMatrixMode(oldMatrixMode);

  }
 
  glPopAttrib();
}
