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
//   $\Revision$
//   $\Date$
//                                                                            
//=============================================================================

#ifndef OSGGVSPATIALIZEFUNCTIONS_H
#define OSGGVSPATIALIZEFUNCTIONS_H

#include "OSGGVBase.h"
#include "OSGGVInput.h"
#include "OSGGVSpatialize.h"
#include "OSGGVFaceSpatialize.h"
#include "OSGGVGenvisPreprocAction.h"
#include "OSGGVMerge.h"
#include "OSGGVOracle.h"

BEGIN_GENVIS_NAMESPACE

template <class BVOL>
inline NodePtr objectSpatialize (const NodePtr& orgFile,
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
   OSGCache::the().clear();
   return file;
}
template <class BVOL>
inline NodePtr faceSpatialize (const NodePtr& orgFile,
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
   OSGCache::the().clear();
   return file;
}

template <class BVOL>
inline NodePtr faceSpatialize2 (const NodePtr& orgFile,
				unsigned levels, unsigned prims, Oracle* oracle,
				unsigned mergeLevels=0)
{
   NodePtr file
   // fill cache
   {
     OSGCache::the().clear();
     GenvisPreprocAction().apply(orgFile);
   }
   {
     // build face adapters
     typedef FaceBVolHierarchy<OpenSGTraits,OpenSGFaceInput<BVOL> > AdapterType;
     FaceBVolHierarchy<OpenSGTraits,OpenSGFaceInput<BVOL> > hierarchy;
     hierarchy.setParameter(oracle, levels, prims);
     OSGCache::the().setHierarchy (&hierarchy);
     OSGCache::the().apply(orgFile);
     // spatialize
     SingleEnterLeaveTraverser<OpenSGTraits,FaceSpatializeTraits<OpenSGTraits> > act;
     act.getData().setFlat(true);

     // build bvol-hierarchy on faces for each node separately
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

	 std::cout << i << "/" << data.getAdapter(AdapterType::getAdapterId()).size()
		   << ": ";
	 hierarchy.clear();
	 std::cout << "*" << std::flush;
	 unsigned depth = hierarchy.hierarchyNode(node);
	 std::cout << depth << "*" << std::flush;
	 // restructure
	 act.getData().setLevel(depth);
	 act.apply(data, hierarchy.getRoot(), data.getToWorldMatrix()); 
	 std::cout << "*" << std::flush;
	 file = act.getData().getRoot(); 
	 beginEditCP(parent);
	 parent->replaceChildBy(node, file);
	 endEditCP(parent);
	 std::cout << "*" << std::endl;
       }
     }
     hierarchy.destroy();
   }
   std::cout << "*" << std::flush;
   // fill cache
   {
     OSGCache::the().clear();
     GenvisPreprocAction().apply(orgFile);
   }
   std::cout << "*" << std::flush;
   // build bvol-hierarchy on nodes
   {
     ObjectBVolHierarchy<OpenSGTraits,OpenSGObjectInput<BVOL> > hierarchy;
     hierarchy.setParameter(oracle, levels, 2);
     OSGCache::the().setHierarchy (&hierarchy);
     OSGCache::the().apply(orgFile);
     unsigned depth = hierarchy.hierarchy();
     MergeLevels(mergeLevels).merge(hierarchy.getRoot()); 
     std::cout << "*" << std::flush;

     SingleEnterLeaveTraverser<OpenSGTraits,SpatializeTraits<OpenSGTraits> > act;
     act.getData().setLevel(depth >> mergeLevels);
     act.apply(OSGCache::the()[orgFile], hierarchy.getRoot(), OSGCache::the()[orgFile].getToWorldMatrix()); 
     file = act.getData().getRoot(); 
     std::cout << "*" << std::flush;
     hierarchy.destroy();
     std::cout << "*" << std::endl;
   }
   // clear cache
   OSGCache::the().clear();
   return file;
}

END_GENVIS_NAMESPACE

#endif
