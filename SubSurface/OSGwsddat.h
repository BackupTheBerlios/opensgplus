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
 * OpenSG - Dynamic Subdivision                                              *
 *                                                                           *
 *            contact: v.settgast@cg.cs.tu-bs.de * 
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

#ifndef WSDDAT_H
#define WSDDAT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//#pragma warning( disable : 4661) 

#include <math.h>
#include <stdio.h>
#include "OSGMyMesh.h"
#include "OSGConfig.h"
#include "OSGSubSurfaceDef.h"
#include "OSGVector.h"
#include "OSGGeometry.h"
#include "OSGSimpleGeometry.h"
#include "OSGSimpleMaterial.h"
#include "OSGMeshType.h"

OSG_BEGIN_NAMESPACE

// Crease Types (Loop):
enum Crease_Type { 
   CREASE_HALF =            -2,  // dart
   CREASE_NO =               0,  // no crease at all
   CREASE_REG_REG =          5,  // regular - regular
   CREASE_IRREG_IRREG =      7,  // irregular - irregluar (has to be handled as regular - irregular after one subdivision step)
   CREASE_IRREG_REG =       10,  // irregular - regular
   CREASE_REG_IRREG =       15,  // regular - irregular
   CREASE_IRREG_REG_ONCE =  20,  // irregular - regular (handled as regular - regular after one subdiv step)
   CREASE_HALF_REG_ONCE =   25,  // dart - regular (handled as regular - regular after ...)
   CREASE_HALF_IRREG_ONCE = 30   // dart - irregular (handled as regular - irregular after ...)
};


//! shared data for all instances
struct OSG_SUBSURFACELIB_DLLMAPPING sharedFields
{
   OSG::GeoPositions3fPtr limitpointsptr;
   OSG::GeoNormals3fPtr   limitnormalsptr;  
   OSG::GeoPositions3f::StoredFieldType* limitpoints;       //!< limit points
   OSG::GeoNormals3f::StoredFieldType*   limitnormals;      //!< limit normals   
   OSG::GeoTexCoords2fPtr texcoordsptr;                     //!< Texture Coordinates
   OSG::GeoTexCoords2f::StoredFieldType* texcoords;  
};

//! data for each instance
struct OSG_SUBSURFACELIB_DLLMAPPING perInstanceData
{
   //!< parent node
   OSG::NodePtr parentNode;

   OSG::GeoPTypesUI8Ptr    oneTypesPtr;                     
   OSG::GeoPLengthsUI32Ptr oneLengthsPtr;
   OSG::GeoIndicesUI32Ptr  oneIndicesPtr;

   OSG::GeoPTypesUI8::StoredFieldType*    oneTypes;
   OSG::GeoPLengthsUI32::StoredFieldType* oneLengths;  
   OSG::GeoIndicesUI32::StoredFieldType*  oneIndis;

   OSG::GeometryPtr onlyOneGeoP; //!< the geometry node core for this instance
   OSG::NodePtr onlyOneNode;     //!< node for instance
};

const Int32 zweihoch[] = {1,2,4,8,16,32,64,128,256,512}; // pow(2,x)

const Int32 wsddepthindexarray[] = {2, 3, 5, 9, 17, 33, 65, 129, 257, 513, 1025, 2049};
const Int32 wsdinnerindexwidth[] = {0, 0, 3, 7, 15, 31, 63, 127, 255, 511, 1023, 2047};

//! maximum valence
const Int32 wsdmaxvalenz = 30;

//! facing types
enum FacingType { BACK = 0, FRONT = 1, SILUETTE = 5 };

//=============================================================================

/*! \brief patch data class. All patch related data is stored in WSDdat. 
    For each patch there is a WSDdat instance.
*/
template<class WSDVector, int mtype> 
class OSG_SUBSURFACELIB_DLLMAPPING WSDdat 
{ 
   enum               { MType = mtype };
   typedef WSDVector    VectorType;  
   typedef WSDVector    VectorCAType[4][wsdmaxvalenz];

   /*==========================  PUBLIC  =================================*/
public:

   //! Constructor
   WSDdat();
   //! Destructor
   ~WSDdat();


   //! maximum tesselation depth
   Int32 wsdmaxdepth;
   // to switch between the two tables
   void rotateptabs(Int32 i);
   // initialize slates 
   void initptabs(void);

   // initialize slates for texture data
   void initptex(void);

   /*! helper for getting a neighbor patch
       \param from the source patch index
       \param ad 1 or -1, dependant on the position ni_from+1 or ni_from-1 is the neighbor we want
       \return index to the neighbor      
   */
   Int32 getneighbor(Int32 from, Int32 ad);

   //! openmesh to wsddat cache (1-neighborhood)
   VectorType slateO[4][4];         
   //! for corners of valence greater than 4
   VectorCAType  cornerO;
   
   //! texture coordinates of the four corners
   VectorType TexCoords[4][4];  

   //! curvature dependent depth
   Int32 maxdepth;            
   //! actual depth for current frame
   Int32 currentdepth;              
   //! planed depth for the next frame
   Int32 solltiefe;   //nextdepth             

   //! patch radius approximation for projected size
   Real32 radiusD;      

   //! indices to the four corner-verts.
   Int32 vertexindex[4];    
   //! helper for the "vertex to face" connect routine
   Int32 cvindex;        
   //! valence at the four corner-verts. 
   Int32 valenz[4];      
   //! sum of the adjacent sharp edges
   Int32 iscorner[4];
   //! only for loop: is the corner-vert. a regular corner (for creases)
   Int32 isregular[4];

   /*
   //    0 ----- 3
   //    |       |
   //    |       |
   //    |       |
   //    1 ----- 2
   */
   //! index to neighbor-patches
   Int32 neighbors[4];
   //! flag to find borders
   Int32 borders[4];

   /*
   //        3
   //      -----
   //   0  |   |  2
   //      -----
   //        1
   */
   //! neighbors of neighbor
   Int32 neighbor0in1, neighbor1in0, neighbor2in1, neighbor1in2,    
      neighbor3in2, neighbor2in3, neighbor0in3, neighbor3in0;

   //! crease data for regular inner edges
   Int32 crease[12];       

   /*
   //   -----   -----   -----
   //  |      |       |       |
   //  |      0       1       |
   //  |      |       |       |
   //   --2-- + --3-- + --4--
   //  |      |       |       |
   //  |      5       6       |
   //  |      |       |       |
   //   --7-- + --8-- + --9--
   //  |      |       |       |
   //  |      10     11       |
   //  |      |       |       |
   //   -----   -----   -----
   */

   //! crease data for triangles
   Int32 tricrease[9];
   
   /*
   //   \     | \     | \      
   //     0   |   1   |   2    
   //       \ |     \ |     \  
   //   ----- + ----- + -----
   //   \     | \     | \      
   //     3   |   4   |   5    
   //       \ |     \ |     \  
   //   ----- + ----- + -----
   //   \     | \     | \      
   //     6   |   7   |   8    
   //       \ |     \ |     \  
   */
   
   //! crease data for corners with valence > 4
   Int32 corcrease[4][wsdmaxvalenz];  
   //! indices to the inner-first two crease edges
   Int32 crindis[4][2];                             
   //! the used size of a slate
   Int32 tabsize;                

   //! resulting orientation
   FacingType isFacing;

   //! average of the four corner vertices
   VectorType faceloc;      
      
   //! \name some helpers
   //! \{
   //! intersteps are the offsets between patches of different depth
   Int32 interstep_l, interstep_r, interstep_o, interstep_u;

   //! helper for the gap prevention
   bool c0_o, c0_l, c1_l, c1_u, c2_r, c2_u, c3_r, c3_o;

   //! for Loop: Single
   bool isSingleTriangle;

   //! \}

   //! the offset within the vertexarray
   Int32 varrayOffset;        

   //! set inner geometry
   void setInnerGeoP(perInstanceData &insta, UInt32 &indisIn);
   //! set outer geometry
   void setOuterGeoP(perInstanceData &insta, Int32 finer_l, Int32 finer_r, 
         Int32 finer_o, Int32 finer_u, UInt32 &indisIn);

   //! test points
   void setFastPoint(perInstanceData &insta, UInt32 &indisIn);
   void setFastQuad(perInstanceData &insta, UInt32 &indisIn);

   //! instance shared data
   sharedFields*        mySharedFields;

   //! slate A
   static VectorType*   slateA;
   //! slate B
   static VectorType*   slateB;
   //! slate pointer A
   static VectorType*   ptabA;
   //! slate pointer B
   static VectorType*   ptabB;
   //! width of slate A
   static Int32      breiteA;   
   //! corners greater valence 4 in slate A
   static VectorCAType  cornerA;  
   //! corners greater valence 4 in slate B
   static VectorCAType  cornerB;  
   //! pointer corners A
   static VectorCAType* pcorA;
   //! pointer corners B
   static VectorCAType* pcorB;

#ifdef DEFINE_SHOWROOM
   static UInt32 _numtris;
   static UInt32 _numquads;
#endif

   /*==========================  PRIVATE  =================================*/      
private:
  
   /*! \name       geometry set methods                                 */
   /*! \{                                                               */
   void setOuterRightLine(OSG::GeoPLengthsUI32::StoredFieldType* newlengths,
        OSG::GeoPTypesUI8::StoredFieldType*    newtypes,
        OSG::GeoIndicesUI32::StoredFieldType*  newindis,
        Int32 finer_r, UInt32 &indisIn);
   void setOuterLeftLine(OSG::GeoPLengthsUI32::StoredFieldType* newlengths,
       OSG::GeoPTypesUI8::StoredFieldType*    newtypes,
       OSG::GeoIndicesUI32::StoredFieldType*  newindis,
       Int32 finer_l, UInt32 &indisIn);
   void setOuterFirstLine(OSG::GeoPLengthsUI32::StoredFieldType* newlengths,
        OSG::GeoPTypesUI8::StoredFieldType*    newtypes,
        OSG::GeoIndicesUI32::StoredFieldType*  newindis,
        Int32 finer_o, UInt32 &indisIn);
   void setOuterLastLine(OSG::GeoPLengthsUI32::StoredFieldType* newlengths,
       OSG::GeoPTypesUI8::StoredFieldType*    newtypes,
       OSG::GeoIndicesUI32::StoredFieldType*  newindis,
       Int32 finer_u, UInt32 &indisIn);
   void setOuterC0(OSG::GeoPLengthsUI32::StoredFieldType* newlengths,
       OSG::GeoPTypesUI8::StoredFieldType*    newtypes,
       OSG::GeoIndicesUI32::StoredFieldType*  newindis,
       Int32 finer_o, Int32 finer_l, UInt32 &indisIn);
   void setOuterC1(OSG::GeoPLengthsUI32::StoredFieldType* newlengths,
       OSG::GeoPTypesUI8::StoredFieldType*    newtypes,
       OSG::GeoIndicesUI32::StoredFieldType*  newindis,
       Int32 finer_u, Int32 finer_l, UInt32 &indisIn);
   void setOuterC2(OSG::GeoPLengthsUI32::StoredFieldType* newlengths,
       OSG::GeoPTypesUI8::StoredFieldType*    newtypes,
       OSG::GeoIndicesUI32::StoredFieldType*  newindis,
       Int32 finer_u, Int32 finer_r, UInt32 &indisIn);
   void setOuterC3(OSG::GeoPLengthsUI32::StoredFieldType* newlengths,
       OSG::GeoPTypesUI8::StoredFieldType*    newtypes,
       OSG::GeoIndicesUI32::StoredFieldType*  newindis,
       Int32 finer_o, Int32 finer_r, UInt32 &indisIn);
  
   void setTiefe0(OSG::GeoPLengthsUI32::StoredFieldType* newlengths,
      OSG::GeoPTypesUI8::StoredFieldType*    newtypes,
      OSG::GeoIndicesUI32::StoredFieldType*  newindis,
      UInt32 &indisIn);  
  
   void setupStrip(OSG::GeoPLengthsUI32::StoredFieldType* newlengths,
      OSG::GeoPTypesUI8::StoredFieldType*    newtypes,
      OSG::GeoIndicesUI32::StoredFieldType*  newindis,
      UInt32 &indisIn, Int32 s1, Int32 s2, Int32 add);
   void setupHalfStrip(OSG::GeoPLengthsUI32::StoredFieldType* newlengths,
      OSG::GeoPTypesUI8::StoredFieldType*    newtypes,
      OSG::GeoIndicesUI32::StoredFieldType*  newindis,
      UInt32 &indisIn, Int32 s1, Int32 s2, Int32 add, Int32 width);
   void setQuadOrTriangle(OSG::GeoPLengthsUI32::StoredFieldType* newlengths,
      OSG::GeoPTypesUI8::StoredFieldType*    newtypes,
      OSG::GeoIndicesUI32::StoredFieldType*  newindis,
      UInt32 &indisIn, Int32 p1, Int32 p2, Int32 p3, Int32 p4);
   void setSpecialTriangle(OSG::GeoPLengthsUI32::StoredFieldType* newlengths,
      OSG::GeoPTypesUI8::StoredFieldType*    newtypes,
      OSG::GeoIndicesUI32::StoredFieldType*  newindis,
      UInt32 &indisIn, Int32 p1, Int32 p2, Int32 p3);
   /*! \}                                                             */
};

OSG_END_NAMESPACE

#endif 

