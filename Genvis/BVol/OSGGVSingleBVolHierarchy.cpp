//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.3 $
//   $Date: 2004/03/12 13:16:55 $
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
  : m_oracle(NULL), m_minObjects(256), m_maxLevel(50)
{
}
template<class BasicTraits, class InputTraits> 
SingleBVolHierarchy<BasicTraits,InputTraits>::~SingleBVolHierarchy ()
{
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
   for (typename std::vector<AdapterType*>::const_iterator it = m_leaf.begin();
	it != m_leaf.end();
	++it) {
      (*it)->update();
   }
   // update inner nodes in reverse order
   typename std::vector<GroupType*>::const_iterator it2 = m_inner.end()-1; 
   while (it2 != m_inner.begin()) {
      (*it2)->update();
      --it2;
   }
}
template <class BasicTraits, class InputTraits>
void SingleBVolHierarchy<BasicTraits,InputTraits>::updateHierarchy (NodePtr node)
{
   CacheData& data = Cache::the()[node];
   for (typename CacheData::AdapterContainer::iterator 
        it = data.getAdapter(AdapterType::getAdapterId()).begin();
	it != data.getAdapter(AdapterType::getAdapterId()).end();
	++it) {
      ::recursiveUpdate(static_cast<BVolAdapterBase*>(*it));
   }
}

template <class BasicTraits, class InputTraits>
void SingleBVolHierarchy<BasicTraits,InputTraits>::addAdapter (const GeomObjectType& obj)
{
   CacheData& data = Cache::the()[obj];
   GeometryPtr geom = GeometryPtr::dcast(obj->getCore());
   TriangleIterator end = geom->endTriangles();
   for (TriangleIterator tri = geom->beginTriangles(); 
	tri!=end; 
	++tri) {
      AdapterType* adapter = LeafFactory::the().newObject(); m_leaf.push_back(adapter);
      adapter->setObjectAdapter(&data);
      adapter->init(tri);
      data.setAdapter(AdapterType::getAdapterId(), adapter);
   }
}

template <class BasicTraits, class InputTraits>
void SingleBVolHierarchy<BasicTraits,InputTraits>::addAdapter (const TransformType&  m2w,
							       const GeomObjectType& obj)
{
   CacheData& data = Cache::the()[obj];
   data.setAdapterMatrix(AdapterType::getAdapterId(), m2w);
   GeometryPtr geom = GeometryPtr::dcast(obj->getCore());
   TriangleIterator end = geom->endTriangles();
   for (TriangleIterator tri = geom->beginTriangles(); 
	tri != end; 
	++tri) {
      AdapterType* adapter = LeafFactory::the().newObject(); m_leaf.push_back(adapter);
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
void SingleBVolHierarchy<BasicTraits,InputTraits>::setParameter (const char* description,
								 u32 maxLevel,
								 u32 minObjects)
{
#if 0
   if (m_oracle != NULL && m_deleteOracle) {
      delete m_oracle;
   }
   m_oracle     = method;
   m_deleteOracle = false;
#else
   m_oracle     = Oracle<BVol>::getOracle(description);
#endif
   m_maxLevel   = maxLevel;
   m_minObjects = minObjects;
}
template<class BasicTraits, class InputTraits> 
void SingleBVolHierarchy<BasicTraits,InputTraits>::setParameter (u32 method,
								 u32 maxLevel,
								 u32 minObjects)
{
#if 0
   if (m_oracle != NULL && m_deleteOracle) {
      delete m_oracle;
   }
   m_oracle     = createOracle(method);
   m_deleteOracle = true;
#else
   m_oracle     = Oracle<BVol>::getOracle(method);
#endif
   m_maxLevel   = maxLevel;
   m_minObjects = minObjects;
}

template<class BasicTraits, class InputTraits> 
void SingleBVolHierarchy<BasicTraits,InputTraits>::process (const GeomObjectType& node)
{
   switch (getCoordinateSystem()) {
   case Local:
   case LocalShared:
      hierarchyLocal(node);
      break;
   default:
   case Global:
      hierarchyGlobal(node);
      break;
   }
}
template<class BasicTraits, class InputTraits> 
void SingleBVolHierarchy<BasicTraits,InputTraits>::hierarchyGlobal (const NodePtr& node)
{
   CacheData& data = Cache::the()[node];
   typename Cache::AdapterVector all;
   Cache::the().collectAdapter(AdapterType::getAdapterId(), all, node, true);
   if (all.size() == 0) {
      return;
   }
   
   Optimizer<GroupType> inst(m_oracle, all, m_maxLevel, m_minObjects);
   inst.setDataStore(&m_inner);
   GroupType* hier = inst.optimize();
   //hier->setParent(static_cast<BVolAdapterBase*>(all[0]));
   AdapterType* adapter = LeafFactory::the().newObject(); m_leaf.push_back(adapter);
   adapter->setObjectAdapter(&data);
   hier->setParent(adapter);
   data.clearAdapter(AdapterType::getAdapterId());
   data.setAdapter(AdapterType::getAdapterId(), hier);

   // propagate to parent nodes
   for (NodePtr parent=data.getParent(); parent!=NullFC; parent=parent->getParent()) {
      CacheData& data2 = Cache::the()[parent];
      if (data2.getNode() == NullFC) { // parent node not in cache 
	 break;
      }
      data2.setAdapter(AdapterType::getAdapterId(), hier);
   }
}

template<class BasicTraits, class InputTraits> 
void SingleBVolHierarchy<BasicTraits,InputTraits>::hierarchyLocal (const NodePtr& node)
{
#if 0
   // generate hierarchies in geometry nodes
   std::vector<bool> isLeaf;
   typename Cache::EntryIterator it;
   for (it = Cache::the().begin(); it != Cache::the().end(); ++it) {
      CacheData& data = Cache::the()[*it];
      if (data.getAdapter(AdapterType::getAdapterId()).size() == 0
	  || static_cast<BVolAdapterBase*>(data.getAdapter(AdapterType::getAdapterId())[0])->isInner()) {
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
      hier->setParent(static_cast<BVolAdapterBase*>(data.getAdapter(AdapterType::getAdapterId())[0]));
      data.clearAdapter(AdapterType::getAdapterId());
      data.setAdapter(AdapterType::getAdapterId(), hier);
      isLeaf.push_back(true);
   }
   // propagate to parent nodes
   typename std::vector<bool>::const_iterator itLeaf;
   for (it=Cache::the().begin(), itLeaf=isLeaf.begin(); it != Cache::the().end(); ++it, ++itLeaf) {
      if (*itLeaf) {
	 CacheData& data = Cache::the()[*it];
	 BVolAdapterBase* leaf = 
	   static_cast<BVolAdapterBase*>(*data.getAdapter(AdapterType::getAdapterId()).begin());
	 for (NodePtr parent=data.getParent(); parent!=NullFC; parent=parent->getParent()) {
	   CacheData& data2 = Cache::the()[parent];
	   if (data2.getNode() == NullFC) { // parent node not in cache 
	     break;
	   }
	   data2.setAdapter(AdapterType::getAdapterId(), leaf);
	 }
      }
   }
#else
   // get geometry nodes
   std::vector<CacheData*> all;
   Cache::the().collectLeaves(all, node);
   if (getCoordinateSystem() == Local) {
      typename std::vector<CacheData*>::const_iterator it;
      for (it=all.begin(); it!=all.end(); ++it) {
	CacheData& data = **it;
	if (data.getAdapter(AdapterType::getAdapterId()).size() == 0) {
	  continue;
	}
	Optimizer<GroupType> inst(m_oracle,
				  data.getAdapter(AdapterType::getAdapterId()), 
				  m_maxLevel, 
				  m_minObjects);
	inst.setDataStore(&m_inner);
	GroupType* hier = inst.optimize();
	// set parent to one of the leaf nodes
	hier->setParent(static_cast<BVolAdapterBase*>(data.getAdapter(AdapterType::getAdapterId())[0]));
	data.clearAdapter(AdapterType::getAdapterId());
	data.setAdapter(AdapterType::getAdapterId(), hier);
	// propagate to parent nodes
	for (NodePtr parent=data.getParent(); parent!=NullFC; parent=parent->getParent()) {
	  CacheData& data2 = Cache::the()[parent];
	  if (data2.getNode() == NullFC) { // parent node not in cache 
	    break;
	  }
	  data2.setAdapter(AdapterType::getAdapterId(), hier);
	}
      }
   } else { // LocalShared
      // create hierarchies
      typename std::vector<CacheData*>::const_iterator it;
      for (it=all.begin(); it!=all.end(); ++it) {
	CacheData& data = **it;
	if (data.getAdapter(AdapterType::getAdapterId()).size() == 0) {
	   continue;
	}
	Optimizer<GroupType> inst(m_oracle,
				  data.getAdapter(AdapterType::getAdapterId()), 
				  m_maxLevel, 
				  m_minObjects);
	inst.setDataStore(&m_inner);
	BVolAdapterBase* hier = inst.optimize();
	// set parent to one of the leaf nodes
	hier->setParent(static_cast<BVolAdapterBase*>(data.getAdapter(AdapterType::getAdapterId())[0]));
	data.clearAdapter(AdapterType::getAdapterId());
	data.setAdapter(AdapterType::getAdapterId(), hier);
      }
      // propagate to parent nodes
      for (it=all.begin(); it!=all.end(); ++it) {
	CacheData& data = **it;
	if (data.getAdapter(AdapterType::getAdapterId()).size() == 0) {
	   // lookup share
	   CacheData& share = Cache::the().getShare(data.getNode());
	   if (share.getAdapter(AdapterType::getAdapterId()).size() == 0) {
	      continue;
	   }
	   // create new root node
	   GroupType* group = InnerFactory::the().newObject(); m_inner.push_back(group);
	   *group = *static_cast<GroupType*>(share.getAdapter(AdapterType::getAdapterId())[0]);
	   AdapterType* adapter = LeafFactory::the().newObject(); m_leaf.push_back(adapter);
	   adapter->setObjectAdapter(&data);
	   group->setParent(adapter);
	   // set into adapter list
	   data.setAdapter(AdapterType::getAdapterId(), group);
	}
	BVolAdapterBase* hier = 
	  static_cast<BVolAdapterBase*>(data.getAdapter(AdapterType::getAdapterId())[0]);
	for (NodePtr parent=data.getParent(); parent!=NullFC; parent=parent->getParent()) {
	   CacheData& data2 = Cache::the()[parent];
	   if (data2.getNode() == NullFC) { // parent node not in cache 
	     break;
	   }
	   data2.setAdapter(AdapterType::getAdapterId(), hier);
	}
      }
   }
#endif
}

template<class BasicTraits, class InputTraits> 
void SingleBVolHierarchy<BasicTraits,InputTraits>::destroy ()
{
   // clear cache
   Cache::the().clearColCache();
   Cache::the().clearAdapter(AdapterType::getAdapterId());
   // clear inner hierarchy nodes
   for (typename std::vector<GroupType*>::const_iterator it=m_inner.begin();
	it != m_inner.end();
	++it){
      InnerFactory::the().deleteObject(*it);
   }
   m_inner.clear();
   // clear adapter nodes
   for (typename std::vector<AdapterType*>::const_iterator it2=m_leaf.begin();
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
   Cache::the().clearColCache();
   Cache::the().clearAdapter(AdapterType::getAdapterId());
   // clear inner hierarchy nodes
   for (typename std::vector<GroupType*>::const_iterator it=m_inner.begin();
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
   Cache::the().registerEnterFunctor(Geometry::getClassType(), 
				   osgTypedMethodFunctor2ObjPtrCPtrRef<bool,
				   SingleBVolHierarchy<BasicTraits,InputTraits>,
				   CNodePtr, 
				   OSGStaticInput*>
				   (this, &SingleBVolHierarchy<BasicTraits,InputTraits>::functorEnterGeometry));
#else
   Cache::the().registerEnterFunction(Geometry::getClassType(),
   cacheFunctionFunctor2(SingleBVolHierarchy<BasicTraits,InputTraits>::staticEnterGeometry));
#endif
}

template<class BasicTraits, class InputTraits> 
bool SingleBVolHierarchy<BasicTraits,InputTraits>::staticEnterGeometry  
(CNodePtr& cnode, OSGStaticInput* input)
{
   SingleBVolHierarchy<BasicTraits,InputTraits>* thisInput = 
     (SingleBVolHierarchy<BasicTraits,InputTraits>*)input;
   if (thisInput == NULL) {
      return false;
   }
   switch (thisInput->getCoordinateSystem()) { 
   case Local:
      thisInput->addAdapter(Cache::the().getCurrentNode());
      break;
   case LocalShared:
      if (Cache::the().findShare(Cache::the().getCurrentNode()) == NULL) {
	 thisInput->addAdapter(Cache::the().getCurrentNode());
      }
      break;
   case Global:
      thisInput->addAdapter(Cache::the().getCurrentMatrix(), Cache::the().getCurrentNode());
      break;
   }
   return true;
}

#ifndef OSG_NOFUNCTORS
template<class BasicTraits, class InputTraits> 
bool SingleBVolHierarchy<BasicTraits,InputTraits>::functorEnterGeometry 
(CNodePtr& cnode, OSGStaticInput* input)
{
   return staticEnterGeometry (cnode, input); 
}
#endif


template <class BVOL> 
SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<BVOL> >::SingleBVolHierarchy ()
  : m_oracle(NULL), m_minObjects(256), m_maxLevel(50)
{
}
template <class BVOL> 
SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<BVOL> >::~SingleBVolHierarchy ()
{
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
   CacheData& data = Cache::the()[obj];
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
void SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<BVOL> >::addAdapter (const TransformType&  m2w,
										      const GeomObjectType& obj)
{
   CacheData& data = Cache::the()[obj];
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
(const char* description,
 u32 maxLevel,
 u32 minObjects)
{
#if 0
   if (m_oracle != NULL && m_deleteOracle) {
      delete m_oracle;
   }
   m_oracle     = method;
   m_deleteOracle = false;
#else
   m_oracle     = Oracle<BVol>::getOracle(description);
#endif
   m_maxLevel   = maxLevel;
   m_minObjects = minObjects;
}
template <class BVOL> 
void SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<BVOL> >::setParameter (u32 method,
								 u32 maxLevel,
								 u32 minObjects)
{
#if 0
   if (m_oracle != NULL && m_deleteOracle) {
      delete m_oracle;
   }
   m_oracle     = createOracle(method);
   m_deleteOracle = true;
#else
   m_oracle     = Oracle<BVol>::getOracle(method);
#endif
   m_maxLevel   = maxLevel;
   m_minObjects = minObjects;
}

template <class BVOL> 
void SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<BVOL> >::process (const GeomObjectType& node)
{
   switch (getCoordinateSystem()) {
   case Local:
   case LocalShared:
      hierarchyLocal(node);
      break;
   default:
   case Global:
      hierarchyGlobal(node);
      break;
   }
}
template <class BVOL> 
void SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<BVOL> >::hierarchyGlobal
(const NodePtr& node)
{
   CacheData& data = Cache::the()[node];
   typename Cache::AdapterVector all;
   Cache::the().collectAdapter(AdapterType::getAdapterId(), all, node, true);
   if (all.size() == 0) {
      return;
   }

   Optimizer<GroupType> inst(m_oracle, all, m_maxLevel, m_minObjects);
   inst.setDataStore(&m_inner);
   GroupType* hier = inst.optimize();
   AdapterType* adapter = LeafFactory::the().newObject(); m_leaf.push_back(adapter);
   adapter->setObjectAdapter(&data);
   hier->setParent(adapter);
   data.clearAdapter(AdapterType::getAdapterId());
   data.setAdapter(AdapterType::getAdapterId(), hier);

   // propagate to parent nodes
   for (NodePtr parent=data.getParent(); parent!=NullFC; parent=parent->getParent()) {
      CacheData& data2 = Cache::the()[parent];
      if (data2.getNode() == NullFC) { // parent node not in cache 
	 break;
      }
      data2.setAdapter(AdapterType::getAdapterId(), hier);
   }
}

template <class BVOL> 
void SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<BVOL> >::hierarchyLocal 
(const NodePtr& node)
{
#if 0
   // generate hierarchies in geometry nodes
   std::vector<bool> isLeaf;
   typename Cache::EntryIterator it;
   for (it = Cache::the().begin(); it != Cache::the().end(); ++it) {
      CacheData& data = Cache::the()[*it];
      if (data.getAdapter(AdapterType::getAdapterId()).size() == 0
	  || static_cast<BVolAdapterBase*>(data.getAdapter(AdapterType::getAdapterId())[0])->isInner()) {
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
      hier->setParent(static_cast<BVolAdapterBase*>(data.getAdapter(AdapterType::getAdapterId())[0]));
      data.clearAdapter(AdapterType::getAdapterId());
      data.setAdapter(AdapterType::getAdapterId(), hier);
      isLeaf.push_back(true);
   }
   // propagate to parent nodes
   typename std::vector<bool>::const_iterator itLeaf;
   for (it=Cache::the().begin(), itLeaf=isLeaf.begin(); it != Cache::the().end(); ++it, ++itLeaf) {
      if (*itLeaf) {
	 CacheData& data = Cache::the()[*it];
	 BVolAdapterBase* leaf = 
	   static_cast<BVolAdapterBase*>(*data.getAdapter(AdapterType::getAdapterId()).begin());
	 for (NodePtr parent=data.getParent(); parent!=NullFC; parent=parent->getParent()) {
	   CacheData& data2 = Cache::the()[parent];
	   if (data2.getNode() == NullFC) { // parent node not in cache 
	     break;
	   }

	   data2.setAdapter(AdapterType::getAdapterId(), leaf);
	 }
      }
   }
#else
   // get geometry nodes
   std::vector<CacheData*> all;
   Cache::the().collectLeaves(all, node);
   if (getCoordinateSystem() == Local) {
      typename std::vector<CacheData*>::const_iterator it;
      for (it=all.begin(); it!=all.end(); ++it) {
	CacheData& data = **it;
	if (data.getAdapter(AdapterType::getAdapterId()).size() == 0) {
	  continue;
	}
	Optimizer<GroupType> inst(m_oracle,
				  data.getAdapter(AdapterType::getAdapterId()), 
				  m_maxLevel, 
				  m_minObjects);
	inst.setDataStore(&m_inner);
	GroupType* hier = inst.optimize();
	// set parent to one of the leaf nodes
	hier->setParent(static_cast<BVolAdapterBase*>(data.getAdapter(AdapterType::getAdapterId())[0]));
	data.clearAdapter(AdapterType::getAdapterId());
	data.setAdapter(AdapterType::getAdapterId(), hier);
	// propagate to parent nodes
	for (NodePtr parent=data.getParent(); parent!=NullFC; parent=parent->getParent()) {
	  CacheData& data2 = Cache::the()[parent];
	  if (data2.getNode() == NullFC) { // parent node not in cache 
	    break;
	  }
	  data2.setAdapter(AdapterType::getAdapterId(), hier);
	}
      }
   } else { // LocalShared
      // create hierarchies
      typename std::vector<CacheData*>::const_iterator it;
      for (it=all.begin(); it!=all.end(); ++it) {
	CacheData& data = **it;
	if (data.getAdapter(AdapterType::getAdapterId()).size() == 0) {
	   continue;
	}
	Optimizer<GroupType> inst(m_oracle,
				  data.getAdapter(AdapterType::getAdapterId()), 
				  m_maxLevel, 
				  m_minObjects);
	inst.setDataStore(&m_inner);
	BVolAdapterBase* hier = inst.optimize();
	// set parent to one of the leaf nodes
	hier->setParent(static_cast<BVolAdapterBase*>(data.getAdapter(AdapterType::getAdapterId())[0]));
	data.clearAdapter(AdapterType::getAdapterId());
	data.setAdapter(AdapterType::getAdapterId(), hier);
      }
      // propagate to parent nodes
      for (it=all.begin(); it!=all.end(); ++it) {
	CacheData& data = **it;
	if (data.getAdapter(AdapterType::getAdapterId()).size() == 0) {
	   // lookup share
	   CacheData& share = Cache::the().getShare(data.getNode());
	   if (share.getAdapter(AdapterType::getAdapterId()).size() == 0) {
	      continue;
	   }
	   // create new root node
	   GroupType* group = InnerFactory::the().newObject(); m_inner.push_back(group);
	   *group = *static_cast<GroupType*>(share.getAdapter(AdapterType::getAdapterId())[0]);
	   AdapterType* adapter = LeafFactory::the().newObject(); m_leaf.push_back(adapter);
	   adapter->setObjectAdapter(&data);
	   group->setParent(adapter);
	   // set into adapter list
	   data.setAdapter(AdapterType::getAdapterId(), group);
	}
	BVolAdapterBase* hier = 
	  static_cast<BVolAdapterBase*>(data.getAdapter(AdapterType::getAdapterId())[0]);
	for (NodePtr parent=data.getParent(); parent!=NullFC; parent=parent->getParent()) {
	   CacheData& data2 = Cache::the()[parent];
	   if (data2.getNode() == NullFC) { // parent node not in cache 
	     break;
	   }
	   data2.setAdapter(AdapterType::getAdapterId(), hier);
	}
      }
   }
#endif
}

template <class BVOL> 
void SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<BVOL> >::destroy ()
{
   // clear cache
   Cache::the().clearColCache();
   Cache::the().clearAdapter(AdapterType::getAdapterId());
   // clear inner hierarchy nodes
   for (typename std::vector<GroupType*>::const_iterator it=m_inner.begin();
	it != m_inner.end();
	++it){
      InnerFactory::the().deleteObject(*it);
   }
   m_inner.clear();
   // clear adapter nodes
   for (typename std::vector<AdapterType*>::const_iterator it2=m_leaf.begin();
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
   Cache::the().clearColCache();
   Cache::the().clearAdapter(AdapterType::getAdapterId());
   // clear inner hierarchy nodes
   for (typename std::vector<GroupType*>::const_iterator it=m_inner.begin();
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
   Cache::the().registerEnterFunctor(Geometry::getClassType(), 
				   osgTypedMethodFunctor2ObjPtrCPtrRef<bool,
				   SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<BVOL> >,
				   CNodePtr, 
				   OSGStaticInput*>
				   (this, &SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<BVOL> >::functorEnterGeometry));
#else
   Cache::the().registerEnterFunction(Geometry::getClassType(),
   cacheFunctionFunctor2(SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<BVOL> >::staticEnterGeometry));
#endif
}
template <class BVOL> 
bool SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<BVOL> >::staticEnterGeometry  
(CNodePtr& cnode, OSGStaticInput* input)
{
   SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<BVOL> >* thisInput = 
     (SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<BVOL> >*)input;
   if (thisInput == NULL) {
      return false;
   }
   switch (thisInput->getCoordinateSystem()) { 
   case Local:
      thisInput->addAdapter(Cache::the().getCurrentNode());
      break;
   case LocalShared:
      if (Cache::the().findShare(Cache::the().getCurrentNode()) == NULL) {
	 thisInput->addAdapter(Cache::the().getCurrentNode());
      }
      break;
   case Global:
      thisInput->addAdapter(Cache::the().getCurrentMatrix(), Cache::the().getCurrentNode());
      break;
   }
   return true;
}
#ifndef OSG_NOFUNCTORS
template <class BVOL> 
bool SingleBVolHierarchy<OpenSGTraits,OpenSGTriangleAlignedInput<BVOL> >::functorEnterGeometry 
(CNodePtr& cnode, OSGStaticInput* input)
{
   return staticEnterGeometry (cnode, this); 
}
#endif






