//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.2 $
//   $Date: 2003/09/19 22:02:20 $
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
template<class BasicTraits> 
void BVolHierarchyBase<BasicTraits>::setParameter (Oracle*, unsigned, unsigned)
{
}
template<class BasicTraits> 
void BVolHierarchyBase<BasicTraits>::setParameter (SplitAlgorithm, unsigned, unsigned)
{
}

template<class BasicTraits, class InputTraits> 
ObjectBVolHierarchy<BasicTraits,InputTraits>::ObjectBVolHierarchy ()
  : m_root(NULL), m_oracle(NULL), m_deleteOracle(false), m_minObjects(256), m_maxLevel(50)
{
}
template<class BasicTraits, class InputTraits> 
ObjectBVolHierarchy<BasicTraits,InputTraits>::~ObjectBVolHierarchy ()
{
   m_root = NULL;
   if (m_deleteOracle) {
      delete m_oracle;
   }
}

template<class BasicTraits, class InputTraits> 
void ObjectBVolHierarchy<BasicTraits,InputTraits>::setParameter (Oracle*  method, 
								 unsigned maxLevel, 
								 unsigned minObjects)
{
   if (m_oracle != NULL && m_deleteOracle) {
      delete m_oracle;
   }
   m_oracle       = method;
   m_deleteOracle = false;
   m_maxLevel   = maxLevel;
   m_minObjects = minObjects;
}
template<class BasicTraits, class InputTraits> 
void ObjectBVolHierarchy<BasicTraits,InputTraits>::setParameter
(SplitAlgorithm method,
 unsigned maxLevel,
 unsigned minObjects)
{
   if (m_oracle != NULL && m_deleteOracle) {
      delete m_oracle;
   }
   m_oracle       = createOracle(method);
   m_deleteOracle = true;
   m_maxLevel   = maxLevel;
   m_minObjects = minObjects;
}

template<class BasicTraits, class InputTraits> 
ObjectBVolHierarchy<BasicTraits,InputTraits>::GroupType* 
ObjectBVolHierarchy<BasicTraits,InputTraits>::getRoot () const
{
   assert(m_root != NULL);
   return m_root;
}

template <class BasicTraits, class InputTraits>
void ObjectBVolHierarchy<BasicTraits,InputTraits>::addAdapter (const TransformType&  m2w,
							       const GeomObjectType& obj)
{
   CacheData& data = getCache()[getCache().getCurrentNode()];
   data.setAdapterMatrix(AdapterType::getAdapterId(), m2w);

   AdapterType* adapter = LeafFactory::the().newObject();
   m_leaf.push_back(adapter);
   adapter->setObjectAdapter(&data);
   adapter->init(m2w, obj);
   data.setAdapter(AdapterType::getAdapterId(), adapter);
}

template<class BasicTraits, class InputTraits> 
void ObjectBVolHierarchy<BasicTraits,InputTraits>::destroy ()
{
   m_root = NULL;
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
void ObjectBVolHierarchy<BasicTraits,InputTraits>::clear ()
{
   m_root = NULL;
   // clear cache
   //getCache().clearAdapter(AdapterType::getAdapterId());
   // clear inner hierarchy nodes
   for (std::vector<GroupType*>::const_iterator it=m_inner.begin();
	it != m_inner.end();
	++it){
      InnerFactory::the().deleteObject(*it);
   }
   m_inner.clear();
}

template<class BasicTraits, class InputTraits> 
unsigned ObjectBVolHierarchy<BasicTraits,InputTraits>::hierarchy ()
{
   typename CacheData::AdapterVector all;
   getCache().collectAdapter(AdapterType::getAdapterId(), all);
   Optimizer<GroupType> builder(m_oracle,
				all,
				m_maxLevel, m_minObjects);
   builder.setDataStore(&m_inner);
   m_root = builder.optimize();
   return builder.getCurrentDepth();
}
template<class BasicTraits, class InputTraits> 
Oracle* ObjectBVolHierarchy<BasicTraits,InputTraits>::createOracle
(SplitAlgorithm method)
{
   switch (method) {
   default:
   case LongestSideMedianId:
     return new LongestSideMedian<BVol> ();
   case LongestSideMeanId:
     return new LongestSideMean<BVol> ();
   case MinimumSICId:
     return new MinimumSIC<BVol> ();
   case MinimumVolumeId:
     return new MinimumVolume<BVol> ();
   }
}

template<class BasicTraits, class InputTraits> 
void ObjectBVolHierarchy<BasicTraits,InputTraits>::registerFunctors (void)
{
#ifndef OSG_NOFUNCTORS
   getCache().registerEnterFunctor(Geometry::getClassType(), 
				   osgTypedMethodVoidFunctor2ObjPtrCPtrRef<
				   ObjectBVolHierarchy<BasicTraits,InputTraits>, 
				   CNodePtr, 
				   OSGStaticInput*>
				   (this,
				    &ObjectBVolHierarchy<BasicTraits,InputTraits>::functorEnterGeometry));
#else
   getCache().registerEnterFunction(Geometry::getClassType(),
   cacheFunctionFunctor2(ObjectBVolHierarchy<BasicTraits,InputTraits>::staticEnterGeometry));
#endif
}

template<class BasicTraits, class InputTraits> 
void ObjectBVolHierarchy<BasicTraits,InputTraits>::staticEnterGeometry (
CNodePtr& cnode, OSGStaticInput* input)
{
   ObjectBVolHierarchy<BasicTraits,InputTraits>* thisInput = 
     (ObjectBVolHierarchy<BasicTraits,InputTraits>*)input;
   if (thisInput == NULL) {
      return;
   }

   Cache& cache = thisInput->getCache();
   if (cache.getCurrentMaterial() != NullFC) {
      GeometryPtr arg = GeometryPtr::dcast(cnode);
      arg->setMaterial(cache.getCurrentMaterial());
   }
   thisInput->addAdapter(cache.getCurrentMatrix(), cache.getCurrentNode());
}

#ifndef OSG_NOFUNCTORS
template<class BasicTraits, class InputTraits> 
void ObjectBVolHierarchy<BasicTraits,InputTraits>::functorEnterGeometry (
CNodePtr& cnode, OSGStaticInput*)
{
   staticEnterGeometry (cnode, this); 
}
#endif


// face
template<class BasicTraits, class InputTraits> 
FaceBVolHierarchy<BasicTraits,InputTraits>::FaceBVolHierarchy ()
  : m_root(NULL), m_oracle(NULL), m_deleteOracle(false), m_minObjects(256), m_maxLevel(50)
{
}
template<class BasicTraits, class InputTraits> 
FaceBVolHierarchy<BasicTraits,InputTraits>::~FaceBVolHierarchy ()
{
   m_root = NULL;
   if (m_deleteOracle) {
      delete m_oracle;
   }
}

template<class BasicTraits, class InputTraits> 
void FaceBVolHierarchy<BasicTraits,InputTraits>::setParameter (Oracle*  method, 
							       unsigned maxLevel, 
							       unsigned minObjects)
{
   if (m_oracle != NULL && m_deleteOracle) {
      delete m_oracle;
   }
   m_oracle       = method;
   m_deleteOracle = false;
   m_maxLevel   = maxLevel;
   m_minObjects = minObjects;
}
template<class BasicTraits, class InputTraits> 
void FaceBVolHierarchy<BasicTraits,InputTraits>::setParameter
(SplitAlgorithm method,
 unsigned maxLevel,
 unsigned minObjects)
{
   if (m_oracle != NULL && m_deleteOracle) {
      delete m_oracle;
   }
   m_oracle       = createOracle(method);
   m_deleteOracle = true;
   m_maxLevel   = maxLevel;
   m_minObjects = minObjects;
}

template<class BasicTraits, class InputTraits> 
FaceBVolHierarchy<BasicTraits,InputTraits>::GroupType* 
FaceBVolHierarchy<BasicTraits,InputTraits>::getRoot () const
{
   assert(m_root != NULL);
   return m_root;
}

template <class BasicTraits, class InputTraits>
void FaceBVolHierarchy<BasicTraits,InputTraits>::addAdapter (const TransformType&  m2w,
							     const GeomObjectType& obj)
{
   CacheData& data = getCache()[obj];
   data.setAdapterMatrix(AdapterType::getAdapterId(), m2w);

   // iterate over all faces
   GeometryPtr geom = GeometryPtr::dcast(obj->getCore());
   FaceIterator end = geom->endFaces();
   for (FaceIterator face=geom->beginFaces(); 
	face!=end; 
	++face) {
      // create face adapter
      AdapterType* adapter = LeafFactory::the().newObject();
      m_leaf.push_back(adapter);
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
void FaceBVolHierarchy<BasicTraits,InputTraits>::clear ()
{
   m_root = NULL;
   // clear cache
   //getCache().clearAdapter(AdapterType::getAdapterId());
   // clear inner hierarchy nodes
   for (std::vector<GroupType*>::const_iterator it=m_inner.begin();
	it != m_inner.end();
	++it){
      InnerFactory::the().deleteObject(*it);
   }
   m_inner.clear();
}

template<class BasicTraits, class InputTraits> 
unsigned FaceBVolHierarchy<BasicTraits,InputTraits>::hierarchy ()
{
   typename CacheData::AdapterVector all;
   getCache().collectAdapter(AdapterType::getAdapterId(), all);
   Optimizer<GroupType> builder(m_oracle,
				all,
				m_maxLevel, m_minObjects);
   builder.setDataStore(&m_inner);
   m_root = builder.optimize();
   return builder.getCurrentDepth();
}
template<class BasicTraits, class InputTraits> 
unsigned FaceBVolHierarchy<BasicTraits,InputTraits>::hierarchyNode (const NodePtr& node)
{
   Optimizer<GroupType> builder(m_oracle,
			        getCache()[node].getAdapter(AdapterType::getAdapterId()),
				m_maxLevel, m_minObjects);
   builder.setDataStore(&m_inner);
   m_root = builder.optimize();
   return builder.getCurrentDepth();
}

template<class BasicTraits, class InputTraits> 
Oracle* FaceBVolHierarchy<BasicTraits,InputTraits>::createOracle
(SplitAlgorithm method)
{
   switch (method) {
   default:
   case LongestSideMedianId:
     return new LongestSideMedian<BVol> ();
   case LongestSideMeanId:
     return new LongestSideMean<BVol> ();
   case MinimumSICId:
     return new MinimumSIC<BVol> ();
   case MinimumVolumeId:
     return new MinimumVolume<BVol> ();
   }
}

template<class BasicTraits, class InputTraits> 
void FaceBVolHierarchy<BasicTraits,InputTraits>::registerFunctors (void)
{
#ifndef OSG_NOFUNCTORS
   getCache().registerEnterFunctor(Geometry::getClassType(), 
				   osgTypedMethodVoidFunctor2ObjPtrCPtrRef<
				   FaceBVolHierarchy<BasicTraits,InputTraits>, 
				   CNodePtr, 
				   OSGStaticInput*>
				   (this,
				    &FaceBVolHierarchy<BasicTraits,InputTraits>::functorEnterGeometry));
#else
   getCache().registerEnterFunction(Geometry::getClassType(),
   cacheFunctionFunctor2(FaceBVolHierarchy<BasicTraits,InputTraits>::staticEnterGeometry));
#endif
}

template<class BasicTraits, class InputTraits> 
void FaceBVolHierarchy<BasicTraits,InputTraits>::staticEnterGeometry (
CNodePtr& cnode, OSGStaticInput* input)
{
   FaceBVolHierarchy<BasicTraits,InputTraits>* thisInput = 
     (FaceBVolHierarchy<BasicTraits,InputTraits>*)input;
   if (thisInput == NULL) {
      return;
   }

   Cache& cache = thisInput->getCache();
   if (cache.getCurrentMaterial() != NullFC) {
      GeometryPtr arg = GeometryPtr::dcast(cnode);
      arg->setMaterial(cache.getCurrentMaterial());
   }
   thisInput->addAdapter(cache.getCurrentMatrix(), cache.getCurrentNode());
}

#ifndef OSG_NOFUNCTORS
template<class BasicTraits, class InputTraits> 
void FaceBVolHierarchy<BasicTraits,InputTraits>::functorEnterGeometry (
CNodePtr& cnode, OSGStaticInput*)
{
   staticEnterGeometry (cnode, this); 
}
#endif


// primitive
template<class BasicTraits, class InputTraits> 
PrimitiveBVolHierarchy<BasicTraits,InputTraits>::PrimitiveBVolHierarchy ()
  : m_root(NULL), m_oracle(NULL), m_deleteOracle(false), m_minObjects(256), m_maxLevel(50)
{
}
template<class BasicTraits, class InputTraits> 
PrimitiveBVolHierarchy<BasicTraits,InputTraits>::~PrimitiveBVolHierarchy ()
{
   m_root = NULL;
   if (m_deleteOracle) {
      delete m_oracle;
   }
}

template<class BasicTraits, class InputTraits> 
void PrimitiveBVolHierarchy<BasicTraits,InputTraits>::setParameter (Oracle*  method, 
							       unsigned maxLevel, 
							       unsigned minObjects)
{
   if (m_oracle != NULL && m_deleteOracle) {
      delete m_oracle;
   }
   m_oracle       = method;
   m_deleteOracle = false;
   m_maxLevel   = maxLevel;
   m_minObjects = minObjects;
}
template<class BasicTraits, class InputTraits> 
void PrimitiveBVolHierarchy<BasicTraits,InputTraits>::setParameter
(SplitAlgorithm method,
 unsigned maxLevel,
 unsigned minObjects)
{
   if (m_oracle != NULL && m_deleteOracle) {
      delete m_oracle;
   }
   m_oracle       = createOracle(method);
   m_deleteOracle = true;
   m_maxLevel   = maxLevel;
   m_minObjects = minObjects;
}

template<class BasicTraits, class InputTraits> 
PrimitiveBVolHierarchy<BasicTraits,InputTraits>::GroupType* 
PrimitiveBVolHierarchy<BasicTraits,InputTraits>::getRoot () const
{
   assert(m_root != NULL);
   return m_root;
}

template <class BasicTraits, class InputTraits>
void PrimitiveBVolHierarchy<BasicTraits,InputTraits>::addAdapter (const TransformType&  m2w,
								  const GeomObjectType& obj)
{
   CacheData& data = getCache()[obj];
   data.setAdapterMatrix(AdapterType::getAdapterId(), m2w);

   // iterate over all prims
   GeometryPtr geom = GeometryPtr::dcast(obj->getCore());
   PrimitiveIterator end = geom->endPrimitives();
   for (PrimitiveIterator prim=geom->beginPrimitives(); 
	prim!=end; 
	++prim) {
      // create prim adapter
      AdapterType* adapter = LeafFactory::the().newObject();
      m_leaf.push_back(adapter);
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
void PrimitiveBVolHierarchy<BasicTraits,InputTraits>::clear ()
{
   m_root = NULL;
   // clear cache
   //getCache().clearAdapter(AdapterType::getAdapterId());
   // clear inner hierarchy nodes
   for (std::vector<GroupType*>::const_iterator it=m_inner.begin();
	it != m_inner.end();
	++it){
      InnerFactory::the().deleteObject(*it);
   }
   m_inner.clear();
}

template<class BasicTraits, class InputTraits> 
unsigned PrimitiveBVolHierarchy<BasicTraits,InputTraits>::hierarchy ()
{
   typename CacheData::AdapterVector all;
   getCache().collectAdapter(AdapterType::getAdapterId(), all);
   Optimizer<GroupType> builder(m_oracle,
				all,
				m_maxLevel, m_minObjects);
   builder.setDataStore(&m_inner);
   m_root = builder.optimize();
   return builder.getCurrentDepth();
}
template<class BasicTraits, class InputTraits> 
unsigned PrimitiveBVolHierarchy<BasicTraits,InputTraits>::hierarchyNode (const NodePtr& node)
{
   Optimizer<GroupType> builder(m_oracle,
			        getCache()[node].getAdapter(AdapterType::getAdapterId()),
				m_maxLevel, m_minObjects);
   builder.setDataStore(&m_inner);
   m_root = builder.optimize();
   return builder.getCurrentDepth();
}

template<class BasicTraits, class InputTraits> 
Oracle* PrimitiveBVolHierarchy<BasicTraits,InputTraits>::createOracle
(SplitAlgorithm method)
{
   switch (method) {
   default:
   case LongestSideMedianId:
     return new LongestSideMedian<BVol> ();
   case LongestSideMeanId:
     return new LongestSideMean<BVol> ();
   case MinimumSICId:
     return new MinimumSIC<BVol> ();
   case MinimumVolumeId:
     return new MinimumVolume<BVol> ();
   }
}

template<class BasicTraits, class InputTraits> 
void PrimitiveBVolHierarchy<BasicTraits,InputTraits>::registerFunctors (void)
{
#ifndef OSG_NOFUNCTORS
   getCache().registerEnterFunctor(Geometry::getClassType(), 
				   osgTypedMethodVoidFunctor2ObjPtrCPtrRef<
				   PrimitiveBVolHierarchy<BasicTraits,InputTraits>, 
				   CNodePtr, 
				   OSGStaticInput*>
				   (this,
				    &PrimitiveBVolHierarchy<BasicTraits,InputTraits>::functorEnterGeometry));
#else
   getCache().registerEnterFunction(Geometry::getClassType(),
   cacheFunctionFunctor2(PrimitiveBVolHierarchy<BasicTraits,InputTraits>::staticEnterGeometry));
#endif
}

template<class BasicTraits, class InputTraits> 
void PrimitiveBVolHierarchy<BasicTraits,InputTraits>::staticEnterGeometry (
CNodePtr& cnode, OSGStaticInput* input)
{
   PrimitiveBVolHierarchy<BasicTraits,InputTraits>* thisInput = 
     (PrimitiveBVolHierarchy<BasicTraits,InputTraits>*)input;
   if (thisInput == NULL) {
      return;
   }

   Cache& cache = thisInput->getCache();
   if (cache.getCurrentMaterial() != NullFC) {
      GeometryPtr arg = GeometryPtr::dcast(cnode);
      arg->setMaterial(cache.getCurrentMaterial());
   }
   thisInput->addAdapter(cache.getCurrentMatrix(), cache.getCurrentNode());
}

#ifndef OSG_NOFUNCTORS
template<class BasicTraits, class InputTraits> 
void PrimitiveBVolHierarchy<BasicTraits,InputTraits>::functorEnterGeometry (
CNodePtr& cnode, OSGStaticInput*)
{
   staticEnterGeometry (cnode, this); 
}
#endif









