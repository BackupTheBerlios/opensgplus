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

#include "OSGGVBVolHierarchy.h"
#include "OSGGVOptimizer.h"
USING_GENVIS_NAMESPACE
OSG_USING_NAMESPACE

// explicit template instantiations
template class OSG_GENVISLIB_DLLMAPPING BVolHierarchyBase<OpenSGTraits>;
template class OSG_GENVISLIB_DLLMAPPING ObjectBVolHierarchy<OpenSGTraits,OpenSGObjectInput<K6Dop> >;
template class OSG_GENVISLIB_DLLMAPPING ObjectBVolHierarchy<OpenSGTraits,OpenSGObjectInput<K14Dop> >;
template class OSG_GENVISLIB_DLLMAPPING ObjectBVolHierarchy<OpenSGTraits,OpenSGObjectInput<K18Dop> >;
template class OSG_GENVISLIB_DLLMAPPING ObjectBVolHierarchy<OpenSGTraits,OpenSGObjectInput<K26Dop> >;
template class OSG_GENVISLIB_DLLMAPPING ObjectBVolHierarchy<OpenSGTraits,OpenSGObjectInput<K12Dop> >;
template class OSG_GENVISLIB_DLLMAPPING ObjectBVolHierarchy<OpenSGTraits,OpenSGObjectInput<K20Dop> >;

template class OSG_GENVISLIB_DLLMAPPING FaceBVolHierarchy<OpenSGTraits,OpenSGFaceInput<K6Dop> >;
template class OSG_GENVISLIB_DLLMAPPING FaceBVolHierarchy<OpenSGTraits,OpenSGFaceInput<K14Dop> >;
template class OSG_GENVISLIB_DLLMAPPING FaceBVolHierarchy<OpenSGTraits,OpenSGFaceInput<K18Dop> >;
template class OSG_GENVISLIB_DLLMAPPING FaceBVolHierarchy<OpenSGTraits,OpenSGFaceInput<K26Dop> >;
template class OSG_GENVISLIB_DLLMAPPING FaceBVolHierarchy<OpenSGTraits,OpenSGFaceInput<K12Dop> >;
template class OSG_GENVISLIB_DLLMAPPING FaceBVolHierarchy<OpenSGTraits,OpenSGFaceInput<K20Dop> >;


template<class BasicTraits> 
BVolHierarchyBase<BasicTraits>::BVolHierarchyBase ()
{
}
template<class BasicTraits> 
BVolHierarchyBase<BasicTraits>::~BVolHierarchyBase ()
{
}


template<class BasicTraits, class InputTraits> 
ObjectBVolHierarchy<BasicTraits,InputTraits>::ObjectBVolHierarchy ()
  : m_root(NULL), m_oracle(NULL), m_minObjects(256), m_maxLevel(50)
{
}
template<class BasicTraits, class InputTraits> 
ObjectBVolHierarchy<BasicTraits,InputTraits>::~ObjectBVolHierarchy ()
{
   m_root = NULL;
}

template<class BasicTraits, class InputTraits> 
void ObjectBVolHierarchy<BasicTraits,InputTraits>::setParameter (const char* description, 
								 u32 maxLevel, 
								 u32 minObjects)
{
   m_oracle = Oracle<BVol>::getOracle(description);
   m_maxLevel   = maxLevel;
   m_minObjects = minObjects;
}
template<class BasicTraits, class InputTraits> 
void ObjectBVolHierarchy<BasicTraits,InputTraits>::setParameter (u32 method,
								 u32 maxLevel,
								 u32 minObjects)
{
   m_oracle = Oracle<BVol>::getOracle(method);
   m_maxLevel   = maxLevel;
   m_minObjects = minObjects;
}

template<class BasicTraits, class InputTraits> 
typename ObjectBVolHierarchy<BasicTraits,InputTraits>::GroupType* 
ObjectBVolHierarchy<BasicTraits,InputTraits>::getRoot () const
{
   assert(m_root != NULL);
   return m_root;
}

template <class BasicTraits, class InputTraits>
void ObjectBVolHierarchy<BasicTraits,InputTraits>::addAdapter (const TransformType&  m2w,
							       const GeomObjectType& obj)
{
   CacheData& data = Cache::the()[Cache::the().getCurrentNode()];
   data.setAdapterMatrix(AdapterType::getAdapterId(), m2w);

   AdapterType* adapter = LeafFactory::the().newObject(); m_leaf.push_back(adapter);
   adapter->setObjectAdapter(&data);
   adapter->init(m2w, obj);
   data.setAdapter(AdapterType::getAdapterId(), adapter);
}

template<class BasicTraits, class InputTraits> 
void ObjectBVolHierarchy<BasicTraits,InputTraits>::destroy ()
{
   m_root = NULL;
   // clear cache
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
void ObjectBVolHierarchy<BasicTraits,InputTraits>::clear ()
{
   m_root = NULL;
   // clear inner hierarchy nodes
   for (typename std::vector<GroupType*>::const_iterator it=m_inner.begin();
	it != m_inner.end();
	++it){
      InnerFactory::the().deleteObject(*it);
   }
   m_inner.clear();
}

template<class BasicTraits, class InputTraits> 
void ObjectBVolHierarchy<BasicTraits,InputTraits>::process   (const GeomObjectType& node)
{
   m_last = node;
}
template<class BasicTraits, class InputTraits> 
void ObjectBVolHierarchy<BasicTraits,InputTraits>::hierarchy (const GeomObjectType& node)
{
   typename CacheData::AdapterVector all;
   if (node == NullFC) {
      if (m_last == NullFC) {
	 Cache::the().collectAdapter(AdapterType::getAdapterId(), all);
      } else {
	 Cache::the().collectAdapter(AdapterType::getAdapterId(), all, m_last);
      }
   } else {
      Cache::the().collectAdapter(AdapterType::getAdapterId(), all, node);
   }
   Optimizer<GroupType> builder(m_oracle,
				all,
				m_maxLevel, m_minObjects);
   builder.setDataStore(&m_inner);
   m_root  = builder.optimize();
   m_depth = builder.getCurrentDepth();
}

template<class BasicTraits, class InputTraits> 
void ObjectBVolHierarchy<BasicTraits,InputTraits>::registerFunctors (void)
{
#ifndef OSG_NOFUNCTORS
   Cache::the().registerEnterFunctor(Geometry::getClassType(), 
				   osgTypedMethodFunctor2ObjPtrCPtrRef<bool,
				   ObjectBVolHierarchy<BasicTraits,InputTraits>, 
				   CNodePtr, 
				   OSGStaticInput*>
				   (this,
				    &ObjectBVolHierarchy<BasicTraits,InputTraits>::functorEnterGeometry));
#else
   Cache::the().registerEnterFunction(Geometry::getClassType(),
   cacheFunctionFunctor2(ObjectBVolHierarchy<BasicTraits,InputTraits>::staticEnterGeometry));
#endif
}

template<class BasicTraits, class InputTraits> 
bool ObjectBVolHierarchy<BasicTraits,InputTraits>::staticEnterGeometry 
(CNodePtr& cnode, OSGStaticInput* input)
{
   ObjectBVolHierarchy<BasicTraits,InputTraits>* thisInput = 
     (ObjectBVolHierarchy<BasicTraits,InputTraits>*)input;
   if (thisInput == NULL) {
      return false;
   }

   if (Cache::the().getCurrentMaterial() != NullFC) {
      GeometryPtr arg = GeometryPtr::dcast(cnode);
      arg->setMaterial(Cache::the().getCurrentMaterial());
   }
   thisInput->addAdapter(Cache::the().getCurrentMatrix(), Cache::the().getCurrentNode());
   return true;
}

#ifndef OSG_NOFUNCTORS
template<class BasicTraits, class InputTraits> 
bool ObjectBVolHierarchy<BasicTraits,InputTraits>::functorEnterGeometry 
(CNodePtr& cnode, OSGStaticInput* input)
{
   return staticEnterGeometry (cnode, input); 
}
#endif


// face
template<class BasicTraits, class InputTraits> 
FaceBVolHierarchy<BasicTraits,InputTraits>::FaceBVolHierarchy ()
  : m_root(NULL), m_oracle(NULL), m_minObjects(256), m_maxLevel(50)
{
}
template<class BasicTraits, class InputTraits> 
FaceBVolHierarchy<BasicTraits,InputTraits>::~FaceBVolHierarchy ()
{
   m_root = NULL;
}

template<class BasicTraits, class InputTraits> 
void FaceBVolHierarchy<BasicTraits,InputTraits>::setParameter (const char* description, 
							       u32 maxLevel, 
							       u32 minObjects)
{
   m_oracle = Oracle<BVol>::getOracle(description);
   m_maxLevel   = maxLevel;
   m_minObjects = minObjects;
}
template<class BasicTraits, class InputTraits> 
void FaceBVolHierarchy<BasicTraits,InputTraits>::setParameter (u32 method,
							       u32 maxLevel,
							       u32 minObjects)
{
   m_oracle = Oracle<BVol>::getOracle(method);
   m_maxLevel   = maxLevel;
   m_minObjects = minObjects;
}

template<class BasicTraits, class InputTraits> 
typename FaceBVolHierarchy<BasicTraits,InputTraits>::GroupType* 
FaceBVolHierarchy<BasicTraits,InputTraits>::getRoot () const
{
   assert(m_root != NULL);
   return m_root;
}

template <class BasicTraits, class InputTraits>
void FaceBVolHierarchy<BasicTraits,InputTraits>::addAdapter (const TransformType&  m2w,
							     const GeomObjectType& obj)
{
   CacheData& data = Cache::the()[obj];
   data.setAdapterMatrix(AdapterType::getAdapterId(), m2w);
   // iterate over all faces
   GeometryPtr geom = GeometryPtr::dcast(obj->getCore());
   FaceIterator end = geom->endFaces();
   for (FaceIterator face=geom->beginFaces(); 
	face!=end; 
	++face) {
      // create face adapter
      AdapterType* adapter = LeafFactory::the().newObject(); m_leaf.push_back(adapter);
      adapter->setObjectAdapter(&data);
      adapter->init(m2w, face);
      data.setAdapter(AdapterType::getAdapterId(), adapter);
   }
}

template<class BasicTraits, class InputTraits> 
void FaceBVolHierarchy<BasicTraits,InputTraits>::destroy ()
{
   m_root = NULL;
   // clear cache
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
void FaceBVolHierarchy<BasicTraits,InputTraits>::clear ()
{
   m_root = NULL;
   // clear inner hierarchy nodes
   for (typename std::vector<GroupType*>::const_iterator it=m_inner.begin();
	it != m_inner.end();
	++it){
      InnerFactory::the().deleteObject(*it);
   }
   m_inner.clear();
}

template<class BasicTraits, class InputTraits> 
void FaceBVolHierarchy<BasicTraits,InputTraits>::process   (const GeomObjectType& node)
{
   m_last = node;
}
template<class BasicTraits, class InputTraits> 
void FaceBVolHierarchy<BasicTraits,InputTraits>::hierarchy (const GeomObjectType& node)
{
   typename CacheData::AdapterVector all;
   if (node == NullFC) {
      if (m_last == NullFC) {
	 Cache::the().collectAdapter(AdapterType::getAdapterId(), all);
      } else {
	 Cache::the().collectAdapter(AdapterType::getAdapterId(), all, m_last);
      }
   } else {
      Cache::the().collectAdapter(AdapterType::getAdapterId(), all, node);
   }
   Optimizer<GroupType> builder(m_oracle,
				all,
				m_maxLevel, m_minObjects);
   builder.setDataStore(&m_inner);
   m_root  = builder.optimize();
   m_depth = builder.getCurrentDepth();
}

template<class BasicTraits, class InputTraits> 
void FaceBVolHierarchy<BasicTraits,InputTraits>::registerFunctors (void)
{
#ifndef OSG_NOFUNCTORS
   Cache::the().registerEnterFunctor(Geometry::getClassType(), 
				   osgTypedMethodFunctor2ObjPtrCPtrRef<bool,
				   FaceBVolHierarchy<BasicTraits,InputTraits>, 
				   CNodePtr, 
				   OSGStaticInput*>
				   (this,
				    &FaceBVolHierarchy<BasicTraits,InputTraits>::functorEnterGeometry));
#else
   Cache::the().registerEnterFunction(Geometry::getClassType(),
   cacheFunctionFunctor2(FaceBVolHierarchy<BasicTraits,InputTraits>::staticEnterGeometry));
#endif
}

template<class BasicTraits, class InputTraits> 
bool FaceBVolHierarchy<BasicTraits,InputTraits>::staticEnterGeometry 
(CNodePtr& cnode, OSGStaticInput* input)
{
   FaceBVolHierarchy<BasicTraits,InputTraits>* thisInput = 
     (FaceBVolHierarchy<BasicTraits,InputTraits>*)input;
   if (thisInput == NULL) {
      return false;
   }

   if (Cache::the().getCurrentMaterial() != NullFC) {
      GeometryPtr arg = GeometryPtr::dcast(cnode);
      arg->setMaterial(Cache::the().getCurrentMaterial());
   }
   thisInput->addAdapter(Cache::the().getCurrentMatrix(), Cache::the().getCurrentNode());
   return true;
}

#ifndef OSG_NOFUNCTORS
template<class BasicTraits, class InputTraits> 
bool FaceBVolHierarchy<BasicTraits,InputTraits>::functorEnterGeometry 
(CNodePtr& cnode, OSGStaticInput* input)
{
   return staticEnterGeometry (cnode, input); 
}
#endif


// primitive
template<class BasicTraits, class InputTraits> 
PrimitiveBVolHierarchy<BasicTraits,InputTraits>::PrimitiveBVolHierarchy ()
  : m_root(NULL), m_oracle(NULL), m_minObjects(256), m_maxLevel(50)
{
}
template<class BasicTraits, class InputTraits> 
PrimitiveBVolHierarchy<BasicTraits,InputTraits>::~PrimitiveBVolHierarchy ()
{
   m_root = NULL;
}

template<class BasicTraits, class InputTraits> 
void PrimitiveBVolHierarchy<BasicTraits,InputTraits>::setParameter (const char* description, 
								    u32 maxLevel, 
								    u32 minObjects)
{
   m_oracle = Oracle<BVol>::getOracle(description);
   m_maxLevel   = maxLevel;
   m_minObjects = minObjects;
}
template<class BasicTraits, class InputTraits> 
void PrimitiveBVolHierarchy<BasicTraits,InputTraits>::setParameter (u32 method,
								    u32 maxLevel,
								    u32 minObjects)
{
   m_oracle = Oracle<BVol>::getOracle(method);
   m_maxLevel   = maxLevel;
   m_minObjects = minObjects;
}

template<class BasicTraits, class InputTraits> 
typename PrimitiveBVolHierarchy<BasicTraits,InputTraits>::GroupType* 
PrimitiveBVolHierarchy<BasicTraits,InputTraits>::getRoot () const
{
   assert(m_root != NULL);
   return m_root;
}

template <class BasicTraits, class InputTraits>
void PrimitiveBVolHierarchy<BasicTraits,InputTraits>::addAdapter (const TransformType&  m2w,
								  const GeomObjectType& obj)
{
   CacheData& data = Cache::the()[obj];
   data.setAdapterMatrix(AdapterType::getAdapterId(), m2w);
   // iterate over all prims
   GeometryPtr geom = GeometryPtr::dcast(obj->getCore());
   PrimitiveIterator end = geom->endPrimitives();
   for (PrimitiveIterator prim=geom->beginPrimitives(); 
	prim!=end; 
	++prim) {
      // create prim adapter
      AdapterType* adapter = LeafFactory::the().newObject(); m_leaf.push_back(adapter);
      adapter->setObjectAdapter(&data);
      adapter->init(m2w, prim);
      data.setAdapter(AdapterType::getAdapterId(), adapter);
   }
}

template<class BasicTraits, class InputTraits> 
void PrimitiveBVolHierarchy<BasicTraits,InputTraits>::destroy ()
{
   m_root = NULL;
   // clear cache
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
void PrimitiveBVolHierarchy<BasicTraits,InputTraits>::clear ()
{
   m_root = NULL;
   // clear inner hierarchy nodes
   for (typename std::vector<GroupType*>::const_iterator it=m_inner.begin();
	it != m_inner.end();
	++it){
      InnerFactory::the().deleteObject(*it);
   }
   m_inner.clear();
}

template<class BasicTraits, class InputTraits> 
void PrimitiveBVolHierarchy<BasicTraits,InputTraits>::process   (const GeomObjectType& node)
{
   m_last = node;
}
template<class BasicTraits, class InputTraits> 
void PrimitiveBVolHierarchy<BasicTraits,InputTraits>::hierarchy (const GeomObjectType& node)
{
   typename CacheData::AdapterVector all;
   if (node == NullFC) {
      if (m_last == NullFC) {
	 Cache::the().collectAdapter(AdapterType::getAdapterId(), all);
      } else {
	 Cache::the().collectAdapter(AdapterType::getAdapterId(), all, m_last);
      }
   } else {
      Cache::the().collectAdapter(AdapterType::getAdapterId(), all, node);
   }
   Optimizer<GroupType> builder(m_oracle,
				all,
				m_maxLevel, m_minObjects);
   builder.setDataStore(&m_inner);
   m_root  = builder.optimize();
   m_depth = builder.getCurrentDepth();
}

template<class BasicTraits, class InputTraits> 
void PrimitiveBVolHierarchy<BasicTraits,InputTraits>::registerFunctors (void)
{
#ifndef OSG_NOFUNCTORS
   Cache::the().registerEnterFunctor(Geometry::getClassType(), 
				   osgTypedMethodFunctor2ObjPtrCPtrRef<bool,
				   PrimitiveBVolHierarchy<BasicTraits,InputTraits>, 
				   CNodePtr, 
				   OSGStaticInput*>
				   (this,
				    &PrimitiveBVolHierarchy<BasicTraits,InputTraits>::functorEnterGeometry));
#else
   Cache::the().registerEnterFunction(Geometry::getClassType(),
   cacheFunctionFunctor2(PrimitiveBVolHierarchy<BasicTraits,InputTraits>::staticEnterGeometry));
#endif
}

template<class BasicTraits, class InputTraits> 
bool PrimitiveBVolHierarchy<BasicTraits,InputTraits>::staticEnterGeometry 
(CNodePtr& cnode, OSGStaticInput* input)
{
   PrimitiveBVolHierarchy<BasicTraits,InputTraits>* thisInput = 
     (PrimitiveBVolHierarchy<BasicTraits,InputTraits>*)input;
   if (thisInput == NULL) {
      return false;
   }

   if (Cache::the().getCurrentMaterial() != NullFC) {
      GeometryPtr arg = GeometryPtr::dcast(cnode);
      arg->setMaterial(Cache::the().getCurrentMaterial());
   }
   thisInput->addAdapter(Cache::the().getCurrentMatrix(), Cache::the().getCurrentNode());
   return true;
}

#ifndef OSG_NOFUNCTORS
template<class BasicTraits, class InputTraits> 
bool PrimitiveBVolHierarchy<BasicTraits,InputTraits>::functorEnterGeometry 
(CNodePtr& cnode, OSGStaticInput* input)
{
   return staticEnterGeometry (cnode, input); 
}
#endif









