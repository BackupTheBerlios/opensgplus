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

//---------------------------------------------------------------------------
//  Includes
//---------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>

#define OSG_COMPILESYSTEMLIB

#include <OSGConfig.h>

#include <OSGGL.h>

#include <OSGAction.h>
#include <OSGDrawAction.h>
#include <OSGGeometry.h>

#include <OSGStateChunk.h>
#include <OSGState.h>
#include <OSGMaterialChunk.h>
#include <OSGImage.h>
#include <OSGTextureChunk.h>
#include <OSGVertexProgramChunk.h>
#include <OSGFragmentProgramChunk.h>
#include <OSGRegisterCombinersChunk.h>
#include <iostream.h>

#include "OSGLafortuneMaterial.h"

OSG_USING_NAMESPACE

/*! \class osg::LafortuneMaterial
Material with using of Lafortune reflectance function. BRDF is computed as follows: f_brdf = Ks*(Cx*(ux*vx+uy*vy)+Cz*uz*vz); and the color at the end is: c = tex*(rho + f_brdf) or c = tex*pho + f_brdf or c = pho + f_brdf
*/

/***************************************************************************\
 *                           Class variables                               *
\***************************************************************************/

/***************************************************************************\
 *                           Class methods                                 *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  protected                                                              -
\*-------------------------------------------------------------------------*/

/* Create the chunks needed by this Material: the chunk for the material properties,
   the chunk for diffuse, vertex program, fragment program/or register combiners
   chunks realize Lafortune lightning model.
*/
static std::string vp_str =
"!!ARBvp1.0\n"
"ATTRIB vpos = vertex.position;\n"
"ATTRIB vnorm = vertex.normal;\n"
"ATTRIB texcoord = vertex.texcoord[0];\n"
"PARAM mvp[4] = { state.matrix.mvp };\n"
"PARAM mv[4] = { state.matrix.modelview };\n"
"PARAM mvinvt[4] = { state.matrix.modelview.invtrans };\n"
"PARAM yaxis = {0.0,1.0,0.0,1.0};\n"
"PARAM iyaxis = {0.0,-1.0,0.0,1.0};\n"
"PARAM zero = {0.0,0.0,0.0,0.0};\n"
"PARAM one = {1.0,1.0,1.0,1.0};\n"
"PARAM ln = {0.0,0.0,1.0,1.0};\n"
"PARAM small = {0.0000000001,0.0000000001,0.0000000001,1.0};\n"
"PARAM small2 = {0.001,0.001,0.001,1.0};\n"
"PARAM big = {10000.0,10000.0,10000.0,1.0};\n"
"PARAM half = {0.5,0.5,0.5,1.0};\n"
//"# outside parametrs:\n"
//"#---------------------------------------------\n"
//"# for some reasons VP doesn't see the light and so we need to add\n"
//"# this parameter as local. And specular coeffitient too.\n"
"PARAM lightpos = program.local[0];\n"
"PARAM Cx = program.local[1];\n" //#{-0.704955,-0.825985,-0.711507,10.7439};
"PARAM Cz = program.local[2];\n" //#{0.881194,0.683593,0.697774,10.7439};
"PARAM Ks = program.local[3];\n" //#{0.881194,0.683593,0.697774,1.0};
"PARAM CompTexCoord = program.local[4];\n"
"PARAM halfObjBBox = program.local[5];\n"
//
"TEMP vtrafo;\n"
"TEMP ntrafo;\n"
"TEMP tmp1;\n"
"TEMP tmp2;\n"
"TEMP tmp3;\n"
"TEMP tmp4;\n"
"TEMP tmp5;\n"
"TEMP lv;\n"
"TEMP ll;\n"
"TEMP lh;\n"
"TEMP tmp6;\n"
//"# computing of the transformed vertex coordinates\n"
//"#----------------------------------------------\n"
"DP3 vtrafo.x, mvinvt[0], vpos;\n"
"DP3 vtrafo.y, mvinvt[1], vpos;\n"
"DP3 vtrafo.z, mvinvt[2], vpos;\n"
"MOV vtrafo.w, one.w;\n"
//"# computing of the transformed normals\n"
//"#----------------------------------------------\n"
"DP3 ntrafo.x, mvinvt[0], vnorm;\n"
"DP3 ntrafo.y, mvinvt[1], vnorm;\n"
"DP3 ntrafo.z, mvinvt[2], vnorm;\n"
"MOV ntrafo.w, one.w;\n"
//"# computing the tangent\n"
//"#----------------------------------------------\n"
"XPD tmp5, ntrafo, yaxis;\n"
"DP3 tmp2.w, tmp5, tmp5;\n"
"RSQ tmp2.w, tmp2.w;\n"
"MUL tmp5.xyz, tmp2.w, tmp5;\n"
"MOV tmp5.w, one.x;\n"           //# tmp5 = binormal = normalize(Cross(normal,yaxis))
"XPD tmp3, tmp5, ntrafo;\n"
"DP3 tmp2.w, tmp3, tmp3;\n"
"RSQ tmp2.w, tmp2.w;\n"
"MUL tmp3.xyz, tmp2.w, tmp3;\n"
"MOV tmp3.w, one.x;\n"           //# tangent1 = tangent = normalize(Cross(binormal,normal))
//"# checking for isZero( (1. - normal.dot( x_axis )) )\n"
//"# (But with this checking we receive flikering during\n"
//"# the object moving)\n"
"DP3 tmp2.x, ntrafo, yaxis;\n"    //# a = normal.dot( y_axis )
"DP3 tmp2.y, ntrafo, iyaxis;\n"   //# b = normal.dot( -y_axis )
"SUB tmp2.x, one.x, tmp2.x;\n"    //# a = 1 - a
"SUB tmp2.y, one.x, tmp2.y;\n"    //# b = 1 - b
"MOV tmp4.x, iyaxis.y;\n"
"MOV tmp4.y, yaxis.y;\n"          //# tmp4: -1 1 .. ..
"SGE tmp1.x, tmp2.y, tmp2.x;\n"   //# b >= a => tmp1.x = 1.0
"SLT tmp1.y, tmp2.y, tmp2.x;\n"   //# b < a  => tmp1.y = 1.0
"MUL tmp4.x, tmp4.x, tmp1.x;\n"
"MUL tmp4.y, tmp4.y, tmp1.y;\n"
"ADD tmp1.z, tmp4.x, tmp4.y;\n"   //# tmp1 => t2 = tangent = (0 0 1)/(0 0 -1)
"MOV tmp1.x, zero.x;\n"
"MOV tmp1.y, zero.y;\n"
"MOV tmp1.w, one.w;\n"
"MIN tmp2.z, tmp2.x, tmp2.y;\n"   //# min(a,b)
"SGE tmp2.x, tmp2.z, small2.x;\n"
"SLT tmp2.y, tmp2.z, small2.x;\n"
"MUL tmp4, tmp2.y, tmp1;\n"       //# tangent with sign
"MUL tmp3, tmp2.x, tmp3;\n"       //# if(a < 0.001 || b < 0.001) t1 = 0
"ADD tmp3, tmp3, tmp4;\n"         //# tmp3 = tangent = tmp4/tangent1
//"# computing binormal, light and eye vectors for local coordinate system\n"
//"#=================================================================\n"
"XPD tmp5, ntrafo, tmp3;\n"       //# tmp5 = binormal
"SUB tmp4, mvinvt[3], vtrafo;\n"
"DP3 tmp2.w, tmp4, tmp4;\n"
"RSQ tmp2.w, tmp2.w;\n"
"MUL tmp4.xyz, tmp2.w, tmp4;\n"
"MOV tmp4.w, one.x;\n"            //# tmp4 = normalize vector eye
//"# computing the light direction\n"
//"#----------------------------------------------\n"
"SUB tmp6, lightpos, vtrafo;\n"
"DP3 tmp2.w, tmp6, tmp6;\n"
"RSQ tmp2.w, tmp2.w;\n"
"MUL tmp6.xyz, tmp2.w, tmp6;\n"
"MOV tmp6.w, one.x;\n"                   //# normalize vector tmp6
//"# computing parameters for local coordinat system\n"
//"#----------------------------------------------\n"
"DP3 lv.x, tmp4, tmp3;\n"                //# local eye vector
"DP3 lv.y, tmp4, tmp5;\n"
"DP3 lv.z, tmp4, ntrafo;\n"
"DP3 ll.x, tmp6, tmp3;\n"                //# local light vector
"DP3 ll.y, tmp6, tmp5;\n"
"DP3 ll.z, tmp6, ntrafo;\n"
//"# Computing LdotN\n"
//"#----------------------------------------------\n"
"DP3 tmp3.z, ll, ln;\n"        //# LdotN
// # Compute or use diffuse texture coordinates
"RCP tmp6.x, halfObjBBox.x;\n"
"MUL tmp6, vpos, tmp6.x;\n"
"MAD tmp6, half.x, tmp6, half.x;\n"
"MOV tmp6.w, one.x;\n"
"MAD tmp5, -CompTexCoord.x, texcoord, texcoord;\n"
"MAD tmp6, CompTexCoord.x, tmp6, tmp5;\n"
"MOV result.texcoord[0], tmp6;\n" //texcoord;\n"       //# pass thorugh texcoords[0] (diffuse part)
//"# computing the orthogonal projection\n"
//"#----------------------------------------------\n"
"DP4 vtrafo.x, mvp[0], vpos;\n"
"DP4 vtrafo.y, mvp[1], vpos;\n"
"DP4 vtrafo.z, mvp[2], vpos;\n"
"DP4 vtrafo.w, mvp[3], vpos;\n"
"MOV result.position, vtrafo;\n"
//"# compute highlight: (Cx*(v.x*l.x + v.y*l.y) + (Cz * v.z * l.z))^N\n"
//"#------------------------------------------------------------------\n"
"MUL tmp1.y, lv.y, ll.y;\n"
"MAD tmp1, lv.x, ll.x, tmp1.y;\n"
"MUL tmp2, lv.z, ll.z;\n"
"MUL tmp2, Cz, tmp2;\n"
"MAD tmp1, tmp1, Cx, tmp2;\n"
"ABS tmp1, tmp1;\n"
"POW tmp1.x, tmp1.x, lightpos.w;\n"
"POW tmp1.y, tmp1.y, Cx.w;\n"
"POW tmp1.z, tmp1.z, Cz.w;\n"
//
"MUL result.color.primary, tmp1, Ks;\n"     //# send specularColor (absolute value) to FP
"MOV result.color.primary.w, tmp3.z;\n"     //# send LdotN as primary.color aplha-channel to FP
"SLT result.color.secondary.x, tmp1.x, zero.x;\n"     //# push 1 to FP if specularColor.R < 0
"SLT result.color.secondary.y, tmp1.y, zero.x;\n"     //# push 1 to FP if specularColor.R < 0
"SLT result.color.secondary.z, tmp1.z, zero.x;\n"     //# push 1 to FP if specularColor.R < 0
"END\n";




static std::string fp_str =
"!!ARBfp1.0\n"
//"# what this does:\n"
//"# * add the Lafortune highlight to the texture or to the simple diffuse\n"
//"#   material:\n"
//"#      Highlight = rs * (C_x*lx*vx + C_y*ly*vy + C_z*lz*vz)^N\n"
//"#                  |  Cx = Cy => |\n"
//"#                = rs * (C_x*(lx*vx + ly*vy) + C_z*lz*vz)^N\n"
//"#\n"
//"#   (l and v here - are the local light and local view vectors)\n"
//"#\n"
//"# if the useweight = 1 => apply the texturing otherwise use diffuse color\n"
//"#    if selector = 0 multiply only the diffuse part by the texture,\n"
//"#    else multiply both highlight and diifuse parts by the texture\n"
//"# Note, that the result should be multiplied by cos(l,n)\n"
"PARAM ambient = program.local[0];\n"     //# ambient color
"PARAM diffuse = program.local[1];\n"     //# diffuse color
"PARAM rhoSelectKa = program.local[2];\n" //# coefficients for diffuse and specular
//"                                         # terms, selector of texture weighting
"PARAM useweight = program.local[3];\n"   //# Use the texture for weighting?
"PARAM const = {1.0, 0.0, 0.0, 0.0};\n"
"PARAM zero = {0.0, 0.0, 0.0, 0.0};\n"
"PARAM one = {1.0, 1.0, 1.0, 1.0};\n"
"TEMP tmp;\n"
"TEMP tmp2;\n"
"TEMP tmp3;\n"
"TEMP weight;\n"
"TEMP weight2;\n"
"TEMP iselector;\n"  //## inverse
"TEMP iuseweight;\n" //## inverse
//"# ------------------------------------------------------------\n"
//"# weight with texture\n"
//"# if useweight = 0 => don't use the texture\n"
//"# else\n"
//"#     if selector = 0 => tex * rho + (Cx*(ux*vx+uy*vy)+Cz*uz*vz)^N\n"
//"#     else => tex*rho + tex*((Cx*(ux*vx+uy*vy)+Cz*uz*vz)^N)\n"
"TEX weight, fragment.texcoord[0], texture[0], 2D;\n"
"MOV tmp3, fragment.color.primary;\n"          // # Take specularColor (absolute value)
// # Check the sign of specularColor
//-------------------------------------------------
"SUB tmp2, one, fragment.color.secondary;\n"
"MAD tmp2, fragment.color.secondary, -const.x, tmp2;\n"
"MOV tmp2.w, const.x;\n"
"MUL tmp3, tmp3, tmp2;\n"
"MOV_SAT tmp2, fragment.color.primary.w;\n"    // # Take correct cosine
// # Use/not diffuse texture, check selector
//-------------------------------------------------
"SUB iuseweight, one, useweight;\n"
"MAD weight, weight, useweight, iuseweight;\n"
"SUB iselector, one, rhoSelectKa.y;\n"
"MAD weight2, weight,  rhoSelectKa.y, iselector;\n"
"MUL tmp, weight, diffuse;\n"
"MUL tmp, tmp, rhoSelectKa.x;\n"
"MAD tmp, weight2, tmp3, tmp;\n"
"MUL tmp, tmp, tmp2;\n"

// Add ambient term
"MAD result.color, rhoSelectKa.z, ambient, tmp;\n"
"END\n";



void LafortuneMaterial::prepareLocalChunks(void)
{
    if(_materialChunk == NullFC)
    {
        _materialChunk = MaterialChunk::create();
        addRefCP(_materialChunk);

        Color4f v4;
        float alpha = 1.f - getTransparency();
	beginEditCP(_materialChunk);
	   v4.setValuesRGBA(0, 0, 0, alpha);
          _materialChunk->setAmbient(v4);
          _materialChunk->setDiffuse(v4);
          _materialChunk->setEmission(v4);
           v4.setValuesRGBA(1, 1, 1, alpha);
          _materialChunk->setSpecular(v4);
          _materialChunk->setShininess( 100 );
          _materialChunk->setLit(false);
          _materialChunk->setColorMaterial(GL_DIFFUSE);
        endEditCP  (_materialChunk);
    }

    if(_textureChunk == NullFC)
    {
        _textureChunk = TextureChunk::create();
        addRefCP(_textureChunk);

        ImagePtr imgt;

        if(getTextureImage() == NullFC)
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
          imgt = getTextureImage();
        }

        beginEditCP(_textureChunk );
          _textureChunk->setImage( imgt );
          _textureChunk->setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
          _textureChunk->setMagFilter(GL_LINEAR);
          _textureChunk->setWrapS(GL_REPEAT);
          _textureChunk->setWrapT(GL_REPEAT);
          _textureChunk->setEnvMode(GL_MODULATE);
        endEditCP(_textureChunk);
    }

    if(_vpChunk == NullFC)
    {
        _vpChunk = VertexProgramChunk::create();
        addRefCP(_vpChunk);

        Pnt3f LightPos = getLightPos();
        Color3f Cxx = getCx();
        Color3f Czz = getCz();
        Color3f Nn = getN();
        Real32 Ks = getKs();
        bool compTexCoords = getCompTexCoord();
	Real32 halfBBoxObj = getHalfBBoxObj();

        beginEditCP(_vpChunk);
          _vpChunk->setProgram(vp_str);
          _vpChunk->addParameter("lpos", 0, Vec4f(LightPos[0], LightPos[1], LightPos[2], Nn[0]) );
          _vpChunk->addParameter("cx", 1, Vec4f(Cxx[0], Cxx[1], Cxx[2], Nn[1]) );
          _vpChunk->addParameter("cz", 2, Vec4f(Czz[0], Czz[1], Czz[2], Nn[2]) );
          _vpChunk->addParameter("Ks", 3, Vec4f(Ks, Ks, Ks, 1.0));
          _vpChunk->addParameter("CompTexCoord", 4, Vec4f(compTexCoords, compTexCoords, compTexCoords, 1.0));
          _vpChunk->addParameter("HalfBBoxObj", 5, Vec4f(halfBBoxObj, halfBBoxObj, halfBBoxObj, 1.0));
        endEditCP(_vpChunk);
    }

    if(_fpChunk == NullFC)
    {
        _fpChunk = FragmentProgramChunk::create();
        addRefCP(_fpChunk);

        Real32 UseFPorRC = getUseFPorRC();

        if(UseFPorRC < 1)
        {
           Color3f ambient, diffuse;
           Real32 rho_d, Ka, selector;
           bool useWeight;

           ambient = getAmbient();
           diffuse = getDiffuse();
           Ka = getKa();
           rho_d = getRho_d();
           selector = getSelector();
           useWeight = getUseweight();
           Vec4f uw = (useWeight==true) ?
             Vec4f(1.0, 1.0, 1.0, 1.0 ) : Vec4f(0.0, 0.0, 0.0, 0.0 );

            beginEditCP(_fpChunk);
              _fpChunk->setProgram(fp_str);
              _fpChunk->addParameter("ambient", 0, Vec4f(ambient[0], ambient[1], ambient[2], 1.0) );
              _fpChunk->addParameter("diffuse", 1, Vec4f(diffuse[0], diffuse[1], diffuse[2], 1.0) );
              _fpChunk->addParameter("rhoSelectorKa", 2, Vec4f(rho_d, selector, Ka, 1.0) );
              _fpChunk->addParameter("useweight", 3, uw);
            endEditCP(_fpChunk);
        }
    }

    if(_rcChunk == NullFC)
    {
        _rcChunk = RegisterCombinersChunk::create();
        addRefCP(_rcChunk);
    }
}

/*-------------------------------------------------------------------------*\
 -  private                                                                -
\*-------------------------------------------------------------------------*/

void LafortuneMaterial::initMethod (void)
{
}


/***************************************************************************\
 *                           Instance methods                              *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  private                                                                 -
\*-------------------------------------------------------------------------*/

/*----------------------- constructors & destructors ----------------------*/

LafortuneMaterial::LafortuneMaterial(void) :
    Inherited(),
    _materialChunk(NullFC),
    _textureChunk  (NullFC),
    _vpChunk      (NullFC),
    _fpChunk      (NullFC),
    _rcChunk      (NullFC)
{
}

LafortuneMaterial::LafortuneMaterial(const LafortuneMaterial &source) :
    Inherited(source),
    _materialChunk(source._materialChunk),
    _textureChunk (source._textureChunk ),
    _vpChunk      (source._vpChunk      ),
    _fpChunk      (source._fpChunk      ),
    _rcChunk      (source._rcChunk      )
{
}

LafortuneMaterial::~LafortuneMaterial(void)
{
    subRefCP(_materialChunk);
    subRefCP(_textureChunk );
    subRefCP(_vpChunk      );
    subRefCP(_fpChunk      );
    subRefCP(_rcChunk      );
}

/*----------------------------- class specific ----------------------------*/

void LafortuneMaterial::changed(BitVector whichField, UInt32 origin)
{
    Inherited::changed(whichField, origin);
}

/*-------------------------- your_category---------------------------------*/

// Combine all parts together (material, texture, Vertex and Fragment Programs/Register Combiners)
StatePtr LafortuneMaterial::makeState(void)
{
    StatePtr state = State::create();

    Color4f RDiffuse(0,0,0,1), RAmbient(0,0,0,1);
    Color3f Cxx, Czz, ambient, diffuse, Nn;
    Real32 rho_d, Ks, Ka;
    Real32 selector;
    bool useWeight;
    Pnt3f LightPos;
    Color4f v4;
    float alpha = 1.f - getTransparency();
    Real32 UseFPorRC = getUseFPorRC();

    prepareLocalChunks();

    // Initialize the main material chunk
    //---------------------------------------------------
    beginEditCP(_materialChunk);
    {
         v4.setValuesRGBA(0, 0, 0, alpha);
        _materialChunk->setAmbient(v4);
        _materialChunk->setDiffuse(v4);
        _materialChunk->setEmission(v4);
         v4.setValuesRGBA(1, 1, 1, alpha);
        _materialChunk->setSpecular(v4);
        _materialChunk->setShininess( 1 );

        _materialChunk->setLit(false);
        _materialChunk->setColorMaterial(GL_DIFFUSE);
    }
    endEditCP  (_materialChunk);

    state->addChunk(_materialChunk);

    // Initialize the texture chunk
    //---------------------------------------------------
    beginEditCP(_textureChunk);
    _textureChunk->setImage( getTextureImage() );
    _textureChunk->setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
    _textureChunk->setMagFilter(GL_LINEAR);
    _textureChunk->setWrapS(GL_REPEAT);
    _textureChunk->setWrapT(GL_REPEAT);
    _textureChunk->setEnvMode(GL_MODULATE);
    endEditCP(_textureChunk);

    state->addChunk(_textureChunk);

    // Take some input variables and push it to the vertex program
    //-------------------------------------------------------------
    LightPos = getLightPos();
    Cxx = getCx();
    Czz = getCz();
    Nn = getN();
    Ks = getKs();
    short int tt = 0;
    bool compTexCoords = getCompTexCoord();
    Real32 halfBBoxObj = getHalfBBoxObj();

    // Change parameters in the Vertex program
    //---------------------------------------------------
    beginEditCP(_vpChunk);
      _vpChunk->setParameter(tt, Vec4f(LightPos[0], LightPos[1], LightPos[2], Nn[0]) );
      _vpChunk->setParameter(1, Vec4f(Cxx[0], Cxx[1], Cxx[2], Nn[1]) );
      _vpChunk->setParameter(2, Vec4f(Czz[0], Czz[1], Czz[2], Nn[2]) );
      _vpChunk->setParameter(3, Vec4f(Ks, Ks, Ks, 1.0) );
      _vpChunk->setParameter(4, Vec4f(compTexCoords, compTexCoords, compTexCoords, 1.0));
      _vpChunk->setParameter(5, Vec4f(halfBBoxObj, halfBBoxObj, halfBBoxObj, 1.0));
   endEditCP(_vpChunk);

    state->addChunk(_vpChunk);

    // Take some input variables and push it to the fragment program
    //---------------------------------------------------------------
    diffuse = getDiffuse();
    ambient = getAmbient();
    selector = getSelector();
    Ka = getKa();
    rho_d = getRho_d();
    useWeight = getUseweight();
    Vec4f uw = (useWeight==true) ?
       Vec4f(1.0, 1.0, 1.0, 1.0 ) : Vec4f(0.0, 0.0, 0.0, 0.0 );

    if(UseFPorRC < 0)
    {
      // Change parameters in the Fragment program
      //---------------------------------------------------
      beginEditCP(_fpChunk);
        _fpChunk->setParameter(tt, Vec4f(ambient[0], ambient[1], ambient[2], 1.0) );
        _fpChunk->setParameter(1, Vec4f(diffuse[0], diffuse[1], diffuse[2], 1.0) );
        _fpChunk->setParameter(2, Vec4f(rho_d, selector, Ka, 1.0) );
        _fpChunk->setParameter(3, uw);
      endEditCP(_fpChunk);

      state->addChunk(_fpChunk);
    }
    else
    {
      // Note that this is always clamped to (1.0,1.0,1.0)!
      // But in fragment program it can be greater...
      RDiffuse[0] = rho_d * diffuse[0];
      RDiffuse[1] = rho_d * diffuse[1];
      RDiffuse[2] = rho_d * diffuse[2];

      RAmbient[0] = Ka * ambient[0];
      RAmbient[1] = Ka * ambient[1];
      RAmbient[2] = Ka * ambient[2];

      // Initialize the Register Combiners chunk
      //---------------------------------------------------
      beginEditCP(_rcChunk);
      {
        _rcChunk->clearCombiners();
        _rcChunk->setColor0(RDiffuse);

        if(useWeight)
        {
          if(selector < 1)
	  {
            // General combiner 0
            // A*B = (1.0 - [if(highlight<0)]) * 1.0              (for each [r,g,b])
            // C*D = -highlight * 1.0                             (for each [r,g,b])
            // SPARE0 = A*B + C*D           -> sign(highlight)    (for each [r,g,b])
            //-----------------------------------------------------------------------
            _rcChunk->setCombinerRGB( 0,
               GL_SECONDARY_COLOR_NV, GL_UNSIGNED_INVERT_NV, GL_RGB,   // variable A
               GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB,                 // variable B
               GL_SECONDARY_COLOR_NV, GL_SIGNED_NEGATE_NV, GL_RGB,     // variable C
               GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB,                 // variable D
               GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV,    // ABout, CDout, Sumout
               GL_NONE, GL_NONE,                              // scale, bias
               GL_FALSE, GL_FALSE, GL_FALSE );                // ABdot, CDdot, muxSum

	    // General combiner 0
            // A*B = texture * diffuse_color                       (for each [r,g,b])
            // C*D = highlight * sign                              (for each [r,g,b])
            // SPARE0 = A*B + C*D
            //-----------------------------------------------------------------------
           _rcChunk->setCombinerRGB( 1,
               GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB,        // variable A
               GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,  // variable B
               GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,    // variable C
               GL_SPARE0_NV, GL_SIGNED_IDENTITY_NV, GL_RGB,             // variable D
               GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV,    // ABout, CDout, Sumout
               GL_NONE, GL_NONE,                              // scale, bias
               GL_FALSE, GL_FALSE, GL_FALSE );                // ABdot, CDdot, muxSum

            // compute A*B + (1-A)*C + D
            // A*B = (texture*diffuse_color + signed_highlight)*cosine (for each [r,g,b])
            //  D = 0, C = 0                                           (for each [r,g,b])
            //  result = (texture*diffuse_color + signed_highlight)*cosine
            //-----------------------------------------------------------------------
            _rcChunk->setFinalCombiner(
               GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,           // variable A
               GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA,  // variable B
               GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB,                // variable C
               GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,  // variable D
               GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB,                // variable E
               GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB,                // variable F
               GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA );            // variable G
          }
	  else
	  {
            // General combiner 0
            // A*B = (1.0 - [if(highlight<0)]) * 1.0              (for each [r,g,b])
            // C*D = -highlight * 1.0                             (for each [r,g,b])
            // SPARE0 = A*B + C*D           -> sign(highlight)    (for each [r,g,b])
            //-----------------------------------------------------------------------
            _rcChunk->setCombinerRGB( 0,
               GL_SECONDARY_COLOR_NV, GL_UNSIGNED_INVERT_NV, GL_RGB,   // variable A
               GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB,                 // variable B
               GL_SECONDARY_COLOR_NV, GL_SIGNED_NEGATE_NV, GL_RGB,     // variable C
               GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB,                 // variable D
               GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV,    // ABout, CDout, Sumout
               GL_NONE, GL_NONE,                              // scale, bias
               GL_FALSE, GL_FALSE, GL_FALSE );                // ABdot, CDdot, muxSum

	    // General combiner 1
            // A*B = highlight * sigh                             (for each [r,g,b])
            // C*D = 0                                            (for each [r,g,b])
            // SPARE0 = A*B
            //-----------------------------------------------------------------------
            _rcChunk->setCombinerRGB( 1,
               GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,   // variable A
               GL_SPARE0_NV, GL_SIGNED_IDENTITY_NV, GL_RGB,            // variable B
               GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB,               // variable C
               GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB,               // variable D
               GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV,    // ABout, CDout, Sumout
               GL_NONE, GL_NONE,                              // scale, bias
               GL_FALSE, GL_FALSE, GL_FALSE );                // ABdot, CDdot, muxSum

	    // General combiner 2
            // A*B = texture * diffuse_color                     (for each [r,g,b])
            // C*D = texture * signed_highlight                  (for each [r,g,b])
            // SPARE0 = A*B + C*D
            //-----------------------------------------------------------------------
            _rcChunk->setCombinerRGB( 2,
               GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB,        // variable A
               GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,  // variable B
               GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB,        // variable C
               GL_SPARE0_NV, GL_SIGNED_IDENTITY_NV, GL_RGB,             // variable D
               GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV,    // ABout, CDout, Sumout
               GL_NONE, GL_NONE,                              // scale, bias
               GL_FALSE, GL_FALSE, GL_FALSE );                // ABdot, CDdot, muxSum

            // Final combiner
            // compute A*B + (1-A)*C + D
            //  A*B = (texture*(diffuse_color + signed_highlight))*cosine  (for each [r,g,b])
            //  D = 0, C = 0                                    (for each [r,g,b])
            //  result = (texture*(diffuse_color + signed_highlight))*cosine
            //-----------------------------------------------------------------------
            _rcChunk->setFinalCombiner(
               GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,           // variable A
               GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA,  // variable B
               GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB,                // variable C
               GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,  // variable D
               GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB,                // variable E
               GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB,                // variable F
               GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA );            // variable G
	  }
        }
        else
        {
            // General combiner 0
            // A*B = (1.0 - [if(highlight<0)]) * 1.0              (for each [r,g,b])
            // C*D = -highlight * 1.0                             (for each [r,g,b])
            // SPARE0 = A*B + C*D           -> sign(highlight)    (for each [r,g,b])
            //-----------------------------------------------------------------------
            _rcChunk->setCombinerRGB( 0,
               GL_SECONDARY_COLOR_NV, GL_UNSIGNED_INVERT_NV, GL_RGB,    // variable A
               GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB,                  // variable B
               GL_SECONDARY_COLOR_NV, GL_SIGNED_NEGATE_NV, GL_RGB,      // variable C
               GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB,                  // variable D
               GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV,    // ABout, CDout, Sumout
               GL_NONE, GL_NONE,                              // scale, bias
               GL_FALSE, GL_FALSE, GL_FALSE );                // ABdot, CDdot, muxSum

	    // General combiner 1
            // A*B = diffuse_color * 1.0                           (for each [r,g,b])
            // C*D = highlight * sign                              (for each [r,g,b])
            // SPARE0 = A*B + C*D
            //-----------------------------------------------------------------------
            _rcChunk->setCombinerRGB( 1,
               GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,  // variable A
               GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB,                  // variable B
               GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,    // variable C
               GL_SPARE0_NV, GL_SIGNED_IDENTITY_NV, GL_RGB,             // variable D
               GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV,    // ABout, CDout, Sumout
               GL_NONE, GL_NONE,                              // scale, bias
               GL_FALSE, GL_FALSE, GL_FALSE );                // ABdot, CDdot, muxSum


            // compute A*B + (1-A)*C + D
            // A*B = (diffuse_color + highlight)*cosine            (for each [r,g,b])
            //  D = 0, C = 0                                       (for each [r,g,b])
            //  result = (diffuse_color + highlight)*cosine
            //-----------------------------------------------------------------------
            _rcChunk->setFinalCombiner(
               GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,           // variable A
               GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA,  // variable B
               GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB,                // variable C
               GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,  // variable D
               GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB,                // variable E
               GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB,                // variable F
               GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA );            // variable G
	}
      }
      endEditCP(_rcChunk);

      state->addChunk(_rcChunk);
    }


    for(MFStateChunkPtr::iterator i  = _mfChunks.begin();
                                  i != _mfChunks.end();
                                ++i)
    {
        state->addChunk(*i);
    }

    return state;
}

// Combine all parts together (material, texture, Vertex and Fragment Programs)
void LafortuneMaterial::rebuildState(void)
{
    Color4f RDiffuse(0,0,0,1), RAmbient(0,0,0,1);
    Color3f Cxx, Czz, ambient, diffuse, Nn;
    Real32 Ks, rho_d, Ka;
    Real32 selector;
    bool useWeight;
    Pnt3f LightPos;
    Real32 UseFPorRC = getUseFPorRC();

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

    _pState->addChunk(_materialChunk);
    _pState->addChunk(_textureChunk);

    // Take some input variables and push it to the vertex program
    //-------------------------------------------------------------
    LightPos = getLightPos();
    Cxx = getCx();
    Czz = getCz();
    Nn = getN();
    Ks = getKs();
    short int tt = 0;
    bool compTexCoords = getCompTexCoord();
    Real32 halfBBoxObj = getHalfBBoxObj();

    // Change parameters in the Vertex program
    //---------------------------------------------------
    beginEditCP(_vpChunk);
      _vpChunk->setParameter(tt, Vec4f(LightPos[0], LightPos[1], LightPos[2], Nn[0]) );
      _vpChunk->setParameter(1, Vec4f(Cxx[0], Cxx[1], Cxx[2], Nn[1]) );
      _vpChunk->setParameter(2, Vec4f(Czz[0], Czz[1], Czz[2], Nn[2]) );
      _vpChunk->setParameter(3, Vec4f(Ks, Ks, Ks, 1.0));
      _vpChunk->setParameter(4, Vec4f(compTexCoords, compTexCoords, compTexCoords, 1.0));
      _vpChunk->setParameter(5, Vec4f(halfBBoxObj, halfBBoxObj, halfBBoxObj, 1.0));
    endEditCP(_vpChunk);

    _pState->addChunk(_vpChunk);

    // Take some input variables and push it to the fragment program
    //---------------------------------------------------------------
    ambient = getAmbient();
    diffuse = getDiffuse();
    Ka = getKa();
    rho_d = getRho_d();
    selector = getSelector();
    useWeight = getUseweight();
    Vec4f uw = (useWeight==true) ?
       Vec4f(1.0, 1.0, 1.0, 1.0 ) : Vec4f(0.0, 0.0, 0.0, 0.0 );

    if(UseFPorRC < 1)
    {
      // Change parameters in the Fragment program
      //---------------------------------------------------
      beginEditCP(_fpChunk);
        _fpChunk->setParameter(tt, Vec4f(ambient[0], ambient[1], ambient[2], 1.0) );
        _fpChunk->setParameter(1, Vec4f(diffuse[0], diffuse[1], diffuse[2], 1.0) );
        _fpChunk->setParameter(2, Vec4f(rho_d, selector, Ka, 1.0) );
        _fpChunk->setParameter(3, uw);
      endEditCP(_fpChunk);

      _pState->addChunk(_fpChunk);
    }
    else
    {
      RDiffuse[0] = rho_d * diffuse[0];
      RDiffuse[1] = rho_d * diffuse[1];
      RDiffuse[2] = rho_d * diffuse[2];

      RAmbient[0] = Ka * ambient[0];
      RAmbient[1] = Ka * ambient[1];
      RAmbient[2] = Ka * ambient[2];

      // Initialize the Register Combiners chunk
      //---------------------------------------------------
      beginEditCP(_rcChunk);
      {
        _rcChunk->clearCombiners();
        _rcChunk->setColor0(RDiffuse);
        _rcChunk->setColor1(RAmbient);

        if(useWeight)
        {
	  if(selector < 1)
	  {
            // General combiner 0
            // A*B = (1.0 - [if(highlight<0)]) * 1.0              (for each [r,g,b])
            // C*D = -highlight * 1.0                             (for each [r,g,b])
            // SPARE0 = A*B + C*D           -> sign(highlight)    (for each [r,g,b])
            //-----------------------------------------------------------------------
            _rcChunk->setCombinerRGB( 0,
               GL_SECONDARY_COLOR_NV, GL_UNSIGNED_INVERT_NV, GL_RGB,   // variable A
               GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB,                 // variable B
               GL_SECONDARY_COLOR_NV, GL_SIGNED_NEGATE_NV, GL_RGB,     // variable C
               GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB,                 // variable D
               GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV,    // ABout, CDout, Sumout
               GL_NONE, GL_NONE,                              // scale, bias
               GL_FALSE, GL_FALSE, GL_FALSE );                // ABdot, CDdot, muxSum

	    // General combiner 0
            // A*B = texture * diffuse_color                       (for each [r,g,b])
            // C*D = highlight * sign                              (for each [r,g,b])
            // SPARE0 = A*B + C*D
            //-----------------------------------------------------------------------
           _rcChunk->setCombinerRGB( 1,
               GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB,        // variable A
               GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,  // variable B
               GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,    // variable C
               GL_SPARE0_NV, GL_SIGNED_IDENTITY_NV, GL_RGB,             // variable D
               GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV,    // ABout, CDout, Sumout
               GL_NONE, GL_NONE,                              // scale, bias
               GL_FALSE, GL_FALSE, GL_FALSE );                // ABdot, CDdot, muxSum

            // compute A*B + (1-A)*C + D
            // A*B = (texture*diffuse_color + signed_highlight)*cosine (for each [r,g,b])
            //  D = 0, C = 0                                           (for each [r,g,b])
            //  result = (texture*diffuse_color + signed_highlight)*cosine
            //-----------------------------------------------------------------------
            _rcChunk->setFinalCombiner(
               GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,           // variable A
               GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA,  // variable B
               GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB,                // variable C
               GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,  // variable D
               GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB,                // variable E
               GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB,                // variable F
               GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA );            // variable G
          }
	  else
	  {
            // General combiner 0
            // A*B = (1.0 - [if(highlight<0)]) * 1.0              (for each [r,g,b])
            // C*D = -highlight * 1.0                             (for each [r,g,b])
            // SPARE0 = A*B + C*D           -> sign(highlight)    (for each [r,g,b])
            //-----------------------------------------------------------------------
            _rcChunk->setCombinerRGB( 0,
               GL_SECONDARY_COLOR_NV, GL_UNSIGNED_INVERT_NV, GL_RGB,   // variable A
               GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB,                 // variable B
               GL_SECONDARY_COLOR_NV, GL_SIGNED_NEGATE_NV, GL_RGB,     // variable C
               GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB,                 // variable D
               GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV,    // ABout, CDout, Sumout
               GL_NONE, GL_NONE,                              // scale, bias
               GL_FALSE, GL_FALSE, GL_FALSE );                // ABdot, CDdot, muxSum

	    // General combiner 1
            // A*B = highlight * sigh                             (for each [r,g,b])
            // C*D = 0                                            (for each [r,g,b])
            // SPARE0 = A*B
            //-----------------------------------------------------------------------
            _rcChunk->setCombinerRGB( 1,
               GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,   // variable A
               GL_SPARE0_NV, GL_SIGNED_IDENTITY_NV, GL_RGB,            // variable B
               GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB,               // variable C
               GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB,               // variable D
               GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV,    // ABout, CDout, Sumout
               GL_NONE, GL_NONE,                              // scale, bias
               GL_FALSE, GL_FALSE, GL_FALSE );                // ABdot, CDdot, muxSum

	    // General combiner 2
            // A*B = texture * diffuse_color                     (for each [r,g,b])
            // C*D = texture * signed_highlight                  (for each [r,g,b])
            // SPARE0 = A*B + C*D
            //-----------------------------------------------------------------------
            _rcChunk->setCombinerRGB( 2,
               GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB,        // variable A
               GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,  // variable B
               GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB,        // variable C
               GL_SPARE0_NV, GL_SIGNED_IDENTITY_NV, GL_RGB,             // variable D
               GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV,    // ABout, CDout, Sumout
               GL_NONE, GL_NONE,                              // scale, bias
               GL_FALSE, GL_FALSE, GL_FALSE );                // ABdot, CDdot, muxSum

            // Final combiner
            // compute A*B + (1-A)*C + D
            //  A*B = (texture*(diffuse_color + signed_highlight))*cosine  (for each [r,g,b])
            //  D = 0, C = 0                                    (for each [r,g,b])
            //  result = (texture*(diffuse_color + signed_highlight))*cosine
            //-----------------------------------------------------------------------
            _rcChunk->setFinalCombiner(
               GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,           // variable A
               GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA,  // variable B
               GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB,                // variable C
               GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,  // variable D
               GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB,                // variable E
               GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB,                // variable F
               GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA );            // variable G
	  }
        }
        else
        {
            // General combiner 0
            // A*B = (1.0 - [if(highlight<0)]) * 1.0              (for each [r,g,b])
            // C*D = -highlight * 1.0                             (for each [r,g,b])
            // SPARE0 = A*B + C*D           -> sign(highlight)    (for each [r,g,b])
            //-----------------------------------------------------------------------
            _rcChunk->setCombinerRGB( 0,
               GL_SECONDARY_COLOR_NV, GL_UNSIGNED_INVERT_NV, GL_RGB,    // variable A
               GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB,                  // variable B
               GL_SECONDARY_COLOR_NV, GL_SIGNED_NEGATE_NV, GL_RGB,      // variable C
               GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB,                  // variable D
               GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV,    // ABout, CDout, Sumout
               GL_NONE, GL_NONE,                              // scale, bias
               GL_FALSE, GL_FALSE, GL_FALSE );                // ABdot, CDdot, muxSum

	    // General combiner 1
            // A*B = diffuse_color * 1.0                           (for each [r,g,b])
            // C*D = highlight * sign                              (for each [r,g,b])
            // SPARE0 = A*B + C*D
            //-----------------------------------------------------------------------
            _rcChunk->setCombinerRGB( 1,
               GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,  // variable A
               GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB,                  // variable B
               GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,    // variable C
               GL_SPARE0_NV, GL_SIGNED_IDENTITY_NV, GL_RGB,             // variable D
               GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV,    // ABout, CDout, Sumout
               GL_NONE, GL_NONE,                              // scale, bias
               GL_FALSE, GL_FALSE, GL_FALSE );                // ABdot, CDdot, muxSum


            // compute A*B + (1-A)*C + D
            // A*B = (diffuse_color + highlight)*cosine            (for each [r,g,b])
            //  D = 0, C = 0                                       (for each [r,g,b])
            //  result = (diffuse_color + highlight)*cosine
            //-----------------------------------------------------------------------
            _rcChunk->setFinalCombiner(
               GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,           // variable A
               GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA,  // variable B
               GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB,                // variable C
               GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,  // variable D
               GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB,                // variable E
               GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB,                // variable F
               GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA );            // variable G
	}
      }
      endEditCP(_rcChunk);

      _pState->addChunk(_rcChunk);
    }

    MFStateChunkPtr::iterator it        = _mfChunks.begin();
    MFStateChunkPtr::iterator chunksEnd = _mfChunks.end();

    for(; it != chunksEnd; ++it)
    {
        _pState->addChunk(*it);
    }
}

bool LafortuneMaterial::isTransparent(void) const
{
    return ((getTransparency() > Eps) || (Inherited::isTransparent()));
}



/*------------------------------- dump ----------------------------------*/

void LafortuneMaterial::dump(      UInt32   uiIndent,
                         const BitVector OSG_CHECK_ARG(bvFlags )) const
{
//    SLOG << "Dump LafortuneMaterial NI" << std::endl;
    LafortuneMaterialPtr thisP(*this);
    thisP.dump(uiIndent, FCDumpFlags::RefCount);

    indentLog(uiIndent, PLOG);
    PLOG << "LafortuneMaterial at " << this << std::endl;

    indentLog(uiIndent, PLOG);
    PLOG << "\tambient: " << getAmbient() << std::endl;

    indentLog(uiIndent, PLOG);
    PLOG << "\tdiffuse: " << getDiffuse()  << std::endl;

    indentLog(uiIndent, PLOG);
    PLOG << "\temission: " << getEmission()  << std::endl;

    indentLog(uiIndent, PLOG);
    PLOG << "\tCx color: " << getCx()  << std::endl;

    indentLog(uiIndent, PLOG);
    PLOG << "\tCz color: " << getCz()  << std::endl;

    indentLog(uiIndent, PLOG);
    PLOG << "\tshininess N: " << getN()  << std::endl;

    indentLog(uiIndent, PLOG);
    PLOG << "\ttransparency: " << getTransparency()  << std::endl;

    indentLog(uiIndent, PLOG);
    PLOG << "\tChunks: " << std::endl;

    for(MFStateChunkPtr::const_iterator i = _mfChunks.begin();
            i != _mfChunks.end(); i++)
    {
        indentLog(uiIndent, PLOG);
        PLOG << "\t" << *i << std::endl;
    }

    indentLog(uiIndent, PLOG);
    PLOG << "LafortuneMaterial end " << this << std::endl;

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
    static Char8 cvsid_cpp       [] = "@(#)$Id: OSGLafortuneMaterial.cpp,v 1.1 2003/12/03 21:49:52 jnkautz Exp $";
    static Char8 cvsid_hpp       [] = OSGLAFORTUNEMATERIALBASE_HEADER_CVSID;
    static Char8 cvsid_inl       [] = OSGLAFORTUNEMATERIALBASE_INLINE_CVSID;

    static Char8 cvsid_fields_hpp[] = OSGLAFORTUNEMATERIALFIELDS_HEADER_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

