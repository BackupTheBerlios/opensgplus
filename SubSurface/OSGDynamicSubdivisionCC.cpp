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

//#define OSG_COMPILEDYNAMICSUBDIVISIONCCINST

#include "OSGwsdmain.h"
#include "OSGConfig.h"
#include "OSGRenderAction.h"
#include "OSGViewport.h"
#include "OSGCamera.h"
#include "OSGPerspectiveCamera.h"

#include "OSGDynamicSubdivisionCC.h"

OSG_USING_NAMESPACE

/*! \class osg::DynamicSubdivisionCC

*/

/*----------------------- constructors & destructors ----------------------*/

template <class MESH>
DynamicSubdivisionCC<MESH>::DynamicSubdivisionCC(void) :
    Inherited()
{
}

template <class MESH>
DynamicSubdivisionCC<MESH>::DynamicSubdivisionCC(const DynamicSubdivisionCC &source) :
    Inherited(source)
{
}

template <class MESH>
DynamicSubdivisionCC<MESH>::~DynamicSubdivisionCC(void)
{
  if (getTesselator() != NULL) {
     delete getTesselator();
  }
}

/*----------------------------- class specific ----------------------------*/

template <class MESH>
void DynamicSubdivisionCC<MESH>::initMethod (void)
{
#if 0
    DrawAction::registerEnterDefault( 
        getClassType(), 
        osgTypedMethodFunctor2BaseCPtrRef<
            Action::ResultE,
            FCPtr<GroupPtr, DynamicSubdivisionCC<MESH> >  , 
            CNodePtr          ,  
            Action           *>(&DynamicSubdivisionCC<MESH>::drawEnter));
#endif	
    RenderAction::registerEnterDefault(
        getClassType(), 
        osgTypedMethodFunctor2BaseCPtrRef<
            Action::ResultE,
            FCPtr<GroupPtr, DynamicSubdivisionCC<MESH> >  , 
            CNodePtr          ,  
            Action           *>(&(DynamicSubdivisionCC<MESH>::renderEnter)));
}

template <class MESH>
void DynamicSubdivisionCC<MESH>::changed(BitVector whichField, UInt32 origin)
{
    Inherited::changed(whichField, origin);	
	
	bool update_parents = false;

	// changed MeshField: 
	// * create tesselator
	// * do tesselator preprocessing	
	if (whichField & MeshFieldMask) {					// mesh has been altered
		if (getTesselator() != NULL) {
		   delete getTesselator();
		}
		if (getMesh() != NULL) {
			setTesselator(new OpenMeshTesselator(getMesh()));
			// preprocess				
			getTesselator()->initPatches();							
			update_parents = true;						// explicit update of the instances
		}
	}
	// changed ParentsField: 
	// * create Instance-List
	// * create Geometry-nodes below
	if (update_parents || 
			((getTesselator() != NULL) && 			 
			 (whichField & ParentsFieldMask))) {			// parentsfield changed
		getTesselator()->clearInstances();				// delete old instances list
		if (getParents().size()>0 && getTesselator()->patchesready) {				
			UInt32 i;
			for (i=0; i<getParents().size(); ++i) {
			   NodePtr parent = getParents()[i];
			   getTesselator()->initInstance(i,parent);				// geoknoten erzeugen			   			   
			   parent->invalidateVolume();			   
			}			
		}			
	}

}

template <class MESH>
void DynamicSubdivisionCC<MESH>::dump(      UInt32    , 
                         const BitVector ) const
{
    SLOG << "Dump DynamicSubdivisionCC NI" << std::endl;
}


template <class MESH>
Action::ResultE DynamicSubdivisionCC<MESH>::drawEnter(Action *action)
{    
    return Group::drawEnter(action);
}


template <class MESH>
Action::ResultE DynamicSubdivisionCC<MESH>::renderEnter(Action * action)
{	
	//Group::renderEnter(action);
    RenderAction *da = dynamic_cast<RenderAction *>(action);

    if (da != NULL) {
       // get camera object
       CameraP cam = da->getCamera();
	   Matrix  camMatrix,transMatrix;	   
	   camMatrix = da->getCameraToWorld();
	   // is view port dependant data set?
	   if (!getTesselator()->isSetViewPort)
	   {
		   PerspectiveCameraP percam = dynamic_cast<PerspectiveCameraP>(cam);
		   float winkel = percam->getFov();
		   float resY = cam->getNear();

		   ViewportP vp = da->getViewport();
		   int yres = vp->getPixelHeight();
		   int xres = vp->getPixelWidth();

		   getTesselator()->initViewPortVars(winkel,xres,yres);
	   }
      // get camera position
		NodePtr parent = da->getActNode();
		transMatrix = parent->getToWorld();  						
		transMatrix.invert();
		transMatrix.mult(camMatrix);
      Vec3f eye(transMatrix[3][0], transMatrix[3][1], transMatrix[3][2]);		
      // frame setup for parent
		getTesselator()->perFrameSetup(parent, eye);
	}
    return Action::Continue;		
}

template <class MESH>
void DynamicSubdivisionCC<MESH>::adjustVolume    (Volume &volume)
{
	if (getTesselator() == NULL) {
		return;
	}

	Pnt3f boundingMin, boundingMax;
	getTesselator()->OpenMeshBoundingBox(boundingMin, boundingMax);

	volume.setValid(false);
	//volume.setStatic(false);
	volume.setEmpty();
	volume.extendBy(boundingMin);
	volume.extendBy(boundingMax);
	//volume.dump();
	//volume.setStatic();
	volume.setValid();
}

/*------------------------------------------------------------------------*/
/*                              cvs id's                                  */

#ifdef OSG_SGI_CC
#pragma set woff 1174
#endif

#ifdef OSG_LINUX_ICC
#pragma warning( disable : 177 )
#endif

#if 0
namespace
{
    static char cvsid_cpp[] = "@(#)$Id: OSGDynamicSubdivisionCC.cpp,v 1.1 2003/07/11 14:46:51 fuenfzig Exp $";
    static char cvsid_hpp[] = OSGDYNAMICSUBDIVISIONCC_HEADER_CVSID;
    static char cvsid_inl[] = OSGDYNAMICSUBDIVISIONCC_INLINE_CVSID;
}
#endif

#ifdef __sgi
#pragma reset woff 1174
#endif

