
#include <OSGConfig.h>
#include <OSGDVRShader.h>
#include <OSGDrawActionBase.h>
#include <OSGDVRVolume.h>
#include <OSGDVRVolumeTexture.h>
#include <OSGDVRIsoSurface.h>
#include <OSGSlicer.h>
#include <OSGWindow.h>
#include <OSGCamera.h>
#include <OSGColor.h>
#include <OSGGLU.h>
#include <OSGGLEXT.h>
#include "OSGDVRIsoShader.h"

#include <math.h>

void DVRIsoShader::initCombiners_IsoSurfaceDiffuse(DrawActionBase *action)
{

    Window * win = action->getWindow();

    // get extension functions
    void (*CombinerParameteriNV) (GLenum, const GLint) = 
        (void (*) (GLenum, const GLint)) win->getFunction(_funcCombinerParameteriNV);
    void (*CombinerInputNV) (GLenum, GLenum, GLenum, GLenum, GLenum, GLenum) =
        (void (*) (GLenum, GLenum, GLenum, GLenum, GLenum, GLenum)) win->getFunction(_funcCombinerInputNV);
    void (*CombinerOutputNV) (GLenum, GLenum, GLenum, GLenum, GLenum, 
                              GLenum, GLenum, GLboolean, GLboolean, GLboolean) =
        (void (*) (GLenum, GLenum, GLenum, GLenum, GLenum, 
                   GLenum, GLenum, GLboolean, GLboolean, GLboolean)) win->getFunction(_funcCombinerOutputNV);
  
    if((m_maxCombiners > 7) && m_hasPerStageConstants){
        // use all available combiners for diffuse shading
        CombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 5 + 3*((m_maxCombiners-5)/3));
    }
    else{
        CombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 2);
    }

    // first general combiner :  interpolate between two slices

    // RGB Portion
    // -----------
    CombinerInputNV(            // A  <- scaled/biased gradients
                    GL_COMBINER0_NV,          // stage
                    GL_RGB,		      // portion
                    GL_VARIABLE_A_NV,	      // variable
                    GL_TEXTURE0_ARB,	      // input
                    GL_EXPAND_NORMAL_NV,      // mapping
                    GL_RGB);		      // componentUsage

    CombinerInputNV(            // b <- interpolation factor
                    GL_COMBINER0_NV,
                    GL_RGB, 
                    GL_VARIABLE_B_NV, 
                    GL_CONSTANT_COLOR0_NV, 
                    GL_UNSIGNED_INVERT_NV,
                    GL_ALPHA);                // usage: Alpha

    CombinerInputNV(            // C <- scaled/biased gradients
                    GL_COMBINER0_NV, 
                    GL_RGB, 
                    GL_VARIABLE_C_NV, 
                    GL_TEXTURE1_ARB, 
                    GL_EXPAND_NORMAL_NV, 		
                    GL_RGB);
	
    CombinerInputNV(           // D <- interpolation factor
                    GL_COMBINER0_NV, 
                    GL_RGB, 
                    GL_VARIABLE_D_NV, 
                    GL_CONSTANT_COLOR0_NV, 
                    GL_UNSIGNED_IDENTITY_NV,
                    GL_ALPHA);              // usage Alpha

    // Alpha Portion
    // -------------

    CombinerInputNV(          //  A <- alpha values
                    GL_COMBINER0_NV, 
                    GL_ALPHA, 
                    GL_VARIABLE_A_NV, 
                    GL_TEXTURE0_ARB, 
                    GL_UNSIGNED_IDENTITY_NV, 
                    GL_ALPHA);

    CombinerInputNV(           //B <- interpolation factor
                    GL_COMBINER0_NV, 
                    GL_ALPHA, 
                    GL_VARIABLE_B_NV, 
                    GL_CONSTANT_COLOR0_NV,
                    GL_UNSIGNED_INVERT_NV, 
                    GL_ALPHA);
	

    CombinerInputNV(          // C <- alpha values
                    GL_COMBINER0_NV, 
                    GL_ALPHA, 
                    GL_VARIABLE_C_NV, 
                    GL_TEXTURE1_ARB, 
                    GL_UNSIGNED_IDENTITY_NV, 
                    GL_ALPHA);
				

    CombinerInputNV(          // D <- interpolation factor
                    GL_COMBINER0_NV, 
                    GL_ALPHA, 
                    GL_VARIABLE_D_NV, 
                    GL_CONSTANT_COLOR0_NV, 
                    GL_UNSIGNED_IDENTITY_NV, 
                    GL_ALPHA);

    // first general combiner output
	
    CombinerOutputNV(  // Spare0(RGB) <- interpolated gradients
                     GL_COMBINER0_NV, // stage
                     GL_RGB,	     // portion
                     GL_DISCARD_NV,   // abOutput
                     GL_DISCARD_NV,   // cdOutput
                     GL_SPARE0_NV,    // sumOutput
                     GL_NONE,	     // scale
                     GL_NONE,	     // bias
                     GL_FALSE,	     // abDotProduct
                     GL_FALSE,	     // cdDotproduct
                     GL_FALSE);	     // muxSum

    CombinerOutputNV(  // Spare0(A) <- interpolated alpha
                     GL_COMBINER0_NV, 
                     GL_ALPHA, 
                     GL_DISCARD_NV, 
                     GL_DISCARD_NV, 
                     GL_SPARE0_NV,
                     GL_NONE, 
                     GL_NONE, 
                     GL_FALSE, 
                     GL_FALSE, 
                     GL_FALSE); 

	
    // second general combiner: compute dot products
	
    // RGB-Portion
    // -----------
	
    CombinerInputNV(        // A(RGB) = interpolated gradients
                    GL_COMBINER1_NV,	  // stage
                    GL_RGB,		  // portion
                    GL_VARIABLE_A_NV,	  // variable
                    GL_SPARE0_NV,	  // input
                    GL_SIGNED_IDENTITY_NV,  // mapping
                    GL_RGB);		  // componentUsage
	
    CombinerInputNV(        // B(RGB) = diffuse dir light 0
                    GL_COMBINER1_NV,
                    GL_RGB, 
                    GL_VARIABLE_B_NV, 
                    GL_CONSTANT_COLOR0_NV, 
                    GL_EXPAND_NORMAL_NV,
                    GL_RGB);

    CombinerInputNV(        // C(RGB) = interpolated gradients
                    GL_COMBINER1_NV,	  // stage
                    GL_RGB,	     	  // portion
                    GL_VARIABLE_C_NV,	  // variable
                    GL_SPARE0_NV,	  // input
                    GL_SIGNED_IDENTITY_NV,  //  mapping
                    GL_RGB);		  // componentUsage
	
    CombinerInputNV(        // D(RGB) = diffuse dir light 2
                    GL_COMBINER1_NV,
                    GL_RGB, 
                    GL_VARIABLE_D_NV, 
                    GL_CONSTANT_COLOR1_NV, 
                    GL_EXPAND_NORMAL_NV,
                    GL_RGB);

    // second general combiner output
	
    CombinerOutputNV(     // Spare0(RGB) = <A|B>
                     GL_COMBINER1_NV,    // stage
                     GL_RGB,		// portion
                     GL_TEXTURE0_ARB,	// abOutput
                     GL_TEXTURE1_ARB,	// cdOutput
                     GL_DISCARD_NV,      // sumOutput
                     GL_NONE,            // scale
                     GL_NONE,		// bias
                     GL_TRUE,		// abDotProduct
                     GL_TRUE,		// cdDotproduct
                     GL_FALSE);		// muxSum

    CombinerOutputNV(     //
                     GL_COMBINER1_NV,    // stage
                     GL_ALPHA,		// portion
                     GL_DISCARD_NV,	// abOutput 
                     GL_DISCARD_NV,	// cdOutput
                     GL_DISCARD_NV,      // sumOutput
                     GL_NONE,		// scale
                     GL_NONE,		// bias
                     GL_FALSE,		// abDotProduct
                     GL_FALSE,		// cdDotproduct
                     GL_FALSE);		// muxSum

    // use more than 2 general combiners/lightsources
    if((m_maxCombiners > 7) && m_hasPerStageConstants){
        initCombiners_DiffuseMultiCombiners(action);
    }
    else{
        initCombiners_Diffuse2Combiners(action);
    }

}

void DVRIsoShader::initCombiners_Diffuse2Combiners(DrawActionBase *action)
{
    Window * win = action->getWindow();

    void (*FinalCombinerInputNV) (GLenum, GLenum, GLenum, GLenum) =
        (void (*) (GLenum, GLenum, GLenum, GLenum)) win->getFunction(_funcFinalCombinerInputNV);
    
    // final combiner input: multiply with light color and add if we have only 2 general combiners
    
    FinalCombinerInputNV(
                         GL_VARIABLE_A_NV, 
                         GL_TEXTURE0_ARB,
                         GL_UNSIGNED_IDENTITY_NV,
                         GL_RGB);

    FinalCombinerInputNV( 
                         GL_VARIABLE_B_NV, 
                         GL_PRIMARY_COLOR_NV,
                         GL_UNSIGNED_IDENTITY_NV, 
                         GL_RGB);
				
    FinalCombinerInputNV( 
                         GL_VARIABLE_C_NV, 
                         GL_ZERO,
                         GL_UNSIGNED_IDENTITY_NV, 
                         GL_RGB);	

    FinalCombinerInputNV(
                         GL_VARIABLE_E_NV, 
                         GL_TEXTURE1_ARB,
                         GL_UNSIGNED_IDENTITY_NV, 
                         GL_RGB);
    
    FinalCombinerInputNV(
                         GL_VARIABLE_F_NV, 
                         GL_SECONDARY_COLOR_NV,
                         GL_UNSIGNED_IDENTITY_NV, 
                         GL_RGB);

    FinalCombinerInputNV( 
                         GL_VARIABLE_D_NV, 
                         GL_E_TIMES_F_NV,
                         GL_UNSIGNED_IDENTITY_NV, 		
                         GL_RGB);
	
    // Alpha component
    FinalCombinerInputNV(      // G(A) = Spare0
                         GL_VARIABLE_G_NV, 
                         GL_SPARE0_NV, 
                         GL_UNSIGNED_IDENTITY_NV, 
                         GL_ALPHA);
}

void DVRIsoShader::initCombiners_DiffuseMultiCombiners(DrawActionBase *action)
{

    Window * win = action->getWindow();

    void (*CombinerInputNV) (GLenum, GLenum, GLenum, GLenum, GLenum, GLenum) =
        (void (*) (GLenum, GLenum, GLenum, GLenum, GLenum, GLenum)) win->getFunction(_funcCombinerInputNV);
    void (*CombinerOutputNV) (GLenum, GLenum, GLenum, GLenum, GLenum, 
                              GLenum, GLenum, GLboolean, GLboolean, GLboolean) =
        (void (*) (GLenum, GLenum, GLenum, GLenum, GLenum, 
                   GLenum, GLenum, GLboolean, GLboolean, GLboolean)) win->getFunction(_funcCombinerOutputNV);
    void (*FinalCombinerInputNV) (GLenum, GLenum, GLenum, GLenum) =
        (void (*) (GLenum, GLenum, GLenum, GLenum)) win->getFunction(_funcFinalCombinerInputNV);
    
    // third general combiner: apply light color to first 2 lightsources

    // RGB-Portion
    // -----------
	
    CombinerInputNV(      
                    GL_COMBINER2_NV,	      // stage
                    GL_RGB,		      // portion
                    GL_VARIABLE_A_NV,	      // variable
                    GL_TEXTURE0_ARB,	      // input
                    GL_UNSIGNED_IDENTITY_NV,// mapping
                    GL_RGB);		      // componentUsage
    
    CombinerInputNV(
                    GL_COMBINER2_NV,
                    GL_RGB,                 
                    GL_VARIABLE_B_NV,       
                    GL_CONSTANT_COLOR0_NV,  
                    GL_UNSIGNED_IDENTITY_NV, 
                    GL_RGB);

    CombinerInputNV(            
                    GL_COMBINER2_NV,	        // stage
                    GL_RGB,	     	        // portion
                    GL_VARIABLE_C_NV,	        // variable
                    GL_TEXTURE1_ARB,	        // input
                    GL_UNSIGNED_IDENTITY_NV,  // mapping
                    GL_RGB);		        // componentUsage
	
    CombinerInputNV(       
                    GL_COMBINER2_NV,
                    GL_RGB, 
                    GL_VARIABLE_D_NV, 
                    GL_CONSTANT_COLOR1_NV, 
                    GL_UNSIGNED_IDENTITY_NV,
                    GL_RGB);
	
    CombinerOutputNV(   
                     GL_COMBINER2_NV,  // stage
                     GL_RGB,		// portion
                     GL_DISCARD_NV,	// abOutput
                     GL_DISCARD_NV,	// cdOutput
                     GL_SPARE1_NV,        // sumOutput
                     GL_NONE,          // scale
                     GL_NONE,		// bias
                     GL_FALSE,		// abDotProduct
                     GL_FALSE,		// cdDotproduct
                     GL_FALSE);	// muxSum

    CombinerOutputNV(
                     GL_COMBINER2_NV,  // stage
                     GL_ALPHA,		// portion
                     GL_DISCARD_NV,	// abOutput 
                     GL_DISCARD_NV,	// cdOutput
                     GL_DISCARD_NV,    // sumOutput
                     GL_NONE,		// scale
                     GL_NONE,		// bias
                     GL_FALSE,		// abDotProduct
                     GL_FALSE,		// cdDotproduct
                     GL_FALSE);	// muxSum

    unsigned int i;
    for(i = 0; i < (m_maxCombiners-5)/3; i++){

        // compute dot products
            
        CombinerInputNV(
                        GLenum(GL_COMBINER3_NV+i*3), // stage
                        GL_RGB,		             // portion
                        GL_VARIABLE_A_NV,	     // variable
                        GL_SPARE0_NV,	             // input
                        GL_SIGNED_IDENTITY_NV,     // mapping
                        GL_RGB);		     // componentUsage
	
        CombinerInputNV(
                        GLenum(GL_COMBINER3_NV+i*3), // stage
                        GL_RGB,                      // portion
                        GL_VARIABLE_B_NV,            // variable
                        GL_CONSTANT_COLOR0_NV,       // input
                        GL_EXPAND_NORMAL_NV,         // mapping
                        GL_RGB);                     // usage

        CombinerInputNV(
                        GLenum(GL_COMBINER3_NV+i*3), // stage
                        GL_RGB,	     	             // portion
                        GL_VARIABLE_C_NV,	     // variable
                        GL_SPARE0_NV,	             // input
                        GL_SIGNED_IDENTITY_NV,     // mapping
                        GL_RGB);		     // componentUsage
	
        CombinerInputNV(      
                        GLenum(GL_COMBINER3_NV+i*3), // stage
                        GL_RGB,                      // portion
                        GL_VARIABLE_D_NV,            // variable
                        GL_CONSTANT_COLOR1_NV,       // input
                        GL_EXPAND_NORMAL_NV,         // mapping 
                        GL_RGB);                     // componentUsage
      
        CombinerOutputNV(          
                         GLenum(GL_COMBINER3_NV+i*3), // stage
                         GL_RGB,		             // portion
                         GL_TEXTURE0_ARB,             // abOutput
                         GL_TEXTURE1_ARB,             // cdOutput
                         GL_DISCARD_NV,               // sumOutput
                         GL_NONE,                     // scale
                         GL_NONE,		     // bias
                         GL_TRUE,		     // abDotProduct
                         GL_TRUE,		     // cdDotproduct
                         GL_FALSE);		     // muxSum

        CombinerOutputNV(     
                         GLenum(GL_COMBINER3_NV+i*3), // stage
                         GL_ALPHA,		     // portion
                         GL_DISCARD_NV,	             // abOutput 
                         GL_DISCARD_NV,	             // cdOutput
                         GL_DISCARD_NV,               // sumOutput
                         GL_NONE,		     // scale
                         GL_NONE,		     // bias
                         GL_FALSE,		     // abDotProduct
                         GL_FALSE,		     // cdDotproduct
                         GL_FALSE);		     // muxSum
      
        // apply light color and sum up

        CombinerInputNV(
                        GLenum(GL_COMBINER3_NV+i*3+1), // stage
                        GL_RGB,		               // portion
                        GL_VARIABLE_A_NV,	       // variable
                        GL_TEXTURE0_ARB,	       // input
                        GL_UNSIGNED_IDENTITY_NV,       // mapping
                        GL_RGB);		       // componentUsage
	
        CombinerInputNV(
                        GLenum(GL_COMBINER3_NV+i*3+1), // stage
                        GL_RGB,                        // portion
                        GL_VARIABLE_B_NV,              // variable
                        GL_CONSTANT_COLOR0_NV,         // input
                        GL_UNSIGNED_IDENTITY_NV,       // mapping
                        GL_RGB);                       // usage

        CombinerInputNV(
                        GLenum(GL_COMBINER3_NV+i*3+1), // stage
                        GL_RGB,	     	               // portion
                        GL_VARIABLE_C_NV,	       // variable
                        GL_TEXTURE1_ARB,               // input
                        GL_UNSIGNED_IDENTITY_NV,       // mapping
                        GL_RGB);		       // componentUsage
	
        CombinerInputNV(      
                        GLenum(GL_COMBINER3_NV+i*3+1), // stage
                        GL_RGB,                        // portion
                        GL_VARIABLE_D_NV,              // variable
                        GL_CONSTANT_COLOR1_NV,         // input
                        GL_UNSIGNED_IDENTITY_NV,       // mapping 
                        GL_RGB);                       // componentUsage
      
        CombinerOutputNV(          
                         GLenum(GL_COMBINER3_NV+i*3+1), // stage
                         GL_RGB,		               // portion
                         GL_DISCARD_NV,                 // abOutput
                         GL_DISCARD_NV,                 // cdOutput
                         GL_TEXTURE0_ARB,               // sumOutput
                         GL_NONE,                       // scale
                         GL_NONE,		       // bias
                         GL_FALSE,		       // abDotProduct
                         GL_FALSE,		       // cdDotproduct
                         GL_FALSE);		       // muxSum

        CombinerOutputNV(     
                         GLenum(GL_COMBINER3_NV+i*3+1), // stage
                         GL_ALPHA,		       // portion
                         GL_DISCARD_NV,	               // abOutput 
                         GL_DISCARD_NV,	               // cdOutput
                         GL_DISCARD_NV,                 //sumOutput
                         GL_NONE,		       // scale
                         GL_NONE,		       // bias
                         GL_FALSE,		       // abDotProduct
                         GL_FALSE,		       // cdDotproduct
                         GL_FALSE);		       // muxSum
      
        // sum up with previous step

        CombinerInputNV(
                        GLenum(GL_COMBINER3_NV+i*3+2), // stage
                        GL_RGB,		               // portion
                        GL_VARIABLE_A_NV,	       // variable
                        GL_SPARE1_NV,                  // input
                        GL_UNSIGNED_IDENTITY_NV,       // mapping
                        GL_RGB);		       // componentUsage
	
        CombinerInputNV(                 // B <- (1,1,1)
                        GLenum(GL_COMBINER3_NV+i*3+2), // stage
                        GL_RGB,                        // portion
                        GL_VARIABLE_B_NV,              // variable
                        GL_ZERO,                    // input
                        GL_UNSIGNED_INVERT_NV,         // mapping
                        GL_RGB);                       // usage

        CombinerInputNV(
                        GLenum(GL_COMBINER3_NV+i*3+2), // stage
                        GL_RGB,	     	               // portion
                        GL_VARIABLE_C_NV,	       // variable
                        GL_TEXTURE0_ARB,               // input
                        GL_UNSIGNED_IDENTITY_NV,       // mapping
                        GL_RGB);		       // componentUsage
	
        CombinerInputNV(                 // D <- (1,1,1)
                        GLenum(GL_COMBINER3_NV+i*3+2), // stage
                        GL_RGB,                        // portion
                        GL_VARIABLE_D_NV,              // variable
                        GL_ZERO,                    // input
                        GL_UNSIGNED_INVERT_NV,         // mapping 
                        GL_RGB);                       // componentUsage
      
        CombinerOutputNV(          
                         GLenum(GL_COMBINER3_NV+i*3+2), // stage
                         GL_RGB,		               // portion
                         GL_DISCARD_NV,                 // abOutput
                         GL_DISCARD_NV,                 // cdOutput
                         GL_SPARE1_NV,                  // sumOutput
                         GL_NONE,                       // scale
                         GL_NONE,		       // bias
                         GL_FALSE,		       // abDotProduct
                         GL_FALSE,		       // cdDotproduct
                         GL_FALSE);		       // muxSum

        CombinerOutputNV(     
                         GLenum(GL_COMBINER3_NV+i*3+2), // stage
                         GL_ALPHA,		       // portion
                         GL_DISCARD_NV,	               // abOutput 
                         GL_DISCARD_NV,	               // cdOutput
                         GL_DISCARD_NV,                 // sumOutput
                         GL_NONE,		       // scale
                         GL_NONE,		       // bias
                         GL_FALSE,		       // abDotProduct
                         GL_FALSE,		       // cdDotproduct
                         GL_FALSE);		       // muxSum

    }

    // special case: last two general combiners -> summation done with final combiner

    // compute dot products
            
    CombinerInputNV(
                    GLenum(GL_COMBINER3_NV+i*3), // stage
                    GL_RGB,		             // portion
                    GL_VARIABLE_A_NV,	     // variable
                    GL_SPARE0_NV,	             // input
                    GL_SIGNED_IDENTITY_NV,     // mapping
                    GL_RGB);		     // componentUsage
	
    CombinerInputNV(
                    GLenum(GL_COMBINER3_NV+i*3), // stage
                    GL_RGB,                      // portion
                    GL_VARIABLE_B_NV,            // variable
                    GL_CONSTANT_COLOR0_NV,       // input
                    GL_EXPAND_NORMAL_NV,         // mapping
                    GL_RGB);                     // usage

    CombinerInputNV(
                    GLenum(GL_COMBINER3_NV+i*3), // stage
                    GL_RGB,	     	             // portion
                    GL_VARIABLE_C_NV,	     // variable
                    GL_SPARE0_NV,	             // input
                    GL_SIGNED_IDENTITY_NV,     // mapping
                    GL_RGB);		     // componentUsage
	
    CombinerInputNV(      
                    GLenum(GL_COMBINER3_NV+i*3), // stage
                    GL_RGB,                      // portion
                    GL_VARIABLE_D_NV,            // variable
                    GL_CONSTANT_COLOR1_NV,       // input
                    GL_EXPAND_NORMAL_NV,         // mapping 
                    GL_RGB);                     // componentUsage
      
    CombinerOutputNV(          
                     GLenum(GL_COMBINER3_NV+i*3), // stage
                     GL_RGB,		             // portion
                     GL_TEXTURE0_ARB,             // abOutput
                     GL_TEXTURE1_ARB,             // cdOutput
                     GL_DISCARD_NV,               // sumOutput
                     GL_NONE,                     // scale
                     GL_NONE,		     // bias
                     GL_TRUE,		     // abDotProduct
                     GL_TRUE,		     // cdDotproduct
                     GL_FALSE);		     // muxSum

    CombinerOutputNV(     
                     GLenum(GL_COMBINER3_NV+i*3), // stage
                     GL_ALPHA,		     // portion
                     GL_DISCARD_NV,	             // abOutput 
                     GL_DISCARD_NV,	             // cdOutput
                     GL_DISCARD_NV,               // sumOutput
                     GL_NONE,		     // scale
                     GL_NONE,		     // bias
                     GL_FALSE,		     // abDotProduct
                     GL_FALSE,		     // cdDotproduct
                     GL_FALSE);		     // muxSum
      
    // apply light color and sum up

    CombinerInputNV(
                    GLenum(GL_COMBINER3_NV+i*3+1), // stage
                    GL_RGB,		               // portion
                    GL_VARIABLE_A_NV,	       // variable
                    GL_TEXTURE0_ARB,	       // input
                    GL_UNSIGNED_IDENTITY_NV,       // mapping
                    GL_RGB);		       // componentUsage
	
    CombinerInputNV(
                    GLenum(GL_COMBINER3_NV+i*3+1), // stage
                    GL_RGB,                        // portion
                    GL_VARIABLE_B_NV,              // variable
                    GL_CONSTANT_COLOR0_NV,         // input
                    GL_UNSIGNED_IDENTITY_NV,       // mapping
                    GL_RGB);                       // usage

    CombinerInputNV(
                    GLenum(GL_COMBINER3_NV+i*3+1), // stage
                    GL_RGB,	     	               // portion
                    GL_VARIABLE_C_NV,	       // variable
                    GL_TEXTURE1_ARB,               // input
                    GL_UNSIGNED_IDENTITY_NV,       // mapping
                    GL_RGB);		       // componentUsage
	
    CombinerInputNV(      
                    GLenum(GL_COMBINER3_NV+i*3+1), // stage
                    GL_RGB,                        // portion
                    GL_VARIABLE_D_NV,              // variable
                    GL_CONSTANT_COLOR1_NV,         // input
                    GL_UNSIGNED_IDENTITY_NV,       // mapping 
                    GL_RGB);                       // componentUsage
      
    CombinerOutputNV(          
                     GLenum(GL_COMBINER3_NV+i*3+1), // stage
                     GL_RGB,		               // portion
                     GL_DISCARD_NV,                 // abOutput
                     GL_DISCARD_NV,                 // cdOutput
                     GL_SPARE0_NV,                  // sumOutput
                     GL_NONE,                       // scale
                     GL_NONE,		       // bias
                     GL_FALSE,		       // abDotProduct
                     GL_FALSE,		       // cdDotproduct
                     GL_FALSE);		       // muxSum

    CombinerOutputNV(     
                     GLenum(GL_COMBINER3_NV+i*3+1), // stage
                     GL_ALPHA,		       // portion
                     GL_DISCARD_NV,	               // abOutput 
                     GL_DISCARD_NV,	               // cdOutput
                     GL_DISCARD_NV,                 // sumOutput
                     GL_NONE,		       // scale
                     GL_NONE,		       // bias
                     GL_FALSE,		       // abDotProduct
                     GL_FALSE,		       // cdDotproduct
                     GL_FALSE);		       // muxSum
    

    // final combiner input: sum up
	
    FinalCombinerInputNV(
                         GL_VARIABLE_A_NV, 
                         GL_ZERO,
                         GL_UNSIGNED_INVERT_NV,
                         GL_RGB);

    FinalCombinerInputNV( 
                         GL_VARIABLE_B_NV, 
                         GL_SPARE1_NV,
                         GL_UNSIGNED_IDENTITY_NV, 
                         GL_RGB);

    FinalCombinerInputNV( 
                         GL_VARIABLE_C_NV, 
                         GL_ZERO,
                         GL_UNSIGNED_IDENTITY_NV, 
                         GL_RGB);
				
    FinalCombinerInputNV( 
                         GL_VARIABLE_D_NV, 
                         GL_SPARE0_PLUS_SECONDARY_COLOR_NV,
                         GL_UNSIGNED_IDENTITY_NV, 		
                         GL_RGB);
	
    // Alpha component
    FinalCombinerInputNV(      // A = Spare0(A)
                         GL_VARIABLE_G_NV, 
                         GL_SPARE0_NV, 
                         GL_UNSIGNED_IDENTITY_NV, 
                         GL_ALPHA);
}



void DVRIsoShader::initCombiners_IsoSurfaceSpecular(DrawActionBase *action)
{

    Window * win = action->getWindow();
    void (*CombinerParameteriNV) (GLenum, const GLint) =
        (void (*) (GLenum, const GLint)) win->getFunction(_funcCombinerParameteriNV);
    void (*CombinerInputNV) (GLenum, GLenum, GLenum, GLenum, GLenum, GLenum) =
        (void (*) (GLenum, GLenum, GLenum, GLenum, GLenum, GLenum)) win->getFunction(_funcCombinerInputNV);
    void (*CombinerOutputNV) (GLenum, GLenum, GLenum, GLenum, GLenum, GLenum, GLenum, GLboolean, GLboolean, GLboolean) =
        (void (*) (GLenum, GLenum, GLenum, GLenum, GLenum, GLenum, GLenum, GLboolean, GLboolean, GLboolean)) win->getFunction(_funcCombinerOutputNV);
 
    if(m_hasPerStageConstants && m_maxCombiners >= 8){

        CombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 8);

    }
    else{

        CombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 2);

    }  

    // first general combiner input: interpolate beween two slices
    // interpolated gradients in text0

    // RGB Portion Combiner 0
    // interpolate gradients
    CombinerInputNV(                       // A <- interpolation factor
                    GL_COMBINER0_NV,       // stage
                    GL_RGB,                // portion
                    GL_VARIABLE_A_NV,      // variable
                    GL_CONSTANT_COLOR0_NV, // input
                    GL_UNSIGNED_INVERT_NV, // mapping
                    GL_ALPHA);	         // componentUsage
  
    CombinerInputNV(                         // B <- packed gradients (0) 
                    GL_COMBINER0_NV,
                    GL_RGB, 
                    GL_VARIABLE_B_NV, 
                    GL_TEXTURE0_ARB, 
                    GL_EXPAND_NORMAL_NV, 
                    GL_RGB);
  
    CombinerInputNV(                         // C <- interpolation factor
                    GL_COMBINER0_NV, 
                    GL_RGB, 
                    GL_VARIABLE_C_NV, 
                    GL_CONSTANT_COLOR0_NV, 
                    GL_UNSIGNED_IDENTITY_NV, 
                    GL_ALPHA);
  
    CombinerInputNV(                         // D <- packed gradients (1)
                    GL_COMBINER0_NV, 
                    GL_RGB, 
                    GL_VARIABLE_D_NV, 
                    GL_TEXTURE1_ARB,
                    GL_EXPAND_NORMAL_NV, 
                    GL_RGB);
  
  
    // Combiner 0, Alpha Portion
    // interpolate alpha values
    CombinerInputNV(                       // A <- interpolation factor
                    GL_COMBINER0_NV,	 // stage
                    GL_ALPHA,		 // portion
                    GL_VARIABLE_A_NV,	 // variable
                    GL_CONSTANT_COLOR0_NV, // input
                    GL_UNSIGNED_INVERT_NV, // mapping
                    GL_ALPHA);		 // componentUsage
  
    CombinerInputNV(                       // B <- alpha value (0)
                    GL_COMBINER0_NV,
                    GL_ALPHA, 
                    GL_VARIABLE_B_NV, 
                    GL_TEXTURE0_ARB, 
                    GL_UNSIGNED_IDENTITY_NV, 
                    GL_ALPHA);
  
    CombinerInputNV(                       // C <- interpolation factor 
                    GL_COMBINER0_NV, 
                    GL_ALPHA, 
                    GL_VARIABLE_C_NV, 
                    GL_CONSTANT_COLOR0_NV, 
                    GL_UNSIGNED_IDENTITY_NV, 
                    GL_ALPHA);
  
    CombinerInputNV(                       // D <- alpha value (0)
                    GL_COMBINER0_NV, 
                    GL_ALPHA, 
                    GL_VARIABLE_D_NV, 
                    GL_TEXTURE1_ARB, 
                    GL_UNSIGNED_IDENTITY_NV, 
                    GL_ALPHA);
  
  
    // first general combiner output (RGB)	
    // put interpolated gradients in TEX0
    CombinerOutputNV(                   // interpolated gradients -> TEX0
                     GL_COMBINER0_NV,   // stage
                     GL_RGB,	      // portion
                     GL_DISCARD_NV,     // abOutput
                     GL_DISCARD_NV,     // cdOutput
                     GL_TEXTURE0_ARB,   // sumOutput
                     GL_NONE,	      // scale
                     GL_NONE,	      // bias
                     GL_FALSE,	      // abDotProduct
                     GL_FALSE,	      // cdDotproduct
                     GL_FALSE);	      // muxSum
  
    // first general combiner output (ALPHA)
    // put interpolated alpha in TEX0
    CombinerOutputNV(                   // interpolated alpha -> TEX0
                     GL_COMBINER0_NV, 
                     GL_ALPHA, 
                     GL_DISCARD_NV, 
                     GL_DISCARD_NV, 
                     GL_TEXTURE0_ARB,
                     GL_NONE, 
                     GL_NONE, 
                     GL_FALSE, 
                     GL_FALSE, 
                     GL_FALSE); 
  
  
    // second general combiner: 
    // compute dotproducts for first (specular/diffuse) lightsource

    // RGB Portion
    CombinerInputNV(                       // A <- interpolated gradients
                    GL_COMBINER1_NV,       // stage
                    GL_RGB,                // portion
                    GL_VARIABLE_A_NV,      // variable
                    GL_TEXTURE0_ARB,       // input
                    GL_SIGNED_IDENTITY_NV, // mapping
                    GL_RGB);	         // componentUsage
  
    CombinerInputNV(                       // B <- diffuse dir light0
                    GL_COMBINER1_NV,       // stage
                    GL_RGB,                // portion
                    GL_VARIABLE_B_NV,      // variable
                    GL_PRIMARY_COLOR_NV,   // input
                    GL_EXPAND_NORMAL_NV,   // mapping
                    GL_RGB);               // componentUsage

    CombinerInputNV(                       // C <- interpolated gradients
                    GL_COMBINER1_NV,       // stage
                    GL_RGB,	         // portion
                    GL_VARIABLE_C_NV,      // variable
                    GL_TEXTURE0_ARB,       // input
                    GL_SIGNED_IDENTITY_NV, // mapping
                    GL_RGB);	         // componentUsage
  
    CombinerInputNV(                       // D <- halfway vector light0
                    GL_COMBINER1_NV,       // stage
                    GL_RGB,                // portion
                    GL_VARIABLE_D_NV,      // variable 
                    GL_CONSTANT_COLOR1_NV, // input
                    GL_EXPAND_NORMAL_NV,   // mapping
                    GL_RGB);               // componentUsage
  
  
    // second general combiner output
    // put dotproducts for first (specular/diffuse) lightsource in SPARE0
    CombinerOutputNV(                    // two dot products (diffuse->SPARE0,  specular->SPARE1)
                     GL_COMBINER1_NV,    // stage
                     GL_RGB,	       // portion
                     GL_SPARE0_NV,       // abOutput 
                     GL_SPARE1_NV,       // cdOutput
                     GL_DISCARD_NV,      // sumOutput
                     GL_NONE,	       // scale
                     GL_NONE,	       // bias
                     GL_TRUE,	       // abDotProduct
                     GL_TRUE,	       // cdDotproduct
                     GL_FALSE);	       // muxSum
  
    CombinerOutputNV(                    // no operation for alpha
                     GL_COMBINER1_NV,    // stage
                     GL_ALPHA,	       // portion
                     GL_DISCARD_NV,      // abOutput 
                     GL_DISCARD_NV,      // cdOutput
                     GL_DISCARD_NV,      // sumOutput
                     GL_NONE,	       // scale
                     GL_NONE,	       // bias
                     GL_FALSE,	       // abDotProduct
                     GL_FALSE,	       // cdDotproduct
                     GL_FALSE);	       // muxSum
  
    // use max. available combiners for shading
    if((m_maxCombiners > 2) && m_hasPerStageConstants){          
        initCombiners_SpecularMultiCombiners(action);
    }
    else{
        initCombiners_Specular2Combiners(action);
    }
}

void DVRIsoShader::initCombiners_Specular2Combiners(DrawActionBase *action)
{
    Window * win = action->getWindow();

    void (*FinalCombinerInputNV) (GLenum, GLenum, GLenum, GLenum) =
        (void (*) (GLenum, GLenum, GLenum, GLenum)) win->getFunction(_funcFinalCombinerInputNV);


    // final combiner input

    FinalCombinerInputNV(        // E <- <grad,halfway>
                         GL_VARIABLE_E_NV, 
                         GL_SPARE1_NV, 
                         GL_UNSIGNED_IDENTITY_NV, 
                         GL_RGB);
    
    FinalCombinerInputNV(        // F <- <grad,halfway>
                         GL_VARIABLE_F_NV, 
                         GL_SPARE1_NV, 
                         GL_UNSIGNED_IDENTITY_NV, 
                         GL_RGB);
    
    FinalCombinerInputNV(       // A <- <grad,halfway>^2
                         GL_VARIABLE_A_NV, 
                         GL_E_TIMES_F_NV,
                         GL_UNSIGNED_IDENTITY_NV, 
                         GL_RGB);
    
    FinalCombinerInputNV(         // B <- <grad,halfway>^2
                         GL_VARIABLE_B_NV, 
                         GL_E_TIMES_F_NV, 
                         GL_UNSIGNED_IDENTITY_NV, 
                         GL_RGB);
    
    FinalCombinerInputNV(        // C <- zero 
                         GL_VARIABLE_C_NV, 
                         GL_ZERO, 
                         GL_UNSIGNED_IDENTITY_NV, 
                         GL_RGB);	
    
    FinalCombinerInputNV(       // D <- ambient + diffuse
                         GL_VARIABLE_D_NV, 
                         GL_SPARE0_PLUS_SECONDARY_COLOR_NV, 
                         GL_UNSIGNED_IDENTITY_NV, 		
                         GL_RGB);
    
    // Alpha component
    FinalCombinerInputNV( 
                         GL_VARIABLE_G_NV, 
                         GL_TEXTURE0_ARB, 
                         GL_UNSIGNED_IDENTITY_NV, 
                         GL_ALPHA);
    
}

void DVRIsoShader::initCombiners_SpecularMultiCombiners(DrawActionBase *action)
{
    Window * win = action->getWindow();

    void (*CombinerInputNV) (GLenum, GLenum, GLenum, GLenum, GLenum, GLenum) =
        (void (*) (GLenum, GLenum, GLenum, GLenum, GLenum, GLenum)) win->getFunction(_funcCombinerInputNV);
    void (*CombinerOutputNV) (GLenum, GLenum, GLenum, GLenum, GLenum, GLenum, GLenum, GLboolean, GLboolean, GLboolean) =
        (void (*) (GLenum, GLenum, GLenum, GLenum, GLenum, GLenum, GLenum, GLboolean, GLboolean, GLboolean)) win->getFunction(_funcCombinerOutputNV);

    void (*FinalCombinerInputNV) (GLenum, GLenum, GLenum, GLenum) =
        (void (*) (GLenum, GLenum, GLenum, GLenum)) win->getFunction(_funcFinalCombinerInputNV);
          
    // third general combiner: 
    // compute dotproducts for specular fraction of second and third lightsource
  
    // RGB Portion
  
    CombinerInputNV(                          // A <- interpolated gradients
                    GL_COMBINER2_NV,          // stage
                    GL_RGB,	            // portion
                    GL_VARIABLE_A_NV,         // variable
                    GL_TEXTURE0_ARB,	    // input
                    GL_SIGNED_IDENTITY_NV,    // mapping
                    GL_RGB);	            // componentUsage
  
    CombinerInputNV(                          // B <- halfway vector light 1
                    GL_COMBINER2_NV,          // stage
                    GL_RGB,                   // portion 
                    GL_VARIABLE_B_NV,         // variable
                    GL_CONSTANT_COLOR0_NV,    // input 
                    GL_EXPAND_NORMAL_NV,     // mapping
                    GL_RGB);                  // componentUsage
  
    CombinerInputNV(                          // C <- interpolated gradients
                    GL_COMBINER2_NV,          // stage
                    GL_RGB,		    // portion
                    GL_VARIABLE_C_NV,         // variable
                    GL_TEXTURE0_ARB,	    // input
                    GL_SIGNED_IDENTITY_NV,    // mapping
                    GL_RGB);	            // componentUsage
  
    CombinerInputNV(                          // D <- halfway vector light 2
                    GL_COMBINER2_NV,          // stage
                    GL_RGB,		    // portion
                    GL_VARIABLE_D_NV,         // variable
                    GL_CONSTANT_COLOR1_NV,    // input
                    GL_EXPAND_NORMAL_NV,      // mapping
                    GL_RGB);	            // componentUsage  
  
    // ALPHA portion

    CombinerInputNV(                          // A <- specular coeff of light 0
                    GL_COMBINER2_NV,          // stage
                    GL_ALPHA,	            // portion
                    GL_VARIABLE_A_NV,         // variable
                    GL_SPARE1_NV, 	    // input
                    GL_UNSIGNED_IDENTITY_NV,  // mapping
                    GL_BLUE);	            // componentUsage
  
    CombinerInputNV(                          // B <- specular coeff of light 0
                    GL_COMBINER2_NV,          // stage
                    GL_ALPHA,                 // portion 
                    GL_VARIABLE_B_NV,         // variable
                    GL_SPARE1_NV,             // input 
                    GL_UNSIGNED_IDENTITY_NV,  // mapping
                    GL_BLUE);                 // componentUsage

    // third general combiner output
   
    CombinerOutputNV(                    // two dot products for specular coeffs of light 1 and light 2                                   
                     GL_COMBINER2_NV,    // stage
                     GL_RGB,	       // portion
                     GL_TEXTURE1_ARB,     // abOutput 
                     GL_SPARE1_NV,       // cdOutput
                     GL_DISCARD_NV,      // sumOutput
                     GL_NONE,	       // scale
                     GL_NONE,	       // bias
                     GL_TRUE,            // abDotProduct
                     GL_TRUE,            // cdDotproduct
                     GL_FALSE);	       // muxSum
   
    CombinerOutputNV(                    // square specular coeff of light 0
                     GL_COMBINER2_NV,    // stage
                     GL_ALPHA,	       // portion
                     GL_SPARE0_NV,       // abOutput 
                     GL_DISCARD_NV,      // cdOutput
                     GL_DISCARD_NV,      // sumOutput
                     GL_NONE,	       // scale
                     GL_NONE,	       // bias
                     GL_FALSE,	       // abDotProduct
                     GL_FALSE,	       // cdDotproduct
                     GL_FALSE);	       // muxSum


    // 4th general combiner: 
    // compute dot products for diffuse fraction of second and third lightsource
  
    // RGB Portion
  
    CombinerInputNV(                          // A <- interpolated gradients
                    GL_COMBINER3_NV,          // stage
                    GL_RGB,	            // portion
                    GL_VARIABLE_A_NV,         // variable
                    GL_TEXTURE0_ARB,	    // input
                    GL_SIGNED_IDENTITY_NV,    // mapping
                    GL_RGB);	            // componentUsage
  
    CombinerInputNV(                          // B <- diffuse dir light 1
                    GL_COMBINER3_NV,          // stage
                    GL_RGB,                   // portion 
                    GL_VARIABLE_B_NV,         // variable
                    GL_CONSTANT_COLOR0_NV,    // input 
                    GL_EXPAND_NORMAL_NV,      // mapping
                    GL_RGB);                  // componentUsage
  
    CombinerInputNV(                          // C <- interpolated gradients
                    GL_COMBINER3_NV,          // stage
                    GL_RGB,		    // portion
                    GL_VARIABLE_C_NV,         // variable
                    GL_TEXTURE0_ARB,	    // input
                    GL_SIGNED_IDENTITY_NV,    // mapping
                    GL_RGB);	            // componentUsage
  
    CombinerInputNV(                          // D <- diffuse dir light 2
                    GL_COMBINER3_NV,          // stage
                    GL_RGB,		    // portion
                    GL_VARIABLE_D_NV,         // variable
                    GL_CONSTANT_COLOR1_NV,    // input
                    GL_EXPAND_NORMAL_NV,      // mapping
                    GL_RGB);	            // componentUsage  
  
    // ALPHA portion

    CombinerInputNV(                          // A <- specular coeff of light 1
                    GL_COMBINER3_NV,          // stage
                    GL_ALPHA,	            // portion
                    GL_VARIABLE_A_NV,         // variable
                    GL_TEXTURE1_ARB, 	    // input
                    GL_UNSIGNED_IDENTITY_NV,  // mapping
                    GL_BLUE);	            // componentUsage

    CombinerInputNV(                          // B <- specular coeff of light 1
                    GL_COMBINER3_NV,          // stage
                    GL_ALPHA,	            // portion
                    GL_VARIABLE_B_NV,         // variable
                    GL_TEXTURE1_ARB, 	    // input
                    GL_UNSIGNED_IDENTITY_NV,  // mapping
                    GL_BLUE);	            // componentUsage
  
    CombinerInputNV(                          // C <- specular coeff of light 2
                    GL_COMBINER3_NV,          // stage
                    GL_ALPHA,                 // portion 
                    GL_VARIABLE_B_NV,         // variable
                    GL_SPARE1_NV,             // input 
                    GL_UNSIGNED_IDENTITY_NV,  // mapping
                    GL_BLUE);                 // componentUsage

    CombinerInputNV(                          // D <- specular coeff of light 2
                    GL_COMBINER3_NV,          // stage
                    GL_ALPHA,                 // portion 
                    GL_VARIABLE_D_NV,         // variable
                    GL_SPARE1_NV,             // input 
                    GL_UNSIGNED_IDENTITY_NV,  // mapping
                    GL_BLUE);                 // componentUsage

    // 4th general combiner output
   
    CombinerOutputNV(                    // two dot products for diffuse coeffs of light 1 and light 2
                     GL_COMBINER3_NV,    // stage
                     GL_RGB,	       // portion
                     GL_TEXTURE1_ARB,    // abOutput 
                     GL_SPARE1_NV,       // cdOutput
                     GL_DISCARD_NV,      // sumOutput
                     GL_NONE,	       // scale
                     GL_NONE,	       // bias
                     GL_TRUE,            // abDotProduct
                     GL_TRUE,            // cdDotproduct
                     GL_FALSE);	       // muxSum
   
    CombinerOutputNV(                    // square specular coeffs of light 1 and light 2
                     GL_COMBINER3_NV,    // stage
                     GL_ALPHA,	       // portion
                     GL_TEXTURE1_ARB,    // abOutput 
                     GL_SPARE1_NV,       // cdOutput
                     GL_DISCARD_NV,      // sumOutput
                     GL_NONE,	       // scale
                     GL_NONE,	       // bias
                     GL_FALSE,	       // abDotProduct
                     GL_FALSE,	       // cdDotproduct
                     GL_FALSE);	       // muxSum

    // 5th general combiner: 
    // compute products for diffuse fraction of second and third lightsource
    // and sum up

    // RGB Portion
  
    CombinerInputNV(                          // A <- diffuse coeff of light 1
                    GL_COMBINER4_NV,          // stage
                    GL_RGB,	            // portion
                    GL_VARIABLE_A_NV,         // variable
                    GL_TEXTURE1_ARB,	    // input
                    GL_UNSIGNED_IDENTITY_NV,  // mapping
                    GL_RGB);	            // componentUsage
  
    CombinerInputNV(                          // B <- diffuse color light 1
                    GL_COMBINER4_NV,          // stage
                    GL_RGB,                   // portion 
                    GL_VARIABLE_B_NV,         // variable
                    GL_CONSTANT_COLOR0_NV,    // input 
                    GL_UNSIGNED_IDENTITY_NV,  // mapping
                    GL_RGB);                  // componentUsage
  
    CombinerInputNV(                          // C <- diffuse coeff of light 2
                    GL_COMBINER4_NV,          // stage
                    GL_RGB,		    // portion
                    GL_VARIABLE_C_NV,         // variable
                    GL_SPARE1_NV, 	    // input
                    GL_UNSIGNED_IDENTITY_NV,  // mapping
                    GL_RGB);	            // componentUsage
  
    CombinerInputNV(                          // D <- diffuse color light 2
                    GL_COMBINER4_NV,          // stage
                    GL_RGB,		    // portion
                    GL_VARIABLE_D_NV,         // variable
                    GL_CONSTANT_COLOR1_NV,    // input
                    GL_UNSIGNED_IDENTITY_NV,  // mapping
                    GL_RGB);	            // componentUsage  
  
    // ALPHA portion

    CombinerInputNV(                          // A <- squared specular coeff of light 1
                    GL_COMBINER4_NV,          // stage
                    GL_ALPHA,	            // portion
                    GL_VARIABLE_A_NV,         // variable
                    GL_TEXTURE1_ARB, 	    // input
                    GL_UNSIGNED_IDENTITY_NV,  // mapping
                    GL_ALPHA);	            // componentUsage

    CombinerInputNV(                          // B <- squared specular coeff of light 1
                    GL_COMBINER4_NV,          // stage
                    GL_ALPHA,	            // portion
                    GL_VARIABLE_B_NV,         // variable
                    GL_TEXTURE1_ARB, 	    // input
                    GL_UNSIGNED_IDENTITY_NV,  // mapping
                    GL_ALPHA);	            // componentUsage
  
    CombinerInputNV(                          // C <- squared specular coeff of light 2
                    GL_COMBINER4_NV,          // stage
                    GL_ALPHA,                 // portion 
                    GL_VARIABLE_B_NV,         // variable
                    GL_SPARE1_NV,             // input 
                    GL_UNSIGNED_IDENTITY_NV,  // mapping
                    GL_ALPHA);                // componentUsage

    CombinerInputNV(                          // D <- squared specular coeff of light 2
                    GL_COMBINER4_NV,          // stage
                    GL_ALPHA,                 // portion 
                    GL_VARIABLE_D_NV,         // variable
                    GL_SPARE1_NV,             // input 
                    GL_UNSIGNED_IDENTITY_NV,  // mapping
                    GL_ALPHA);                // componentUsage

    // 5th general combiner output
   
    CombinerOutputNV(                    // two products for diffuse color of light 1 and light 2 and sum up
                     GL_COMBINER4_NV,    // stage
                     GL_RGB,	       // portion
                     GL_DISCARD_NV,      // abOutput 
                     GL_DISCARD_NV,      // cdOutput
                     GL_SPARE1_NV,       // sumOutput
                     GL_NONE,	       // scale
                     GL_NONE,	       // bias
                     GL_FALSE,           // abDotProduct
                     GL_FALSE,           // cdDotproduct
                     GL_FALSE);	       // muxSum
   
    CombinerOutputNV(                    // square squared specular coeffs of light 1 and light 2
                     GL_COMBINER4_NV,    // stage
                     GL_ALPHA,	       // portion
                     GL_TEXTURE1_ARB,    // abOutput 
                     GL_SPARE1_NV,       // cdOutput
                     GL_DISCARD_NV,      // sumOutput
                     GL_NONE,	       // scale
                     GL_NONE,	       // bias
                     GL_FALSE,	       // abDotProduct
                     GL_FALSE,	       // cdDotproduct
                     GL_FALSE);	       // muxSum


    // 6th general combiner: 
    // compute products for color of specular fraction of second and third lightsource
    // and sum up

    // RGB Portion
  
    CombinerInputNV(                          // A <- ^4 specular coeff of light 1
                    GL_COMBINER5_NV,          // stage
                    GL_RGB,	            // portion
                    GL_VARIABLE_A_NV,         // variable
                    GL_TEXTURE1_ARB,	    // input
                    GL_UNSIGNED_IDENTITY_NV,  // mapping
                    GL_ALPHA);	            // componentUsage
  
    CombinerInputNV(                          // B <- specular color light 1
                    GL_COMBINER5_NV,          // stage
                    GL_RGB,                   // portion 
                    GL_VARIABLE_B_NV,         // variable
                    GL_CONSTANT_COLOR0_NV,    // input 
                    GL_UNSIGNED_IDENTITY_NV,  // mapping
                    GL_RGB);                  // componentUsage
  
    CombinerInputNV(                          // C <- ^4 specular coeff of light 2
                    GL_COMBINER5_NV,          // stage
                    GL_RGB,		    // portion
                    GL_VARIABLE_C_NV,         // variable
                    GL_SPARE1_NV, 	    // input
                    GL_UNSIGNED_IDENTITY_NV,  // mapping
                    GL_ALPHA);	            // componentUsage
  
    CombinerInputNV(                          // D <- specular color light 2
                    GL_COMBINER5_NV,          // stage
                    GL_RGB,		    // portion
                    GL_VARIABLE_D_NV,         // variable
                    GL_CONSTANT_COLOR1_NV,    // input
                    GL_UNSIGNED_IDENTITY_NV,  // mapping
                    GL_RGB);	            // componentUsage  
  
    // ALPHA portion

    CombinerInputNV(                          // A <- squared specular coeff of light 0
                    GL_COMBINER5_NV,          // stage
                    GL_ALPHA,	            // portion
                    GL_VARIABLE_A_NV,         // variable
                    GL_SPARE0_NV, 	    // input
                    GL_UNSIGNED_IDENTITY_NV,  // mapping
                    GL_ALPHA);	            // componentUsage

    CombinerInputNV(                          // B <- squared specular coeff of light 0
                    GL_COMBINER5_NV,          // stage
                    GL_ALPHA,	            // portion
                    GL_VARIABLE_B_NV,         // variable
                    GL_SPARE0_NV, 	    // input
                    GL_UNSIGNED_IDENTITY_NV,  // mapping
                    GL_ALPHA);	            // componentUsage

    // 6th general combiner output
   
    CombinerOutputNV(                    // two products for specular color of light 1 and light 2 and sum up
                     GL_COMBINER5_NV,    // stage
                     GL_RGB,	       // portion
                     GL_DISCARD_NV,      // abOutput 
                     GL_DISCARD_NV,      // cdOutput
                     GL_TEXTURE1_ARB,    // sumOutput
                     GL_NONE,	       // scale
                     GL_NONE,	       // bias
                     GL_FALSE,           // abDotProduct
                     GL_FALSE,           // cdDotproduct
                     GL_FALSE);	       // muxSum
   
    CombinerOutputNV(                    // square squared specular coeffs of light 0
                     GL_COMBINER5_NV,    // stage
                     GL_ALPHA,	       // portion
                     GL_SPARE0_NV,       // abOutput 
                     GL_DISCARD_NV,      // cdOutput
                     GL_DISCARD_NV,      // sumOutput
                     GL_NONE,	       // scale
                     GL_NONE,	       // bias
                     GL_FALSE,	       // abDotProduct
                     GL_FALSE,	       // cdDotproduct
                     GL_FALSE);	       // muxSum


    // 7th general combiner: 
    // compute sum of contribution second and third lightsource

    // RGB Portion
  
    CombinerInputNV(                          // A <- diffuse contribution from light1 and light 2
                    GL_COMBINER6_NV,          // stage
                    GL_RGB,	            // portion
                    GL_VARIABLE_A_NV,         // variable
                    GL_SPARE1_NV, 	    // input
                    GL_UNSIGNED_IDENTITY_NV,  // mapping
                    GL_RGB);	            // componentUsage
  
    CombinerInputNV(                          // B <- (1,1,1)
                    GL_COMBINER6_NV,          // stage
                    GL_RGB,	            // portion
                    GL_VARIABLE_B_NV,         // variable
                    GL_ZERO,       	    // input
                    GL_UNSIGNED_INVERT_NV,    // mapping
                    GL_RGB);	            // componentUsage

    CombinerInputNV(                          // C <- specular contribution of light 1 and light 2
                    GL_COMBINER6_NV,          // stage
                    GL_RGB,                   // portion 
                    GL_VARIABLE_C_NV,         // variable
                    GL_TEXTURE1_ARB,          // input 
                    GL_UNSIGNED_IDENTITY_NV,  // mapping
                    GL_RGB);                  // componentUsage

    CombinerInputNV(                          // D <- (1,1,1)
                    GL_COMBINER6_NV,          // stage
                    GL_RGB,	            // portion
                    GL_VARIABLE_D_NV,         // variable
                    GL_ZERO,       	    // input
                    GL_UNSIGNED_INVERT_NV,    // mapping
                    GL_RGB);	            // componentUsage

    // 7th general combiner output
   
    CombinerOutputNV(                    // sum up diffuse and specular contributions of light 1 and 2 
                     GL_COMBINER6_NV,    // stage
                     GL_RGB,	       // portion
                     GL_DISCARD_NV,      // abOutput 
                     GL_DISCARD_NV,      // cdOutput
                     GL_SPARE1_NV,       // sumOutput
                     GL_NONE,	       // scale
                     GL_NONE,	       // bias
                     GL_FALSE,           // abDotProduct
                     GL_FALSE,           // cdDotproduct
                     GL_FALSE);	       // muxSum
   
    CombinerOutputNV(                    // nothing todo
                     GL_COMBINER6_NV,    // stage
                     GL_ALPHA,	       // portion
                     GL_DISCARD_NV,      // abOutput 
                     GL_DISCARD_NV,      // cdOutput
                     GL_DISCARD_NV,      // sumOutput
                     GL_NONE,	       // scale
                     GL_NONE,	       // bias
                     GL_FALSE,	       // abDotProduct
                     GL_FALSE,	       // cdDotproduct
                     GL_FALSE);	       // muxSum

    // 8th general combiner: 
    // apply colors and compute sum of contribution first lightsource

    // RGB Portion
  
    CombinerInputNV(                          // A <- ^4 specular coeff of light0
                    GL_COMBINER7_NV,          // stage
                    GL_RGB,	            // portion
                    GL_VARIABLE_A_NV,         // variable
                    GL_SPARE0_NV, 	    // input
                    GL_UNSIGNED_IDENTITY_NV,  // mapping
                    GL_ALPHA);	            // componentUsage
  
    CombinerInputNV(                           // B <- specular color of light0
                    GL_COMBINER7_NV,          // stage
                    GL_RGB,	            // portion
                    GL_VARIABLE_B_NV,         // variable
                    GL_CONSTANT_COLOR0_NV,    // input
                    GL_UNSIGNED_IDENTITY_NV,  // mapping
                    GL_RGB);	            // componentUsage

    CombinerInputNV(                          // C <- diffuse coeff of light 0
                    GL_COMBINER7_NV,          // stage
                    GL_RGB,                   // portion 
                    GL_VARIABLE_C_NV,         // variable
                    GL_SPARE0_NV,             // input 
                    GL_UNSIGNED_IDENTITY_NV,  // mapping
                    GL_RGB);                  // componentUsage

    CombinerInputNV(                          // D <- diffuse color of light 0
                    GL_COMBINER7_NV,          // stage
                    GL_RGB,	            // portion
                    GL_VARIABLE_D_NV,         // variable
                    GL_CONSTANT_COLOR1_NV,    // input
                    GL_UNSIGNED_IDENTITY_NV,  // mapping
                    GL_RGB);	            // componentUsage

    // 7th general combiner output
   
    CombinerOutputNV(                    // sum up diffuse and specular contribution of light 0 
                     GL_COMBINER7_NV,    // stage
                     GL_RGB,	       // portion
                     GL_DISCARD_NV,      // abOutput 
                     GL_DISCARD_NV,      // cdOutput
                     GL_SPARE0_NV,       // sumOutput
                     GL_NONE,	       // scale
                     GL_NONE,	       // bias
                     GL_FALSE,           // abDotProduct
                     GL_FALSE,           // cdDotproduct
                     GL_FALSE);	       // muxSum
   
    CombinerOutputNV(                    // nothing todo
                     GL_COMBINER7_NV,    // stage
                     GL_ALPHA,	       // portion
                     GL_DISCARD_NV,      // abOutput 
                     GL_DISCARD_NV,      // cdOutput
                     GL_DISCARD_NV,      // sumOutput
                     GL_NONE,	       // scale
                     GL_NONE,	       // bias
                     GL_FALSE,	       // abDotProduct
                     GL_FALSE,	       // cdDotproduct
                     GL_FALSE);	       // muxSum


    // final combiner input
    // sum up contributions of all lightsources
        
    FinalCombinerInputNV(       // A <- contribution of light 1 and light 2
                         GL_VARIABLE_A_NV, 
                         GL_SPARE1_NV,
                         GL_UNSIGNED_IDENTITY_NV, 
                         GL_RGB);
    
    FinalCombinerInputNV(         // B <- (1,1,1)
                         GL_VARIABLE_B_NV, 
                         GL_ZERO, 
                         GL_UNSIGNED_INVERT_NV, 
                         GL_RGB);
    
    FinalCombinerInputNV(        // C <- zero 
                         GL_VARIABLE_C_NV, 
                         GL_ZERO, 
                         GL_UNSIGNED_IDENTITY_NV, 
                         GL_RGB);	
    
    FinalCombinerInputNV(       // D <- ambient + contribution of light 0
                         GL_VARIABLE_D_NV, 
                         GL_SPARE0_PLUS_SECONDARY_COLOR_NV, 
                         GL_UNSIGNED_IDENTITY_NV, 		
                         GL_RGB);
    
    // Alpha component
    FinalCombinerInputNV( 
                         GL_VARIABLE_G_NV, 
                         GL_TEXTURE0_ARB, 
                         GL_UNSIGNED_IDENTITY_NV, 
                         GL_ALPHA);    
}

void DVRIsoShader::setupCombinerParametersDiffuse(DVRVolume *volume, DrawActionBase *action)
{

    Window * win = action->getWindow();

    void (*CombinerParameterfvNV)(GLenum, GLfloat *) = 
        (void (*)(GLenum, GLfloat *)) win->getFunction(_funcCombinerParameterfvNV);
    
    void (*SecondaryColor3fvEXT) (GLfloat *) =
        (void (*) (GLfloat *)) win->getFunction(_funcSecondaryColor3fvEXT);
  
    void (*CombinerStageParameterfvNV) (GLenum, GLenum, GLfloat *) = 
        (void (*) (GLenum, GLenum, GLfloat *)) win->getFunction(_funcCombinerStageParameterfvNV);


    DirLightList diffuseLights;
    DirLightList specularLights;
    Color4f      ambientLight;
    getLightSources(diffuseLights,specularLights,ambientLight);

    // check for max. supported diffuse lightsources
    unsigned int maxDiffuseLights = 2;
    if((m_maxCombiners > 5) && m_hasPerStageConstants){
        maxDiffuseLights = 4 + 2*((m_maxCombiners-5)/3);
    }

    // get light sources
    Color4f * diffuseColor = new Color4f[maxDiffuseLights];
    Vec4f   * diffuseDir   = new Vec4f[maxDiffuseLights];
      
    unsigned int i;
    DirLightList::iterator l = diffuseLights.begin();
    for(i = 0; i < maxDiffuseLights && l != diffuseLights.end(); i++){ 
        Vec3f tmp;
        diffuseColor[i] = l->color;
        Slicer::rotateToLocal(action,l->dir,tmp);
        tmp.normalize();
        diffuseDir[i].setValues(0.5f+0.5f*tmp[0],0.5f+0.5f*tmp[1],0.5f+0.5f*tmp[2],0.0f);
        l++;
    }
      
    while(i < maxDiffuseLights){
        diffuseColor[i] = Color4f(0.0f,0.0f,0.0f,1.0f);
        i++;
    }
      
    if((m_maxCombiners > 5) && m_hasPerStageConstants){
        // use all available combiners
        
        glEnable(GL_PER_STAGE_CONSTANTS_NV);
        
        // set constant for slice interpolation
        CombinerStageParameterfvNV(GL_COMBINER0_NV,GL_CONSTANT_COLOR0_NV,diffuseDir[0].getValues());
        
        // set direction of first two lights
        CombinerStageParameterfvNV(GL_COMBINER1_NV,GL_CONSTANT_COLOR0_NV,diffuseDir[0].getValues());
        CombinerStageParameterfvNV(GL_COMBINER1_NV,GL_CONSTANT_COLOR1_NV,diffuseDir[1].getValues());

        // set color for first two lights
        CombinerStageParameterfvNV(GL_COMBINER2_NV,GL_CONSTANT_COLOR0_NV,diffuseColor[0].getValuesRGBA());
        CombinerStageParameterfvNV(GL_COMBINER2_NV,GL_CONSTANT_COLOR1_NV,diffuseColor[1].getValuesRGBA());
        
        unsigned int i;
       
        // set values for all lights
        //      float ones[4] = {1.0f,1.0f,1.0f,1.0f};
        for(i = 0; i < (m_maxCombiners-5)/3; i++){
            // set direction next two lights
            CombinerStageParameterfvNV(GLenum(GL_COMBINER3_NV+i*3),GL_CONSTANT_COLOR0_NV,diffuseDir[2+2*i].getValues());
            CombinerStageParameterfvNV(GLenum(GL_COMBINER3_NV+i*3),GL_CONSTANT_COLOR1_NV,diffuseDir[3+2*i].getValues());

            // set color for next two lights
            CombinerStageParameterfvNV(GLenum(GL_COMBINER3_NV+i*3+1),GL_CONSTANT_COLOR0_NV,diffuseColor[2+2*i].getValuesRGBA());
            CombinerStageParameterfvNV(GLenum(GL_COMBINER3_NV+i*3+1),GL_CONSTANT_COLOR1_NV,diffuseColor[3+2*i].getValuesRGBA());        
        }

        // set direction for last two lights
        CombinerStageParameterfvNV(GLenum(GL_COMBINER3_NV+i*3),GL_CONSTANT_COLOR0_NV,diffuseDir[2+2*i].getValues());
        CombinerStageParameterfvNV(GLenum(GL_COMBINER3_NV+i*3),GL_CONSTANT_COLOR1_NV,diffuseDir[3+2*i].getValues());

        // set color for last two lights
        CombinerStageParameterfvNV(GLenum(GL_COMBINER3_NV+i*3+1),GL_CONSTANT_COLOR0_NV,diffuseColor[2+2*i].getValuesRGBA());
        CombinerStageParameterfvNV(GLenum(GL_COMBINER3_NV+i*3+1),GL_CONSTANT_COLOR1_NV,diffuseColor[3+2*i].getValuesRGBA());

        // set additional ambient lighting
        SecondaryColor3fvEXT(ambientLight.getValuesRGBA());
    }
    else{

        // use only two general combiners

        // set primary color   = color of light 0
        glColor3fv(diffuseColor[0].getValuesRGBA());
        
        // set constant color 0 = direction of light 0    	
        CombinerParameterfvNV(GL_CONSTANT_COLOR0_NV, diffuseDir[0].getValues()); 
        
        // set secondary color = color of light 1    
        SecondaryColor3fvEXT(diffuseColor[1].getValuesRGBA());
	
        // set constant color 1 = direction of light 1    	
        CombinerParameterfvNV(GL_CONSTANT_COLOR1_NV, diffuseDir[1].getValues());
    }

    delete [] diffuseColor;
    delete [] diffuseDir;

}

void DVRIsoShader::setupCombinerParametersSpecular(DVRVolume *volume, DrawActionBase *action)
{

    Window * win = action->getWindow();

    void (*CombinerParameterfvNV)(GLenum, GLfloat *) = 
        (void (*)(GLenum, GLfloat *)) win->getFunction(_funcCombinerParameterfvNV);
    
    void (*SecondaryColor3fvEXT) (GLfloat *) =
        (void (*) (GLfloat *)) win->getFunction(_funcSecondaryColor3fvEXT);
  
    void (*CombinerStageParameterfvNV) (GLenum, GLenum, GLfloat *) = 
        (void (*) (GLenum, GLenum, GLfloat *)) win->getFunction(_funcCombinerStageParameterfvNV);

    DirLightList diffuseLights;
    DirLightList specularLights;
    Color4f      ambientLight;
    getLightSources(diffuseLights,specularLights,ambientLight);
    
    // get light source params
    Vec4f diffuseDir[3];
    Vec4f halfway[3];
    Color4f diffuseColor[3];
    Color4f specularColor[3];
    unsigned int i;
      
    // get diffuse lights
    DirLightList::iterator ld = diffuseLights.begin();
    for(i = 0; i < 3 && ld != diffuseLights.end(); i++){ 
        Vec3f tmp;
        diffuseColor[i] = ld->color;
        Slicer::rotateToLocal(action,ld->dir,tmp);
        tmp.normalize();
        diffuseDir[i].setValues(0.5f+0.5f*tmp[0],0.5f+0.5f*tmp[1],0.5f+0.5f*tmp[2],0.0f);
        ld++;
    }
    while(i < 3){
        diffuseColor[i] = Color4f(0.0f,0.0f,0.0f,1.0f);
        diffuseDir[i]   = Vec4f(0.0f,0.0f,0.0f,0.0f);
        i++;
    }

    // get and classify the slicing direction
    Vec3f  viewDir;
    Slicer::getSlicingDirection(action, &viewDir);
    viewDir.normalize();

    // get specular lights
    DirLightList::iterator ls = specularLights.begin();
    for(i = 0; i < 3 && ls != specularLights.end(); i++){ 
        Vec3f tmp;
        specularColor[i] = ls->color;
        Slicer::rotateToLocal(action,ls->dir,tmp);
        tmp.normalize();
        // calc halfway
        tmp += viewDir;
        tmp.normalize();
        halfway[i].setValues(0.5f+0.5f*tmp[0],0.5f+0.5f*tmp[1],0.5f+0.5f*tmp[2],0.0f);
        ls++;
    }
    while(i < 3){
        specularColor[i] = Color4f(0.0f,0.0f,0.0f,1.0f);
        halfway[i]       = Vec4f(0.0f,0.0f,0.0f,0.0f);
        i++;
    }	

    if((m_maxCombiners >= 8) && m_hasPerStageConstants){

        // set lightsource colors
	
        glEnable(GL_PER_STAGE_CONSTANTS_NV);

        // set halfway vector light 0
        CombinerStageParameterfvNV(GL_COMBINER1_NV,GL_CONSTANT_COLOR1_NV,halfway[0].getValues());
        
        // set diffuse dir light 0
        glColor4fv(diffuseDir[0].getValues());
        
        // set halfway vector light 1
        CombinerStageParameterfvNV(GL_COMBINER2_NV,GL_CONSTANT_COLOR0_NV,halfway[1].getValues());

        // set halfway vector light 2
        CombinerStageParameterfvNV(GL_COMBINER2_NV,GL_CONSTANT_COLOR1_NV,halfway[2].getValues());

        // set diffuse dir light 1
        CombinerStageParameterfvNV(GL_COMBINER3_NV,GL_CONSTANT_COLOR0_NV,diffuseDir[1].getValues());

        // set diffuse dir light 2
        CombinerStageParameterfvNV(GL_COMBINER3_NV,GL_CONSTANT_COLOR1_NV,diffuseDir[2].getValues());

        // set diffuse color light 1
        CombinerStageParameterfvNV(GL_COMBINER4_NV,GL_CONSTANT_COLOR0_NV,diffuseColor[1].getValuesRGBA());

        // set diffuse color light 2
        CombinerStageParameterfvNV(GL_COMBINER4_NV,GL_CONSTANT_COLOR1_NV,diffuseColor[2].getValuesRGBA());

        // set specular color light 1
        CombinerStageParameterfvNV(GL_COMBINER5_NV,GL_CONSTANT_COLOR0_NV,specularColor[1].getValuesRGBA());

        // set specular color light 2
        CombinerStageParameterfvNV(GL_COMBINER5_NV,GL_CONSTANT_COLOR1_NV,specularColor[2].getValuesRGBA());

        // set specular color light 0
        CombinerStageParameterfvNV(GL_COMBINER7_NV,GL_CONSTANT_COLOR0_NV,specularColor[0].getValuesRGBA());

        // set diffuse color light 0
        CombinerStageParameterfvNV(GL_COMBINER7_NV,GL_CONSTANT_COLOR1_NV,diffuseColor[0].getValuesRGBA());

    }
    else {

        // we can only use white lightsources due to limited register combiner ressources, 
        // so at least scale them 
        diffuseDir[0] *= diffuseColor[0][0];
        halfway[0]    *= specularColor[0][0];

        // set constant color 0 =  halfway    	
        CombinerParameterfvNV(GL_CONSTANT_COLOR1_NV, halfway[0].getValues()); 

        // set direction of diffuse light
        glColor3f(0.5 + 0.5 * diffuseDir[0][0],0.5 + 0.5 * diffuseDir[0][1], 0.5 + 0.5 * diffuseDir[0][2]);

    }
      
    // set ambient light
    SecondaryColor3fvEXT(ambientLight.getValuesRGBA());

}

void DVRIsoShader::activate_NVRegisterCombinerShading(DVRVolume *volume, DrawActionBase *action)
{

    if (volume->getDoTextures()) { 

        Window * win = action->getWindow();

        glGetIntegerv(GL_MAX_GENERAL_COMBINERS_NV,&m_maxCombiners);    
        FINFO(("DVRIsoShader - found %d general register combiners\n",m_maxCombiners));

        if(win->hasExtension(_NV_register_combiners2)){
            FINFO(("DVRIsoShader - found per (register combiner) stage constants\n"));
            m_hasPerStageConstants = true;
        }
        else{
            FINFO(("DVRIsoShader - no per (register combiner) stage constants available\n"));
            m_hasPerStageConstants = false;
        }
    
        void (*ActiveTextureARB)(GLenum target) =
            (void (*)(GLenum target)) win->getFunction(_funcActiveTextureARB);
    
        // get parameters from isosurface attachment if available
        Real32 isoValue;
        UInt32 alphaMode;
        bool   doSpecular;
        DVRIsoSurfacePtr    isoParam = DVRVOLUME_PARAMETER(volume, DVRIsoSurface);
        if(isoParam != NullFC){
            isoValue   = isoParam->getIsoValue();
            alphaMode  = isoParam->getAlphaMode();
            doSpecular = isoParam->getSpecularLighting();
        }
        else{
            isoValue   = 1.0;
            alphaMode  = GL_GREATER;
            doSpecular = false;
        }

        // push the relevant parts of the current OpenGL state	
        glPushAttrib(GL_ENABLE_BIT | 
                     GL_COLOR_BUFFER_BIT |               
                     GL_DEPTH_BUFFER_BIT | 
                     GL_POLYGON_BIT | 
                     GL_TEXTURE_BIT);
	
        // enable register combiners
        glEnable(GL_REGISTER_COMBINERS_NV);

        // enable multitextures
        if(m_shadingMode == SM_REGISTER_COMBINERS_MULTI2D){
            ActiveTextureARB(GL_TEXTURE0_ARB);
            ActiveTextureARB(GL_TEXTURE1_ARB);
        }

        // case NV_ISOSURFACE_SHADED:

        glDisable(GL_BLEND);  
        glDisable(GL_LIGHTING);


        // enable alpha test	
        glEnable(GL_ALPHA_TEST); 
        glAlphaFunc(alphaMode,isoValue);

        // initialize combiner setup
        if (doSpecular) {
            setupCombinerParametersSpecular(volume,action);
            initCombiners_IsoSurfaceSpecular(action);
        } else {
            setupCombinerParametersDiffuse(volume,action);
            initCombiners_IsoSurfaceDiffuse(action);
        }
    
        //      case NV_ISOSURFACE_SHADED_2SIDED:
        //        if (fIsoOpacity < 1.0) {
        //  				// TRANSPARENT_ISO SURFACE
        //  				// write opacity value into destination alpha
        //          glClearColor(0.0, 0.0, 0.0, fIsoOpacity),
        //            glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
        //          glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        //          glBlendFunc(GL_DST_ALPHA,GL_ONE_MINUS_DST_ALPHA); 	
        //          glEnable(GL_BLEND); 
        //        }else {
        //          glDisable(GL_BLEND);
        //        }
    
    }
  
}

void DVRIsoShader::renderSlice_NVRegisterCombinerShading(DVRVolume *volume, DrawActionBase *action,
                                                         Real32 *data, UInt32 vertices, UInt32 values)
{ 

    if (values < 6) {
        SWARNING << "DVRIsoShader::renderSlice - insufficient values" << std::endl;
	return;
    }

    Window *win = action->getWindow();

    // get extension functions
    void (*CombinerParameterfvNV) (GLenum, const GLfloat *) = 
        (void (*) (GLenum, const GLfloat *)) win->getFunction(_funcCombinerParameterfvNV);
    void (*MultiTexCoord2dARB) (GLenum, GLdouble, GLdouble) = 
        (void (*) (GLenum, GLdouble, GLdouble)) win->getFunction(_funcMultiTexCoord2dARB); 

    // set interpolation factor

    bool   doSpecular;
    DVRIsoSurfacePtr    isoParam = DVRVOLUME_PARAMETER(volume, DVRIsoSurface);
    if(isoParam != NullFC){
        doSpecular = isoParam->getSpecularLighting();
    }
    else{
        doSpecular = false;
    }
 
    if(((doSpecular && (m_maxCombiners > 2)) || (m_maxCombiners > 5)) && m_hasPerStageConstants){
        void (*CombinerStageParameterfvNV) (GLenum, GLenum, const GLfloat *) = 
            (void (*) (GLenum, GLenum, const GLfloat *)) win->getFunction(_funcCombinerStageParameterfvNV);
        GLfloat col[4] = {0.0f,0.0f,0.0f,data[5]};
        CombinerStageParameterfvNV(GL_COMBINER0_NV,GL_CONSTANT_COLOR0_NV, col);
    }
    else{      
        GLfloat col[4];
        glGetFloatv(GL_CONSTANT_COLOR0_NV,col);
        col[3] =  data[5];
        CombinerParameterfvNV(GL_CONSTANT_COLOR0_NV, col);
    }

    if (volume->getDoTextures()){
        glBegin(GL_TRIANGLE_FAN);
    }
    else{
        glColor4f(1.0, 1.0, 1.0, 1.0);
        glBegin(GL_LINE_LOOP);
    }
    
    // draw polygon

    for (UInt32 i = 0; i < vertices; i++){
        Real32 * vert = data + i * values;
      
        MultiTexCoord2dARB(GL_TEXTURE0_ARB, vert[3], vert[4]);
        MultiTexCoord2dARB(GL_TEXTURE1_ARB, vert[3], vert[4]);
        glVertex3f(vert[0], vert[1], vert[2]);
    } 
    glEnd();

}

// render a clipped slice for 2D multitexture register combiner shading
void DVRIsoShader::renderSlice_NVRegisterCombinerShading(DVRVolume *volume, 
                                                         DrawActionBase *action,
                                                         DVRRenderSlice *clippedSlice)
{
    Window *win = action->getWindow();

    // get extension functions
    void (*CombinerParameterfvNV) (GLenum, const GLfloat *) = 
        (void (*) (GLenum, const GLfloat *)) win->getFunction(_funcCombinerParameterfvNV);
    void (*MultiTexCoord2dARB) (GLenum, GLdouble, GLdouble) = 
        (void (*) (GLenum, GLdouble, GLdouble)) win->getFunction(_funcMultiTexCoord2dARB); 

    // set interpolation factor
    bool   doSpecular;
    DVRIsoSurfacePtr    isoParam = DVRVOLUME_PARAMETER(volume, DVRIsoSurface);
    if(isoParam != NullFC){
        doSpecular = isoParam->getSpecularLighting();
    }
    else{
        doSpecular = false;
    }
    
    GLfloat intFactor;
    switch(clippedSlice->orientation){
    case DVRRenderSlice::XY:
        intFactor = (*(*clippedSlice->begin())->vertices.begin())[5];
        break;
    case DVRRenderSlice::XZ:
        intFactor = (*(*clippedSlice->begin())->vertices.begin())[4];
        break;
    case DVRRenderSlice::YZ:
        intFactor = (*(*clippedSlice->begin())->vertices.begin())[3];
        break;
    default:
        break;
    }

    if(((doSpecular && (m_maxCombiners > 2)) || (m_maxCombiners > 5)) && m_hasPerStageConstants){
        void (*CombinerStageParameterfvNV) (GLenum, GLenum, const GLfloat *) = 
            (void (*) (GLenum, GLenum, const GLfloat *)) win->getFunction(_funcCombinerStageParameterfvNV);
      
        GLfloat col[4] = {0.0f,0.0f,0.0f,intFactor};
        CombinerStageParameterfvNV(GL_COMBINER0_NV,GL_CONSTANT_COLOR0_NV, col);
    }
    else{      
        GLfloat col[4];
        glGetFloatv(GL_CONSTANT_COLOR0_NV,col);
        col[3] =  intFactor;
        CombinerParameterfvNV(GL_CONSTANT_COLOR0_NV, col);
    }

    switch(clippedSlice->orientation){
    case DVRRenderSlice::XY:
        for(std::vector<DVRRenderSlicePrimitive*>::const_iterator prim = clippedSlice->begin();
            prim != clippedSlice->end();
            prim++){
            glBegin((*prim)->type);
            std::vector<GLdouble*>::const_iterator vert = (*prim)->vertices.begin();        
            for(; vert != (*prim)->vertices.end(); vert++){
                MultiTexCoord2dARB(GL_TEXTURE0_ARB, (*vert)[3], (*vert)[4]);
                MultiTexCoord2dARB(GL_TEXTURE1_ARB, (*vert)[3], (*vert)[4]);
                glVertex3dv(*vert);
            }      
            glEnd();
        }
        break;
    case DVRRenderSlice::XZ:
        for(std::vector<DVRRenderSlicePrimitive*>::const_iterator prim = clippedSlice->begin();
            prim != clippedSlice->end();
            prim++){
            glBegin((*prim)->type);
            std::vector<GLdouble*>::const_iterator vert = (*prim)->vertices.begin();        
            for(; vert != (*prim)->vertices.end(); vert++){
                MultiTexCoord2dARB(GL_TEXTURE0_ARB, (*vert)[3], (*vert)[5]);
                MultiTexCoord2dARB(GL_TEXTURE1_ARB, (*vert)[3], (*vert)[5]);
                glVertex3dv(*vert);
            }      
            glEnd();
        }
        break;
    case DVRRenderSlice::YZ:
        for(std::vector<DVRRenderSlicePrimitive*>::const_iterator prim = clippedSlice->begin();
            prim != clippedSlice->end();
            prim++){
            glBegin((*prim)->type);
            std::vector<GLdouble*>::const_iterator vert = (*prim)->vertices.begin();        
            for(; vert != (*prim)->vertices.end(); vert++){
                MultiTexCoord2dARB(GL_TEXTURE0_ARB, (*vert)[4], (*vert)[5]);
                MultiTexCoord2dARB(GL_TEXTURE1_ARB, (*vert)[4], (*vert)[5]);
                glVertex3dv(*vert);
            }      
            glEnd();
        }
        break;
    default:
        break;
    }
}

void DVRIsoShader::deactivate_NVRegisterCombinerShading(DVRVolume *volume, DrawActionBase *action)
{

    if(volume->getDoTextures()){

        glDisable(GL_REGISTER_COMBINERS_NV);

        if((m_maxCombiners > 2) && m_hasPerStageConstants)
            glDisable(GL_PER_STAGE_CONSTANTS_NV);

        glDisable(GL_ALPHA_TEST);      

        glPopAttrib();

    }

}
