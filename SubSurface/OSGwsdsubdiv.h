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

#ifndef WSDSUBDIV_H
#define WSDSUBDIV_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <math.h>
#include <stdio.h>
#include "OSGwsddat.h"



OSG_BEGIN_NAMESPACE

//! subdivision algorithm interface
template<class WSDVector, int mtype>
class OSG_SUBSURFACELIB_DLLMAPPING WSDsubdiv  
{
   enum            { MType = mtype };
   typedef WSDVector VectorType;
   typedef WSDVector VectorCAType[4][wsdmaxvalenz];
  
  /*==========================  PUBLIC  =================================*/
public:
   
   //! Constructor          
	WSDsubdiv();

   //! Destructor           
	virtual ~WSDsubdiv();

   //! flag for texture usage
   bool useTexture;

   //! maximum tesselation depth
   Int32 wsdmaxdepth;

   //! subdivision entry point
	void subdiv(Int32 depth, WSDdat<VectorType, MType> *pp);

 /*==========================  PRIVATE  =================================*/
private:

	WSDdat<VectorType, MType> *ppatch; //!< current patch
	Int32 targetdepth;                //!< target subdiv depth

	void initwsdsubdiv(void);         //!< init

	void corsupdate(void);            //!< helper to keep the corners up to date (for cc only)
   void allcorners(void);            //!< helper to calc. all edges around the 4 corners
   //! \name slate row processing methods
   //! \{    
	void firstedgerow(void);
	void edgerow(Int32 readindex, Int32 writeindex);
	void lastedgerow(Int32 readindex, Int32 writeindex);
	void inneredgerow(Int32 ri, Int32 wi, Int32& i, Int32& j);
   void tricrease4_inneredgerow(Int32 ri, Int32 wi, Int32& i, Int32& j);
   void first_last_inneredgerow(Int32 ri, Int32 wi, Int32& i, Int32& j);
	void firstvertexrow(void);
	void vertexrow(Int32 readindex, Int32 writeindex);
	void lastvertexrow(Int32 readindex, Int32 writeindex);   
	void innervertexrow(Int32 ri, Int32 wi, Int32& i, Int32& j);   
   void tricrease4_innervertexrow(Int32 ri, Int32 wi, Int32& i, Int32& j);   

	void vertexpoint(VectorType& v, VectorType& v0, VectorType& v1, VectorType& v2, VectorType& v3, 
		   VectorType& v4, VectorType& w1, VectorType& w2,VectorType& w3, VectorType& w4);
	void vertexpointmulti(VectorType& v, VectorType& v0, VectorType* array, Int32 val);

   //! \name for texture coords
   //! \{
 	void edgerowtex(Int32 readindex, Int32 writeindex);
   void vertexrowtex(Int32 readindex, Int32 writeindex);
   void textureCoords();    //!< main method for texcoords calc.
   //! \}

	//! \name for creases
   //! \{
	void creaseinnervertexrow(Int32 ri, Int32 wi, Int32& i, Int32& j, Int32 ce);
	void vertexpointcrease(VectorType& v, VectorType& v0, VectorType& v1, VectorType& v2);   
	void edgepoint(VectorType& v, VectorType& v1, VectorType& v2, 
		   VectorType& w1, VectorType& w2,VectorType& w3, VectorType& w4, Int32 crease);
   //! \}

	//! only for quads: face point
	void facepoint(VectorType& v, VectorType& v1, VectorType& v2, VectorType& v3, VectorType& v4);
   //! \}

   //! \name limit point calculations
   //! \{
	void limits();    //!< entry point

	void firstlimitrow(UInt32 &lwi);
	void limitrow(Int32 ri, UInt32 &lwi);
	void lastlimitrow(UInt32 &lwi);
	void innerlimitrow(Int32 ri, UInt32& lwi, Int32& i, UInt32& nwi);

	void creaseinnerlimitfirstrow(Int32 ri, UInt32& lwi, Int32& i, UInt32& nwi);
	void creaseinnerlimitlastrow(Int32 ri, UInt32& lwi, Int32& i, UInt32& nwi);

	void limitpointcrease(UInt32& lwi, VectorType& v0, VectorType& v1, 
	 VectorType& v2);
   void limitpointcreaseirreg(UInt32& lwi, VectorType& v0, VectorType& v1, 
	 VectorType& v2);
	void limitpoint(UInt32& lwi, VectorType& v0, VectorType& v1, VectorType& v2, VectorType& v3, 
		   VectorType& v4, VectorType& w1, VectorType& w2, VectorType& w3, VectorType& w4);

	void limitpointmulti(UInt32& lwi, VectorType& v0, 
	 VectorType *array, Int32 val);
   //! \}

   //! \name limit normal calculations
   //! \{
	void innernormal(UInt32& nwi, VectorType& v0, VectorType& v1, 
	 VectorType& v2, VectorType& v3, VectorType& v4, VectorType& w1, VectorType& w2, VectorType& w3, 
	 VectorType& w4);

	void innernormalmulti(UInt32& nwi, 
	 VectorType *array, Int32 val);

	void creasenormal(UInt32& nwi, VectorType& v0, VectorType *array, Int32 val, Int32 a, Int32 b);
	void regcreasenormal(UInt32& nwi, VectorType& v0, 	 
	 VectorType& v1, VectorType& w1, VectorType& v2, VectorType& w2, VectorType& v3);
   //! \}

   //! Helper for Loop-Crease-Types
   Int32 getCreaseType(Int32 crease, Int32 C0, Int32 C1, Int32 ri);

};


/*****************************
    
    slateA          -------->         slateB
    
..---*---*---*---..     
 |   |   |   |   |        
 |   |   |   |   |  firstedgerow    ..-*-*-*-*-*-..
 |   |   |   |   |                   ' ' ' ' ' ' '
 *---*---*---*---*  firstvertexrow   *-*-*-*-*-*-*
 |   |   |   |   |                   ' ' ' ' ' ' '
 |   |   |   |   |  edgerow          *-*-*-*-*-*-*
 |   |   |   |   |                   ' ' ' ' ' ' '
 *---*---*---*---*  vertexrow        *-*-*-*-*-*-*
 |   |   |   |   |                   ' ' ' ' ' ' '   
 |   |   |   |   |  edgerow          *-*-*-*-*-*-*
 |   |   |   |   |                   ' ' ' ' ' ' '
 *---*---*---*---*  lastvertexrow    *-*-*-*-*-*-*
 |   |   |   |   |                   ' ' ' ' ' ' ' 
 |   |   |   |   |  lastedgerow     ..-*-*-*-*-*-..
 |   |   |   |   |
..---*---*---*---..


******************************/

OSG_END_NAMESPACE

#endif 
