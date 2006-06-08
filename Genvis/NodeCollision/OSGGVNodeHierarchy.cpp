//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.4 $
//   $Date: 2006/06/08 17:03:30 $
//                                                                            
//=============================================================================

#include "OSGGVNodeHierarchy.h"
OSG_USING_NAMESPACE
USING_GENVIS_NAMESPACE

BEGIN_GENVIS_NAMESPACE

template <class BasicTraits> 
NodeHierarchy<BasicTraits>::NodeHierarchy ()
  : Inherited()
{
}
template <class BasicTraits> 
NodeHierarchy<BasicTraits>::~NodeHierarchy ()
{
}

template <class BasicTraits>
void NodeHierarchy<BasicTraits>::addAdapter (const GeomObjectType& obj)
{
   CacheData& data = Cache::the()[obj];
   AdapterType* adapter = FactoryType::the().newObject();
   m_leaf.push_back(adapter);
   adapter->setObjectAdapter(&data); 
   adapter->init(obj); 
   data.setAdapter(AdapterType::getAdapterId(), adapter);
}

template <class BasicTraits> 
void NodeHierarchy<BasicTraits>::process (const GeomObjectType& node)
{
   // get geometry nodes
   std::vector<CacheData*> all;
   Cache::the().collectLeaves(all, node);
   // propagate to inner nodes
   std::vector<CacheData*>::const_iterator it;
   for (it=all.begin(); it!=all.end(); ++it) {
      CacheData& data = **it;
      if (data.getAdapter(AdapterType::getAdapterId()).size() == 0) {
	 continue;
      }
      AdapterType* leaf =
	static_cast<AdapterType*>(data.getAdapter(AdapterType::getAdapterId())[0]);
      for (NodePtr parent=data.getParent(); parent!=NullFC; parent=parent->getParent()) {
	 CacheData& data2 = Cache::the()[parent];
	 if (data2.getNode() == NullFC) { // parent node not in cache 
	    break;
	 }
	 data2.setAdapter(AdapterType::getAdapterId(), leaf);
      }
   }
}

template<class BasicTraits> 
void NodeHierarchy<BasicTraits>::destroy ()
{
   Cache::the().clearAdapter(AdapterType::getAdapterId());
   // clear adapter nodes
   for (std::vector<AdapterType*>::const_iterator it=m_leaf.begin();
	it != m_leaf.end();
	++it){
      FactoryType::the().deleteObject(*it);
   }
   m_leaf.clear();
}
template<class BasicTraits> 
void NodeHierarchy<BasicTraits>::clear   ()
{
   Cache::the().clearAdapter(AdapterType::getAdapterId());
}

template<class BasicTraits> 
void NodeHierarchy<BasicTraits>::registerFunctors (void)
{
#ifndef OSG_NOFUNCTORS
   Cache::the().registerEnterFunctor(Geometry::getClassType(), 
				   osgTypedMethodFunctor2ObjPtrCPtrRef<bool,
				   NodeHierarchy<BasicTraits>,
				   CNodePtr, 
				   OSGStaticInput*>
				   (this, &NodeHierarchy<BasicTraits>::functorEnterGeometry));
#else
   Cache::the().registerEnterFunction(Geometry::getClassType(),
   cacheFunctionFunctor2(NodeHierarchy<BasicTraits>::staticEnterGeometry));
#endif
}

template<class BasicTraits> 
bool NodeHierarchy<BasicTraits>::staticEnterGeometry  
(CNodePtr& cnode, OSGStaticInput* input)
{
   NodeHierarchy<BasicTraits>* thisInput = (NodeHierarchy<BasicTraits>*)input;
   if (thisInput == NULL) {
      return false;
   }
   thisInput->addAdapter(Cache::the().getCurrentNode());
   return true;
}

#ifndef OSG_NOFUNCTORS
template<class BasicTraits> 
bool NodeHierarchy<BasicTraits>::functorEnterGeometry 
(CNodePtr& cnode, OSGStaticInput* input)
{
   return staticEnterGeometry (cnode, input); 
}
#endif

// explicit template instantiations 
template class OSG_GENVISLIB_DLLMAPPING NodeHierarchy<OpenSGTraits>;

END_GENVIS_NAMESPACE
