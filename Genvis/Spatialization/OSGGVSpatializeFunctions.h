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
//   $Revision: 1.4 $
//   $Date: 2004/03/12 13:31:38 $
//                                                                            
//=============================================================================

#ifndef OSGGVSPATIALIZEFUNCTIONS_H
#define OSGGVSPATIALIZEFUNCTIONS_H

#include "OSGGVBase.h"
#include "OSGGVInput.h"
#include "OSGGVBVolHierarchy.h"
#include "OSGGVSpatialize.h"
#include "OSGGVFaceSpatialize.h"
#include "OSGGVFaceSpatializeIndexed.h"
#include "OSGGVMerge.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief Spatialize the scenegraph subtree, given by the root node, at object level.
    The geometry nodes are not subdivided, they are just organized into a spatial hierarchy. 
    The parameters for hierarchy creation are:
    The maximum number of levels, the maximum number of primitives, the number of level merges,
    and the name of the creation oracle, \see Oracle.
 */
template <class BVOL>
inline NodePtr objectSpatialize (const NodePtr& root,
				 u32 maxDepth, u32 maxPrim, const char* name,
				 u32 numMerges=0,
				 bool modifyCache=true)
{
   NodePtr newRoot;
   // fill cache
   if (modifyCache) {
     OSGCache::the().clear();
     OSGCache::the().setHierarchy(NULL);
     OSGCache::the().apply(root);
   }
   {
     // build bvol-hierarchy on objects
     ObjectBVolHierarchy<OpenSGTraits,OpenSGObjectInput<BVOL> > hierarchy;
     hierarchy.setParameter(name, maxDepth, maxPrim);
     OSGCache::the().setHierarchy (&hierarchy);
     OSGCache::the().apply(root);
     u32 depth = hierarchy.getDepth();
     // merge hierarchy levels
     {
     SingleEnterLeaveTraverser<OpenSGTraits,MergeTraits<OpenSGTraits> > act;
     act.getData().setNumLevels(numMerges);
     act.apply(OSGCache::the()[root], hierarchy.getRoot(), OSGCache::the()[root].getToWorldMatrix()); 
     }
     // use hierarchy to spatialize
     {
     SingleEnterLeaveTraverser<OpenSGTraits,SpatializeTraits<OpenSGTraits> > act;
     act.getData().setDepth(depth >> numMerges);
     act.apply(OSGCache::the()[root], hierarchy.getRoot(), OSGCache::the()[root].getToWorldMatrix()); 
     newRoot = act.getData().getRoot(); 
     }
     hierarchy.destroy();
   }
   // clear cache
   if (modifyCache) {
      OSGCache::the().clear();
   }
   return newRoot;
}

/*! \brief Spatialize the scenegraph subtree, given by the root node, at face level.
    The geometry nodes are subdivided and the faces are organized into a spatial hierarchy. 
    The parameters for hierarchy creation are:
    The maximum number of levels, the maximum number of primitives, the number of level merges,
    and the name of the creation oracle, \see Oracle.
 */
template <class BVOL>
inline NodePtr faceSpatialize (const NodePtr& root,
			       u32 maxDepth, u32 maxPrim, const char* name,
			       u32 numMerges=0,
			       bool     modifyCache=true)
{
   NodePtr newRoot;
   // fill cache
   if (modifyCache) {
     OSGCache::the().clear();
     OSGCache::the().setHierarchy(NULL);
     OSGCache::the().apply(root);
   }
   {
     // build bvol-hierarchy on faces
     FaceBVolHierarchy<OpenSGTraits,OpenSGFaceInput<BVOL> > hierarchy;
     hierarchy.setParameter(name, maxDepth, maxPrim);
     OSGCache::the().setHierarchy (&hierarchy);
     OSGCache::the().apply(root);
     u32 depth = hierarchy.getDepth();
     // merge hierarchy levels
     {
     SingleEnterLeaveTraverser<OpenSGTraits,MergeTraits<OpenSGTraits> > act;
     act.getData().setNumLevels(numMerges);
     act.apply(OSGCache::the()[root], hierarchy.getRoot(), OSGCache::the()[root].getToWorldMatrix()); 
     }
     // use hierarchy to spatialize
     {
     SingleEnterLeaveTraverser<OpenSGTraits,FaceSpatializeTraits<OpenSGTraits> > act;
     act.getData().setDepth(depth >> numMerges);
     act.apply(OSGCache::the()[root], hierarchy.getRoot(), OSGCache::the()[root].getToWorldMatrix()); 
     newRoot = act.getData().getRoot(); 
     }
     hierarchy.destroy();
   }
   // clear cache
   if (modifyCache) {
      OSGCache::the().clear();
   }
   return newRoot;
}
/*! \brief Spatialize the scenegraph subtree, given by the root node, at face level.
    The geometry nodes are subdivided and the faces are organized into a spatial hierarchy. 
    The parameters for hierarchy creation are:
    The maximum number of levels, the maximum number of primitives, the number of level merges,
    and the name of the creation oracle, \see Oracle.
 */
template <class BVOL>
inline NodePtr faceSpatializeIndexed (const NodePtr& root,
				      u32 maxDepth, u32 maxPrim, const char* name,
				      u32 numMerges=0,
				      bool modifyCache=true)
{
   NodePtr newRoot;
   // fill cache
   if (modifyCache) {
     OSGCache::the().clear();
     OSGCache::the().setHierarchy(NULL);
     OSGCache::the().apply(root);
   }
   {
     // build bvol-hierarchy on faces
     FaceBVolHierarchy<OpenSGTraits,OpenSGFaceInput<BVOL> > hierarchy;
     hierarchy.setParameter(name, maxDepth, maxPrim);
     OSGCache::the().setHierarchy (&hierarchy);
     OSGCache::the().apply(root);
     u32 depth = hierarchy.getDepth();
     // merge hierarchy levels
     {
     SingleEnterLeaveTraverser<OpenSGTraits,MergeTraits<OpenSGTraits> > act;
     act.getData().setDepth(numMerges);
     act.apply(OSGCache::the()[root], hierarchy.getRoot(), OSGCache::the()[root].getToWorldMatrix()); 
     }
     // use hierarchy to spatialize
     {
     SingleEnterLeaveTraverser<OpenSGTraits,FaceSpatializeIndexedTraits<OpenSGTraits> > act;
     act.getData().setDepth(depth >> numMerges);
     act.apply(OSGCache::the()[root], hierarchy.getRoot(), OSGCache::the()[root].getToWorldMatrix()); 
     newRoot = act.getData().getRoot(); 
     }
     hierarchy.destroy();
   }
   // clear cache
   if (modifyCache) {
      OSGCache::the().clear();
   }
   return newRoot;
}

/*! \brief Spatialize the scenegraph subtree, given by the root node, at face level.
    The geometry nodes are subdivided and the faces are organized into a spatial hierarchy. 
    Here a two pass algorithm is used which is more memory efficient, \see objectSpatialize.
    The parameters for hierarchy creation are:
    The maximum number of levels, the maximum number of primitives, the number of level merges,
    and the name of the creation oracle, \see Oracle.
 */
template <class BVOL>
inline NodePtr spatialize (const NodePtr& root,
			   u32 maxDepth, u32 maxPrim, const char* name,
			   u32 numMerges=0)
{
   NodePtr newRoot;
   // fill cache
   {
     OSGCache::the().clear();
     OSGCache::the().setHierarchy(NULL);
     OSGCache::the().apply(root);
   }
   {
     typedef FaceBVolHierarchy<OpenSGTraits,OpenSGFaceInput<BVOL> > HierarchyType;
     typedef typename HierarchyType::AdapterType                    AdapterType;
     // build face adapters
     HierarchyType hierarchy;
     hierarchy.setParameter(name, maxDepth, maxPrim);
     OSGCache::the().setHierarchy (&hierarchy);
     OSGCache::the().apply(root);
     // spatialize
     SingleEnterLeaveTraverser<OpenSGTraits,FaceSpatializeTraits<OpenSGTraits> > act;
     act.getData().setFlat(true);

     // build bvol-hierarchy on faces for each node separately
     u32 i = 0;
     for (OSGCache::EntryIterator it = OSGCache::the().begin();
	  it != OSGCache::the().end();
	  ++it, ++i) {
       OSGCache::CacheData& data = OSGCache::the()[*it];
       NodePtr     node = data.getNode();
       if (node == NullFC) {
	  continue;
       }
       GeometryPtr geom = GeometryPtr::dcast(node->getCore());
       if (geom == NullFC) {
 	  continue;
       }
       // no parent: impossible!
       NodePtr parent = data.getParent();
       if (parent == NullFC) {
	  continue;
       }
       
       // delete hierarchy
       hierarchy.clear();
       // omit nodes with less than prims faces
       if (data.getAdapter(AdapterType::getAdapterId()).size() <= maxPrim) {
	  continue;
       }
       // build hierarchy
       hierarchy.hierarchy(node);
       u32 depth = hierarchy.getDepth();
       SLOG << "restructure " << i << "(" << data.getAdapter(AdapterType::getAdapterId()).size() 
	    << " faces) up to depth " << depth << std::endl;
       // restructure
       act.getData().setDepth(depth);
       act.apply(data, hierarchy.getRoot(), data.getToWorldMatrix()); 
       newRoot = act.getData().getRoot(); 
       beginEditCP(parent);
       parent->replaceChildBy(node, newRoot);
       endEditCP(parent);
     }
     hierarchy.destroy();
   }
   // fill cache
   {
     OSGCache::the().clear();
     OSGCache::the().setHierarchy(NULL);
     OSGCache::the().apply(root);
   }
   // build bvol-hierarchy on nodes
   {
     typedef ObjectBVolHierarchy<OpenSGTraits,OpenSGObjectInput<BVOL> > HierarchyType;
     // build object adapters
     HierarchyType hierarchy;
     hierarchy.setParameter(name, maxDepth, 2);
     OSGCache::the().setHierarchy (&hierarchy);
     OSGCache::the().apply(root);
     hierarchy.hierarchy();
     u32 depth = hierarchy.getDepth();
     {
     SingleEnterLeaveTraverser<OpenSGTraits,MergeTraits<OpenSGTraits> > act;
     act.getData().setNumLevels(numMerges);
     act.apply(OSGCache::the()[root], hierarchy.getRoot(), OSGCache::the()[root].getToWorldMatrix()); 
     }
     {
     SingleEnterLeaveTraverser<OpenSGTraits,SpatializeTraits<OpenSGTraits> > act;
     act.getData().setDepth(depth >> numMerges);
     act.apply(OSGCache::the()[root], hierarchy.getRoot(), OSGCache::the()[root].getToWorldMatrix()); 
     newRoot = act.getData().getRoot(); 
     }
     hierarchy.destroy();
   }
   // clear cache
   {
      OSGCache::the().clear();
   }
   return newRoot;
}
/*! \brief Spatialize the scenegraph subtree, given by the root node, at face level.
    The geometry nodes are subdivided and the faces are organized into a spatial hierarchy. 
    The parameters for hierarchy creation are:
    The maximum number of levels, the maximum number of primitives, the number of level merges,
    and the name of the creation oracle, \see Oracle.
 */
template <class BVOL>
inline NodePtr spatializeIndexed (const NodePtr& root,
				  u32 maxDepth, u32 maxPrim, const char* name,
				  u32 numMerges=0)
{
   NodePtr newRoot;
   // fill cache
   {
     OSGCache::the().clear();
     OSGCache::the().setHierarchy(NULL);
     OSGCache::the().apply(root);
   }
   {
     typedef FaceBVolHierarchy<OpenSGTraits,OpenSGFaceInput<BVOL> > HierarchyType;
     typedef typename HierarchyType::AdapterType                    AdapterType;
     // build face adapters
     HierarchyType hierarchy;
     hierarchy.setParameter(name, maxDepth, maxPrim);
     OSGCache::the().setHierarchy (&hierarchy);
     OSGCache::the().apply(root);
     // spatialize
     SingleEnterLeaveTraverser<OpenSGTraits,FaceSpatializeIndexedTraits<OpenSGTraits> > act;
     act.getData().setFlat(true);

     // build bvol-hierarchy on faces for each node separately
     u32 i = 0;
     for (OSGCache::EntryIterator it = OSGCache::the().begin();
	  it != OSGCache::the().end();
	  ++it, ++i) {
       OSGCache::CacheData& data = OSGCache::the()[*it];
       NodePtr     node = data.getNode();
       if (node == NullFC) {
	  continue;
       }
       GeometryPtr geom = GeometryPtr::dcast(node->getCore());
       if (geom == NullFC) {
 	  continue;
       }
       // no parent: impossible!
       NodePtr parent = data.getParent();
       if (parent == NullFC) {
	  continue;
       }
       
       // delete hierarchy
       hierarchy.clear();
       // omit nodes with less than prims faces
       if (data.getAdapter(AdapterType::getAdapterId()).size() <= maxPrim) {
	  continue;
       }
       // build hierarchy
       hierarchy.hierarchy(node);
       u32 depth = hierarchy.getDepth();
       SLOG << "restructure " << i << "(" << data.getAdapter(AdapterType::getAdapterId()).size() 
	    << " faces) up to depth " << depth << std::endl;
       // restructure
       act.getData().setDepth(depth);
       act.apply(data, hierarchy.getRoot(), data.getToWorldMatrix()); 
       newRoot = act.getData().getRoot(); 
       beginEditCP(parent);
       parent->replaceChildBy(node, newRoot);
       endEditCP(parent);
     }
     hierarchy.destroy();
   }
   // fill cache
   {
     OSGCache::the().clear();
     OSGCache::the().setHierarchy(NULL);
     OSGCache::the().apply(root);
   }
   // build bvol-hierarchy on nodes
   {
     typedef ObjectBVolHierarchy<OpenSGTraits,OpenSGObjectInput<BVOL> > HierarchyType;
     // build object adapters
     HierarchyType hierarchy;
     hierarchy.setParameter(name, maxDepth, 2);
     OSGCache::the().setHierarchy (&hierarchy);
     OSGCache::the().apply(root);
     hierarchy.hierarchy();
     u32 depth = hierarchy.getDepth();
     {
     SingleEnterLeaveTraverser<OpenSGTraits,MergeTraits<OpenSGTraits> > act;
     act.getData().setNumLevels(numMerges);
     act.apply(OSGCache::the()[root], hierarchy.getRoot(), OSGCache::the()[root].getToWorldMatrix()); 
     }
     {
     SingleEnterLeaveTraverser<OpenSGTraits,SpatializeTraits<OpenSGTraits> > act;
     act.getData().setDepth(depth >> numMerges);
     act.apply(OSGCache::the()[root], hierarchy.getRoot(), OSGCache::the()[root].getToWorldMatrix()); 
     newRoot = act.getData().getRoot(); 
     }
     hierarchy.destroy();
   }
   // clear cache
   {
      OSGCache::the().clear();
   }
   return newRoot;
}

END_GENVIS_NAMESPACE

#endif







