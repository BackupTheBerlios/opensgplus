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
 *					  contact: v.settgast@cg.cs.tu-bs.de * 
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
// always include first
#include "OSGMyMesh.h"
#include "OSGSubSurfaceDef.h"
#include "OSGVector.h"
#include "OSGGeometry.h"
#include "OSGSimpleGeometry.h"
#include "OSGSimpleMaterial.h"

OSG_BEGIN_NAMESPACE

//! shared data for all instances
struct OSG_SUBSURFACELIB_DLLMAPPING sharedFields
{
   OSG::GeoPositions3fPtr limitpointsptr;
   OSG::GeoNormals3fPtr   limitnormalsptr;	
   OSG::GeoPositions3f::StoredFieldType* limitpoints;       //!< limit points
   OSG::GeoNormals3f::StoredFieldType*   limitnormals;      //!< limit normals
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
   OSG::NodePtr onlyOneNode;	 //!< node for instance
};

const Int32 zweihoch[] = {1,2,4,8,16,32,64,128,256,512}; // pow(2,x)

const Int32 wsddepthindexarray[] = {2, 3, 5, 9, 17, 33, 65, 129};
const Int32 wsdinnerindexwidth[] = {0, 0, 3, 7, 15, 31, 63, 127};

const Int32 wsdmaxdepth = 5;            //!< maximum subdivision depth possible
const Int32 wsdmaxindex = 33;		// = wsddepthindexarray[wsdmaxdepth]
const Int32 wsdmaxvarray = wsdmaxindex*wsdmaxindex; //maximum size of vertexarray

//! maximum valence
const Int32 wsdmaxvalenz = 30;

//! curvature epsilon
const Real32 wsdkruemmungseps = 0.01;

enum MeshType   { TRIANGLE, QUAD }; 
//! facing types
enum FacingType { BACK = 0, FRONT = 1, SILUETTE = 5 };

//=============================================================================

/*! \brief patch data class. All patch related data is stored in WSDdat. 
    For each patch there is a WSDdat instance.
*/
template<class WSDVector, int mtype> 
class OSG_SUBSURFACELIB_DLLMAPPING WSDdat 
{   
   typedef WSDVector WSDVectorVS[4][wsdmaxvalenz];
   //typedef WSDVector WSDVector2[wsdmaxindex][wsdmaxindex];
   //typedef WSDVector WSDVector1[wsdmaxindex];
   //typedef UInt32    WSDlimitindex[wsdmaxindex*wsdmaxindex]; //eigentlich -2, da nur innen!

   /*==========================  PUBLIC  =================================*/
public:

   //! Constructor
   WSDdat();
   //! Destructor
   ~WSDdat();

   // to switch between the two tables
   void rotateptabs(Int32 i);
   // initialize tables 
   void initptabs(void);

   /*! helper for getting a neighbor patch
       \param from the source patch index
       \param ad 1 or -1, dependant on the position ni_from+1 or ni_from-1 is the neighbor we want
       \return index to the neighbor      
   */
   Int32 getneighbor(Int32 from, Int32 ad);


   //! openmesh to wsddat cache (1-neighborhood)
   WSDVector tableO[4][4];			   
   //! for corners of valence greater than 4
   WSDVectorVS  cornerO;			   

   //! curvature dependent depth
   Int32 maxdepth;					  
   //! actual depth for current frame
   Int32 currentdepth;				      
   //! planed depth for the next frame
   Int32 solltiefe;					      

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

   //		0 ----- 3
   //		|       |
   //		|       |
   //		|       |
   //		1 ----- 2
   //! index to neighbor-patches
   Int32 neighbors[4];		

   //      3
   //		-----
   //	 0	|   |	2
   //		-----
   //		  1
   //! neighbors of neighbor
   Int32 neighbor0in1, neighbor1in0, neighbor2in1, neighbor1in2,		
      neighbor3in2, neighbor2in3, neighbor0in3, neighbor3in0;

   //! crease data for regular inner edges
   Int32 crease[12];			 

   //   -----   -----   -----
   //  |      |       |       |
   //  |      0       1       |
   //  |      |       |       |
   //   --2--   --3--   --4--
   //  |      |       |       |
   //  |      5       6       |
   //  |      |       |       |
   //   --7--   --8--   --9--
   //  |      |       |       |
   //  |      10     11       |
   //  |      |       |       |
   //   -----   -----   -----
   //! crease data for corners with valence > 4
   Int32 corcrease[4][wsdmaxvalenz];	
   //! indices to the inner-first two crease edges
   Int32 crindis[4][2];				   						      
   //! the used size of a slate
   Int32 tabsize;					      

   //! resulting orientation
   FacingType isFacing;

   //! average of the four corner vertices
   WSDVector faceloc;			
      
   //! \name some helpers
   //! \{
   //! intersteps are the offsets between patches of different depth
   Int32 interstep_l, interstep_r, interstep_o, interstep_u;

   //! helper for the gap prevention
   bool c0_o, c0_l, c1_l, c1_u, c2_r, c2_u, c3_r, c3_o;
   //! \}

   //! the offset within the vertexarray
   Int32 varrayOffset;				

   //! set inner geometry
   void setInnerGeoP(perInstanceData &insta, UInt32 &indisIn);
   //! set outer geometry
   void setOuterGeoP(perInstanceData &insta, Int32 finer_l, Int32 finer_r, 
		     Int32 finer_o, Int32 finer_u, UInt32 &indisIn);


   //! instance shared data
   sharedFields	    *mySharedFields;

   //! slate A
   static WSDVector    tableA[wsdmaxindex][wsdmaxindex];
   //! slate B
   static WSDVector    tableB[wsdmaxindex][wsdmaxindex];
   //! slate pointer A
   static WSDVector*   ptabA;
   //! slate pointer B
   static WSDVector*   ptabB;
   //! width of slate A
   static Int32			breiteA;   
   //! corners greater valence 4 in slate A
   static WSDVectorVS  cornerA;	
   //! corners greater valence 4 in slate B
   static WSDVectorVS  cornerB;	
   //! pointer corners A
   static WSDVectorVS* pcorA;
   //! pointer corners B
   static WSDVectorVS* pcorB;

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
   /*! \}                                                             */
};

OSG_END_NAMESPACE

#endif 
