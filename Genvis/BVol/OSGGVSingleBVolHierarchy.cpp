//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.1 $
//   $Date: 2003/09/11 16:20:29 $
//                                                                            
//=============================================================================

#include <stack>
#include "OSGGVSingleBVolHierarchy.h"
//#include "OSGGVTriangleAdapterExt.h"
#include "OSGGVOptimizer.h"
#include "OSGGVCache.h"
USING_GENVIS_NAMESPACE
OSG_USING_NAMESPACE

// explicit template instantiations
template class OSG_GENVISLIB_DLLMAPPING SingleBVolHierarchyBase<OpenSGTraits>;
template class OSG_GENVISLIB_DLLMAPPING SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleInput<K6Dop> >;
template class OSG_GENVISLIB_DLLMAPPING SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleInput<K14Dop> >;
template class OSG_GENVISLIB_DLLMAPPING SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleInput<K18Dop> >;
template class OSG_GENVISLIB_DLLMAPPING SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleInput<K26Dop> >;
template class OSG_GENVISLIB_DLLMAPPING SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleInput<K12Dop> >;
template class OSG_GENVISLIB_DLLMAPPING SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleInput<K20Dop> >;

template class OSG_GENVISLIB_DLLMAPPING SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<K6Dop> >;
template class OSG_GENVISLIB_DLLMAPPING SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<K14Dop> >;
template class OSG_GENVISLIB_DLLMAPPING SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<K18Dop> >;
template class OSG_GENVISLIB_DLLMAPPING SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<K26Dop> >;
template class OSG_GENVISLIB_DLLMAPPING SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<K12Dop> >;
template class OSG_GENVISLIB_DLLMAPPING SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<K20Dop> >;

template<class BasicTraits> 
SingleBVolHierarchyBase<BasicTraits>::SingleBVolHierarchyBase ()
{
}
template<class BasicTraits> 
SingleBVolHierarchyBase<BasicTraits>::~SingleBVolHierarchyBase ()
{
}


template<class BasicTraits, class InputTraits> 
SingleBVolHierarchy<BasicTraits,InputTraits>::SingleBVolHierarchy ()
  : m_oracle(NULL), m_deleteOracle(false), m_minObjects(256), m_maxLevel(50)
{
}
template<class BasicTraits, class InputTraits> 
SingleBVolHierarchy<BasicTraits,InputTraits>::~SingleBVolHierarchy ()
{
   if (m_deleteOracle) {
      delete m_oracle;
   }
}

static void recursiveUpdate (BVolAdapterBase* root)
{
   if (root->isInner()) {
      BVolGroupBase* group = static_cast<BVolGroupBase*>(root);
      for (BVolGroupBase::Iterator it = group->getSons().begin(); 
	   it != group->getSons().end(); 
	   ++it) {
	 ::recursiveUpdate(*it);
      }
   }
   root->update();
}

template <class BasicTraits, class InputTraits>
void SingleBVolHierarchy<BasicTraits,InputTraits>::updateHierarchy ()
{
   // first update leaf nodes in arbitrary order
   for (std::vector<AdapterType*>::const_iterator it = m_leaf.begin();
	it != m_leaf.end();
	++it) {
      (*it)->update();
   }
   // update inner nodes in reverse order
   std::vector<GroupType*>::const_iterator it2 = m_inner.end()-1; 
   while (it2 != m_inner.begin()) {
      (*it2)->update();
      --it2;
   }
}
template <class BasicTraits, class InputTraits>
void SingleBVolHierarchy<BasicTraits,InputTraits>::updateHierarchy (NodePtr node)
{
   Cache::CacheData& data = getCache()[node];
   for (CacheData::AdapterContainer::const_iterator 
        it = data.getAdapter(AdapterType::getAdapterId()).begin();
	it != data.getAdapter(AdapterType::getAdapterId()).end();
	++it) {
      ::recursiveUpdate(static_cast<BVolAdapterBase*>(*it));
   }
}

template <class BasicTraits, class InputTraits>
void SingleBVolHierarchy<BasicTraits,InputTraits>::addAdapter (const GeomObjectType& obj)
{
   CacheData& data = getCache()[obj];
   GeometryPtr geom = GeometryPtr::dcast(obj->getCore());
   TriangleIterator end = geom->endTriangles();
   for (TriangleIterator tri=geom->beginTriangles(); 
	tri!=end; 
	++tri) {
      AdapterType* adapter = LeafFactory::the().newObject();
      m_leaf.push_back(adapter);
      adapter->setObjectAdapter(&data);
      adapter->init(tri);
      data.setAdapter(AdapterType::getAdapterId(), adapter);
   }
}

template <class BasicTraits, class InputTraits>
void SingleBVolHierarchy<BasicTraits,InputTraits>::addAdapter (const TransformType&  m2w,
							       const GeomObjectType& obj)
{
   CacheData& data = getCache()[obj];
   data.setAdapterMatrix(AdapterType::getAdapterId(), m2w);

   GeometryPtr geom = GeometryPtr::dcast(obj->getCore());
   TriangleIterator end = geom->endTriangles();
   for (TriangleIterator tri=geom->beginTriangles(); 
	tri!=end; 
	++tri) {
      AdapterType* adapter = LeafFactory::the().newObject();
      m_leaf.push_back(adapter);
      adapter->setObjectAdapter(&data);
      adapter->init(m2w, tri);
      data.setAdapter(AdapterType::getAdapterId(), adapter);
   }
}

#if 0
void SingleBVolHierarchy<BasicTraits,InputTraits>::restructureHierarchy 
(BVolGroup<BVol>* root,
 BVolAdapterBase* node)
{
   std::stack<BVolGroup<BVol>*> s;
   s.push(root);

   while (s.size() > 0) {
      BVolGroup<BVol>* current = s.top(); s.pop();

      if (current->getSons().size() < 2) {
	 current->getSons().push_back(node);
	 current->setValid(false);
	 node->setParent(current);
	 return;
      }
      if (current->getSons()[0]->isLeaf()) {
 	 BVolAdapterBase* old = current->getSons()[0];
	 m_innerStore.push_back(BVolGroup<BVol>());
	 root = m_innerStore.begin() + (m_innerStore.size()-1);
	 // add old group as child
	 root->getSons().push_back(old);
	 root->getSons().push_back(node);
	 old->setParent(root);
	 current->getSons()[0] = root;
	 return;
      }
      if (current->getSons()[1]->isLeaf()) {
	 BVolAdapterBase* old = current->getSons()[1];
	 m_innerStore.push_back(BVolGroup<BVol>());
	 root = m_innerStore.begin() + (m_innerStore.size()-1);
	 // add old group as child
	 root->getSons().push_back(old);
	 root->getSons().push_back(node);
	 old->setParent(root);
	 current->getSons()[1] = root;
	 return;
      }
      s.push((BVolGroup<BVol>*)current->getSons()[0]);
      s.push((BVolGroup<BVol>*)current->getSons()[1]);
   }
}
#endif

template<class BasicTraits, class InputTraits> 
void SingleBVolHierarchy<BasicTraits,InputTraits>::setParameter
(Oracle*  method,
 unsigned maxLevel,
 unsigned minObjects)
{
   if (m_oracle != NULL && m_deleteOracle) {
      delete m_oracle;
   }
   m_oracle     = method;
   m_deleteOracle = false;
   m_maxLevel   = maxLevel;
   m_minObjects = minObjects;
}
template<class BasicTraits, class InputTraits> 
void SingleBVolHierarchy<BasicTraits,InputTraits>::setParameter
(SplitAlgorithm method,
 unsigned maxLevel,
 unsigned minObjects)
{
   if (m_oracle != NULL && m_deleteOracle) {
      delete m_oracle;
   }
   m_oracle     = createOracle(method);
   m_deleteOracle = true;
   m_maxLevel   = maxLevel;
   m_minObjects = minObjects;
}

template<class BasicTraits, class InputTraits> 
void SingleBVolHierarchy<BasicTraits,InputTraits>::hierarchy ()
{
   if (getCoordinateSystem() == LocalCoordSystem) {
      hierarchyLocal();
   } else {
      hierarchyGlobal();
   }
}

template<class BasicTraits, class InputTraits> 
void SingleBVolHierarchy<BasicTraits,InputTraits>::hierarchyGlobal ()
{
   // generate hierarchies in geometry nodes
   std::vector<bool> isLeaf;
   Cache::EntryIterator it;
   for (it = getCache().begin(); it != getCache().end(); ++it) {
      Cache::CacheData& data = getCache()[*it];
      if (data.getAdapter(AdapterType::getAdapterId()).size() == 0) {
	 isLeaf.push_back(false);
	 continue;
      }

      Optimizer<GroupType> inst(m_oracle,
				data.getAdapter(AdapterType::getAdapterId()), 
				m_maxLevel, 
				m_minObjects);
      inst.setDataStore(&m_inner);
      GroupType* hier = inst.optimize();
      data.clearAdapter(AdapterType::getAdapterId());
      data.setAdapter(AdapterType::getAdapterId(), hier);
      isLeaf.push_back(true);
   }
   // create BVolGroups for inner nodes
   std::vector<bool>::const_iterator itLeaf;
   for (it=getCache().begin(), itLeaf=isLeaf.begin(); it != getCache().end(); ++it, ++itLeaf) {
      if (*itLeaf) {
	 Cache::CacheData& data = getCache()[*it];
	 BVolAdapterBase* leaf =
	   static_cast<BVolAdapterBase*>(*data.getAdapter(AdapterType::getAdapterId()).begin());
	 for (NodePtr parent=data.getParent(); parent!=NullFC; parent=parent->getParent()) {
	   Cache::CacheData& data2 = getCache()[parent];
	   if (data2.getNode() == NullFC) { // parent node not in cache anymore
	     break;
	   }

	   data2.setAdapterMatrix(AdapterType::getAdapterId(), 
				  data.getAdapterMatrix(AdapterType::getAdapterId()));
	   if (data2.getAdapter(AdapterType::getAdapterId()).size() > 0) { 
	     BVolAdapterBase* leaf2 = 
	       static_cast<BVolAdapterBase*>(*data2.getAdapter(AdapterType::getAdapterId()).begin());

	     // create newGroup and add leaf and leaf2
	     GroupType* newGroup = InnerFactory::the().newObject();
	     m_inner.push_back(newGroup);
	     data2.clearAdapter(AdapterType::getAdapterId());
	     data2.setAdapter(AdapterType::getAdapterId(), newGroup);
	     
	     newGroup->getSons().push_back(leaf);
	     newGroup->getSons().push_back(leaf2);
	     leaf ->setParent(newGroup);
	     leaf2->setParent(newGroup);
	   } else { 
	     // insert leaf into cache
	     data2.setAdapter(AdapterType::getAdapterId(), leaf);
	   }
	 }
      }
   }
}

template<class BasicTraits, class InputTraits> 
void SingleBVolHierarchy<BasicTraits,InputTraits>::hierarchyLocal ()
{
   // generate hierarchies in geometry nodes
   std::vector<bool> isLeaf;
   Cache::EntryIterator it;
   for (it = getCache().begin(); it != getCache().end(); ++it) {
      Cache::CacheData& data = getCache()[*it];
      if (data.getAdapter(AdapterType::getAdapterId()).size() == 0) {
	 isLeaf.push_back(false);
	 continue;
      }
      Optimizer<GroupType> inst(m_oracle,
				data.getAdapter(AdapterType::getAdapterId()), 
				m_maxLevel, 
				m_minObjects);
      inst.setDataStore(&m_inner);
      GroupType* hier = inst.optimize();
      // set parent to one of the leaf nodes
      hier->setParent(static_cast<BVolAdapterBase*>(*data.getAdapter(AdapterType::getAdapterId()).begin()));
      data.clearAdapter(AdapterType::getAdapterId());
      data.setAdapter(AdapterType::getAdapterId(), hier);
      isLeaf.push_back(true);
   }
   // propagate to inner nodes
   std::vector<bool>::const_iterator itLeaf;
   for (it=getCache().begin(), itLeaf=isLeaf.begin(); it != getCache().end(); ++it, ++itLeaf) {
      if (*itLeaf) {
	 Cache::CacheData& data = getCache()[*it];
	 BVolAdapterBase* leaf = 
	   static_cast<BVolAdapterBase*>(*data.getAdapter(AdapterType::getAdapterId()).begin());
	 for (NodePtr parent=data.getParent(); parent!=NullFC; parent=parent->getParent()) {
	   Cache::CacheData& data2 = getCache()[parent];
	   if (data2.getNode() == NullFC) { // parent node not in cache 
	     break;
	   }
	   data2.setAdapter(AdapterType::getAdapterId(), leaf);
	 }
      }
   }
}

template<class BasicTraits, class InputTraits> 
Oracle* SingleBVolHierarchy<BasicTraits,InputTraits>::createOracle
(SplitAlgorithm method)
{
   switch (method) {
   default:
   case LongestSideMedianId:
     return new LongestSideMedian2<BVol> ();
   case LongestSideMeanId:
     return new LongestSideMean2<BVol> ();
   case MinimumSICId:
     return new MinimumSIC<BVol> ();
   case MinimumVolumeId:
     return new HeuristicGrouping<BVol> ();
     //return new MinimumVolume<BVol> ();
   }
}

template<class BasicTraits, class InputTraits> 
void SingleBVolHierarchy<BasicTraits,InputTraits>::destroy ()
{
   // clear cache
   getCache().clearAdapter(AdapterType::getAdapterId());
   // clear inner hierarchy nodes
   for (std::vector<GroupType*>::const_iterator it=m_inner.begin();
	it != m_inner.end();
	++it){
      InnerFactory::the().deleteObject(*it);
   }
   m_inner.clear();
   // clear adapter nodes
   for (std::vector<AdapterType*>::const_iterator it2=m_leaf.begin();
	it2 != m_leaf.end();
	++it2){
      LeafFactory::the().deleteObject(*it2);
   }
   m_leaf.clear();
}
template<class BasicTraits, class InputTraits> 
void SingleBVolHierarchy<BasicTraits,InputTraits>::clear   ()
{
   // clear cache
   getCache().clearAdapter(AdapterType::getAdapterId());
   // clear inner hierarchy nodes
   for (std::vector<GroupType*>::const_iterator it=m_inner.begin();
	it != m_inner.end();
	++it){
      InnerFactory::the().deleteObject(*it);
   }
   m_inner.clear();
}

template<class BasicTraits, class InputTraits> 
void SingleBVolHierarchy<BasicTraits,InputTraits>::registerFunctors (void)
{
#ifndef OSG_NOFUNCTORS
   getCache().registerEnterFunctor(Geometry::getClassType(), 
				   osgTypedMethodVoidFunctor2ObjPtrCPtrRef<
				   SingleBVolHierarchy<BasicTraits,InputTraits>,
				   CNodePtr, 
				   OSGStaticInput*>
				   (this, &SingleBVolHierarchy<BasicTraits,InputTraits>::functorEnterGeometry));
#else
   getCache().registerEnterFunction(Geometry::getClassType(),
   cacheFunctionFunctor2(SingleBVolHierarchy<BasicTraits,InputTraits>::staticEnterGeometry));
#endif
}

template<class BasicTraits, class InputTraits> 
void SingleBVolHierarchy<BasicTraits,InputTraits>::staticEnterGeometry  
(CNodePtr& cnode, OSGStaticInput* input)
{
   SingleBVolHierarchy<BasicTraits,InputTraits>* thisInput = 
     (SingleBVolHierarchy<BasicTraits,InputTraits>*)input;
   if (thisInput == NULL) {
      return;
   }
   if (thisInput->getCoordinateSystem() == LocalCoordSystem) {
      thisInput->addAdapter(getCache().getCurrentNode());
   } else {
      thisInput->addAdapter(getCache().getCurrentMatrix(), getCache().getCurrentNode());
   }
}

#ifndef OSG_NOFUNCTORS
template<class BasicTraits, class InputTraits> 
void SingleBVolHierarchy<BasicTraits,InputTraits>::functorEnterGeometry (
CNodePtr& cnode, OSGStaticInput*)
{
   staticEnterGeometry (cnode, this); 
}
#endif


#if 1
template <class BVOL> 
SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<BVOL> >::SingleBVolHierarchy ()
  : m_oracle(NULL), m_deleteOracle(false), m_minObjects(256), m_maxLevel(50)
{
}
template <class BVOL> 
SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<BVOL> >::~SingleBVolHierarchy ()
{
   if (m_deleteOracle) {
      delete m_oracle;
   }
}

template <class BVOL> 
void SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<BVOL> >::updateHierarchy ()
{
}
template <class BVOL> 
void SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<BVOL> >::updateHierarchy (NodePtr )
{
}


template <class BVOL>
void SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<BVOL> >::addAdapter (const GeomObjectType& obj)
{
   CacheData& data = getCache()[obj];
   GeometryPtr geom = GeometryPtr::dcast(obj->getCore());
   TriangleIterator end = geom->endTriangles();
   for (TriangleIterator tri=geom->beginTriangles(); 
	tri!=end; 
	++tri) {
      AdapterType* adapter = LeafFactory::the().newObject();
      m_leaf.push_back(adapter);
      adapter->setObjectAdapter(&data);
      adapter->init(tri);
      data.setAdapter(AdapterType::getAdapterId(), adapter);
   }
}

template <class BVOL>
void SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<BVOL> >::addAdapter 
(const TransformType&  m2w,
 const GeomObjectType& obj)
{
   CacheData& data = getCache()[obj];
   data.setAdapterMatrix(AdapterType::getAdapterId(), m2w);

   GeometryPtr geom = GeometryPtr::dcast(obj->getCore());
   TriangleIterator end = geom->endTriangles();
   for (TriangleIterator tri=geom->beginTriangles(); 
	tri!=end; 
	++tri) {
      AdapterType* adapter = LeafFactory::the().newObject();
      m_leaf.push_back(adapter);
      adapter->setObjectAdapter(&data);
      adapter->init(m2w, tri);
      data.setAdapter(AdapterType::getAdapterId(), adapter);
   }
}

template <class BVOL> 
void SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<BVOL> >::setParameter
(Oracle*  method,
 unsigned maxLevel,
 unsigned minObjects)
{
   if (m_oracle != NULL && m_deleteOracle) {
      delete m_oracle;
   }
   m_oracle     = method;
   m_deleteOracle = false;
   m_maxLevel   = maxLevel;
   m_minObjects = minObjects;
}
template <class BVOL> 
void SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<BVOL> >::setParameter
(SplitAlgorithm method,
 unsigned maxLevel,
 unsigned minObjects)
{
   if (m_oracle != NULL && m_deleteOracle) {
      delete m_oracle;
   }
   m_oracle     = createOracle(method);
   m_deleteOracle = true;
   m_maxLevel   = maxLevel;
   m_minObjects = minObjects;
}

template <class BVOL> 
void SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<BVOL> >::hierarchy ()
{
   if (getCoordinateSystem() == LocalCoordSystem) {
      hierarchyLocal();
   } else {
      hierarchyGlobal();
   }
}

template <class BVOL> 
void SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<BVOL> >::hierarchyGlobal ()
{
   // generate hierarchies in geometry nodes
   std::vector<bool> isLeaf;
   Cache::EntryIterator it;
   for (it = getCache().begin(); it != getCache().end(); ++it) {
      Cache::CacheData& data = getCache()[*it];
      if (data.getAdapter(AdapterType::getAdapterId()).size() == 0) {
	 isLeaf.push_back(false);
	 continue;
      }

      Optimizer<GroupType> inst(m_oracle,
				data.getAdapter(AdapterType::getAdapterId()), 
				m_maxLevel, 
				m_minObjects);
      inst.setDataStore(&m_inner);
      GroupType* hier = inst.optimize();
      data.clearAdapter(AdapterType::getAdapterId());
      data.setAdapter(AdapterType::getAdapterId(), hier);
      isLeaf.push_back(true);
   }
   // create BVolGroups for inner nodes
   std::vector<bool>::const_iterator itLeaf;
   for (it = getCache().begin(), itLeaf=isLeaf.begin(); it != getCache().end(); ++it, ++itLeaf) {
      Cache::CacheData& data = getCache()[*it];
      if (!(*itLeaf)) {
	 continue;
      }

      BVolAdapterBase*  leaf =
	static_cast<BVolAdapterBase*>(*data.getAdapter(AdapterType::getAdapterId()).begin());
      NodePtr parent;
      for (parent = data.getParent(); parent != NullFC; parent = parent->getParent()) {
	   Cache::CacheData& data2 = getCache()[parent];
	   if (data2.getNode() == NullFC) { // parent node not in cache anymore
	     break;
	   }

	   data2.setAdapterMatrix(AdapterType::getAdapterId(), 
				  data.getAdapterMatrix(AdapterType::getAdapterId()));
	   if (data2.getAdapter(AdapterType::getAdapterId()).size() > 0) { 
	     GroupType* leaf2 = 
	       static_cast<GroupType*>(*data2.getAdapter(AdapterType::getAdapterId()).begin());

	     // create newGroup and add leaf and leaf2
	     GroupType* newGroup = InnerFactory::the().newObject();
	     newGroup->init(*leaf2);
	     m_inner.push_back(newGroup);
	     data2.clearAdapter(AdapterType::getAdapterId());
	     data2.setAdapter(AdapterType::getAdapterId(), newGroup);
	     
	     newGroup->getSons()[0] = leaf;
	     newGroup->getSons()[1] = leaf2;
	     newGroup->setValid(false);
	     leaf ->setParent(newGroup);
	     leaf2->setParent(newGroup);
	   } else { 
	     // insert leaf into cache
	     data2.setAdapter(AdapterType::getAdapterId(), leaf);
	   }
      }
   }
}

template <class BVOL> 
void SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<BVOL> >::hierarchyLocal ()
{
   // generate hierarchies in geometry nodes
   std::vector<bool> isLeaf;
   Cache::EntryIterator it;
   for (it = getCache().begin(); it != getCache().end(); ++it) {
      Cache::CacheData& data = getCache()[*it];
      if (data.getAdapter(AdapterType::getAdapterId()).size() == 0) {
	 isLeaf.push_back(false);
	 continue;
      }

      Optimizer<GroupType> inst(m_oracle,
				data.getAdapter(AdapterType::getAdapterId()), 
				m_maxLevel, 
				m_minObjects);
      inst.setDataStore(&m_inner);
      GroupType* hier = inst.optimize();
      // set parent to one of the leaf nodes
      hier->setParent(static_cast<BVolAdapterBase*>(*data.getAdapter(AdapterType::getAdapterId()).begin()));
      data.clearAdapter(AdapterType::getAdapterId());
      data.setAdapter(AdapterType::getAdapterId(), hier);
      isLeaf.push_back(true);
   }
   // propagate to inner nodes
   std::vector<bool>::const_iterator itLeaf;
   for (it=getCache().begin(), itLeaf=isLeaf.begin(); it != getCache().end(); ++it, ++itLeaf) {
      if (*itLeaf) {
	 Cache::CacheData& data = getCache()[*it];
	 BVolAdapterBase* leaf = 
	   static_cast<BVolAdapterBase*>(*data.getAdapter(AdapterType::getAdapterId()).begin());
	 for (NodePtr parent=data.getParent(); parent!=NullFC; parent=parent->getParent()) {
	   Cache::CacheData& data2 = getCache()[parent];
	   if (data2.getNode() == NullFC) { // parent node not in cache 
	     break;
	   }

	   data2.setAdapter(AdapterType::getAdapterId(), leaf);
	 }
      }
   }
}
template <class BVOL> 
Oracle* SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<BVOL> >::createOracle
(SplitAlgorithm method)
{
   switch (method) {
   default:
   case LongestSideMedianId:
     return new LongestSideMedian2<BVol> ();
   case LongestSideMeanId:
     return new LongestSideMean2<BVol> ();
   case MinimumSICId:
   case MinimumVolumeId:
     return new HeuristicGrouping<BVol> ();
   }
}

template <class BVOL> 
void SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<BVOL> >::destroy ()
{
   // clear cache
   getCache().clearAdapter(AdapterType::getAdapterId());
   // clear inner hierarchy nodes
   for (std::vector<GroupType*>::const_iterator it=m_inner.begin();
	it != m_inner.end();
	++it){
      InnerFactory::the().deleteObject(*it);
   }
   m_inner.clear();
   // clear adapter nodes
   for (std::vector<AdapterType*>::const_iterator it2=m_leaf.begin();
	it2 != m_leaf.end();
	++it2){
      LeafFactory::the().deleteObject(*it2);
   }
   m_leaf.clear();
}
template <class BVOL> 
void SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<BVOL> >::clear   ()
{
   // clear cache
   getCache().clearAdapter(AdapterType::getAdapterId());
   // clear inner hierarchy nodes
   for (std::vector<GroupType*>::const_iterator it=m_inner.begin();
	it != m_inner.end();
	++it){
      InnerFactory::the().deleteObject(*it);
   }
   m_inner.clear();
}

template <class BVOL> 
void SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<BVOL> >::registerFunctors (void)
{
#ifndef OSG_NOFUNCTORS
   getCache().registerEnterFunctor(Geometry::getClassType(), 
				   osgTypedMethodVoidFunctor2ObjPtrCPtrRef<
				   SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<BVOL> >,
				   CNodePtr, 
				   OSGStaticInput*>
				   (this, &SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<BVOL> >::functorEnterGeometry));
#else
   getCache().registerEnterFunction(Geometry::getClassType(),
   cacheFunctionFunctor2(SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<BVOL> >::staticEnterGeometry));
#endif
}
template <class BVOL> 
void SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<BVOL> >::staticEnterGeometry  
(CNodePtr& cnode, OSGStaticInput* input)
{
   SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<BVOL> >* thisInput = 
     (SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<BVOL> >*)input;
   if (thisInput == NULL) {
      return;
   }
   if (thisInput->getCoordinateSystem() == LocalCoordSystem) {
      thisInput->addAdapter(getCache().getCurrentNode());
   } else {
      thisInput->addAdapter(getCache().getCurrentMatrix(), getCache().getCurrentNode());
   }
}
#ifndef OSG_NOFUNCTORS
template <class BVOL> 
void SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<BVOL> >::functorEnterGeometry (
CNodePtr& cnode, OSGStaticInput*)
{
   staticEnterGeometry (cnode, this); 
}
#endif

#endif
