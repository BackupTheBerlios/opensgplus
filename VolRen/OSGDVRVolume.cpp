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

#include "OSGDVRVolume.h"

#include <OSGGL.h>

#include <OSGAction.h>
#include <OSGCamera.h>
#include <OSGDrawAction.h>
#include <OSGRenderAction.h>
#include <OSGIntersectAction.h>
#include <OSGSimpleMaterial.h>
#include <OSGDVRVolumeTexture.h>
#include <OSGDVRAppearance.h>
#include <OSGBrick.h>



OSG_USING_NAMESPACE

#ifdef __sgi
#pragma set woff 1174
#endif

namespace
{
    static char cvsid_cpp[] = "@(#)$Id: OSGDVRVolume.cpp,v 1.1 2002/10/10 11:11:26 weiler Exp $";
    static char cvsid_hpp[] = OSGDVRVOLUME_HEADER_CVSID;
    static char cvsid_inl[] = OSGDVRVOLUME_INLINE_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

/*! \class osg::DVRVolume
OpenSG NodeCore for direct volume rendering
*/

UInt32 DVRVolume::_extTex3D = Window::registerExtension( "GL_EXT_texture3D" );


/*----------------------- constructors & destructors ----------------------*/

//! Constructor

DVRVolume::DVRVolume(void) :
    Inherited(),
    textureManager(this)
{
    SINFO << "DVRVolume::DVRVolume(void) this: " << this << std::endl;

    commonConstructor();

}

//! Copy Constructor

DVRVolume::DVRVolume(const DVRVolume &source) :
    Inherited(source),
    textureManager(this)
{
    SINFO << "DVRVolume::DVRVolume(const DVRVolume &source) this: " << this << std::endl;

    //!! FIXME:
    //!! This is only performed during instantiation of real objects
    //!! Which is only done with the copy constructor
    //!!
    //!! Otherwise my code cores - (maybe the copy constructor of FCPtr is broken?!)
    DVRVolumePtr ptr(*this);
    
    // Fake material for render action
    SimpleMaterialPtr m = SimpleMaterial::create();
    beginEditCP(m);
    m->setTransparency(0.001);
    m->setLit(false);
    m->setDiffuse(Color3f(1.0, 1.0, 1.0));
    m->setAmbient(Color3f(1.0, 1.0, 1.0));
    endEditCP(m);

    // Chunk material as storage fieldcontainer for textures
    ChunkMaterialPtr cm = SimpleMaterial::create();

    // Add all
    beginEditCP(ptr, RenderMaterialFieldMask | TextureStorageFieldMask);
    setRenderMaterial(m);
    setTextureStorage(cm);
    endEditCP  (ptr, RenderMaterialFieldMask | TextureStorageFieldMask);

    commonConstructor();
}

//! Destructor

DVRVolume::~DVRVolume(void)
{
    SINFO << "~DVRVolume this: " << this << std::endl;
    SINFO << "~DVRVolume appearance: " << getAppearance() << std::endl;
    SINFO << "~DVRVolume::subRefCP(Appearance)" << std::endl;
    subRefCP(_sfAppearance.getValue());

    SINFO << "~DVRVolume::subRefCP(Geometry)" << std::endl;
//    SINFO << "~DVRVolume geometry: " << int(_sfGeometry.getValue()) << std::endl;
    subRefCP(_sfGeometry.getValue());

    SINFO << "~DVRVolume::subRefCP(Shader)" << std::endl;
    SINFO << "~DVRVolume shader: " << getShader() << std::endl;
    subRefCP(_sfShader.getValue());

    SINFO << "~DVRVolume::subRefCP(RenderMaterial)" << std::endl;
    SINFO << "~DVRVolume renderMaterial: " << getRenderMaterial() << std::endl;
    subRefCP(_sfRenderMaterial.getValue());

    SINFO << "~DVRVolume::subRefCP(TextureStorage)" << std::endl;
    SINFO << "~DVRVolume textureStorage: " << getTextureStorage() << std::endl;
    subRefCP(_sfTextureStorage.getValue());

}

/*----------------------------- class specific ----------------------------*/


//! set the bounding volume of the node
void DVRVolume::adjustVolume(Volume &volume)
{
    volume.setValid();
    volume.setEmpty();
    
    DVRVolumeTexturePtr tex = DVRVOLUME_PARAMETER(this, DVRVolumeTexture);
    if (tex != NullFC) {
        Vec3f & res   = tex->getResolution();
	Vec3f & slice = tex->getSliceThickness();
	
        Vec3f minBB(-0.5 * res[0] * slice[0],
		    -0.5 * res[1] * slice[1],
		    -0.5 * res[2] * slice[2]);
	Vec3f maxBB(-minBB);
	
	volume.extendBy( minBB );
	volume.extendBy( maxBB );
    }    
    else {
        // something wrong with initialization - show boundingbox either
        Vec3f minBB(-0.5, -0.5, -0.5);
	Vec3f maxBB( 0.5,  0.5,  0.5);
    
	volume.extendBy( minBB );
	volume.extendBy( maxBB );
    }
}


//! initialize the static features of the class, e.g. action callbacks

void DVRVolume::initMethod (void)
{
	DrawAction::registerEnterDefault( getClassType(), 
		osgTypedMethodFunctor2BaseCPtrRef<Action::ResultE,
					          DVRVolumePtr, 
					          CNodePtr,  
					          Action *>(&DVRVolume::doDraw));
	
	IntersectAction::registerEnterDefault( getClassType(), 
		osgTypedMethodFunctor2BaseCPtrRef<Action::ResultE,
						  DVRVolumePtr, 
					          CNodePtr,  
						  Action *>(&DVRVolume::intersect));
	
	RenderAction::registerEnterDefault( getClassType(), 
		osgTypedMethodFunctor2BaseCPtrRef<Action::ResultE,
						  DVRVolumePtr, 
					          CNodePtr,  
					          Action *>(&DVRVolume::render));
}


//! react to field changes

void DVRVolume::changed(BitVector whichField, UInt32)
{
    FDEBUG(("DVRVolume::changed\n"));

    if ((whichField & BrickStaticMemoryMBFieldMask)) {
        // override invalid value
        if (_sfBrickStaticMemoryMB.getValue() == 0)
	    _sfBrickStaticMemoryMB.setValue(1);
    }

    if ((whichField & ShaderFieldMask)) {
        shadingInitialized = false;
    }

    if ((whichField & Textures2DFieldMask)) {
        shadingInitialized = false;
    }
}

//! output the instance for debug purposes

void DVRVolume::dump(      UInt32        uiIndent, 
                         const BitVector ) const
{
    DVRVolumePtr thisP(*this);

    thisP.dump(uiIndent, FCDumpFlags::RefCount);

    indentLog(uiIndent, PLOG);
    PLOG << "DVRVolume at " << this << std::endl;

    indentLog(uiIndent, PLOG);
    PLOG << "\trenderMaterial: " << getRenderMaterial() << std::endl;

    if (getRenderMaterial() != NullFC)
      getRenderMaterial()->dump(uiIndent);
    
    indentLog(uiIndent, PLOG);
    PLOG << "\ttextureStorage: " << getTextureStorage() << std::endl;

    if (getTextureStorage() != NullFC)
      getTextureStorage()->dump(uiIndent);
    
#if 0
    indentLog(uiIndent, PLOG);
    PLOG << "\tambient: " << getAmbient() << endl;

    indentLog(uiIndent, PLOG);
    PLOG << "\tdiffuse: " << getDiffuse()  << endl;

    indentLog(uiIndent, PLOG);
    PLOG << "\tspecular: " << getSpecular()  << endl;

    indentLog(uiIndent, PLOG);
    PLOG << "\tshininess: " << getShininess()  << endl;

    indentLog(uiIndent, PLOG);
    PLOG << "\temission: " << getEmission()  << endl;

    indentLog(uiIndent, PLOG);
    PLOG << "\ttransparency: " << getTransparency()  << endl;

    indentLog(uiIndent, PLOG);
    PLOG << "\tlit: " << getLit() << endl;

    indentLog(uiIndent, PLOG);
    PLOG << "\tChunks: " << endl;

    for(MFStateChunkPtr::const_iterator i = _mfChunks.begin();
            i != _mfChunks.end(); i++)
    {
        indentLog(uiIndent, PLOG);
        PLOG << "\t" << *i << endl;
    }
#endif
    
    indentLog(uiIndent, PLOG);
    PLOG << "DVRVolume end " << this << std::endl;
}


//! execute the OpenGL c\ommands to draw the geometry	
Action::ResultE DVRVolume::doDraw(Action * action )
{
//    FDEBUG(("DVRVolume::doDraw\n"));

    DrawAction *a = dynamic_cast<DrawAction*>(action);

    if (a == NULL) {
      SWARNING << "DVRVolume::doDraw - unexpected action" << std::endl; 
      return Action::Skip;
    }

#if 1
    Material::DrawFunctor func;
    func=osgTypedMethodFunctor1ObjPtr(this, &DVRVolume::draw);
    
    // implementation from OGSGeometry
    if(a->getMaterial() != NULL)
    {
        a->getMaterial()->draw( func, a );
    }
//     else if ( getMaterial() != NullFC )
//     {
//         getMaterial()->draw( func );
//     }
    else
    {
        draw( a );
    }

    return Action::Skip;

#else
    return draw(a);
#endif

}

//! low-level OpenGL calls, ignoring materials	
Action::ResultE DVRVolume::draw(DrawActionBase *action)
{
    FINFO(("DVRVolume::draw\n"));

    Window * window = action->getWindow();

    if (getShader() != NullFC) {

        // determine texture mode
        TextureManager::TextureMode mode;
	switch (getTextures2D()) {
 	    case 0: // 3D textures
	        mode = TextureManager::TM_3D;
		break;
	    case 1: // 2D textures
	        mode = TextureManager::TM_2D;
		break;
	    default: // auto
	        mode = window->hasExtension(_extTex3D) ?
		  TextureManager::TM_3D : TextureManager::TM_2D;
	}
	if (getShader()->useMTSlabs())
	    mode = TextureManager::TM_2D_Multi;


	FDEBUG(("DVRVolume::draw - using Shader: %s\n",
//		getShader()->getClassname()));
		getShader()->getType().getCName()));

	// initialization
	if (shadingInitialized == false) {     
	    // clear old texture chunks 
	    textureManager.clearTextures( getTextureStorage() );
	    // initialize shader 
	    bool result = getShader()->initialize( this, action );
	    if (result != true)
	      SFATAL << "DVRVolume::draw - error initializing shader" << std::endl;

	    //!! Shader::initialize may change the useMTSlabs() flag
	    if (getShader()->useMTSlabs())
	        mode = TextureManager::TM_2D_Multi;
	    
	    // create texture chunks
	    SINFO << "TextureMode = " << mode << std::endl;
	    textureManager.buildTextures( getTextureStorage(), this, mode );
	    shadingInitialized = true;
	}

	// sort bricks
	Vec3f eyePointWorld;
	eyePointWorld.setValue(action->getCamera()->getBeacon()->getToWorld()[3]);
	Matrix	 modelMat = action->getActNode()->getToWorld();
      
	// SINFO << "DVRVolume::draw - modelMat " << modelMat << std::endl;
	// SINFO << "DVRVolume::draw - eyePoint " << eyePointWorld << std::endl;
	Brick * first = textureManager.sortBricks(modelMat, eyePointWorld, this, mode);
	
	// render brick boundaries
	if (getShowBricks())
	  for (Brick * current = first; current != NULL; current = current->getNext())
	    current->renderBrick();
	
	//!! Rendering
	getShader()->activate(this, action);

	// for all bricks
	Brick * prev = NULL;
	for (Brick * current = first;
	     current != NULL;
	     prev = current, current = current->getNext())
	{
//	    FDEBUG(("DVRVolume::draw - render Brick: %d\n" i++));

	    // switch to texture chunk
	    if (getDoTextures()) {
	      if (prev == NULL)
		current->activateTexture(action);
	      else
		current->changeFromTexture(action, prev);
	    }
	    
	    // activate shader
	    getShader()->brickActivate(this, action, 0 /* brickId */);

	    // render slices
	    current->renderSlices(this, action, getShader(), mode);
	}

	// deactivate last texture chunk
	prev->deactivateTexture(action);
	
	getShader()->deactivate(this, action);
    }
    else {
        // Show some slices anyway
        Vec3f min, max;
	action->getActNode()->getVolume().getBounds(min, max);

	FDEBUG(("DVRVolume::draw dummy geometry - %f %f %f -> %f %f %f\n",
		min[0], min[1], min[2], max[0], max[1], max[2]));

	// resolution of volume in voxel
        UInt32 res[3] = {64, 64, 64};
        // offset for slices
	Real32 offset[3] = {(max[0] - min[0]) / 2 / res[0],
			    (max[1] - min[1]) / 2 / res[1],
			    (max[2] - min[2]) / 2 / res[2]};
	// increment for slices
	Real32 inc[3] = {(max[0] - min[0]) / res[0],
			 (max[1] - min[1]) / res[1],
			 (max[2] - min[2]) / res[2]};

	// slices perpenticular to Z-axis
	Real32 value = min[2] + offset[2];
	for (UInt32 i = 0; i < res[2]; i++, value += inc[2]) {
	  
 	    glBegin(GL_LINE_LOOP); {
	        glVertex3f( min[0] + offset[0], min[1] + offset[1], value);
		glTexCoord2f( 0.0 + offset[0],  0.0 + offset[1]);
		
		glVertex3f( max[0] - offset[0], min[1] + offset[1], value);
		glTexCoord2f( 1.0 - offset[0],  0.0 + offset[1]);
	      
		glVertex3f( min[0] + offset[0],  max[1] - offset[1], value);
		glTexCoord2f( 0.0 + offset[0],  1.0 - offset[1]);
		
		glVertex3f( max[0] - offset[0],  max[1] - offset[1], value);
		glTexCoord2f( 1.0 - offset[0],  1.0 - offset[1]);
		
	    } glEnd();
	}
    }
    
    return Action::Skip;
//    return Geometry::draw(action);
}


//! generate draw tree
Action::ResultE DVRVolume::render(Action *action)
{
    FDEBUG(("DVRVolume::render\n"));
    RenderAction *a = dynamic_cast<RenderAction *>(action);

    Material::DrawFunctor func;
    func=osgTypedMethodFunctor1ObjPtr(this, &DVRVolume::draw);

    a->dropFunctor(func, &(*getRenderMaterial()));

    return Action::Continue;
}


//! used to initialize member variables - called by every constructor
void DVRVolume::commonConstructor( void )
{

    drawStyleListValid = false;
    shadingInitialized = false;

    dump();
}


//! intersection test
Action::ResultE DVRVolume::intersect(Action * action )
{ 
    FDEBUG(("DVRVolume::intersect\n"));

    IntersectAction     * ia = dynamic_cast<IntersectAction*>(action);
    const DynamicVolume  &dv = ia->getActNode()->getVolume();
    
    if( dv.isValid() && !dv.intersect(ia->getLine()) )
    {
        return Action::Skip; //bv missed -> can not hit children
    }

    //!! FIXME: simulate hit when bounding volume is hit
    Real32 t, v;
    Vec3f norm;

    if (dv.intersect(ia->getLine(), t, v))
        ia->setHit( t, ia->getActNode(), 0, norm );
    
    return Action::Continue;
}


//! conveniance function for finding a specific attachment in the volume kit
AttachmentPtr DVRVolume::findParameter(const FieldContainerType &type, UInt16 binding)
{
    DVRAppearancePtr  app = getAppearance();

    if (osgLog().getLogLevel() == LOG_DEBUG)
    {
        FDEBUG(("DVRVolume::findParameter: \n"));
        type.dump();
    }

    if (app != NullFC) return app->findAttachment(type, binding);

    SWARNING << "DVRVolume::findParameter - NO such parameter " << std::endl;
    
    return NullFC;
}


//! conveniance function for finding a specific attachment in the volume kit
AttachmentPtr DVRVolume::findParameter(UInt32 groupId, UInt16 binding)
{
    DVRAppearancePtr  app = getAppearance();

    if (app != NullFC) return app->findAttachment(groupId, binding);

    return NullFC;
}


//! aquires DrawStyleNames and insert them into the name field
void DVRVolume::buildDrawStyleList(Window *OSG_CHECK_ARG(win))
{
}


//! reinitialize inventor volume
void DVRVolume::reload()
{
    if (getShader() != NullFC) {
        textureManager.clearTextures( getTextureStorage() );
        shadingInitialized = false;
    }
}


