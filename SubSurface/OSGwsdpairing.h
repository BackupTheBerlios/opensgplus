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

#ifndef WSDPAIRING_H
#define WSDPAIRING_H

//#pragma warning( disable : 4661) 

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <math.h>
#include <stdio.h>
#include <vector>
#include "OSGwsddat.h"

OSG_BEGIN_NAMESPACE


//! struct for set of faces
template <class Mesh> 
struct OSG_SUBSURFACELIB_DLLMAPPING PairFaceDataTemplate
{	
   typedef Mesh                                 MeshType;
   typedef typename  MeshType::FaceHandle       FaceHandle;
	FaceHandle	fh;      //!< facehandle into openmesh face
   Int32       partner;                      //!< -1 if not yet connected
   FaceHandle	fh2;     //!< facehandle into openmesh partner-face
	Int32	      n[3];                         //!< index to neighbor
   Int32       freeneighbors;                //!< number of free neighbors
   Int32       prio;                         //!< priority class
};

//! triangle pairing algo
template<class WSDVector, class Mesh>
class OSG_SUBSURFACELIB_DLLMAPPING WSDpairing
{
  /*==========================  PUBLIC  =================================*/
public:
   typedef Mesh                                 MeshType;
   typedef typename  MeshType::FaceHandle       FaceHandle;
   typedef PairFaceDataTemplate<MeshType>       PairFaceData;
   typedef std::vector<PairFaceData>            PairFaceDataContainer;
   typedef std::vector<Int32>	                  IndexContainer;
   

      
   //! Constructor          
	WSDpairing( MeshType *_mesh );

   //! Destructor           
	virtual ~WSDpairing();

   //! entry point
	void buildPairs( void );

   PairFaceDataContainer pairs;        //!< finished pairing list
 
 /*==========================  PRIVATE  =================================*/
private:

   MeshType *mesh;	
   PairFaceDataContainer prepairs;
   
   Int32 getIndexOfFace(FaceHandle fh);
   IndexContainer::iterator getIterator(IndexContainer &v, Int32 i);
};



OSG_END_NAMESPACE

#endif 
