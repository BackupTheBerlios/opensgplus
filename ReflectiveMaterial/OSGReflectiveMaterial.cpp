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
 *   Initial coding by Polina Kondratieva                                    *
 *                                                                           *
 *   contact:    PolinaKondratieva@gmx.de     (supervisor Jan Kautz)         *
 *                                                                           *
 *                                                                           *
\*---------------------------------------------------------------------------*/

//---------------------------------------------------------------------------
//  Includes
//---------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>

#define OSG_COMPILESYSTEMLIB

#include <OSGConfig.h>
#include <OSGMaterialChunk.h>
#include <OSGImage.h>
#include <OSGTextureChunk.h>
#include <OSGVertexProgramChunk.h>
#include <OSGRegisterCombinersChunk.h>

//#include <OSGGLUTWindow.h>
#include <OSGViewport.h>
#include <OSGPerspectiveCamera.h>
#include <OSGMatrixCamera.h>

#include <OSGTextureGrabForeground.h>
#include <OSGSolidBackground.h>
#include <OSGMatrixCameraDecorator.h>

#include <OSGCubeTextureChunk.h>
#include <OSGTexGenChunk.h>
#include <OSGTextureTransformChunk.h>

#include <OSGVertexProgramChunk.h>
#include "OSGSimpleSphericalHarmonics.h"


#include "OSGReflectiveMaterial.h"

OSG_USING_NAMESPACE

/***************************************************************************\
 *                            Description                                  *
\***************************************************************************/

/*! \class osg::ReflectiveMaterial
    Reflective material. Object with this material mirror the surrounding
    scene. CubeMapping is applied as the Environment Mapping technique.
    Sherical harmonics are used to represent the reflectance model of
    material. The level of reflectiveness is tuned by the fresnel coefficient.
*/

/***************************************************************************\
 *                           Class variables                               *
\***************************************************************************/

/***************************************************************************\
 *                           Class methods                                 *
\***************************************************************************/

// Tragets for CubeMapping
//===============================================================================
static GLenum targets[6] = { GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB,
                             GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB,
                             GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB,
                             GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB,
                             GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB,
                             GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB
                           };


// Transformations for camera which grab the scene from the six different
// view directions
//===============================================================================
static Matrix transforms[6] = { Matrix( 0,0,-1,0,  0,-1,0,0, -1,0,0,0,  0,0,0,1), //  90 ab. Y, 180 ab. X
                                Matrix( 0,0, 1,0,  0,-1,0,0,  1,0,0,0,  0,0,0,1), // -90 ab. Y, 180 ab. X
                                Matrix( 1,0, 0,0,  0,0, 1,0,  0,-1,0,0, 0,0,0,1), // -90 ab. X
                                Matrix( 1,0, 0,0,  0,0,-1,0,  0,1, 0,0, 0,0,0,1), //  90 ab. X
                                Matrix( 1,0, 0,0,  0,-1,0,0,  0,0,-1,0, 0,0,0,1), //  180 ab. X
                                Matrix(-1,0, 0,0,  0,-1,0,0,  0,0, 1,0, 0,0,0,1)  //  180 ab. Z
                              };
//===============================================================================

static bool sh_coef_build = false;
static int cmsize = 128;

// The part of the Vertex program (just a string which contain it)
//===============================================================================
static std::string begin_vp =
"!!ARBvp1.0\n"
"##### Params, attribs, etc...\n"
"#====================================================================\n"
"ATTRIB vpos = vertex.position;\n"
"ATTRIB normal = vertex.normal;\n"
//--------------------------------------------------------------------
"PARAM mvp[4] = { state.matrix.mvp };\n"
"PARAM  mvinv[4] = { state.matrix.modelview.invtrans };\n"
//--------------------------------------------------------------------
"PARAM Mr[4] = { program.local[0..3] };  # SH coefficients\n"
"PARAM Mg[4] = { program.local[4..7] };  # SH coefficients\n"
"PARAM Mb[4] = { program.local[8..11] }; # SH coefficients\n"
"PARAM diffusebase = program.local[12];  # diffuse color\n"
"PARAM fcoeff = program.local[13];       # Fresnel\n"
"PARAM fcoeffm1 = program.local[14];     # 1 - Fresnel\n"
"PARAM lightpos = program.local[15];\n"
"PARAM view = program.local[16];\n"
"PARAM halfObjBBox = program.local[17];\n"
//--------------------------------------------------------------------
"PARAM consts = {0.1,0.25,0.5,1.0};\n"
"PARAM five = {5,5,5,5};\n"
"PARAM one = {1.0,1.0,1.0,1.0};\n"
"PARAM zero = {0.0,0.0,0.0,0.0};\n"
"PARAM axisX = {1.0,0.05,0.0,1.0};\n"
//--------------------------------------------------------------------
"TEMP vtrafo;       # trafo vertex\n"
"TEMP ntrafo;       # trafo normal\n"
"TEMP nr, ng, nb;   # intermediate diffuse SH stuff\n"
"TEMP rv;           # reflected view\n"
"TEMP fresnel;      # fresnel coeff\n"
"TEMP tmp1;\n"
"TEMP tmp2;\n"
"TEMP tmp3;\n"
//--------------------------------------------------------------------
"##### Transform vertex for projection onto the screen\n"
"#====================================================================\n"
"DP4 vtrafo.x, mvp[0], vpos;\n"
"DP4 vtrafo.y, mvp[1], vpos;\n"
"DP4 vtrafo.z, mvp[2], vpos;\n"
"DP4 vtrafo.w, mvp[3], vpos;\n"
//--------------------------------------------------------------------
"MOV result.position, vtrafo;\n"
//--------------------------------------------------------------------
"##### Transform vertex to make all computations\n"
"#====================================================================\n"
"DP3 tmp3.x, mvinv[0], vpos;\n"
"DP3 tmp3.y, mvinv[1], vpos;\n"
"DP3 tmp3.z, mvinv[2], vpos;\n"
"MOV tmp3.w, one.x;\n"
//--------------------------------------------------------------------
"##### Transform normal to make all computations\n"
"#====================================================================\n"
"DP3 ntrafo.x, mvinv[0], normal;\n"
"DP3 ntrafo.y, mvinv[1], normal;\n"
"DP3 ntrafo.z, mvinv[2], normal;\n"
"MOV ntrafo.w, consts.w;\n"
"##### Now compute SH result\n"
"#====================================================================\n"
//--------------------------------------------------------------------
"# Do Mr, Mg, Mb....\n"
"##----------------------------------\n"
"DP4 nr.x, Mr[0], ntrafo;\n"
"DP4 nr.y, Mr[1], ntrafo;\n"
"DP4 nr.z, Mr[2], ntrafo;\n"
"DP4 nr.w, Mr[3], ntrafo;\n"
//-----------------------------------
"DP4 ng.x, Mg[0], ntrafo;\n"
"DP4 ng.y, Mg[1], ntrafo;\n"
"DP4 ng.z, Mg[2], ntrafo;\n"
"DP4 ng.w, Mg[3], ntrafo;\n"
//-----------------------------------
"DP4 nb.x, Mb[0], ntrafo;\n"
"DP4 nb.y, Mb[1], ntrafo;\n"
"DP4 nb.z, Mb[2], ntrafo;\n"
"DP4 nb.w, Mb[3], ntrafo;\n"
//-----------------------------------
"# Final dot product, store as color\n"
"##----------------------------------\n"
"DP4 tmp1.x, nr, ntrafo;\n"
"DP4 tmp1.y, ng, ntrafo;\n"
"DP4 tmp1.z, nb, ntrafo;\n"
//-----------------------------------
"# multiply with base color\n"
"##----------------------------------\n"
"MUL result.color.primary, tmp1, diffusebase; # some base color\n"
"##### Compute reflection vector rv\n"
"#====================================================================\n"
"# Compute view vector from the vertex\n"
"# position to the eye position\n"
"##----------------------------------\n"
"SUB tmp2, view, tmp3;\n"
//-----------------------------------
"# 2(n,v)n - v\n"
"##----------------------------------\n"
"DP3 rv, ntrafo, tmp2;\n"
"ADD rv, rv, rv;\n"
"MUL rv, rv, ntrafo;\n"
"ADD rv, rv, -tmp2;\n"
//-----------------------------------
"MOV result.texcoord[1], rv;            # CubeMap texture coordinates\n"
//--------------------------------------------------------------------
"##### Compute Fresnel term   # store in fresnel->alpha\n"
"#====================================================================\n"
"# normalize view vector\n"
"##----------------------------------\n"
"DP3 tmp1.x, tmp2, tmp2;\n"
"RSQ tmp1.y, tmp1.x;\n"
"MUL tmp3, tmp2, tmp1.y;\n"
"DP3 tmp1.x, ntrafo, tmp3;                # cos(V, N)\n"
//--------------------------------------------------------------------
/*"########################################################################\n"
"#     Sometimes normals of the object are 'reversed' for some reasons\n"
"# and it gives incorrect result -> normal of the vertex which we\n"
"# see on the screen appears with opposite direction to the\n"
"# vector from the vertex to the eye (V)\n"
"#     We can see only the 'front' face of the object, so, the\n"
"# maximum angle between the normal of the visible vertex and\n"
"# vector V is 90. All the other vertices whose normals are opposite to\n"
"# the vector V just invisible. Because of   0 < Cos(V,N) < 1\n"
"# for angle(N,V) <=90 # we can take absolute value of cosine to avoid\n"
"# situation with incorrect model normals.\n"
"########################################################################\n"*/
//--------------------------------------------------------------------
"ABS tmp1.x, tmp1.x;\n"
"SUB fresnel.w, consts.w, tmp1.x;         # 1-cos(N*V)\n"
"POW fresnel, fresnel.w, five.w;          # pow5 = (1-cos(N*V))^5\n"
"MAD fresnel, fcoeffm1, fresnel, fcoeff;  # (1-F)*pow5 + F\n"
"SUB fresnel, one, fresnel;               # return: 1-fresnel\n"
"MOV result.color.secondary, fresnel;     # Note, fresnel - multi color\n";


// Assemble Vertex program
//===============================================================================
std::string ReflectiveMaterial::makeVP(bool compTexCoord)
{
    std::string comp_tc, take_tc, whole_vp;

    comp_tc = "##### Coodrdinates for Diffuse texture\n"
  /* "#====================================================================\n"
     "#     If model do not contain texture coordinates, let's define them\n"
     "# in very simple way.\n"
     "#     Let's imagine that we have 3D texture consists of copies\n"
     "# of given 2D texture. This 3D texture is assumed to be a spicemen\n"
     "# which we want to 'made' the object of. Cube is 1x1x1 so we need\n"
     "# to scale our object. For this purpose, we take parameter 'length'\n"
     "# (the length of half-diagonal of bounding box of the object) from\n"
     "# the main program and divide all the vpos by it.\n"
     "#     The texture coordinates are in range [0..1], but the vpos_scaled\n"
     "# are in the range [-1..1]. So, we should make the following\n"
     "# transformation: tc = (vpos + 1)/2. And the simple texture coordinates\n"
     "# are ready to be used...\n"
     "########################################################################\n"*/
     "RCP tmp1.x, halfObjBBox.x;\n"
     "MUL tmp1, vpos, tmp1.x;\n"
     "MAD tmp1, consts.z, tmp1, consts.z;\n"
     "MOV tmp1.w, one.x;\n"
     "MOV result.texcoord[0], tmp1;\n"
     "END\n";

    take_tc = "##### Coodrdinates for Diffuse texture\n"
     "#====================================================================\n"
     "MOV result.texcoord[0], vertex.texcoord[0]; # if model contains tex.coords.\n"
     "END\n";

    if(compTexCoord)
       whole_vp = begin_vp + comp_tc;
    else
       whole_vp = begin_vp + take_tc;

    return whole_vp;
}

// Helper function, which setup all the chunks of this Material
//===============================================================================
void ReflectiveMaterial::prepareLocalChunks(void)
{
    // Setup base material chunk
    //---------------------------------------------------------
    if(_materialChunk == NullFC)
    {
       Color3f v3;
       Color4f v4;

       _materialChunk = MaterialChunk::create();
       addRefCP(_materialChunk);

       float alpha = 1.f - getTransparency();

       beginEditCP(_materialChunk);
          v4.setValuesRGBA(0, 0, 0, alpha);
         _materialChunk->setAmbient(v4);
         _materialChunk->setDiffuse(v4);
         _materialChunk->setEmission(v4);
          v4.setValuesRGBA(1, 1, 1, alpha);
         _materialChunk->setSpecular(v4);
         _materialChunk->setShininess( 1 );
         _materialChunk->setLit(false);
         _materialChunk->setColorMaterial(GL_DIFFUSE);
       endEditCP  (_materialChunk);
    }

    // Setup diffuse texture chunk
    //---------------------------------------------------------
    if(_textureChunkDiffuse == NullFC)
    {
       _textureChunkDiffuse = TextureChunk::create();
       addRefCP(_textureChunkDiffuse);

       ImagePtr imgt;

       if(getImageDiffuse() == NullFC)
       {
          // If no Diffuse texture is given
	  UChar8 data[] = {  255, 0xff, 0xff,  0xff, 0xff, 0xff,
                       0xff, 0xff, 0xff,  0xff, 0xff, 0xff };

          imgt = Image::create();
          beginEditCP(imgt);
            imgt->set( Image::OSG_RGB_PF, 2, 2, 1, 1, 1, 0, data );
          endEditCP(imgt);
       }
       else
       {
          imgt = getImageDiffuse();
       }

       beginEditCP(_textureChunkDiffuse);
         _textureChunkDiffuse->setImage( imgt );
         _textureChunkDiffuse->setWrapS(GL_REPEAT);
         _textureChunkDiffuse->setWrapT(GL_REPEAT);
         _textureChunkDiffuse->setEnvMode(GL_MODULATE);
         _textureChunkDiffuse->setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
         _textureChunkDiffuse->setMagFilter(GL_LINEAR);
       endEditCP(_textureChunkDiffuse);
    }

    // Setup cube texture chunk
    //---------------------------------------------------------
    if(_cubeTextureChunk == NullFC)
    {
       _cubeTextureChunk = CubeTextureChunk::create();

       addRefCP(_cubeTextureChunk);

       // After grabbing the scene this texture will be replaced
       ImagePtr imgt = Image::create();
       beginEditCP(imgt);
          imgt->set(Image::OSG_RGB_PF, 256, 256);
       endEditCP(imgt);

       beginEditCP(_cubeTextureChunk);
         _cubeTextureChunk->setImage(imgt);
         _cubeTextureChunk->setPosZImage(imgt);
         _cubeTextureChunk->setNegXImage(imgt);
         _cubeTextureChunk->setPosXImage(imgt);
         _cubeTextureChunk->setNegYImage(imgt);
         _cubeTextureChunk->setPosYImage(imgt);
         _cubeTextureChunk->setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
         _cubeTextureChunk->setMagFilter(GL_LINEAR);
         _cubeTextureChunk->setWrapR(GL_REPEAT);
         _cubeTextureChunk->setWrapS(GL_REPEAT);
         _cubeTextureChunk->setWrapT(GL_REPEAT);
       endEditCP(_cubeTextureChunk);
    }

    // Compute spherical harmonics coeffisients
    // (OSGSimpleSphericalHarmonics.* were taken from Jan Kautz)
    //---------------------------------------------------------
    if(!sh_coef_build)
    {
       ImagePtr imfg[6];
       imfg[0] = getImagePosX();
       imfg[1] = getImageNegX();
       imfg[2] = getImagePosY();
       imfg[3] = getImageNegY();
       imfg[4] = getImagePosZ();
       imfg[5] = getImageNegZ();
       getSHCoefficients(imfg);
       sh_coef_build = true;
    }

    // Setup vertex program chunk
    //---------------------------------------------------------
    if(_vpChunk == NullFC)
    {
       _vpChunk = VertexProgramChunk::create();

       addRefCP(_vpChunk);

       const float c1 = 0.429043/M_PI, c2 = 0.511664/M_PI, c3 = 0.743126/M_PI;
       const float c4 = 0.886227/M_PI, c5 = 0.247708/M_PI;
       Real32 halfBBox = getHalfBBoxObj();
       Color3f difColor = getDiffuse();

       std::string vertex_program = makeVP(getCompTexCoord());

       beginEditCP(_vpChunk);
         _vpChunk->setProgram(vertex_program);
         for(int j = 0; j < 3; j++ )
	 { // for each color
           _vpChunk->addParameter("a", j*4+0, Vec4f( c1*Clm[8][j],
				                     c1*Clm[4][j],
					             c1*Clm[7][j],
					             c2*Clm[3][j] ) );
           _vpChunk->addParameter("b", j*4+1, Vec4f( c1*Clm[4][j],
					            -c1*Clm[8][j],
					             c1*Clm[5][j],
					             c2*Clm[1][j] ) );
           _vpChunk->addParameter("c", j*4+2, Vec4f( c1*Clm[7][j],
					             c1*Clm[5][j],
					             c3*Clm[6][j],
					             c2*Clm[2][j] ) );
           _vpChunk->addParameter("d", j*4+3, Vec4f( c2*Clm[3][j],
					             c2*Clm[1][j],
					             c2*Clm[2][j],
					             c4*Clm[0][j]-c5*Clm[6][j] ) );
        }
         _vpChunk->addParameter("kd", 12, Vec4f(difColor[0],difColor[1],difColor[2],0.0) );
         _vpChunk->addParameter("fresnel", 13, Vec4f(0.05,0.05,0.05,1.0) );
         _vpChunk->addParameter("mfresnel", 14, Vec4f(0.95,0.95,0.95,1.0) );
         _vpChunk->addParameter("lightpos", 15, Vec4f(0,0,1,1));
         _vpChunk->addParameter("viewpos", 16, Vec4f(0,0,1,1));
         _vpChunk->addParameter("halfObjBBox", 17, Vec4f(halfBBox,halfBBox,halfBBox,1.0));
       endEditCP(_vpChunk);
    }

    // Setup register combiners program chunk
    //---------------------------------------------------------
    if(_rcChunk == NullFC)
    {
        _rcChunk = RegisterCombinersChunk::create();

        addRefCP(_rcChunk);
    }

}

// Intern function which builds viewport to grab the scene from the given
// (by index) direction. In other words, this function makes one side
// of the CubeMap.
//===============================================================================
void ReflectiveMaterial::make_env_map(int index, PerspectiveCameraPtr &cam_env, WindowPtr &win, NodePtr &environment)
{
    SolidBackgroundPtr bkgnd = SolidBackground::create();

    // Add the grab foreground
    //---------------------------------------------------------
    beginEditCP(texGrabForeground[index]);
      texGrabForeground[index]->setTexture(_cubeTextureChunk);
      texGrabForeground[index]->setBindTarget(GL_TEXTURE_CUBE_MAP_ARB);
      texGrabForeground[index]->setCopyTarget(targets[index]);
    endEditCP(texGrabForeground[index]);

    Real32 l, b;
    l = b = 0;
    // take the viewport
    //---------------------------------------------------------
    beginEditCP(viewPort[index]);
      viewPort[index]->setBackground( bkgnd );
      viewPort[index]->setRoot( environment );

      MatrixCameraDecoratorPtr deco = MatrixCameraDecorator::create();
      beginEditCP(deco);
        deco->setDecoratee(cam_env);
        deco->setPreProjection(transforms[index]);
      endEditCP(deco);

      viewPort[index]->setCamera( deco );
      viewPort[index]->getForegrounds().push_back(texGrabForeground[index]);
      viewPort[index]->setLeft(l);
      viewPort[index]->setRight(l + cmsize - 1);
      viewPort[index]->setBottom(b);
      viewPort[index]->setTop(b + cmsize - 1);
    endEditCP  (viewPort[index]);

    beginEditCP(win);
      win->addPort(viewPort[index]);
    endEditCP(win);

}


// This function is very important. It builds 6 viewports to grab the scene
// from the different view direction (view position is the same - the place,
// where the reflective object is situated).
// This function should be called !!!only after!!! all parameters of material
// will be set inside the additional beginEdit/EndEdit pair (see example)
//===============================================================================
void ReflectiveMaterial::MakeCubeMap(PerspectiveCameraPtr &cam_env, WindowPtr &win, NodePtr &environment)
{
    int i;
    if(viewPort[0] == NullFC || texGrabForeground[0] == NullFC)
    {
       for(i = 0; i < 6; i++)
       {
          viewPort[i] = Viewport::create();
          texGrabForeground[i] = TextureGrabForeground::create();
       }
    }

    for(i = 0; i < 6; i++)
    {
       make_env_map(i, cam_env, win, environment);  // here make one side of the CubeMap
    }
}

// Simple Spherical Harmonics computation
//===============================================================================
void *ReflectiveMaterial::getSHCoefficients(ImagePtr imfg[6])
{
    SimpleSphericalHarmonics sh;
    sh.resetCoeffs();

    int img_height, img_width, samples, last;

    last = imfg[0]->getWidth();

    //for(int i = 0; i < 6; i++)
    //{
    img_width = imfg[0]->getWidth();
    img_height = imfg[0]->getHeight();
      //if(img_width != last || img_height != last)
      //{
      //   The CubeMapping works even in the case this requirenment is
      //   violated. So I decided just do not take it into account
      //   std::cout << "\nAll the textures for CubeMap should be the squares of equal size!\n";
      //   osgExit();
      //   exit(1);
      //}
    //}
    samples = img_width * img_height;
    cmsize = img_width;
    int side;

    // Cycle for all 6 textures involved by the CubeMap
    //---------------------------------------------------------
    for ( int s = 0; s < 6; s++ )
    {
       char side;
       switch (s )
       {
          case 0: side = 'X';   break;  //current image -> posX;
          case 1: side = 'x';   break;  //current image -> negX;
          case 2: side = 'Y';   break;  //current image -> posY;
          case 3: side = 'y';   break;  //current image -> negY;
          case 4: side = 'Z';   break;  //current image -> posZ;
          case 5: side = 'z';   break;  //current image -> negZ;
       }

       int xx(0), yy(0);

       // Cycles for each pixel in the current texture-side of the CubeMap
       //---------------------------------------------------------
       for ( yy = 0; yy < img_height; yy++ )
       {
         for ( xx = 0; xx < img_width; xx++ )
	 {
	   float u = 2.0*float(xx)/float(img_width-1) - 1.0;
	   float v = 2.0*float(yy)/float(img_height-1) - 1.0;

	   // compute direction
	   Vec3f dir;
	   {
	     switch( side )
	     {
	        case 'X': dir = Vec3f(  1.0,   -v,  -u ); break;
	        case 'x': dir = Vec3f( -1.0,   -v,   u ); break;
	        case 'Y': dir = Vec3f(    u,  1.0,   v ); break;
	        case 'y': dir = Vec3f(    u, -1.0,  -v ); break;
	        case 'Z': dir = Vec3f(    u,   -v, 1.0 ); break;
	        case 'z': dir = Vec3f(   -u,   -v,-1.0 ); break;
	     }
	     dir.normalize();
	   }

	   // compute diff solid angle
           //---------------------------------------------------------
	   float dsangle;
	   {
	     // cos(theta) = 1.0/sqrt(1.0+u*u+v*v)
	     //        r^2 = 1.0/(1.0+u*u+v*v);
	     //       area = 4.0  (i.e. -1..1 * -1..1)
	     //    samples = width * height
	     // DiffSolidAngle: cos(theta)/r^2  *  area / samples

	     dsangle = 4.0 / ((1.0+u*u+v*v)*sqrt(1.0+u*u+v*v) * (img_width*img_height));
	   }

	   // get value
	   assert( imfg[s]->getPixelFormat()==Image::OSG_RGB_PF );
	   Vec3f val = Vec3f( float(imfg[s]->getData()[3*(xx+img_width*yy)+0]/255.),
			      float(imfg[s]->getData()[3*(xx+img_width*yy)+1]/255.),
			      float(imfg[s]->getData()[3*(xx+img_width*yy)+2]/255.));

	   // add sample to spherical harmonics approximation
 	   //---------------------------------------------------------
           sh.add( dir, val * dsangle );
         }
       }
    }

    Vec3f *Clm_local = sh.getCoefficients();

    for(int i = 0; i < 25; i++)
    {
       Clm[i] = Clm_local[i];
    }
}


/*-------------------------------------------------------------------------*\
 -  private                                                                -
\*-------------------------------------------------------------------------*/

void ReflectiveMaterial::initMethod (void)
{
}


/***************************************************************************\
 *                           Instance methods                              *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  private                                                                 -
\*-------------------------------------------------------------------------*/

/*----------------------- constructors & destructors ----------------------*/

ReflectiveMaterial::ReflectiveMaterial(void) :
    Inherited(),
    _materialChunk(NullFC),
    _textureChunkDiffuse  (NullFC),
    _cubeTextureChunk (NullFC),
    _vpChunk      (NullFC),
    _rcChunk      (NullFC)
{
}

ReflectiveMaterial::ReflectiveMaterial(const ReflectiveMaterial &source) :
    Inherited(source),
    _materialChunk(source._materialChunk),
    _textureChunkDiffuse  (source._textureChunkDiffuse ),
    _cubeTextureChunk  (source._cubeTextureChunk ),
    _vpChunk      (source._vpChunk      ),
    _rcChunk      (source._rcChunk      )
{
}

ReflectiveMaterial::~ReflectiveMaterial(void)
{
    subRefCP(_materialChunk);
    subRefCP(_textureChunkDiffuse );
    subRefCP(_cubeTextureChunk );
    subRefCP(_vpChunk      );
    subRefCP(_rcChunk      );
}

/*----------------------------- class specific ----------------------------*/

void ReflectiveMaterial::changed(BitVector whichField, UInt32 origin)
{
    Inherited::changed(whichField, origin);
}


// Combine all parts together (material, texture, Vertex and Fragment Programs)
//===============================================================================
StatePtr ReflectiveMaterial::makeState(void)
{
    StatePtr state = State::create();

    Color3f v3;
    Color4f v4;
    Color3f Fresnel;
    Vec3f LightPos, ViewPos;
    float alpha = 1.f - getTransparency();

    prepareLocalChunks();

    beginEditCP(_materialChunk);
       v4.setValuesRGBA(0, 0, 0, alpha);
      _materialChunk->setAmbient(v4);
      _materialChunk->setDiffuse(v4);
      _materialChunk->setEmission(v4);
       v4.setValuesRGBA(1, 1, 1, alpha);
      _materialChunk->setSpecular(v4);
      _materialChunk->setShininess( 1 );
      _materialChunk->setLit(false);
      _materialChunk->setColorMaterial(GL_DIFFUSE);
    endEditCP  (_materialChunk);

    state->addChunk(_materialChunk);
    state->addChunk(_textureChunkDiffuse);

    // Take some input variables and push it to the vertex program
    //---------------------------------------------------------
    Fresnel = getFresnel();
    LightPos = getLightPos();
    ViewPos = getViewPos();

    state->addChunk(_cubeTextureChunk);

    // Set changable parameters for vertex program
    //---------------------------------------------------------
    beginEditCP(_vpChunk);
      _vpChunk->setParameter(13, Vec4f( Fresnel[0], Fresnel[1], Fresnel[2], 0.0) );
      _vpChunk->setParameter(14, Vec4f(1.-Fresnel[0],1.-Fresnel[1],1.-Fresnel[2], 1.0) );
      _vpChunk->setParameter(15, Vec4f(LightPos[0],LightPos[1],LightPos[2],1.0));
      _vpChunk->setParameter(16, Vec4f(ViewPos[0],ViewPos[1],ViewPos[2],1.0));
    endEditCP(_vpChunk);

    state->addChunk(_vpChunk);

    // Assemble all the parts of Material together
    //---------------------------------------------------------
    beginEditCP(_rcChunk);
    {
      _rcChunk->clearCombiners();

      // General combiner
      // A*B = texDiffuse * sphericalHarmonics (for each [r,g,b])
      // C*D = texCubeMap * (1-Fresnel)        (for each [r,g,b])
      // SPARE0 = A*B, SPARE1 = C*D
      //-----------------------------------------------------------------------
      _rcChunk->setCombinerRGB( 0,
         GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB,        // variable A
         GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,    // variable B
         GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB,        // variable C
         GL_SECONDARY_COLOR_NV, GL_UNSIGNED_INVERT_NV, GL_RGB,    // variable D
         GL_SPARE0_NV, GL_SPARE1_NV, GL_DISCARD_NV,     // ABout, CDout, Sumout
         GL_NONE, GL_NONE,                              // scale, bias
         GL_FALSE, GL_FALSE, GL_FALSE );                // ABdot, CDdot, muxSum

      // Final combiner
      // compute A*B + (1-A)*C + D
      //  A*B = texDiffuse * sphericalHarmonics * Fresnel (for each [r,g,b])
      //  D =  texCubeMap * (1-Fresnel) * 1.0             (for each [r,g,b])
      //  result = texDiffuse * sphericalHarmonics * Fresnel
      //           + texCubeMap * (1-Fresnel)
      //-----------------------------------------------------------------------
      _rcChunk->setFinalCombiner(
         GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,            // variable A
         GL_SECONDARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,   // variable B
         GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB,                 // variable C
         GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,            // variable D
         GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB,                 // variable E
         GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB,                 // variable F
         GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA );             // variable G
        }
      endEditCP(_rcChunk);

      state->addChunk(_rcChunk);

    for(MFStateChunkPtr::iterator i  = _mfChunks.begin();
                                  i != _mfChunks.end();
                                ++i)
    {
        state->addChunk(*i);
    }

    return state;
}



// Combine all parts together (material, texture, Vertex and Fragment Programs)
//===============================================================================
void ReflectiveMaterial::rebuildState(void)
{
    Color3f v3;
    Color4f v4;
    Color3f Fresnel;
    Vec3f LightPos, ViewPos;
    float alpha = 1.f - getTransparency();

    if(_pState != NullFC)
    {
        _pState->clearChunks();
    }
    else
    {
        _pState = State::create();
        addRefCP(_pState);
    }

    prepareLocalChunks();


    beginEditCP(_materialChunk);
       v4.setValuesRGBA(0, 0, 0, alpha);
      _materialChunk->setAmbient(v4);
      _materialChunk->setDiffuse(v4);
      _materialChunk->setEmission(v4);
       v4.setValuesRGBA(1, 1, 1, alpha);
      _materialChunk->setSpecular(v4);
      _materialChunk->setShininess( 1 );
      _materialChunk->setLit(false);
      _materialChunk->setColorMaterial(GL_DIFFUSE);
    endEditCP  (_materialChunk);

    _pState->addChunk(_materialChunk);
    _pState->addChunk(_textureChunkDiffuse);

    // Take some input variables and push it to the vertex program
    //---------------------------------------------------------
    Fresnel = getFresnel();
    LightPos = getLightPos();
    ViewPos = getViewPos();

    _pState->addChunk(_cubeTextureChunk);

    // Set changable parameters for vertex program
    //---------------------------------------------------------
    beginEditCP(_vpChunk);
      _vpChunk->setParameter(13, Vec4f( Fresnel[0], Fresnel[1], Fresnel[2], 0.0) );
      _vpChunk->setParameter(14, Vec4f(1.-Fresnel[0],1.-Fresnel[1],1.-Fresnel[2], 1.0) );
      _vpChunk->setParameter(15, Vec4f(LightPos[0],LightPos[1],LightPos[2],1.0));
      _vpChunk->setParameter(16, Vec4f(ViewPos[0],ViewPos[1],ViewPos[2],1.0));
    endEditCP(_vpChunk);

    _pState->addChunk(_vpChunk);


    // Assemble all the parts of Material together
    //---------------------------------------------------------
    beginEditCP(_rcChunk);
    {
      _rcChunk->clearCombiners();

      // General combiner
      // A*B = texDiffuse * sphericalHarmonics (for each [r,g,b])
      // C*D = texCubeMap * (1-Fresnel) (for each [r,g,b])
      // SPARE0 = A*B, SPARE1 = C*D
      //-----------------------------------------------------------------------
      _rcChunk->setCombinerRGB( 0,
         GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB,        // variable A
         GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,    // variable B
         GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB,        // variable C
         GL_SECONDARY_COLOR_NV, GL_UNSIGNED_INVERT_NV, GL_RGB,    // variable D
         GL_SPARE0_NV, GL_SPARE1_NV, GL_DISCARD_NV,     // ABout, CDout, Sumout
         GL_NONE, GL_NONE,                              // scale, bias
         GL_FALSE, GL_FALSE, GL_FALSE );                // ABdot, CDdot, muxSum

      // Final combiner
      // compute A*B + (1-A)*C + D
      //  A*B = texDiffuse * sphericalHarmonics * Fresnel(for each [r,g,b])
      //  D =  texCubeMap * (1-Fresnel) * 1.0                     (for each [r,g,b])
      //  result = texDiffuse * sphericalHarmonics * Fresnel
      //           + texCubeMap * (1-Fresnel)
      //-----------------------------------------------------------------------
      _rcChunk->setFinalCombiner(
         GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,           // variable A
         GL_SECONDARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,  // variable B
         GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB,                // variable C
         GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,           // variable D
         GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB,                // variable E
         GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB,                // variable F
         GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA );            // variable G
    }
    endEditCP(_rcChunk);

    _pState->addChunk(_rcChunk);


    for(MFStateChunkPtr::iterator i  = _mfChunks.begin();
                                  i != _mfChunks.end();
                                ++i)
    {
        _pState->addChunk(*i);
    }

}

void ReflectiveMaterial::dump(      UInt32    ,
                         const BitVector ) const
{
    SLOG << "Dump ReflectiveMaterial NI" << std::endl;
}


/*------------------------------------------------------------------------*/
/*                              cvs id's                                  */

#ifdef OSG_SGI_CC
#pragma set woff 1174
#endif

#ifdef OSG_LINUX_ICC
#pragma warning( disable : 177 )
#endif

namespace
{
    static Char8 cvsid_cpp       [] = "@(#)$Id: OSGReflectiveMaterial.cpp,v 1.1 2003/11/03 19:13:00 jnkautz Exp $";
    static Char8 cvsid_hpp       [] = OSGREFLECTIVEMATERIALBASE_HEADER_CVSID;
    static Char8 cvsid_inl       [] = OSGREFLECTIVEMATERIALBASE_INLINE_CVSID;

    static Char8 cvsid_fields_hpp[] = OSGREFLECTIVEMATERIALFIELDS_HEADER_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

