//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//                                License                                     
//                                                                            
//   This library is free software; you can redistribute it and/or modify it 
//   under the terms of the GNU Library General Public License as published  
//   by the Free Software Foundation, version 2.                             
//                                                                             
//   This library is distributed in the hope that it will be useful, but       
//   WITHOUT ANY WARRANTY; without even the implied warranty of                
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         
//   Library General Public License for more details.                          
//                                                                            
//   You should have received a copy of the GNU Library General Public         
//   License along with this library; if not, write to the Free Software       
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.3 $
//   $Date: 2003/10/13 09:46:34 $
//                                                                            
//=============================================================================

#ifndef OSGGVSPATIALIZEFUNCTIONS_H
#define OSGGVSPATIALIZEFUNCTIONS_H

#include "OSGGVBase.h"
#include "OSGGVInput.h"
#include "OSGGVBVolHierarchy.h"
#include "OSGGVSpatialize.h"
#include "OSGGVFaceSpatialize.h"
#include "OSGGVGenvisPreprocAction.h"
#include "OSGGVMerge.h"
#include "OSGGVOracle.h"

BEGIN_GENVIS_NAMESPACE

template <class BVOL>
inline NodePtr objectSpatialize (const NodePtr& orgFile,
				 unsigned levels, unsigned prims, Oracle* oracle,
				 unsigned mergeLevels=0,
				 bool     modifyCache=true)
{
   NodePtr file;
   // fill cache
   if (modifyCache) {
     OSGCache::the().clear();
     GenvisPreprocAction().apply(orgFile);
   }
   {
     // build bvol-hierarchy on faces
     ObjectBVolHierarchy<OpenSGTraits,OpenSGObjectInput<BVOL> > hierarchy;
     hierarchy.setParameter(oracle, levels, prims);
     OSGCache::the().setHierarchy (&hierarchy);
     OSGCache::the().apply(orgFile);
     unsigned depth = hierarchy.hierarchy();
     // merge hierarchy levels
     MergeLevels(mergeLevels).merge(hierarchy.getRoot()); 

     // use hierarchy to spatialize
     SingleEnterLeaveTraverser<OpenSGTraits,SpatializeTraits<OpenSGTraits> > act;
     act.getData().setLevel(depth >> mergeLevels);
     act.apply(OSGCache::the()[orgFile], hierarchy.getRoot(), OSGCache::the()[orgFile].getToWorldMatrix()); 
     file = act.getData().getRoot(); 

     hierarchy.destroy();
   }
   // clear cache
   if (modifyCache) {
      OSGCache::the().clear();
   }
   return file;
}

template <class BVOL>
inline NodePtr faceSpatialize (const NodePtr& orgFile,
			       unsigned levels, unsigned prims, Oracle* oracle,
			       unsigned mergeLevels=0,
			       bool     modifyCache=true)
{
   NodePtr file;
   // fill cache
   if (modifyCache) {
     OSGCache::the().clear();
     GenvisPreprocAction().apply(orgFile);
   }
   {
     // build bvol-hierarchy on faces
     FaceBVolHierarchy<OpenSGTraits,OpenSGFaceInput<BVOL> > hierarchy;
     hierarchy.setParameter(oracle, levels, prims);
     OSGCache::the().setHierarchy (&hierarchy);
     OSGCache::the().apply(orgFile);
     unsigned depth = hierarchy.hierarchy();
     // merge hierarchy levels
     MergeLevels(mergeLevels).merge(hierarchy.getRoot()); 

     // use hierarchy to spatialize
     SingleEnterLeaveTraverser<OpenSGTraits,FaceSpatializeTraits<OpenSGTraits> > act;
     act.getData().setLevel(depth >> mergeLevels);
     act.apply(OSGCache::the()[orgFile], hierarchy.getRoot(), OSGCache::the()[orgFile].getToWorldMatrix()); 
     file = act.getData().getRoot(); 

     hierarchy.destroy();
   }
   // clear cache
   if (modifyCache) {
      OSGCache::the().clear();
   }
   return file;
}

template <class BVOL>
inline NodePtr faceSpatialize2 (const NodePtr& orgFile,
				unsigned levels, unsigned prims, Oracle* oracle,
				unsigned mergeLevels=0)
{
   NodePtr file;
   // fill cache
   {
     OSGCache::the().clear();
     GenvisPreprocAction().apply(orgFile);
   }
   {
     // build face adapters
     typedef FaceBVolHierarchy<OpenSGTraits,OpenSGFaceInput<BVOL> > HierarchyType;
     typedef typename HierarchyType::AdapterType                    AdapterType;
     HierarchyType hierarchy;
     hierarchy.setParameter(oracle, levels, prims);
     OSGCache::the().setHierarchy (&hierarchy);
     OSGCache::the().apply(orgFile);
     // spatialize
     SingleEnterLeaveTraverser<OpenSGTraits,FaceSpatializeTraits<OpenSGTraits> > act;
     act.getData().setFlat(true);

     // build bvol-hierarchy on faces for each node separately
     SINFO << "phase 1:" << std::endl;
     unsigned i = 0;
     for (OSGCache::EntryIterator it = OSGCache::the().begin();
	  it != OSGCache::the().end();
	  ++it, ++i) {
       OSGCache::CacheData& data = it->second;
       NodePtr node = data.getNode();
       if (node != NullFC && GeometryPtr::dcast(node->getCore()) != NullFC) {
 	 // do not process nodes with less than prims faces
	 if (data.getAdapter(AdapterType::getAdapterId()).size() <= prims) {
	   continue;
	 }
	 // no parent: impossible!
	 NodePtr parent = data.getParent();
	 if (parent == NullFC) {
	   continue;
	 }

	 SINFO << i << "/" << data.getAdapter(AdapterType::getAdapterId()).size()
	       << std::endl;
	 hierarchy.clear();

	 unsigned depth = hierarchy.hierarchyNode(node);

	 // restructure
	 act.getData().setLevel(depth);
	 act.apply(data, hierarchy.getRoot(), data.getToWorldMatrix()); 

	 file = act.getData().getRoot(); 
	 beginEditCP(parent);
	 parent->replaceChildBy(node, file);
	 endEditCP(parent);
       }
     }
     hierarchy.destroy();
   }
   // fill cache
   {
     OSGCache::the().clear();
     GenvisPreprocAction().apply(orgFile);
   }
   SINFO << "phase 2" << std::endl;
   // build bvol-hierarchy on nodes
   {
     typedef ObjectBVolHierarchy<OpenSGTraits,OpenSGObjectInput<BVOL> > HierarchyType;
     HierarchyType hierarchy;
     hierarchy.setParameter(oracle, levels, 2);
     OSGCache::the().setHierarchy (&hierarchy);
     OSGCache::the().apply(orgFile);

     unsigned depth = hierarchy.hierarchy();
     MergeLevels(mergeLevels).merge(hierarchy.getRoot()); 

     SingleEnterLeaveTraverser<OpenSGTraits,SpatializeTraits<OpenSGTraits> > act;
     act.getData().setLevel(depth >> mergeLevels);
     act.apply(OSGCache::the()[orgFile], hierarchy.getRoot(), OSGCache::the()[orgFile].getToWorldMatrix()); 
     file = act.getData().getRoot(); 
     hierarchy.destroy();
   }
   // clear cache
   {
     OSGCache::the().clear();
   }
   return file;
}

END_GENVIS_NAMESPACE

#endif

