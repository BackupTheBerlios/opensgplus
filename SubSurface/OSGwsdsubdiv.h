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

//#pragma warning( disable : 4661) 

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <math.h>
#include <stdio.h>
#include "OSGwsddat.h"

OSG_BEGIN_NAMESPACE

template<class WSDVector, int mtype> class WSDsubdiv;

//! subdivision algorithm interface
template<class WSDVector, int mtype>
class OSG_SUBSURFACELIB_DLLMAPPING WSDsubdiv  
{
   typedef WSDVector WSDVector2[wsdmaxindex][wsdmaxindex];
   typedef WSDVector WSDVectorVS[4][wsdmaxvalenz];
  
  /*==========================  PUBLIC  =================================*/
public:
   
   //! Constructor          
	WSDsubdiv();

   //! Destructor           
	virtual ~WSDsubdiv();

   //! subdivision entry point
	void subdiv(Int32 depth, WSDdat<WSDVector, mtype> *pp);

 /*==========================  PRIVATE  =================================*/
private:

	WSDdat<WSDVector, mtype> *ppatch; //!< current patch
	Int32 targetdepth;                //!< target subdiv depth

	void initwsdsubdiv(void);         //!< init

	void corsupdate(void);            //!< helper to keep the corners up to date
   //! \name slate row processing methods
   //! \{ 
	void firstedgerow(void);
	void edgerow(Int32 readindex, Int32 writeindex);
	void lastedgerow(Int32 readindex, Int32 writeindex);
	void inneredgerow(Int32 ri, Int32 wi, Int32& i, Int32& j);
	void firstvertexrow(void);
	void vertexrow(Int32 readindex, Int32 writeindex);
	void lastvertexrow(Int32 readindex, Int32 writeindex);
	void innervertexrow(Int32 ri, Int32 wi, Int32& i, Int32& j);   

	void vertexpoint(WSDVector& v, WSDVector& v0, WSDVector& v1, WSDVector& v2, WSDVector& v3, 
		   WSDVector& v4, WSDVector& w1, WSDVector& w2,WSDVector& w3, WSDVector& w4);
	void vertexpointmulti(WSDVector& v, WSDVector& v0, WSDVector* array, Int32 val);

	//! \name for creases
   //! \{

	void creaseinnervertexrow(Int32 ri, Int32 wi, Int32& i, Int32& j, Int32 ce);
	void vertexpointcrease(WSDVector& v, WSDVector& v0, WSDVector& v1, WSDVector& v2);   
	void edgepoint(WSDVector& v, WSDVector& v1, WSDVector& v2, 
		   WSDVector& w1, WSDVector& w2,WSDVector& w3, WSDVector& w4, Int32 crease);
   //! \}

	//! only for quads: face point
	void facepoint(WSDVector& v, WSDVector& v1, WSDVector& v2, WSDVector& v3, WSDVector& v4);
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

	void limitpointcrease(UInt32& lwi, WSDVector& v0, WSDVector& v1, 
	 WSDVector& v2);
	void limitpoint(UInt32& lwi, WSDVector& v0, WSDVector& v1, WSDVector& v2, WSDVector& v3, 
		   WSDVector& v4, WSDVector& w1, WSDVector& w2, WSDVector& w3, WSDVector& w4);

	void limitpointmulti(UInt32& lwi, WSDVector& v0, 
	 WSDVector *array, Int32 val);
   //! \}

   //! \name limit normal calculations
   //! \{
	void innernormal(UInt32& nwi, WSDVector& v0, WSDVector& v1, 
	 WSDVector& v2, WSDVector& v3, WSDVector& v4, WSDVector& w1, WSDVector& w2, WSDVector& w3, 
	 WSDVector& w4);

	void innernormalmulti(UInt32& nwi, 
	 WSDVector *array, Int32 val);

	void creasenormal(UInt32& nwi, WSDVector& v0, WSDVector *array, Int32 val, Int32 a, Int32 b);
	void regcreasenormal(UInt32& nwi, WSDVector& v0, 	 
	 WSDVector& v1, WSDVector& w1, WSDVector& v2, WSDVector& w2, WSDVector& v3);
   //! \}

};


/*****************************
    
    tableA          -------->         tableB
    
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
