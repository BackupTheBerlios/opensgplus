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

#include "OSGGVCache.h"
#include "OSGGVStaticInput.h"
#include "OSGMaterialGroup.h"
#include "OSGTransform.h"

OSG_USING_NAMESPACE
USING_GENVIS_NAMESPACE


#ifdef GV_CACHE_HASHMAP

//OpenSGCache::HashFunctor OpenSGCache::s_hasher;

void OpenSGCache::setHierarchy (genvis::OSGStaticInput* hier)
{
   m_hierarchy = hier;
   m_hierarchy->registerFunctors();
}

OpenSGCache::OpenSGCache ()
  : m_data(),
    m_currentNode(NullFC),
    m_hierarchy(NULL)
{
   m_data[NullFC] = CacheData(NullFC, Matrix::identity());
   registerFunctors();
}

OpenSGCache& OpenSGCache::the ()
{
  static OpenSGCache theInstance;
  return theInstance;
}

void       OpenSGCache::clear ()
{
   m_data.clear();
   m_data[NullFC] = CacheData(NullFC, Matrix::identity());
}
void       OpenSGCache::clear (const NodePtr& node)
{
   setCurrentNode(node);
   Entry::iterator it = m_data.find(node);
   if (it != m_data.end()) {
      CacheData& data = it->second;
      for (ChildContainer::iterator it2 = data.getChildren().begin();
	   it2 != data.getChildren().end();
	   ++it2) {
	 clear(*it2);
      }
      m_data.erase(it);
   }
   setCurrentNode(node);
}

void OpenSGCache::addNode    (const NodePtr& n)
{
   Entry::iterator it = m_data.find(n);
   if (it == m_data.end()) {
      m_data[n] = CacheData(n, n->getToWorld());
   }
}
void OpenSGCache::updateNode (const NodePtr& n)
{
   // CF: not complete yet
   addNode(n);
}

OpenSGCache::CacheData&       
OpenSGCache::getNode (const osg::NodePtr& n)
{
   Entry::iterator it = m_data.find(n);
   if (it != m_data.end()) {
      return it->second;
   } else {
      return m_data.find(NullFC)->second;
   }
}
const OpenSGCache::CacheData& 
OpenSGCache::getNode (const osg::NodePtr& n) const
{
   Entry::const_iterator it = m_data.find(n);
   if (it != m_data.end()) {
      return it->second;
   } else {
      return m_data.find(NullFC)->second;
   }
}

OpenSGCache::CacheData&       
OpenSGCache::getNode (const EntryValue& n)
{
   return getNode(n.first);
}
const OpenSGCache::CacheData& 
OpenSGCache::getNode (const EntryValue& n) const
{
   return getNode(n.first);
}

OpenSGCache::CacheData&       
OpenSGCache::operator[] (const osg::NodePtr& n)
{
   Entry::iterator it = m_data.find(n);
   if (it != m_data.end()) {
      return it->second;
   } else {
      return m_data.find(NullFC)->second;
   }
}
const OpenSGCache::CacheData& 
OpenSGCache::operator[] (const osg::NodePtr& n) const
{
   Entry::const_iterator it = m_data.find(n);
   if (it != m_data.end()) {
      return it->second;
   } else {
      return m_data.find(NullFC)->second;
   }
}

OpenSGCache::CacheData&       
OpenSGCache::getParent (const osg::NodePtr& n, int i)
{
   NodePtr p = n;
   while (i-- > 0) {
      CacheData& data = operator[](p);
      p = data.getParent();
   }
   return operator[](p);
}
const OpenSGCache::CacheData& 
OpenSGCache::getParent (const osg::NodePtr& n, int i) const
{
   NodePtr p = n;
   while (i-- > 0) {
      const CacheData& data = operator[](p);
      p = data.getParent();
   }
   return operator[](p);
}

std::ostream&    OpenSGCache::dump (std::ostream& os) const
{
#ifdef OSG_STL_HAS_HASH_MAP
   os << "OpenSGCache::dump(" 
      << m_data.size() << " entries, "
      << m_data.max_size() << " size, "
      << m_data.bucket_count() << " buckets, "
      << m_data.max_bucket_count() << " max buckets)"
      << std::flush;
#else
   os << "OpenSGCache::dump(" 
      << m_data.size() << " entries, "
      << m_data.max_size() << " size)"
      << std::flush;
#endif
#if 0
   unsigned empty, deleted, free;
   m_data.statusInformation(empty, deleted, free);
   os << "OpenSGCache::dump(" 
      << empty << " empty entries, "
      << deleted << " deleted entries, "
      << free << " free entries)" 
      << std::flush;
#endif
   return os;
}

void OpenSGCache::callEnterFunctor ( NodePtr node )
{
   assert(node != NullFC);
   if (node->getCore() == NullFC) {
      return;
   }
   UInt32 uiFunctorIndex = node->getCore()->getType().getId();
   CNodePtr cnode(node);
   
   if ( uiFunctorIndex < m_enterFunctors.size() ) {
      m_enterFunctors[uiFunctorIndex].call(cnode, getHierarchy());
   } else { // unknown field container
      defaultFunction(cnode, getHierarchy());
   }
}
void OpenSGCache::callLeaveFunctor ( NodePtr node )
{
   assert(node != NullFC);
   if (node->getCore() == NullFC) {
      return;
   }
   UInt32 uiFunctorIndex = node->getCore()->getType().getId();
   CNodePtr cnode(node);
   
   if ( uiFunctorIndex < m_leaveFunctors.size() ) {
      m_leaveFunctors[uiFunctorIndex].call(cnode, getHierarchy());
   } else { // unknown field container
      defaultFunction(cnode, getHierarchy());
   }
}

void OpenSGCache::apply (const NodePtr& node)
{
   getCurrentMatrix().setIdentity();
   OSG::TransformPtr trf = OSG::TransformPtr::dcast(node->getCore());
   if (trf == NullFC) {
      applyIntern(node);
   } else {
      setCurrentNode(node);
      Entry::iterator it = m_data.find(node);
      if (it != m_data.end()) {
	 CacheData& data = it->second;
	 for (ChildContainer::iterator it2 = data.getChildren().begin();
	      it2 != data.getChildren().end();
	      ++it2) {
	   applyIntern(*it2);
	 }
      }
   }
}

void OpenSGCache::applyIntern (const NodePtr& node)
{
   setCurrentNode(node);
   callEnterFunctor(node);
   Entry::iterator it = m_data.find(node);
   if (it != m_data.end()) {
      CacheData& data = it->second;
      for (ChildContainer::iterator it2 = data.getChildren().begin();
	   it2 != data.getChildren().end();
	   ++it2) {
	 applyIntern(*it2);
      }
   }
   setCurrentNode(node);
   callLeaveFunctor(node);
}

void OpenSGCache::defaultFunction(CNodePtr&, OSGStaticInput*)
{
}

void OpenSGCache::registerEnterFunctor (const FieldContainerType& type, 
					const Cache::Functor&    func)
{
   FieldContainerFactory::TypeMapIterator tit;
   for (tit = FieldContainerFactory::the()->beginTypes(); 
	tit != FieldContainerFactory::the()->endTypes();
	++tit) {
      FieldContainerType* derived = tit->second;
      if (derived != NULL && derived->isDerivedFrom(type)) {
	 while(tit->first >= m_enterFunctors.size()) {
	   m_enterFunctors.push_back( 
	   osgTypedFunctionVoidFunctor2CPtrRef<CNodePtr, OSGStaticInput*>
	   (&Cache::defaultFunction));
	 }
	 m_enterFunctors[tit->first] = func;
      }
   }
}

void OpenSGCache::registerLeaveFunctor (const FieldContainerType& type, 
					const Cache::Functor&    func)
{
   FieldContainerFactory::TypeMapIterator tit;
   for (tit = FieldContainerFactory::the()->beginTypes(); 
	tit != FieldContainerFactory::the()->endTypes();
	++tit) {
      FieldContainerType* derived = tit->second;
      if (derived != NULL && derived->isDerivedFrom(type)) {
	 while(tit->first >= m_leaveFunctors.size()) {
	   m_leaveFunctors.push_back( 
	   osgTypedFunctionVoidFunctor2CPtrRef<CNodePtr, OSGStaticInput*>
	   (&Cache::defaultFunction));
	 }
	 m_leaveFunctors[tit->first] = func;
      }
   }
}

#ifndef OSG_NOFUNCTORS
void OpenSGCache::functorEnterMaterialGroup (CNodePtr& cnode, 
							     OSGStaticInput* input)
{
   staticEnterMaterialGroup(cnode, getHierarchy()); 
}

void OpenSGCache::functorLeaveMaterialGroup (CNodePtr& cnode, 
							     OSGStaticInput* input)
{
   staticLeaveMaterialGroup(cnode, getHierarchy()); 
}
void OpenSGCache::functorEnterTransform (CNodePtr& cnode, 
							     OSGStaticInput* input)
{
   staticEnterTransform(cnode, getHierarchy()); 
}

void OpenSGCache::functorLeaveTransform (CNodePtr& cnode, 
							     OSGStaticInput* input)
{
   staticLeaveTransform(cnode, getHierarchy()); 
}
#endif

void    OpenSGCache::registerFunctors (void)
{
#ifndef OSG_NOFUNCTORS
   registerEnterFunctor(MaterialGroup::getClassType(), 
			osgTypedMethodVoidFunctor2ObjPtrCPtrRef<
			Cache, 
			CNodePtr, 
			OSGStaticInput*>
			(this,
			 &Cache::functorEnterMaterialGroup));
   registerLeaveFunctor(MaterialGroup::getClassType(), 
			osgTypedMethodVoidFunctor2ObjPtrCPtrRef<
			Cache, 
			CNodePtr, 
			OSGStaticInput*>
			(this,
			 &Cache::functorLeaveMaterialGroup));
   registerEnterFunctor(Transform::getClassType(), 
			osgTypedMethodVoidFunctor2ObjPtrCPtrRef<
			Cache, 
			CNodePtr, 
			OSGStaticInput*>
			(this,
			 &Cache::functorEnterTransform));
   registerLeaveFunctor(Transform::getClassType(), 
			osgTypedMethodVoidFunctor2ObjPtrCPtrRef<
			Cache, 
			CNodePtr, 
			OSGStaticInput*>
			(this,
			 &Cache::functorLeaveTransform));
#else
   registerEnterFunction(MaterialGroup::getClassType(),
   cacheFunctionFunctor2(Cache::staticEnterMaterialGroup));
   registerLeaveFunction(MaterialGroup::getClassType(),
   cacheFunctionFunctor2(Cache::staticLeaveMaterialGroup));
   registerEnterFunction(Transform::getClassType(),
   cacheFunctionFunctor2(Cache::staticEnterTransform));
   registerLeaveFunction(Transform::getClassType(),
   cacheFunctionFunctor2(Cache::staticLeaveTransform));
#endif
}

void OpenSGCache::staticEnterMaterialGroup (CNodePtr& cnode, 
					    OSGStaticInput* input)
{
   if (input == NULL) {
      return;
   }
   MaterialGroupPtr arg = MaterialGroupPtr::dcast(cnode);
   input->getCache().setCurrentMaterial(arg->getMaterial());
}

void OpenSGCache::staticLeaveMaterialGroup (CNodePtr& cnode, 
					    OSGStaticInput* input)
{
   if (input == NULL) {
      return;
   }
   MaterialGroupPtr arg = MaterialGroupPtr::dcast(cnode);
   input->getCache().setCurrentMaterial(NullFC);
}
void OpenSGCache::staticEnterTransform (CNodePtr& cnode, 
					OSGStaticInput* input)
{
   if (input == NULL) {
      return;
   }
   TransformPtr arg = TransformPtr::dcast(cnode);
   input->getCache().getCurrentMatrix().mult(arg->getMatrix());
}

void OpenSGCache::staticLeaveTransform (CNodePtr& cnode, 
					OSGStaticInput* input)
{
   if (input == NULL) {
      return;
   }
   TransformPtr arg = TransformPtr::dcast(cnode);
   Matrix inv; inv.invertFrom(arg->getMatrix());
   input->getCache().getCurrentMatrix().mult(inv);
}

void OpenSGCache::clearAdapter (unsigned id, 
				unsigned len)
{
   for (Entry::iterator it = m_data.begin();
	it != m_data.end();
	++it) {
      Cache::CacheData& data = it->second;
      data.clearAdapter(id, len);
   }
}
void OpenSGCache::destroyAdapter (unsigned id,
				  unsigned len)
{
   for (Entry::iterator it = m_data.begin();
	it != m_data.end();
	++it) {
      Cache::CacheData& data = it->second;
      data.destroyAdapter(id, len);
   }
}

void OpenSGCache::collectAdapter (unsigned       id,
				  AdapterVector& all)
{
   for (Entry::iterator it = m_data.begin();
	it != m_data.end();
	++it) {
      Cache::CacheData& data = it->second;
      all.insert(all.end(), 
		 data.getAdapter(id).begin(),
		 data.getAdapter(id).end());
   }
}
void OpenSGCache::collectAdapter (unsigned       id,
				  AdapterVector& all,
				  NodePtr        node)
{
   Entry::iterator it = m_data.find(node);
   if (it != m_data.end()) {
      CacheData& data = it->second;
      AdapterVector& array = data.getAdapter(id);
      all.insert(all.end(), array.begin(), array.end());
      if (&all != &array) {
	 array.erase(array.begin(), array.end());
      }
      for (ChildContainer::iterator it2 = data.getChildren().begin();
	   it2 != data.getChildren().end();
	   ++it2) {
	 collectAdapter(id, all, *it2);
      }
   }
}


#else


GenvisCachePtr OpenSGCache::s_dummy;

void OpenSGCache::setHierarchy (genvis::OSGStaticInput* hier)
{
   m_hierarchy = hier;
   m_hierarchy->registerFunctors();
}

OpenSGCache::OpenSGCache ()
  : m_hierarchy(NULL)
{
   registerFunctors();
   if (s_dummy == NullFC) {
      s_dummy = GenvisCache::create();
   } 
   addRefCP(s_dummy);
}
OpenSGCache::~OpenSGCache ()
{
   subRefCP(s_dummy);
}

OpenSGCache& OpenSGCache::the ()
{
  static OpenSGCache theInstance;
  return theInstance;
}


std::ostream& OpenSGCache::dump (std::ostream& os) const
{
   return os;
}

void OpenSGCache::callEnterFunctor (const NodePtr& node)
{
   assert(node != NullFC);
   if (node->getCore() == NullFC) {
      return;
   }
   UInt32 uiFunctorIndex = node->getCore()->getType().getId();
   CNodePtr cnode(node);
   
   if ( uiFunctorIndex < m_enterFunctors.size() ) {
      m_enterFunctors[uiFunctorIndex].call(cnode, getHierarchy());
   } else { // unknown field container
      defaultFunction(cnode, getHierarchy());
   }
}
void OpenSGCache::callLeaveFunctor (const NodePtr& node)
{
   assert(node != NullFC);
   if (node->getCore() == NullFC) {
      return;
   }
   UInt32 uiFunctorIndex = node->getCore()->getType().getId();
   CNodePtr cnode(node);
   
   if ( uiFunctorIndex < m_leaveFunctors.size() ) {
      m_leaveFunctors[uiFunctorIndex].call(cnode, getHierarchy());
   } else { // unknown field container
      defaultFunction(cnode, getHierarchy());
   }
}

void OpenSGCache::addNode    (const NodePtr& node)
{
   GenvisCachePtr nc = findNode(node);
   if (nc == s_dummy) {
      nc = GenvisCache::create();
      beginEditCP(node, Node::AttachmentsFieldMask);
      node->addAttachment(nc);
      endEditCP(node, Node::AttachmentsFieldMask);
   }
}
void OpenSGCache::updateNode (const NodePtr& node)
{
   // CF: not complete yet
   addNode(node);
}

void OpenSGCache::clear (const NodePtr& node)
{
   setCurrentNode(node);
   // remove node from m_all
   Entry::iterator nodeEntry = m_all.find(node);
   m_all.erase(nodeEntry);

   // clear current node
   GenvisCachePtr nc = findNode(node);
   beginEditCP(node, Node::AttachmentsFieldMask);
   node->subAttachment(nc);
   endEditCP(node, Node::AttachmentsFieldMask);

   // iterate over children
   for (ChildContainer::iterator it2 = nc->getChildren().begin();
	it2 != nc->getChildren().end();
	++it2) {
      clear(*it2);
   }
}

void OpenSGCache::apply (const NodePtr& node)
{
   m_root = node;
   getCurrentMatrix().setIdentity();
   OSG::TransformPtr trf = OSG::TransformPtr::dcast(node->getCore());
   if (trf == NullFC) {
      applyIntern(node);
   } else {
      m_all.insert(Entry::value_type(node,node));
      CacheData& data = getNode(node);
      for (ChildContainer::iterator it2 = data.getChildren().begin();
	   it2 != data.getChildren().end();
	   ++it2) {
	 applyIntern(*it2);
      }
   }
}

void OpenSGCache::applyIntern (const NodePtr& node)
{
   m_all.insert(Entry::value_type(node,node));
   //m_all.push_back(node);

   setCurrentNode(node);
   callEnterFunctor(node);

   CacheData& data = getNode(node);
   for (ChildContainer::iterator it2 = data.getChildren().begin();
	it2 != data.getChildren().end();
	++it2) {
      applyIntern(*it2);
   }

   setCurrentNode(node);
   callLeaveFunctor(node);
}

void OpenSGCache::defaultFunction(CNodePtr&, OSGStaticInput*)
{
}

void OpenSGCache::registerEnterFunctor (const FieldContainerType& type, 
					const Cache::Functor&    func)
{
   FieldContainerFactory::TypeMapIterator tit;
   for (tit = FieldContainerFactory::the()->beginTypes(); 
	tit != FieldContainerFactory::the()->endTypes();
	++tit) {
      FieldContainerType* derived = tit->second;
      if (derived != NULL && derived->isDerivedFrom(type)) {
	 while(tit->first >= m_enterFunctors.size()) {
	   m_enterFunctors.push_back( 
	   osgTypedFunctionVoidFunctor2CPtrRef<CNodePtr, OSGStaticInput*>
	   (&Cache::defaultFunction));
	 }
	 m_enterFunctors[tit->first] = func;
      }
   }
}

void OpenSGCache::registerLeaveFunctor (const FieldContainerType& type, 
					const Cache::Functor&    func)
{
   FieldContainerFactory::TypeMapIterator tit;
   for (tit = FieldContainerFactory::the()->beginTypes(); 
	tit != FieldContainerFactory::the()->endTypes();
	++tit) {
      FieldContainerType* derived = tit->second;
      if (derived != NULL && derived->isDerivedFrom(type)) {
	 while(tit->first >= m_leaveFunctors.size()) {
	   m_leaveFunctors.push_back( 
	   osgTypedFunctionVoidFunctor2CPtrRef<CNodePtr, OSGStaticInput*>
	   (&Cache::defaultFunction));
	 }
	 m_leaveFunctors[tit->first] = func;
      }
   }
}

#ifndef OSG_NOFUNCTORS
void OpenSGCache::functorEnterMaterialGroup (CNodePtr& cnode, 
					      OSGStaticInput* input)
{
   staticEnterMaterialGroup(cnode, getHierarchy()); 
}

void OpenSGCache::functorLeaveMaterialGroup (CNodePtr& cnode, 
					     OSGStaticInput* input)
{
   staticLeaveMaterialGroup(cnode, getHierarchy()); 
}
void OpenSGCache::functorEnterTransform (CNodePtr& cnode, 
					 OSGStaticInput* input)
{
   staticEnterTransform(cnode, getHierarchy()); 
}

void OpenSGCache::functorLeaveTransform (CNodePtr& cnode, 
					 OSGStaticInput* input)
{
   staticLeaveTransform(cnode, getHierarchy()); 
}
#endif

void    OpenSGCache::registerFunctors (void)
{
#ifndef OSG_NOFUNCTORS
   registerEnterFunctor(MaterialGroup::getClassType(), 
			osgTypedMethodVoidFunctor2ObjPtrCPtrRef<
			Cache, 
			CNodePtr, 
			OSGStaticInput*>
			(this,
			 &Cache::functorEnterMaterialGroup));
   registerLeaveFunctor(MaterialGroup::getClassType(), 
			osgTypedMethodVoidFunctor2ObjPtrCPtrRef<
			Cache, 
			CNodePtr, 
			OSGStaticInput*>
			(this,
			 &Cache::functorLeaveMaterialGroup));
   registerEnterFunctor(Transform::getClassType(), 
			osgTypedMethodVoidFunctor2ObjPtrCPtrRef<
			Cache, 
			CNodePtr, 
			OSGStaticInput*>
			(this,
			 &Cache::functorEnterTransform));
   registerLeaveFunctor(Transform::getClassType(), 
			osgTypedMethodVoidFunctor2ObjPtrCPtrRef<
			Cache, 
			CNodePtr, 
			OSGStaticInput*>
			(this,
			 &Cache::functorLeaveTransform));
#else
   registerEnterFunction(MaterialGroup::getClassType(),
   cacheFunctionFunctor2(Cache::staticEnterMaterialGroup));
   registerLeaveFunction(MaterialGroup::getClassType(),
   cacheFunctionFunctor2(Cache::staticLeaveMaterialGroup));
   registerEnterFunction(Transform::getClassType(),
   cacheFunctionFunctor2(Cache::staticEnterTransform));
   registerLeaveFunction(Transform::getClassType(),
   cacheFunctionFunctor2(Cache::staticLeaveTransform));
#endif
}

void OpenSGCache::staticEnterMaterialGroup (CNodePtr& cnode, 
					    OSGStaticInput* input)
{
   if (input == NULL) {
      return;
   }
   MaterialGroupPtr arg = MaterialGroupPtr::dcast(cnode);
   input->getCache().setCurrentMaterial(arg->getMaterial());
}

void OpenSGCache::staticLeaveMaterialGroup (CNodePtr& cnode, 
					    OSGStaticInput* input)
{
   if (input == NULL) {
      return;
   }
   MaterialGroupPtr arg = MaterialGroupPtr::dcast(cnode);
   input->getCache().setCurrentMaterial(NullFC);
}
void OpenSGCache::staticEnterTransform (CNodePtr& cnode, 
					OSGStaticInput* input)
{
   if (input == NULL) {
      return;
   }
   TransformPtr arg = TransformPtr::dcast(cnode);
   input->getCache().getCurrentMatrix().mult(arg->getMatrix());
}

void OpenSGCache::staticLeaveTransform (CNodePtr& cnode, 
					OSGStaticInput* input)
{
   if (input == NULL) {
      return;
   }
   TransformPtr arg = TransformPtr::dcast(cnode);
   Matrix inv; inv.invertFrom(arg->getMatrix());
   input->getCache().getCurrentMatrix().mult(inv);
}

void OpenSGCache::clearAdapter (unsigned id, unsigned len)
{
   for (EntryIterator it = begin(); it != end(); ++it) {
      Cache::CacheData& data = getNode(*it);
      data.clearAdapter(id, len);
   }
}
void OpenSGCache::destroyAdapter (unsigned id, unsigned len)
{
   for (EntryIterator it = begin(); it != end(); ++it) {
      Cache::CacheData& data = getNode(*it);
      data.destroyAdapter(id, len);
   }
}

void OpenSGCache::collectAdapter (unsigned id, AdapterVector& all)
{
   for (EntryIterator it = begin(); it != end(); ++it) {
      Cache::CacheData& data = getNode(*it);
      all.insert(all.end(), data.getAdapter(id).begin(), data.getAdapter(id).end());
   }
}
void OpenSGCache::collectAdapter (unsigned id, AdapterVector& all, NodePtr node)
{
   Cache::CacheData& data = getNode(node);
   all.insert(all.end(), data.getAdapter(id).begin(), data.getAdapter(id).end());
   for (ChildContainer::iterator it = data.getChildren().begin();
	it != data.getChildren().end();
	++it) {
      collectAdapter(id, all, *it);
   }
}

#endif

