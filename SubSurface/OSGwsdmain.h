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

#ifndef WSDMAIN_H
#define WSDMAIN_H

//#pragma warning( disable : 4661) 

#include <math.h>
#include <stdio.h>
#include <vector>
#include "OSGwsddat.h"
#include "OSGwsdmesh2dat.h"
#include "OSGwsdsubdiv.h"

OSG_BEGIN_NAMESPACE

//! datastruct for vertices
template<class WSDVector, class Mesh> 
struct OSG_SUBSURFACELIB_DLLMAPPING SVertexData
{
	typename Mesh::VertexHandle vh;
	FacingType					ft;			// back, siluette or front
	WSDVector					limPo;
	WSDVector					limNo;
	Int32							maxdepth;
};
//! struct for quick subdivision while approx. the curvature 
template <class Mesh> 
struct OSG_SUBSURFACELIB_DLLMAPPING workArray
{
	typename Mesh::Point		p;
	typename Mesh::FaceHandle	fh;
	Int32							isCrease;
};

/*! Tesselator class.
*/
template<class WSDVector, class Mesh, int mtype>
class OSG_SUBSURFACELIB_DLLMAPPING WSDmain  
{
   typedef workArray<Mesh>		 tWorkArray[wsdmaxvalenz*2];
   typedef typename Mesh::FaceHandle	 FaceHandle;
   typedef typename Mesh::HalfedgeHandle HalfedgeHandle;
   typedef typename Mesh::VertexHandle	 VertexHandle;
   typedef typename Mesh::Point		 OMPoint;

   typedef std::vector<perInstanceData>	 perInstanceDataContainer;
   typedef WSDdat<WSDVector, mtype>	 PatchData;
   typedef std::vector<PatchData>	 PatchDataContainer;

   /*==========================  PUBLIC  =================================*/
public:
   /*! Constructor                                */
   WSDmain(Mesh *m);
   
   /*! Destructors                                */   
   virtual ~WSDmain();
   
   sharedFields			mySharedFields;		//!< Limitpoints, -normals are being shared
   perInstanceDataContainer	myInstances;		   //!< "per parent one Instance"-data

   void initPatches();								      //!< init patches (preprocessing)
   void initInstance(Int32 n, OSG::NodePtr& parent);	//!< init instances
   void clearInstances(void);						      //!< completely remove instances

   //! output method where the geometry is set up
   void perFrameSetup(OSG::NodePtr& parent, OSG::Vec3f eyepoint);

   //! call through for WSDmesh2dat::getBoundingBox   
   void OpenMeshBoundingBox(OSG::Pnt3f &boundingMin, OSG::Pnt3f &boundingMax);

   //! FaceIndies-list
   FaceHandle *OmeshFaceListe;  
   //! returns index into OmeshFaceListe for face handle
   Int32 getFaceIndex(FaceHandle &fh);  

   //! Vertexlist
   std::vector<SVertexData<WSDVector, Mesh> > VertexListe;

   bool patchesready;			//!< flag: are the patches ready?
   bool isSetViewPort;			//!< flag: are the viewport parameters set?

   //! \name for the projected patch size
   //! \{

   //! setup the viewport parameters
   //! \param fovy field of view in y direction
   //! \param resX viewport width in pixel
   //! \param resY viewport height in pixel
   void initViewPortVars(Real32 fovy, Int32 resX, Int32 resY);
   //! \}

   Real32 gamma_winkel;			//!< angle for one pixel

   //! \name projected size parameters
   //! \{

   Real32 hmin,hmax,hminSil;	
   Real32 Pmin,Pmax,PminSil;   
   Real32 silEpsi;				//!< silhouette epsilon
   //! \}

   //! \name for the curvature approximation:
   //! \{
   
   //! \param pmesh pointer to base mesh
   //! \param v_h vertex for calculation
   //! for a given vertex return the optimal depth
   void getOptiDepth(Mesh *pmesh, VertexHandle v_h);
   //! \}

 /*==========================  PRIVATE  =================================*/
 private:
   Mesh *pmesh;			       //!< the mesh
   PatchDataContainer patches;	       //!< the patches

   //! class for reading the mesh data
   WSDmesh2dat<WSDVector, Mesh, mtype>  mesh2wsd;	
   //! class containing the subdivision algorithms
   WSDsubdiv<WSDVector, mtype> subdivwsd;

   //! \name for the curvature calculation
   //! \{
   OMPoint NormCCinner(tWorkArray* wara_, Int32 v);
   OMPoint NormCCcrease(OMPoint alpha_, tWorkArray* wara_, Int32 v, Int32 g, Int32 h);
   void simpleSubdiv(tWorkArray* wara_, tWorkArray* waB_, OMPoint &a, Int32 i);
   void simpleCreaseSubdiv(tWorkArray* wara_, tWorkArray* waB_, OMPoint &a,
							   Int32 g, Int32 h, Int32 i);
   Int32 getLimOffset(WSDdat<WSDVector, mtype> *pp, OMPoint a);
   Real32 getGreatestAngle(tWorkArray* wara_, OMPoint &a, OMPoint &n, Int32 s, Int32 num, Int32 i);

   //! \}

   //! \name some helpers
   //! \{

   void collectNeighbors(FaceHandle f_h,WSDdat<WSDVector, mtype> *pp);   

   void initOneGeo(perInstanceData *instance);				//!< OpenSG - Geometry init (per Instance)

   Int32 getIndex(OSG::NodePtr &p);
   //! \}
};

OSG_END_NAMESPACE

#endif 
