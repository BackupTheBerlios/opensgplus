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

#include "OSGNDFMaterial.h"

OSG_USING_NAMESPACE

/*! \class osg::NDFMaterial

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

/* Create the chunks needed by this Material, one for the material properties,
   one for the NDFcoloring (texture), vp and fp for realise Lafortune
   lightning model.
*/

// The first part of the Vertex Program
static std::string begin_vp =
"!!ARBvp1.0\n"
"ATTRIB vpos = vertex.position;\n"
"ATTRIB vnorm = vertex.normal;\n"
"ATTRIB texcoord = vertex.texcoord[0];\n"
"PARAM mvp[4] = { state.matrix.mvp };\n"
"PARAM mv[4] = { state.matrix.modelview };\n"
"PARAM mvinvt[4] = { state.matrix.modelview.invtrans };\n"
"PARAM yaxis = {0.0,1.0,0.0,1.0};\n"
"PARAM iyaxis = {0.0,-1.0,0.0,1.0};\n"
"PARAM half = {0.5,0.5,0.5,1.0};\n"
"PARAM zero = {0.0,0.0,0.0,0.0};\n"
"PARAM ione = {-1.0,-1.0,-1.0,1.0};\n"
"PARAM one = {1.0,1.0,1.0,1.0};\n"
"PARAM two = {2.0,3.0,4.0,5.0};\n"
"PARAM five = {5.0,5.0,5.0,5.0};\n"
"PARAM ln = {0.0,0.0,1.0,1.0};\n"
"PARAM PI = {3.141592654,0.0,0.0,1.0};\n"
"PARAM small = {0.0000000001,0.0000000001,0.0000000001,1.0};\n"
"PARAM small2 = {0.001,0.001,0.001,1.0};\n"
"PARAM big = {10000.0,10000.0,10000.0,1.0};\n"
"PARAM red = {1.0,0.0,0.0,1.0};\n"
"PARAM green = {0.0,1.0,0.0,1.0};\n"
//"# outside parametrs:\n"
//"#---------------------------------------------\n"
"PARAM SelfShadowingTerm = program.local[18];\n"
"PARAM FresnelTerm       = program.local[19];\n"
"PARAM Fresnel = program.local[20];\n"
"PARAM MaterialSpecular = program.local[21];\n"
"PARAM MaterialKs = program.local[22];\n"
//"# for some reasons VP doesn't see the light and so we need to add\n"
//"# this parameter as local. And specular coeffitient too.\n"
"#PARAM lightpos = state.light[0].position;\n"
"#PARAM int_spec = state.light[0].specular;\n"
"PARAM lightpos = program.local[23];\n"
"PARAM int_spec = program.local[24];\n"
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
//"# computing the orthogonal projection\n"
//"#----------------------------------------------\n"
"DP4 vtrafo.x, mv[0], vpos;\n"
"DP4 vtrafo.y, mv[1], vpos;\n"
"DP4 vtrafo.z, mv[2], vpos;\n"
"DP4 vtrafo.w, mv[3], vpos;\n"
//"# computing the orthogonal projection"
//"#----------------------------------------------"
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
"MOV tmp5.w, one.x; \n"          //# binormal = normalize(Cross(normal,yaxis))
"XPD tmp3, tmp5, ntrafo;\n"
"DP3 tmp2.w, tmp3, tmp3;\n"
"RSQ tmp2.w, tmp2.w;\n"
"MUL tmp3.xyz, tmp2.w, tmp3;\n"
"MOV tmp3.w, one.x; \n"          //# t1 = tangent = normalize(Cross(binormal,normal))
//"# checking for isZero( (1. - normal.dot( x_axis )) )\n"
//"# (But with this checking we receive flikering during\n"
//"# the object moving)\n"
"DP3 tmp2.x, ntrafo, yaxis;\n"    //# a = normal.dot( y_axis )
"DP3 tmp2.y, ntrafo, iyaxis;\n"   //# b = normal.dot( -y_axis )
"SUB tmp2.x, one.x, tmp2.x;\n"   //# a = 1 - a
"SUB tmp2.y, one.x, tmp2.y;\n"   //# b = 1 - b
"MOV tmp4.x, iyaxis.y;\n"
"MOV tmp4.y, yaxis.y;\n"         //# tmp4: 1 -1 .. ..
"SGE tmp1.x, tmp2.y, tmp2.x;\n"  //# b >= a => tmp1.x = 1.0
"SLT tmp1.y, tmp2.y, tmp2.x;\n"  //# b < a  => tmp1.y = 1.0
"MUL tmp4.x, tmp4.x, tmp1.x;\n"
"MUL tmp4.y, tmp4.y, tmp1.y;\n"
"ADD tmp1.z, tmp4.x, tmp4.y;\n"  //# tmp1 => t2 = tangent = (0 0 1)/(0 0 -1)
"MOV tmp1.x, zero.x;\n"
"MOV tmp1.y, zero.y;\n"
"MOV tmp1.w, one.w;\n"
"MIN tmp2.z, tmp2.x, tmp2.y;\n"
"SGE tmp2.x, tmp2.z, small2.x;\n"
"SLT tmp2.y, tmp2.z, small2.x;\n"
"MUL tmp4, tmp2.y, tmp1;\n"      //# tangent with sign
"MUL tmp3, tmp2.x, tmp3;\n"      //# if(a < 0.001 || b < 0.001) t1 = 0
"ADD tmp3, tmp3, tmp4;\n"        //# t1 = tmp4/t1
//"# computing texture coordinates by NDF-model\n"
//"#=================================================================\n"
"XPD tmp5, ntrafo, tmp3;\n"
"SUB tmp4, mvinvt[3], vtrafo;\n"
"DP3 tmp2.w, tmp4, tmp4;\n"
"RSQ tmp2.w, tmp2.w;\n"
"MUL tmp4.xyz, tmp2.w, tmp4;\n"
"MOV tmp4.w, one.x;\n"           //# normalize vector eye
//"# computing the light direction\n"
//"#----------------------------------------------\n"
"SUB tmp6, lightpos, vtrafo;\n"
"DP3 tmp2.w, tmp6, tmp6;\n"
"RSQ tmp2.w, tmp2.w;\n"
"MUL tmp6.xyz, tmp2.w, tmp6;\n"
"MOV tmp6.w, one.x;\n"           //# normalize vector tmp6
//"# comuping parametrs for local coordinat system\n"
//"#----------------------------------------------
"DP3 lv.x, tmp4, tmp3;\n"        //# local eye vector
"DP3 lv.y, tmp4, tmp5;\n"
"DP3 lv.z, tmp4, ntrafo;\n"
"DP3 ll.x, tmp6, tmp3;\n"        //# local light vector
"DP3 ll.y, tmp6, tmp5;\n"
"DP3 ll.z, tmp6, ntrafo;\n"       //#ntrafo;
"ADD lh, lv, ll;\n"
"# normalize vector lh\n"
"DP3 tmp2.x, lh, lh;\n"
"RSQ tmp2.x, tmp2.x;\n"
"MUL lh, lh, tmp2.x;\n"
"MOV lh.w, one.x;\n"
//"# at last compute the texture coordinates\n"
//"#----------------------------------------------\n"
"MAD ntrafo, lh, half, half;\n"
"MOV ntrafo.z, zero.y;\n"
"MOV ntrafo.w, one.x;\n"
//"# computing the color by NDF-model\n"
//"#=================================================================\n"
//"# Computing HdotV, HdotN, LdotN, VdotN\n"
//"#----------------------------------------------\n"
"DP3 tmp3.x, lh, lv;\n"      //# HdotV
"DP3 tmp3.y, lh, ln;\n"      //# HdotN
"DP3 tmp3.z, ll, ln;\n"      //# LdotN
"DP3 tmp3.w, lv, ln;\n"      //# VdotN
"MOV tmp1, one;\n";

// The function to assembly together all parts of the Vertex Program
// add SelfShadowingTerm, Fresnel Term if needed.
std::string NDFMaterial::makeVP(bool SelfShadowing, bool FresnelTerm, bool CorrectCosine)
{
  //"# Computing SelfShadowingTerm\n"
  //"#===================================================\n"
  //"# G(ll, lv) = min{1, (HdotN)*(VdotN)/(HdotV),\n"
  //"#                    (HdotN)*(LdotN)/(HdotV)}\n"
  //"#----------------------------------------------\n"
  std::string self_shad, fresnel, end_vp, CorCos, whole_vp;

  if(SelfShadowing)
  {
     self_shad = "MUL tmp1.x, one.x, tmp3.y;\n"
                 "MUL tmp1.x, tmp1.x, tmp3.w;\n"
                 "RCP tmp4, tmp3.x;\n"
                 "MUL tmp1, tmp1.x, tmp4.x;\n"
                 "MUL tmp2.x, one.x, tmp3.y;\n"
                 "MUL tmp2.x, tmp2.x, tmp3.z;\n"
                 "RCP tmp4, tmp3.x;\n"
                 "MUL tmp2, tmp2.x, tmp4.x;\n"
                 "MIN tmp4, tmp1, tmp2;\n"
                 "MIN tmp4, tmp4, one;\n"
     //"# fs = Li * G/(Pi*LdotN*VdotN) -> result in tmp1\n"
     //"#----------------------------------------------\n"
                 "MOV tmp1, one;\n"
                 "MUL tmp2.x, tmp3.z, tmp3.w;\n"
                 "MUL tmp2.x, tmp2.x, PI.x;\n"  //# tmp2.x = denominator
                 "RCP tmp1, tmp2.x;\n"          //# tmp1  /= denominator
                 "MUL tmp1, tmp4, tmp1;\n"
     //"# !!!!!!!!! all works OK even if denominator = 0!\n"
     //"## check for: if( LdotN == 0.0 || VdotN == 0.0 ) fs[i] = 0.0;\n"
                 "ABS tmp2.y, tmp3.z;\n"
                 "SGE tmp5, tmp2.y, small.x;\n" //# tmp5 = (LdotN >= 0.0000000001)
                 "ABS tmp2.y, tmp3.w;\n"
                 "SGE tmp4, tmp2.y, small.x;\n" //# tmp4 = (VdotN >= 0.0000000001)
                 "ADD tmp4, tmp4, tmp5;\n"
                 "SGE tmp4, tmp4, one;\n"       //# tmp4 = (tmp4 || tmp5)
                 "MUL tmp1, tmp1, tmp4;\n"      //# if(tmp5.i == 0)  tmp1.i = 0
     //"## The result\n"
                 "MUL tmp1, tmp1, int_spec;\n"
                 "MOV tmp1.w, one.x;\n";
  }
  else
  {
     self_shad = "MOV tmp1, int_spec;\n"
                 "MOV tmp1.w, one.x;\n";
  }

  //"# taking into account the Fresnel Term\n"
  //"#===================================================\n"
  if(FresnelTerm)
  {
     fresnel = "SUB tmp2.x, one.x, tmp3.x;\n"     //# 1.0-HdotV
               "POW tmp4, tmp2.x, five.w;\n"      //# powf( 1.0-HdotV, 5.0)
               "SUB tmp2, one, Fresnel;\n"        // # 1-FrenselColor
               "MAD tmp5, tmp2, tmp4, Fresnel;\n" //# Fl[0] + (1-Fl[0])*pow5
               "MUL tmp5, tmp5, MaterialKs;\n"
               "MUL tmp5, tmp5, MaterialSpecular;\n"
               //"## The result\n"
               "MUL tmp1, tmp1, tmp5;\n"
               "MOV tmp1.w, one.x;\n";
  }
  else
  {
    fresnel = "";
  }

  //"# last step - add correct cosine if needed\n"
  //"#===================================================\n"
  //"## if(lh[2] < 0.0) color = ( 0.0, 0.0, 0.0 )\n"
  if(CorrectCosine) CorCos = "MOV tmp2, tmp3.z;\n";
  else   CorCos = "MOV tmp2, one;\n";

  end_vp = "SGE tmp6, lh.z, zero.x;\n"
  //"# computing the orthogonal projection\n"
  //"#----------------------------------------------\n"
  "MUL tmp2, tmp6, tmp2;\n"
  "MOV tmp2.w, one.x;\n"
  "MAX tmp6, zero.x, tmp3.z;\n"
  "DP4 vtrafo.x, mvp[0], vpos;\n"
  "DP4 vtrafo.y, mvp[1], vpos;\n"
  "DP4 vtrafo.z, mvp[2], vpos;\n"
  "DP4 vtrafo.w, mvp[3], vpos;\n"
  "MOV result.position, vtrafo;\n"
  "MOV result.texcoord[0], texcoord;\n"      //# pass thorugh texcoords0
  "MOV result.texcoord[1], ntrafo;\n"        //# pass thorugh texcoords1
  "MUL result.color.primary, tmp1, tmp2;\n"  //# send specularColor*CorCos to RC/FP
  "MOV result.color.secondary, tmp6;\n"      //# send LdotN as secondary.color to RC/FP
  "END\n";

  whole_vp = begin_vp + self_shad + fresnel + CorCos + end_vp;

  return whole_vp;
}



// The first part of the Fragment Program
static std::string begin_fp =
"!!ARBfp1.0\n"
"PARAM one = {1.0,1.0,1.0,1.0};\n"
"PARAM two = {2.0,2.0,2.0,1.0};\n"
"PARAM ione = {-1.0,-1.0,-1.0,1.0};\n"
"PARAM red = {1.0, 0.0, 0.0, 1.0};\n"
//"PARAM CorrectCos = program.local[1];\n"
"PARAM MaterialAmbient = program.local[2];\n"
"PARAM zero = {0.01,0.0,0.0,1.0};\n"
"PARAM MaterialDiffuse = program.local[3];\n"
"PARAM MaterialKa = program.local[4];\n"
"PARAM MaterialKd = program.local[5]; \n"
"ATTRIB  iTex0    = fragment.texcoord[0];\n"
"ATTRIB  iTex1    = fragment.texcoord[1];\n"
"ATTRIB  iColor0  = fragment.color;\n"
"ATTRIB  LdotN = fragment.color.secondary;\n"
"TEMP tex_color;\n"
"TEMP tmp1;\n"
"TEMP tmp2;\n"
"TEMP tmp3;\n"
"TEMP tmp4;\n"
"TEMP mod_tex_col;\n"
"TEMP dif_spec;\n"
"TEMP amb_dif_spec;\n"
"TEMP correctColor;\n";

// The function to assembly together all parts of the Fragment Program
// add Correct Cosine or make only diffuse color if needed.
std::string NDFMaterial::makeFP(bool OnlyDiffuse, bool CorrectCosine)
{
  std::string with_CorCos, without_CorCos, mid_fp, end_fp, whole_fp, only_diffuse;

  with_CorCos = "MOV correctColor, LdotN.x;\n";
  without_CorCos = "MOV correctColor, one.x;\n";
  mid_fp =  //# diffuse = Kd * diffuseColor
           "TEX dif_spec, iTex0, texture[0], 2D;\n"     //# 'Diffuse' texture
           "MUL dif_spec, MaterialDiffuse, dif_spec;\n"
           "MUL dif_spec, dif_spec, MaterialKd;\n"
           "MUL_SAT dif_spec, LdotN.x, dif_spec;\n";

  end_fp =  //#TXP tex_color, iTex0, texture[0], 2D;  #? What is better?
           "TEX tex_color, iTex1, texture[1], 2D;\n"
           //# modulate texture with specularColor
           "MUL mod_tex_col, tex_color, iColor0;\n"
           //"MUL mod_tex_col, mod_tex_col, tmp3;\n"
           //# dif_spec = diffuse + specular
           "ADD tmp4, mod_tex_col, dif_spec;\n"
           //# ambient = Ka * ambientColor
           "MUL amb_dif_spec, MaterialAmbient, MaterialKa;\n"
           //# all together
           "ADD result.color, tmp4, amb_dif_spec;\n"
           "END\n";


  only_diffuse = //# diffuse = Kd * diffuseColor * texDiffuse
           "MOV result.color, dif_spec;\n"
	   "END\n";


  if(OnlyDiffuse)
  {
     whole_fp = begin_fp + mid_fp + only_diffuse;
  }
  else
  {
     if(CorrectCosine)
     {
       whole_fp = begin_fp + with_CorCos + mid_fp + end_fp;
     }
     else
     {
       whole_fp = begin_fp + without_CorCos + mid_fp + end_fp;
     }
  }

  return whole_fp;
}


void NDFMaterial::prepareLocalChunks(void)
{
    if(_materialChunk == NullFC)
    {
        _materialChunk = MaterialChunk::create();

        addRefCP(_materialChunk);
    }

    if(_textureChunkDiffuse == NullFC)
    {
        _textureChunkDiffuse = TextureChunk::create();

        addRefCP(_textureChunkDiffuse);
    }

    if(_textureChunkSpecular == NullFC)
    {
        _textureChunkSpecular = TextureChunk::create();

        addRefCP(_textureChunkSpecular);
    }

    if(_vpChunk == NullFC)
    {
        _vpChunk = VertexProgramChunk::create();

        addRefCP(_vpChunk);
    }

    if(_fpChunk == NullFC)
    {
        _fpChunk = FragmentProgramChunk::create();

        addRefCP(_fpChunk);
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

void NDFMaterial::initMethod (void)
{
}


/***************************************************************************\
 *                           Instance methods                              *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  private                                                                 -
\*-------------------------------------------------------------------------*/

/*----------------------- constructors & destructors ----------------------*/

NDFMaterial::NDFMaterial(void) :
    Inherited(),
    _materialChunk(NullFC),
    _textureChunkDiffuse  (NullFC),
    _textureChunkSpecular  (NullFC),
    _vpChunk      (NullFC),
    _fpChunk      (NullFC),
    _rcChunk      (NullFC)
{
}

NDFMaterial::NDFMaterial(const NDFMaterial &source) :
    Inherited(source),
    _materialChunk(source._materialChunk),
    _textureChunkDiffuse  (source._textureChunkDiffuse ),
    _textureChunkSpecular (source._textureChunkSpecular ),
    _vpChunk      (source._vpChunk      ),
    _fpChunk      (source._fpChunk      ),
    _rcChunk      (source._rcChunk      )
{
}

NDFMaterial::~NDFMaterial(void)
{
    subRefCP(_materialChunk);
    subRefCP(_textureChunkDiffuse );
    subRefCP(_textureChunkSpecular );
    subRefCP(_vpChunk      );
    subRefCP(_fpChunk      );
    subRefCP(_rcChunk      );
}

/*----------------------------- class specific ----------------------------*/

void NDFMaterial::changed(BitVector whichField, UInt32 origin)
{
    Inherited::changed(whichField, origin);
}

/*-------------------------- your_category---------------------------------*/

// Combine all parts together (material, texture, Vertex and Fragment Programs)
StatePtr NDFMaterial::makeState(void)
{
    StatePtr state = State::create();

    bool CorCos, SelfShadTerm, FresnelTerm, OnlyDiffuse;
    Color3f MaterialSpecular, MaterialAmbient, MaterialDiffuse;
    Color3f Fresnel, LightSpecular;
    Color4f RDiffuse(0,0,0,1), one(1.0,1.0,1.0,1.0), RAmbient(0,0,0,1);
    float MaterialKs, MaterialKa, MaterialKd;
    Pnt3f LightPos;
    Color3f v3;
    Color4f v4;
    UInt8 UseFPorRC;
    float alpha = 1.f - getTransparency();

    UseFPorRC = getUseFPorRC();

    prepareLocalChunks();

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

    // NOTE: the image is NOT copied, the variable needs to be
    // kept around as long as the texture is used
    beginEditCP(_textureChunkDiffuse);
    _textureChunkDiffuse->setImage( getDiffuseImage() );
    _textureChunkDiffuse->setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
    _textureChunkDiffuse->setMagFilter(GL_LINEAR);
    _textureChunkDiffuse->setWrapS(GL_REPEAT);
    _textureChunkDiffuse->setWrapT(GL_REPEAT);
    _textureChunkDiffuse->setEnvMode(GL_MODULATE);
    endEditCP(_textureChunkDiffuse);

    state->addChunk(_textureChunkDiffuse);

    beginEditCP(_textureChunkSpecular);
    _textureChunkSpecular->setImage( getNDFImage() );
    _textureChunkSpecular->setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
    _textureChunkSpecular->setMagFilter(GL_LINEAR);
    _textureChunkSpecular->setWrapS(GL_REPEAT);
    _textureChunkSpecular->setWrapT(GL_REPEAT);
    _textureChunkSpecular->setEnvMode(GL_MODULATE);
    endEditCP(_textureChunkSpecular);

    state->addChunk(_textureChunkSpecular);

    // Take some input variables and push it to the vertex program
    SelfShadTerm = getSelfShadowingTerm();
    FresnelTerm = getFresnelTerm();
    Fresnel = getFresnel();
    MaterialKs = getKs();
    MaterialSpecular = getSpecular();
    LightPos = getLightPos();
    LightSpecular = getLightSpecular();
    CorCos = getCorrectCosine();

    std::string whole_vp = makeVP(SelfShadTerm, FresnelTerm, CorCos);

    beginEditCP(_vpChunk);
    _vpChunk->setProgram(begin_vp);
    _vpChunk->addParameter("selfshadowingterm", 18, Vec4f(SelfShadTerm, SelfShadTerm, SelfShadTerm, 1.0) );
    _vpChunk->addParameter("fresnelterm", 19, Vec4f(FresnelTerm, FresnelTerm, FresnelTerm, 1.0) );
    _vpChunk->addParameter("fresnel", 20, Vec4f(Fresnel[0], Fresnel[1], Fresnel[2], 1.0) );
    _vpChunk->addParameter("materialspec", 21, Vec4f(MaterialSpecular[0], MaterialSpecular[1], MaterialSpecular[2], 1.0) );
    _vpChunk->addParameter("materialKs", 22, Vec4f(MaterialKs, MaterialKs, MaterialKs, 1.0) );
    _vpChunk->addParameter("lightpos", 23, Vec4f(LightPos[0], LightPos[1], LightPos[2], 1.0));
    _vpChunk->addParameter("specular", 24, Vec4f(LightSpecular[0], LightSpecular[1], LightSpecular[2], 1.0) );
    endEditCP(_vpChunk);

    state->addChunk(_vpChunk);

    // Take some input variables and push it to the fragment program
    // or to the register combiners
    MaterialAmbient = getAmbient();
    MaterialDiffuse = getDiffuse();
    MaterialKa = getKa();
    MaterialKd = getKd();
    OnlyDiffuse = getOnlyDiffuse();

    if(UseFPorRC == 0)
    {
      std::string whole_fp = makeFP(OnlyDiffuse, CorCos);

      beginEditCP(_fpChunk);
      _fpChunk->setProgram(whole_fp);
      _fpChunk->addParameter("correctcos", 1, Vec4f(CorCos, CorCos, CorCos, 1.0) );
      _fpChunk->addParameter("materialamb", 2, Vec4f(MaterialAmbient[0], MaterialAmbient[1], MaterialAmbient[2], 1.0) );
      _fpChunk->addParameter("materialdif", 3, Vec4f(MaterialDiffuse[0], MaterialDiffuse[1], MaterialDiffuse[2], 1.0) );
      _fpChunk->addParameter("materialKa", 4, Vec4f(MaterialKa, MaterialKa, MaterialKa, 1.0) );
      _fpChunk->addParameter("materialKd", 5, Vec4f(MaterialKd, MaterialKd, MaterialKd, 1.0) );
      endEditCP(_fpChunk);

      state->addChunk(_fpChunk);
    }
    else
    {
      RDiffuse[0] = MaterialKd * MaterialDiffuse[0];
      RDiffuse[1] = MaterialKd * MaterialDiffuse[1];
      RDiffuse[2] = MaterialKd * MaterialDiffuse[2];

      RAmbient[0] = MaterialKa * MaterialAmbient[0];
      RAmbient[1] = MaterialKa * MaterialAmbient[1];
      RAmbient[2] = MaterialKa * MaterialAmbient[2];

      beginEditCP(_rcChunk);
      {
        _rcChunk->clearCombiners();
        _rcChunk->setColor0(RDiffuse);
        _rcChunk->setColor1(RAmbient);

        if(!OnlyDiffuse)
        {
          // General combiner 0
          // A*B = texDiffuse * 1.0       (for each [r,g,b])
          // C*D = diffuse_color * CorCos (for each [r,g,b])
          // SPARE0 = A*B, SPARE1 = C*D
          //-----------------------------------------------------------------------
          _rcChunk->setCombinerRGB( 0,
             GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB,        // variable A
             GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB,                  // variable B
             GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,  // variable C
             GL_SECONDARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,  // variable D
             GL_SPARE0_NV, GL_SPARE1_NV, GL_DISCARD_NV,    // ABout, CDout, Sumout
             GL_NONE, GL_NONE,                              // scale, bias
             GL_FALSE, GL_FALSE, GL_FALSE );                // ABdot, CDdot, muxSum

          // General combiner 1
          // A*B = texDiffuse * diffuse_color * CorCos  (for each [r,g,b])
          // C*D = texSpecular * specular_color         (for each [r,g,b])
          // SPARE0 = A*B + C*D
          //-----------------------------------------------------------------------
          _rcChunk->setCombinerRGB( 1,
             GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,        // variable A
             GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,                // variable B
             GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB,  // variable C
             GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,  // variable D
             GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV,   // ABout, CDout, Sumout
             GL_NONE, GL_NONE,                              // scale, bias
             GL_FALSE, GL_FALSE, GL_FALSE );                // ABdot, CDdot, muxSum

          // Final combiner
          // compute A*B + (1-A)*C + D
          //  A*B = ambient_color * 1.0  (for each [r,g,b])
          //  D = SPARE0 * 1.0        (for each [r,g,b])
          //  result = ambient_color + texDiffuse * diffuse * CorCos + tex * specular_color
          //-----------------------------------------------------------------------
          _rcChunk->setFinalCombiner(
             GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB, // variable A
             GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB,                 // variable B = 1
             GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB,               // variable C
             GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,          // variable D
             GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB,               // variable E
             GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB,               // variable F
             GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA );           // variable G
        }
        else
        {
          // General combiner
          // A*B = diffuse_color * CorCos (for each [r,g,b])
          // C*D = texDiffuse * 1 (for each [r,g,b])
          // SPARE0 = A*B, SPARE1 = C*D
          //-----------------------------------------------------------------------
          _rcChunk->setCombinerRGB( 0,
             GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,  // variable A
             GL_SECONDARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,  // variable B
             GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB,        // variable C
             GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB,                  // variable D
             GL_SPARE0_NV, GL_SPARE1_NV, GL_DISCARD_NV,     // ABout, CDout, Sumout
             GL_NONE, GL_NONE,                              // scale, bias
             GL_FALSE, GL_FALSE, GL_FALSE );                // ABdot, CDdot, muxSum

          // Final combiner
          // compute A*B + (1-A)*C + D
          //  A*B = texDiffuse * (diffuse_color * CorCos) (for each [r,g,b])
          //  D = ambient_color * 1.0                     (for each [r,g,b])
          //  result = ambient_color + diffuse * CorCos + tex * specular_color
          //-----------------------------------------------------------------------
          _rcChunk->setFinalCombiner(
             GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,           // variable A
             GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,           // variable B
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

void NDFMaterial::rebuildState(void)
{
    bool CorCos, SelfShadTerm, FresnelTerm, OnlyDiffuse;
    Color3f MaterialSpecular, MaterialAmbient, MaterialDiffuse;
    Color3f Fresnel, LightSpecular;
    Color4f RDiffuse(0,0,0,0), one(1.0,1.0,1.0,1.0), RAmbient(0,0,0,1);;
    float MaterialKs, MaterialKa, MaterialKd;
    Pnt3f LightPos;
    Color3f v3;
    Color4f v4;
    UInt8 UseFPorRC;
    Real32  alpha = 1.f - getTransparency();

    UseFPorRC = getUseFPorRC();

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

    _pState->addChunk(_materialChunk);

   // NOTE: the image is NOT copied, the variable needs to be
   // kept around as long as the texture is used
    beginEditCP(_textureChunkDiffuse);
    _textureChunkDiffuse->setImage( getDiffuseImage() );
    _textureChunkDiffuse->setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
    _textureChunkDiffuse->setMagFilter(GL_LINEAR);
    _textureChunkDiffuse->setWrapS(GL_REPEAT);
    _textureChunkDiffuse->setWrapT(GL_REPEAT);
    _textureChunkDiffuse->setEnvMode(GL_MODULATE);
    endEditCP(_textureChunkDiffuse);

    _pState->addChunk(_textureChunkDiffuse);

   beginEditCP(_textureChunkSpecular);
    _textureChunkSpecular->setImage( getNDFImage() );
    _textureChunkSpecular->setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
    _textureChunkSpecular->setMagFilter(GL_LINEAR);
    _textureChunkSpecular->setWrapS(GL_REPEAT);
    _textureChunkSpecular->setWrapT(GL_REPEAT);
    _textureChunkSpecular->setEnvMode(GL_MODULATE);
    endEditCP(_textureChunkSpecular);

    _pState->addChunk(_textureChunkSpecular);

    // Take some input variables and push it to the vertex program
    SelfShadTerm = getSelfShadowingTerm();
    FresnelTerm = getFresnelTerm();
    Fresnel = getFresnel();
    MaterialKs = getKs();
    MaterialSpecular = getSpecular();
    LightPos = getLightPos();
    LightSpecular = getLightSpecular();
    CorCos = getCorrectCosine();

    std::string whole_vp = makeVP(SelfShadTerm, FresnelTerm, CorCos);

    beginEditCP(_vpChunk);
    _vpChunk->setProgram(whole_vp);
    _vpChunk->addParameter("selfshadowingterm", 18, Vec4f(SelfShadTerm, SelfShadTerm, SelfShadTerm, 1.0) );
    _vpChunk->addParameter("fresnelterm", 19, Vec4f(FresnelTerm, FresnelTerm, FresnelTerm, 1.0) );
    _vpChunk->addParameter("fresnel", 20, Vec4f(Fresnel[0], Fresnel[1], Fresnel[2], 1.0) );
    _vpChunk->addParameter("materialspec", 21, Vec4f(MaterialSpecular[0], MaterialSpecular[1], MaterialSpecular[2], 1.0) );
    _vpChunk->addParameter("materialKs", 22, Vec4f(MaterialKs, MaterialKs, MaterialKs, 1.0) );
    _vpChunk->addParameter("lightpos", 23, Vec4f(LightPos[0], LightPos[1], LightPos[2], 1.0));
    _vpChunk->addParameter("specular", 24, Vec4f(LightSpecular[0], LightSpecular[1], LightSpecular[2], 1.0) );
    endEditCP(_vpChunk);

    _pState->addChunk(_vpChunk);

    // Take some input variables and push it to the fragment program
    // or to the register combiners
    MaterialAmbient = getAmbient();
    MaterialDiffuse = getDiffuse();
    MaterialKa = getKa();
    MaterialKd = getKd();
    OnlyDiffuse = getOnlyDiffuse();

    if(UseFPorRC == 0)
    {
      std::string whole_fp = makeFP(OnlyDiffuse, CorCos);

      beginEditCP(_fpChunk);
      _fpChunk->setProgram(whole_fp);
      _fpChunk->addParameter("correctcos", 1, Vec4f(CorCos, CorCos, CorCos, 1.0) );
      _fpChunk->addParameter("materialamb", 2, Vec4f(MaterialAmbient[0], MaterialAmbient[1], MaterialAmbient[2], 1.0) );
      _fpChunk->addParameter("materialdif", 3, Vec4f(MaterialDiffuse[0], MaterialDiffuse[1], MaterialDiffuse[2], 1.0) );
      _fpChunk->addParameter("materialKa", 4, Vec4f(MaterialKa, MaterialKa, MaterialKa, 1.0) );
      _fpChunk->addParameter("materialKd", 5, Vec4f(MaterialKd, MaterialKd, MaterialKd, 1.0) );
      endEditCP(_fpChunk);

      _pState->addChunk(_fpChunk);
    }
    else
    {
      RDiffuse[0] = MaterialKd * MaterialDiffuse[0];
      RDiffuse[1] = MaterialKd * MaterialDiffuse[1];
      RDiffuse[2] = MaterialKd * MaterialDiffuse[2];

      RAmbient[0] = MaterialKa * MaterialAmbient[0];
      RAmbient[1] = MaterialKa * MaterialAmbient[1];
      RAmbient[2] = MaterialKa * MaterialAmbient[2];

      beginEditCP(_rcChunk);
      {
        _rcChunk->clearCombiners();
        _rcChunk->setColor0(RDiffuse);
        _rcChunk->setColor1(RAmbient);

        if(!OnlyDiffuse)
        {
          // General combiner 0
          // A*B = texDiffuse * 1.0       (for each [r,g,b])
          // C*D = diffuse_color * CorCos (for each [r,g,b])
          // SPARE0 = A*B, SPARE1 = C*D
          //-----------------------------------------------------------------------
          _rcChunk->setCombinerRGB( 0,
             GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB,        // variable A
             GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB,                  // variable B
             GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,  // variable C
             GL_SECONDARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,  // variable D
             GL_SPARE0_NV, GL_SPARE1_NV, GL_DISCARD_NV,    // ABout, CDout, Sumout
             GL_NONE, GL_NONE,                              // scale, bias
             GL_FALSE, GL_FALSE, GL_FALSE );                // ABdot, CDdot, muxSum

          // General combiner 1
          // A*B = texDiffuse * diffuse_color * CorCos  (for each [r,g,b])
          // C*D = texSpecular * specular_color         (for each [r,g,b])
          // SPARE0 = A*B + C*D
          //-----------------------------------------------------------------------
          _rcChunk->setCombinerRGB( 1,
             GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,        // variable A
             GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,                // variable B
             GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB,  // variable C
             GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,  // variable D
             GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV,   // ABout, CDout, Sumout
             GL_NONE, GL_NONE,                              // scale, bias
             GL_FALSE, GL_FALSE, GL_FALSE );                // ABdot, CDdot, muxSum

          // Final combiner
          // compute A*B + (1-A)*C + D
          //  A*B = ambient_color * 1.0  (for each [r,g,b])
          //  D = SPARE0 * 1.0        (for each [r,g,b])
          //  result = ambient_color + texDiffuse * diffuse * CorCos + tex * specular_color
          //-----------------------------------------------------------------------
          _rcChunk->setFinalCombiner(
             GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB, // variable A
             GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB,                 // variable B = 1
             GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB,               // variable C
             GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,          // variable D
             GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB,               // variable E
             GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB,               // variable F
             GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA );           // variable G
        }
        else
        {
          // General combiner
          // A*B = diffuse_color * CorCos (for each [r,g,b])
          // C*D = texDiffuse * 1 (for each [r,g,b])
          // SPARE0 = A*B, SPARE1 = C*D
          //-----------------------------------------------------------------------
          _rcChunk->setCombinerRGB( 0,
             GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,  // variable A
             GL_SECONDARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,  // variable B
             GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB,        // variable C
             GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB,                  // variable D
             GL_SPARE0_NV, GL_SPARE1_NV, GL_DISCARD_NV,     // ABout, CDout, Sumout
             GL_NONE, GL_NONE,                              // scale, bias
             GL_FALSE, GL_FALSE, GL_FALSE );                // ABdot, CDdot, muxSum

          // Final combiner
          // compute A*B + (1-A)*C + D
          //  A*B = texDiffuse * (diffuse_color * CorCos) (for each [r,g,b])
          //  D = ambient_color * 1.0                     (for each [r,g,b])
          //  result = ambient_color + diffuse * CorCos + tex * specular_color
          //-----------------------------------------------------------------------
          _rcChunk->setFinalCombiner(
             GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,           // variable A
             GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB,           // variable B
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

bool NDFMaterial::isTransparent(void) const
{
    return ((getTransparency() > Eps) || (Inherited::isTransparent()));
}






/*------------------------------- dump ----------------------------------*/
void NDFMaterial::dump(      UInt32    uiIndent,
                         const BitVector OSG_CHECK_ARG(bvFlags )) const
{
    //SLOG << "Dump NDFMaterial NI" << std::endl;
    NDFMaterialPtr thisP(*this);

    thisP.dump(uiIndent, FCDumpFlags::RefCount);

    indentLog(uiIndent, PLOG);
    PLOG << "NDFMaterial at " << this << std::endl;

    indentLog(uiIndent, PLOG);
    PLOG << "\tambient: " << getAmbient() << std::endl;

    indentLog(uiIndent, PLOG);
    PLOG << "\tdiffuse: " << getDiffuse()  << std::endl;

    indentLog(uiIndent, PLOG);
    PLOG << "\tspecular: " << getSpecular()  << std::endl;

    indentLog(uiIndent, PLOG);
    PLOG << "\temission: " << getEmission()  << std::endl;

    indentLog(uiIndent, PLOG);
    PLOG << "\tFrensel color: " << getFresnel()  << std::endl;

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
    PLOG << "NDFMaterial end " << this << std::endl;
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
    static Char8 cvsid_cpp       [] = "@(#)$Id: OSGNDFMaterial.cpp,v 1.1 2003/05/19 12:04:39 jnkautz Exp $";
    static Char8 cvsid_hpp       [] = OSGNDFMATERIALBASE_HEADER_CVSID;
    static Char8 cvsid_inl       [] = OSGNDFMATERIALBASE_INLINE_CVSID;

    static Char8 cvsid_fields_hpp[] = OSGNDFMATERIALFIELDS_HEADER_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

