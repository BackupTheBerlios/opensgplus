//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.1 $
//   $Date: 2003/09/11 16:20:31 $
//                                                                            
//=============================================================================

#include "OSGGVRAPIDHierarchy.h"

OSG_USING_NAMESPACE
USING_GENVIS_NAMESPACE

// explicit template instantiations
template class OSG_GENVISLIB_DLLMAPPING RAPIDHierarchy<OpenSGTraits>;

template <class BasicTraits> 
RAPIDHierarchy<BasicTraits>::RAPIDHierarchy ()
  : Inherited()
{
}
template <class BasicTraits> 
RAPIDHierarchy<BasicTraits>::~RAPIDHierarchy ()
{
}

template <class BasicTraits>
void RAPIDHierarchy<BasicTraits>::addAdapter (const GeomObjectType& obj)
{
   CacheData& data = getCache()[getCache().getCurrentNode()];
   AdapterType* adapter = FactoryType::the().newObject();
   m_leaf.push_back(adapter);
   adapter->setObjectAdapter(&data); 
   adapter->init(obj); 
   data.setAdapter(AdapterType::getAdapterId(), adapter);
}

template <class BasicTraits> 
void RAPIDHierarchy<BasicTraits>::hierarchy ()
{
   // mark geometry nodes
   std::vector<bool> isLeaf;
   Cache::EntryIterator it;
   for (it = getCache().begin(); it != getCache().end(); ++it) {
      Cache::CacheData& data = getCache()[*it];
      if (data.getAdapter(AdapterType::getAdapterId()).size() == 0) {
	 isLeaf.push_back(false);
      } else {
	 isLeaf.push_back(true);
      }
   }
   // propagate to inner nodes
   std::vector<bool>::const_iterator itLeaf;
   for (it=getCache().begin(), itLeaf=isLeaf.begin(); it != getCache().end(); ++it, ++itLeaf) {
      if (*itLeaf) {
	 Cache::CacheData& data = getCache()[*it];
	 AdapterType* leaf =
	   static_cast<AdapterType*>(*data.getAdapter(AdapterType::getAdapterId()).begin());
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

template<class BasicTraits> 
void RAPIDHierarchy<BasicTraits>::destroy ()
{
   getCache().clearAdapter(AdapterType::getAdapterId());
   // clear adapter nodes
   for (std::vector<AdapterType*>::const_iterator it=m_leaf.begin();
	it != m_leaf.end();
	++it){
      FactoryType::the().deleteObject(*it);
   }
   m_leaf.clear();
}
template<class BasicTraits> 
void RAPIDHierarchy<BasicTraits>::clear   ()
{
   getCache().clearAdapter(AdapterType::getAdapterId());
}

template<class BasicTraits> 
void RAPIDHierarchy<BasicTraits>::registerFunctors (void)
{
#ifndef OSG_NOFUNCTORS
   getCache().registerEnterFunctor(Geometry::getClassType(), 
				   osgTypedMethodVoidFunctor2ObjPtrCPtrRef<
				   RAPIDHierarchy<BasicTraits>,
				   CNodePtr, 
				   OSGStaticInput*>
				   (this, &RAPIDHierarchy<BasicTraits>::functorEnterGeometry));
#else
   getCache().registerEnterFunction(Geometry::getClassType(),
   cacheFunctionFunctor2(RAPIDHierarchy<BasicTraits>::staticEnterGeometry));
#endif
}

template<class BasicTraits> 
void RAPIDHierarchy<BasicTraits>::staticEnterGeometry  
(CNodePtr& cnode, OSGStaticInput* input)
{
   RAPIDHierarchy<BasicTraits>* thisInput = (RAPIDHierarchy<BasicTraits>*)input;
   if (thisInput == NULL) {
      return;
   }
   thisInput->addAdapter(getCache().getCurrentNode());
}

#ifndef OSG_NOFUNCTORS
template<class BasicTraits> 
void RAPIDHierarchy<BasicTraits>::functorEnterGeometry (
CNodePtr& cnode, OSGStaticInput*)
{
   staticEnterGeometry (cnode, this); 
}
#endif
