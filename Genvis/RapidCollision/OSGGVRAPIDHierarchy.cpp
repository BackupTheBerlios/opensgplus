//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.5 $
//   $Date: 2006/06/08 16:57:46 $
//                                                                            
//=============================================================================

#include "OSGGVRAPIDHierarchy.h"
OSG_USING_NAMESPACE
USING_GENVIS_NAMESPACE

BEGIN_GENVIS_NAMESPACE

template <class BasicTraits> 
RAPIDHierarchy<BasicTraits>::RAPIDHierarchy ()
  : Inherited()
{
   m_current = NULL;
}
template <class BasicTraits> 
RAPIDHierarchy<BasicTraits>::~RAPIDHierarchy ()
{
}

template <class BasicTraits>
void RAPIDHierarchy<BasicTraits>::addAdapter (const GeomObjectType& obj)
{
   CacheData& data = Cache::the()[Cache::the().getCurrentNode()];
   AdapterType* adapter = FactoryType::the().newObject(); m_leaf.push_back(adapter);
   adapter->setObjectAdapter(&data); 
   adapter->init(obj); 
   data.setAdapter(AdapterType::getAdapterId(), adapter);
}
template <class BasicTraits>
void RAPIDHierarchy<BasicTraits>::addAdapter (const TransformType&  m2w,
					      const GeomObjectType& obj)
{
   CacheData& data = Cache::the()[obj];
   data.setAdapterMatrix(AdapterType::getAdapterId(), m2w);

   if (m_current == NULL) {
      m_current = FactoryType::the().newObject();
      m_leaf.push_back(m_current);
      m_current->setObjectAdapter(&data);
      m_current->init(m2w, obj);
   } else {
      m_current->init(m2w, obj);
   }
}

template<class BasicTraits> 
void RAPIDHierarchy<BasicTraits>::hierarchyGlobal (const GeomObjectType& node)
{
   if (m_current == NULL) {
      return;
   }
   // post init
   m_current->postInit(node);
   // propagate to parent nodes
   for (NodePtr parent=node; parent!=NullFC; parent=parent->getParent()) {
      CacheData& data2 = Cache::the()[parent];
      if (data2.getNode() == NullFC) { // parent node not in cache 
	 break;
      }
      data2.setAdapter(AdapterType::getAdapterId(), m_current);
   }
   m_current = NULL;
}

template<class BasicTraits> 
void RAPIDHierarchy<BasicTraits>::hierarchyLocal (const GeomObjectType& node)
{
   // mark geometry nodes
   std::vector<CacheData*> all;
   Cache::the().collectLeaves(all, node);
   // propagate to inner nodes
   typename std::vector<CacheData*>::const_iterator it;
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

template <class BasicTraits> 
void RAPIDHierarchy<BasicTraits>::process (const GeomObjectType& node)
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

template<class BasicTraits> 
void RAPIDHierarchy<BasicTraits>::destroy ()
{
   Cache::the().clearAdapter(AdapterType::getAdapterId());
   // clear adapter nodes
   typename std::vector<AdapterType*>::const_iterator it;
   for (it = m_leaf.begin();
	it != m_leaf.end();
	++it){
      FactoryType::the().deleteObject(*it);
   }
   m_leaf.clear();
}
template<class BasicTraits> 
void RAPIDHierarchy<BasicTraits>::clear   ()
{
   Cache::the().clearAdapter(AdapterType::getAdapterId());
}

template<class BasicTraits> 
void RAPIDHierarchy<BasicTraits>::registerFunctors (void)
{
#ifndef OSG_NOFUNCTORS
   Cache::the().registerEnterFunctor(Geometry::getClassType(), 
				   osgTypedMethodFunctor2ObjPtrCPtrRef<bool,
				   RAPIDHierarchy<BasicTraits>,
				   CNodePtr, 
				   OSGStaticInput*>
				   (this, &RAPIDHierarchy<BasicTraits>::functorEnterGeometry));
#else
   Cache::the().registerEnterFunction(Geometry::getClassType(),
   cacheFunctionFunctor2(RAPIDHierarchy<BasicTraits>::staticEnterGeometry));
#endif
}

template<class BasicTraits> 
bool RAPIDHierarchy<BasicTraits>::staticEnterGeometry  
(CNodePtr& cnode, OSGStaticInput* input)
{
   RAPIDHierarchy<BasicTraits>* thisInput = (RAPIDHierarchy<BasicTraits>*)input;
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
template<class BasicTraits> 
bool RAPIDHierarchy<BasicTraits>::functorEnterGeometry 
(CNodePtr& cnode, OSGStaticInput* input)
{
   return staticEnterGeometry (cnode, input); 
}
#endif

// explicit template instantiations
template class OSG_GENVISLIB_DLLMAPPING RAPIDHierarchy<OpenSGTraits>;

END_GENVIS_NAMESPACE
