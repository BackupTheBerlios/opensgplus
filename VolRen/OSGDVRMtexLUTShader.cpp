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

#include <OSGDVRMtexLUTShader.h>

#include <OSGDVRVolume.h>
#include <OSGDVRVolumeTexture.h>
#include <OSGDVRLookupTable.h>

#include <OSGWindow.h>


OSG_USING_NAMESPACE

#ifdef __sgi
#pragma set woff 1174
#endif

namespace
{
    static char cvsid_cpp[] = "@(#)$Id: OSGDVRMtexLUTShader.cpp,v 1.1 2002/10/10 11:11:26 weiler Exp $";
    static char cvsid_hpp[] = OSGDVRMTEXLUTSHADER_HEADER_CVSID;
    static char cvsid_inl[] = OSGDVRMTEXLUTSHADER_INLINE_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

/*! \class osg::DVRMtexLUTShader
Abstract shader class - not to be instantiated. Is intended to be stateless - NO FIELDS!!
*/

/*----------------------- constructors & destructors ----------------------*/

//! Constructor

DVRMtexLUTShader::DVRMtexLUTShader(void) :
    Inherited()
{
}

//! Copy Constructor

DVRMtexLUTShader::DVRMtexLUTShader(const DVRMtexLUTShader &source) :
    Inherited(source)
{
}

//! Destructor

DVRMtexLUTShader::~DVRMtexLUTShader(void)
{
}

/*----------------------------- class specific ----------------------------*/

//! initialize the static features of the class, e.g. action callbacks

void DVRMtexLUTShader::initMethod (void)
{
}

//! react to field changes

void DVRMtexLUTShader::changed(BitVector, UInt32)
{
}

//! output the instance for debug purposes

void DVRMtexLUTShader::dump(      UInt32    , 
                         const BitVector ) const
{
    SLOG << "Dump DVRMtexLUTShader NI" << std::endl;
}

//! callback to set up shader - register textures here
bool DVRMtexLUTShader::initialize     (DVRVolume *volume, DrawActionBase *action)
{
//    FDEBUG(("DVRMtexLUTShader::initialize\n"));

    GLenum nInternalFormat;
    GLenum nExternalFormat;

    getPaletteFormat( action, nInternalFormat, nExternalFormat, m_nTexturePaletteMode );

    DVRLookupTablePtr   lut = DVRVOLUME_PARAMETER(volume, DVRLookupTable);
    DVRVolumeTexturePtr vol = DVRVOLUME_PARAMETER(volume, DVRVolumeTexture);

    if ((volume == NULL) || (vol == NullFC)) {
        SWARNING << "NO Volume..." << std::endl;
        return false;
    }

    // This shader requires ARB_multitexture and NV_register_combiner extensions
    if (!action->getWindow()->hasExtension(_arbMultitexture))
    {
        SWARNING << "NO mulit textures ..." << std::endl;
        return false;
    }
    std::cerr << "Multitextures found" << std::endl;

    if (!action->getWindow()->hasExtension(_nvRegisterCombiners))
    {
        SWARNING << "NO register combiners" << std::endl;
	return false;
    }
    std::cerr << "Register Combiners found" << std::endl;
    
    m_nTextureId = volume->getTextureManager().registerTexture(
                                                 vol->getImage(), // image
						 nInternalFormat, // internalFormat
						 nExternalFormat, // externalFormat
						 1,               // doBricking
						 0,               // textureStage0
						 1,               // textureStage1
						 0);              // doCopy

    if (m_nTextureId == -1) {
        SWARNING << "Error registering textures ..." << std::endl;
        return false;
    }
    
    if (lut != NullFC)
      lut->setTouched( true ); // force initialization in first render pass

    return true;
}


//! callback before any slice is rendered - setup per volume
void DVRMtexLUTShader::activate(    DVRVolume *volume, DrawActionBase *action)
{
//    FDEBUG(("DVRMtexLUTShader::activate\n"));

#if !defined(GL_ARB_multitexture) || !defined(GL_NV_register_combiners)
    SWARNING << "DVRtexLUTShader requires ARB_multitexture and "
	     << "NV_register_combiners extensions" << std::endl;
    volume = volume; action = action; // calm down compiler
#else

    DVRSimpleLUTShader::activate(volume, action);

    if (volume->getDoTextures())
    {
        glEnable(GL_REGISTER_COMBINERS_NV);
	initCombiners(action);
    }

#endif // required extensions available
}


//! callback after all rendering of the volume is done
void DVRMtexLUTShader::deactivate(DVRVolume *volume, DrawActionBase *action)
{
//    FDEBUG(("DVRMtexLUTShader::deactivate\n"));

#if !defined(GL_ARB_multitexture) || !defined(GL_NV_register_combiners)
    SWARNING << "DVRtexLUTShader requires ARB_multitexture and " 
	     << "NV_register_combiners extensions" << std::endl;
    volume = volume; action = action; // calm down compiler
#else
    
    if (volume->getDoTextures())
    {
        glDisable(GL_REGISTER_COMBINERS_NV);
    }
    
    DVRSimpleLUTShader::deactivate(volume, action);

#endif // required extensions available
}


//! callback for rendering slices
void DVRMtexLUTShader::renderSlice( DVRVolume *volume, DrawActionBase *action,
				    Real32 *data, UInt32 vertices, UInt32 values)
{
#if !defined(GL_ARB_multitexture) || !defined(GL_NV_register_combiners)
    SWARNING << "DVRtexLUTShader requires ARB_multitexture and "
	     << "NV_register_combiners extensions" << std::endl;
    volume = volume; action = action; data = data; // calm down compiler
    vertices = vertices; values = values; 
#else

    Window * win = action->getWindow();
    void (*CombinerParameterfvNV) (GLenum, const GLfloat *) = (void (*) (GLenum, const GLfloat *)) win->getFunction(_funcCombinerParameterfvNV);
    void (*MultiTexCoord2dARB) (GLenum, GLdouble, GLdouble) = (void (*) (GLenum, GLdouble, GLdouble)) win->getFunction(_funcMultiTexCoord2dARB); 
    
//     FINFO(("DVRMtexLUTShader::renderSlice - vert(%d) - values(%d)\n", vertices, values));
    
    if (values < 6) {
        SWARNING << "DVRMtexLUTShader::renderSlice - insufficient values" << std::endl;
	return;
    }


    float col[4] = {0, 0, 0, data[5]};
//     FINFO(("DVRMtexLUTShader::renderSlice - slice (%f %f %f %f %f %f)\n",
// 	   data[0], data[1], data[2], data[3], data[4], data[5]));
    
//     FINFO(("DVRMtexLUTShader::renderSlice - constant color (%f %f %f %f)\n",
//  	   col[0], col[1], col[2], col[3]));
    CombinerParameterfvNV(GL_CONSTANT_COLOR0_NV, col);
    
    if (volume->getDoTextures())
    {
        glBegin(GL_TRIANGLE_FAN);
    }
    else
    {
        //!! TODO - alpha correction
        glColor4f(1.0, 1.0, 1.0, 1.0);
	glBegin(GL_LINE_LOOP);
    }

    
    for (UInt32 i = 0; i < vertices; i++)
    {
        Real32 * vert = data + i * values;

	// constant color is used as blending factor between tex0 and tex1
	// a == 0 means tex0
	// a == 1 means tex1 
	// float col[4] = {0, 0, 0, vert[5]};
	// CombinerParameterfvNV(GL_CONSTANT_COLOR0_NV, col);
	  
	MultiTexCoord2dARB(GL_TEXTURE0_ARB, vert[3], vert[4]);
	MultiTexCoord2dARB(GL_TEXTURE1_ARB, vert[3], vert[4]);
	glVertex3f(vert[0], vert[1], vert[2]);
	
// 	FINFO(("DVRMtexLUTShader::renderSlices (texcoor): %f %f %f\n",
// 	       vert[3], vert[4], vert[5])); 
	
// 	FINFO(("DVRMtexLUTShader::renderSlices (coor): %f %f %f\n",
// 	       vert[0], vert[1], vert[2]));

    }

    glEnd();

#endif // required extensions
}


#if defined(GL_ARB_multitexture) && defined(GL_NV_register_combiners)

//! set up correct register combiner state
void DVRMtexLUTShader::initCombiners(DrawActionBase *action)
{
    Window * win = action->getWindow();
    void (*CombinerParameteriNV) (GLenum, const GLint) = (void (*) (GLenum, const GLint)) win->getFunction(_funcCombinerParameteriNV);
    void (*CombinerInputNV) (GLenum, GLenum, GLenum, GLenum, GLenum, GLenum) =
      (void (*) (GLenum, GLenum, GLenum, GLenum, GLenum, GLenum)) win->getFunction(_funcCombinerInputNV);
    void (*CombinerOutputNV) (GLenum, GLenum, GLenum, GLenum, GLenum, GLenum, GLenum, GLboolean, GLboolean, GLboolean) =
      (void (*) (GLenum, GLenum, GLenum, GLenum, GLenum, GLenum, GLenum, GLboolean, GLboolean, GLboolean)) win->getFunction(_funcCombinerOutputNV);
    void (*FinalCombinerInputNV) (GLenum, GLenum, GLenum, GLenum) =
      (void (*) (GLenum, GLenum, GLenum, GLenum)) win->getFunction(_funcFinalCombinerInputNV);
    
    // use only first general combiner
    CombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 1);

    // RGB-Portion
    // --------------------------------------------
    // Alpha value of const color is used as
    // blending factor for the two textures 0 and 1

    CombinerInputNV( // (1 - ConstColor0) -> A
        GL_COMBINER0_NV,	// stage
	GL_RGB,			// portion
	GL_VARIABLE_A_NV,	// variable
	GL_CONSTANT_COLOR0_NV,	// input  <- const color 0
	GL_UNSIGNED_INVERT_NV,  // mapping (1-x)
	GL_ALPHA);		// componentUsage

    CombinerInputNV( // Texture0     -> B
        GL_COMBINER0_NV,        // stage         
	GL_RGB, 		// portion       
	GL_VARIABLE_B_NV, 	// variable      
	GL_TEXTURE0_ARB, 	// input <- texture 0         
	GL_UNSIGNED_IDENTITY_NV,// mapping 
	GL_RGB);		// componentUsage

    CombinerInputNV( // ConstColor0    -> C
        GL_COMBINER0_NV,        // stage         
	GL_RGB, 		// portion       
	GL_VARIABLE_C_NV, 	// variable      
	GL_CONSTANT_COLOR0_NV,  // input    <- const color 0     
	GL_UNSIGNED_IDENTITY_NV,// mapping 
	GL_ALPHA);		// componentUsage

    CombinerInputNV( // Texture1      -> D
        GL_COMBINER0_NV,        // stage         
	GL_RGB, 		// portion       
	GL_VARIABLE_D_NV, 	// variable      
	GL_TEXTURE1_ARB, 	// input         
	GL_UNSIGNED_IDENTITY_NV,// mapping 
	GL_RGB);		// componentUsage

    // Alpha-Portion
    // --------------------------------------------
    // Alpha value of const color is used as
    // blending factor for the two textures 0 and 1
    CombinerInputNV(            // (1 - ConstColor0) -> A/
        GL_COMBINER0_NV,        // stage         
	GL_ALPHA, 		// portion       
	GL_VARIABLE_A_NV, 	// variable 
	GL_CONSTANT_COLOR0_NV,  // input <- const color 0     
	GL_UNSIGNED_INVERT_NV,  // mapping (1-x) 
	GL_ALPHA);		// componentUsage

    CombinerInputNV(            //  Texture0 -> A
        GL_COMBINER0_NV,        // stage         
	GL_ALPHA, 		// portion       
	GL_VARIABLE_B_NV, 	// variable      
	GL_TEXTURE0_ARB, 	// input         
	GL_UNSIGNED_IDENTITY_NV,// mapping 
	GL_ALPHA);		// componentUsage
	
    CombinerInputNV(            // ConstColor0 -> C
        GL_COMBINER0_NV,        // stage         
	GL_ALPHA, 		// portion       
	GL_VARIABLE_C_NV, 	// variable      
	GL_CONSTANT_COLOR0_NV,  // input         
	GL_UNSIGNED_IDENTITY_NV,// mapping 
	GL_ALPHA);		// componentUsage
	
    CombinerInputNV(            // Texture1 -> D
        GL_COMBINER0_NV,        // stage         
	GL_ALPHA, 		// portion       
	GL_VARIABLE_D_NV, 	// variable      
	GL_TEXTURE1_ARB, 	// input         
	GL_UNSIGNED_IDENTITY_NV,// mapping
	GL_ALPHA);		// componentUsage

    // general combiner output
    //-------------------------------------	
 
    CombinerOutputNV( // Spare0(RGB) = AB+CD
        GL_COMBINER0_NV,// stage
	GL_RGB,		// portion
	GL_DISCARD_NV,	// abOutput  (not used)
	GL_DISCARD_NV,	// cdOutput  (not used)
	GL_SPARE0_NV,	// sumOutput (this is AB+CD) 
	GL_NONE,	// scale     (not used)
	GL_NONE,	// bias      (not used)
	GL_FALSE,	// abDotProduct
	GL_FALSE,	// cdDotproduct
	GL_FALSE);	// muxSum                      

    CombinerOutputNV( // Spare0(A) = AB+CD
        GL_COMBINER0_NV,// stage                       
	GL_ALPHA, 	// portion                     
	GL_DISCARD_NV, 	// abOutput  (not used)        
	GL_DISCARD_NV, 	// cdOutput  (not used)        
	GL_SPARE0_NV, 	// sumOutput (this is AB+CD)   
	GL_NONE, 	// scale     (not used)        
	GL_NONE, 	// bias      (not used)        
	GL_FALSE,       // abDotProduct                
	GL_FALSE, 	// cdDotproduct                
	GL_FALSE);	// muxSum                      

    // final combiner input
    //-------------------------------------	

    FinalCombinerInputNV( // A(RGB) = 0
        GL_VARIABLE_A_NV,       // variable
	GL_ZERO,		// input
	GL_UNSIGNED_IDENTITY_NV,// mapping 
	GL_RGB);		// componentUsage

    FinalCombinerInputNV( // B(RGB) = 0
        GL_VARIABLE_B_NV,       // variable         
	GL_ZERO, 		// input         
	GL_UNSIGNED_IDENTITY_NV,// mapping        
	GL_RGB);		// componentUsage
		
    FinalCombinerInputNV( // C(RGB) = 0
        GL_VARIABLE_C_NV,       // variable      
	GL_ZERO, 		// input         
	GL_UNSIGNED_IDENTITY_NV,// mapping        
	GL_RGB);		// componentUsage

    FinalCombinerInputNV( // D = Spare0(RGB)
        GL_VARIABLE_D_NV,       // variable      
	GL_SPARE0_NV, 	        // input         
	GL_UNSIGNED_IDENTITY_NV,// mapping        
	GL_RGB);		// componentUsage

    FinalCombinerInputNV( // G = Spare0(A)
        GL_VARIABLE_G_NV,       // variable      
	GL_SPARE0_NV, 	        // input         
	GL_UNSIGNED_IDENTITY_NV,// mapping        
	GL_ALPHA);		// componentUsage
}

#endif // required extensions


UInt32 DVRMtexLUTShader::_arbMultitexture           = Window::registerExtension( "GL_ARB_multitexture" );
UInt32 DVRMtexLUTShader::_nvRegisterCombiners       = Window::registerExtension( "GL_NV_register_combiners" );
    
UInt32 DVRMtexLUTShader::_funcMultiTexCoord2dARB    = Window::registerFunction( OSG_DLSYM_UNDERSCORE"glMultiTexCoord2dARB" );
UInt32 DVRMtexLUTShader::_funcCombinerParameteriNV  = Window::registerFunction( OSG_DLSYM_UNDERSCORE"glCombinerParameteriNV" );
UInt32 DVRMtexLUTShader::_funcCombinerParameterfvNV = Window::registerFunction( OSG_DLSYM_UNDERSCORE"glCombinerParameterfvNV" );
UInt32 DVRMtexLUTShader::_funcCombinerInputNV       = Window::registerFunction( OSG_DLSYM_UNDERSCORE"glCombinerInputNV" );
UInt32 DVRMtexLUTShader::_funcCombinerOutputNV      = Window::registerFunction( OSG_DLSYM_UNDERSCORE"glCombinerOutputNV" );
UInt32 DVRMtexLUTShader::_funcFinalCombinerInputNV  = Window::registerFunction( OSG_DLSYM_UNDERSCORE"glFinalCombinerInputNV" );
