//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.3 $
//   $Date: 2004/12/20 15:56:35 $
//                                                                            
//=============================================================================

#include "OSGGVCache.h"
#include "OSGGVStaticInput.h"

#include "OSGMaterialGroup.h"
#include "OSGComponentTransform.h"
#include "OSGTransform.h"
#include "OSGSwitch.h"

OSG_USING_NAMESPACE
USING_GENVIS_NAMESPACE


#ifdef GV_CACHE_HASHMAP

OpenSGCache::CacheData&
OpenSGCache::getShare (const NodePtr& node)
{
   SharesContainer::iterator it = m_shares.find(node->getCore());
   if (it == m_shares.end()) {
      CacheData& data = getNode(node);
      m_shares.insert(std::make_pair(node->getCore(), &data));
   }
   return *(m_shares[node->getCore()]);
}
OpenSGCache::CacheData*
OpenSGCache::findShare (const NodePtr& node)
{
   SharesContainer::iterator it = m_shares.find(node->getCore());
   if (it == m_shares.end()) {
      return NULL;
   }
   return it->second;
}

genvis::OSGStaticInput* OpenSGCache::getHierarchy () const
{
   return m_hierarchy;
}

OSG::NodePtr OpenSGCache::getCurrentNode () const
{
   return m_currentNode;
}
void         OpenSGCache::setCurrentNode (const OSG::NodePtr& node)
{
   m_currentNode = node;
}
OSG::MaterialPtr OpenSGCache::getCurrentMaterial () const
{
   if (m_materials.empty()) {
      return NullFC;
   }
   return m_materials.top();
}
void OpenSGCache::pushMaterial (const OSG::MaterialPtr& mat)
{
   m_materials.push(mat);
}
void OpenSGCache::popMaterial ()
{
   m_materials.pop();
}
Matrix& OpenSGCache::getCurrentMatrix ()
{
   return m_currentMatrix;
}
const Matrix& OpenSGCache::getCurrentMatrix () const
{
   return m_currentMatrix;
}

OpenSGCache::EntryIterator OpenSGCache::begin ()
{
   return m_data.begin();
}
OpenSGCache::EntryIterator OpenSGCache::end   ()
{
   return m_data.end();
}
OpenSGCache::Entry& OpenSGCache::getHashTable ()
{
   return m_data;
}
const OpenSGCache::Entry& OpenSGCache::getHashTable () const
{
   return m_data;
}

OpenSGCache::CacheData& OpenSGCache::operator[] (EntryValue& n)
{
   return n.second;
}
const OpenSGCache::CacheData& OpenSGCache::operator[] (const EntryValue& n) const
{
   return n.second;
}


void OpenSGCache::setHierarchy (genvis::OSGStaticInput* hier)
{
   m_hierarchy = hier;
   if (m_hierarchy != NULL) {
      m_hierarchy->registerFunctors();
   } else {
      registerFunctors();
   }
}

OpenSGCache::OpenSGCache ()
  : m_data(),
    m_hierarchy(NULL)
{
   SLOG << "OpenSGCache (version with hash_map) created!" << std::endl;
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
OpenSGCache::getParent (const osg::NodePtr& n, i32 i)
{
   NodePtr p = n;
   while (i-- > 0) {
      CacheData& data = operator[](p);
      p = data.getParent();
   }
   return operator[](p);
}
const OpenSGCache::CacheData& 
OpenSGCache::getParent (const osg::NodePtr& n, i32 i) const
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
   return os;
}

bool OpenSGCache::callEnterFunctor (const NodePtr& node )
{
   assert(node != NullFC);
   if (node->getCore() == NullFC) {
      return false;
   }
   u32 uiFunctorIndex = node->getCore()->getType().getId();
   CNodePtr cnode(node);

   if (uiFunctorIndex < m_enterFunctors.size()) {
      return m_enterFunctors[uiFunctorIndex].call(cnode, getHierarchy());
   }
   // unknown field container
   return defaultFunction(cnode, getHierarchy());
}
bool OpenSGCache::callLeaveFunctor (const NodePtr& node )
{
   assert(node != NullFC);
   if (node->getCore() == NullFC) {
      return false;
   }
   u32 uiFunctorIndex = node->getCore()->getType().getId();
   CNodePtr cnode(node);
   
   if ( uiFunctorIndex < m_leaveFunctors.size() ) {
      return m_leaveFunctors[uiFunctorIndex].call(cnode, getHierarchy());
   } 
   // unknown field container
   return defaultFunction(cnode, getHierarchy());
}

void OpenSGCache::apply (const NodePtr& node)
{
  //setStartNode(node);
   getCurrentMatrix().setIdentity();
   addNode(node);

   TransformPtr trf = TransformPtr::dcast(node->getCore());
   if (trf == NullFC) {
      setCurrentNode(node);
      if (callEnterFunctor(node)) {
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

   if (getHierarchy() != NULL) {
      getHierarchy()->process(node);
   }
}

void OpenSGCache::applyIntern (const NodePtr& node)
{
   addNode(node);

   setCurrentNode(node);
   if (callEnterFunctor(node)) {
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
}

bool OpenSGCache::defaultFunction(CNodePtr&, OSGStaticInput*)
{
   return true;
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
	   osgTypedFunctionFunctor2CPtrRef<bool, CNodePtr, OSGStaticInput*>
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
	   osgTypedFunctionFunctor2CPtrRef<bool, CNodePtr, OSGStaticInput*>
	   (&Cache::defaultFunction));
	 }
	 m_leaveFunctors[tit->first] = func;
      }
   }
}

#ifndef OSG_NOFUNCTORS
bool OpenSGCache::functorEnterMaterialGroup (CNodePtr& cnode, OSGStaticInput* input)
{
   return staticEnterMaterialGroup(cnode, input); 
}

bool OpenSGCache::functorLeaveMaterialGroup (CNodePtr& cnode, OSGStaticInput* input)
{
   return staticLeaveMaterialGroup(cnode, input); 
}
bool OpenSGCache::functorEnterTransform (CNodePtr& cnode, OSGStaticInput* input)
{
   return staticEnterTransform(cnode, input); 
}

bool OpenSGCache::functorLeaveTransform (CNodePtr& cnode, OSGStaticInput* input)
{
   return staticLeaveTransform(cnode, input); 
}

bool OpenSGCache::functorEnterGeometry (CNodePtr& cnode, OSGStaticInput* input)
{
   return staticEnterGeometry (cnode, input); 
}

bool OpenSGCache::functorEnterSwitch (CNodePtr& cnode, OSGStaticInput* input)
{
   return staticEnterSwitch (cnode, input); 
}
#endif

void    OpenSGCache::registerFunctors (void)
{
#ifndef OSG_NOFUNCTORS
   registerEnterFunctor(MaterialGroup::getClassType(), 
			osgTypedMethodFunctor2ObjPtrCPtrRef<bool,
			Cache, 
			CNodePtr, 
			OSGStaticInput*>
			(this,
			 &Cache::functorEnterMaterialGroup));
   registerLeaveFunctor(MaterialGroup::getClassType(), 
			osgTypedMethodFunctor2ObjPtrCPtrRef<bool,
			Cache, 
			CNodePtr, 
			OSGStaticInput*>
			(this,
			 &Cache::functorLeaveMaterialGroup));
   registerEnterFunctor(Transform::getClassType(), 
			osgTypedMethodFunctor2ObjPtrCPtrRef<bool,
			Cache, 
			CNodePtr, 
			OSGStaticInput*>
			(this,
			 &Cache::functorEnterTransform));
   registerLeaveFunctor(Transform::getClassType(), 
			osgTypedMethodFunctor2ObjPtrCPtrRef<bool,
			Cache, 
			CNodePtr, 
			OSGStaticInput*>
			(this,
			 &Cache::functorLeaveTransform));
   registerEnterFunctor(ComponentTransform::getClassType(), 
			osgTypedMethodFunctor2ObjPtrCPtrRef<bool,
			Cache, 
			CNodePtr, 
			OSGStaticInput*>
			(this,
			 &Cache::functorEnterTransform));
   registerLeaveFunctor(ComponentTransform::getClassType(), 
			osgTypedMethodFunctor2ObjPtrCPtrRef<bool,
			Cache, 
			CNodePtr, 
			OSGStaticInput*>
			(this,
			 &Cache::functorLeaveTransform));
   registerEnterFunctor(Geometry::getClassType(), 
			osgTypedMethodFunctor2ObjPtrCPtrRef<bool,
			Cache, 
			CNodePtr, 
			OSGStaticInput*>
			(this,
			 &Cache::functorEnterGeometry));
   registerEnterFunctor(Switch::getClassType(), 
			osgTypedMethodFunctor2ObjPtrCPtrRef<bool,
			Cache, 
			CNodePtr, 
			OSGStaticInput*>
			(this,
			 &Cache::functorEnterSwitch));
#else
   registerEnterFunction(MaterialGroup::getClassType(),
   cacheFunctionFunctor2(Cache::staticEnterMaterialGroup));
   registerLeaveFunction(MaterialGroup::getClassType(),
   cacheFunctionFunctor2(Cache::staticLeaveMaterialGroup));
   registerEnterFunction(Transform::getClassType(),
   cacheFunctionFunctor2(Cache::staticEnterTransform));
   registerLeaveFunction(Transform::getClassType(),
   cacheFunctionFunctor2(Cache::staticLeaveTransform));
   registerEnterFunction(ComponentTransform::getClassType(),
   cacheFunctionFunctor2(Cache::staticEnterTransform));
   registerLeaveFunction(ComponentTransform::getClassType(),
   cacheFunctionFunctor2(Cache::staticLeaveTransform));
   registerEnterFunction(Geometry::getClassType(),
   cacheFunctionFunctor2(Cache::staticEnterGeometry));
   registerEnterFunction(Switch::getClassType(),
   cacheFunctionFunctor2(Cache::staticEnterSwitch));
#endif
}

bool OpenSGCache::staticEnterGeometry (CNodePtr& cnode, OSGStaticInput*)
{
   if (the().getCurrentMaterial() != NullFC) {
      GeometryPtr arg = GeometryPtr::dcast(cnode);
      arg->setMaterial(the().getCurrentMaterial());
   }
   return true;
}

bool OpenSGCache::staticEnterSwitch (CNodePtr&, OSGStaticInput*)
{
   return false;
}

bool OpenSGCache::staticEnterMaterialGroup (CNodePtr& cnode, OSGStaticInput*)
{
   MaterialGroupPtr arg = MaterialGroupPtr::dcast(cnode);
   the().pushMaterial(arg->getMaterial());
   return true;
}

bool OpenSGCache::staticLeaveMaterialGroup (CNodePtr& cnode, OSGStaticInput*)
{
   the().popMaterial();
   return true;
}
bool OpenSGCache::staticEnterTransform (CNodePtr& cnode, OSGStaticInput*)
{
   TransformPtr arg = TransformPtr::dcast(cnode);
   the().getCurrentMatrix().mult(arg->getMatrix());
   return true;
}

bool OpenSGCache::staticLeaveTransform (CNodePtr& cnode, OSGStaticInput*)
{
   TransformPtr arg = TransformPtr::dcast(cnode);
   Matrix inv; inv.invertFrom(arg->getMatrix());
   the().getCurrentMatrix().mult(inv);
   return true;
}

void OpenSGCache::clearColCache ()
{
   for (Entry::iterator it = m_data.begin();
	it != m_data.end();
	++it) {
      CacheData& data = it->second;
      data.clearColCache();
   }
}
void OpenSGCache::clearAdapter (u32 id, u32 len)
{
   for (Entry::iterator it = m_data.begin();
	it != m_data.end();
	++it) {
      Cache::CacheData& data = it->second;
      data.clearAdapter(id, len);
   }
}

void OpenSGCache::collectLeaves (std::vector<CacheData*>& all, const NodePtr& node)
{
   Entry::iterator it = m_data.find(node);
   if (it != m_data.end()) {
      CacheData& data = it->second;
      if (data.getNumChildren() > 0) {
	 for (ChildContainer::iterator it2 = data.getChildren().begin();
	      it2 != data.getChildren().end();
	      ++it2) {
	    collectLeaves(all, *it2);
	 }
      } else {
	 all.push_back(&data);
      }
   }
}

void OpenSGCache::collectAdapter (u32       id,
				  AdapterVector& all,
				  bool           del)
{
   for (Entry::iterator it = m_data.begin();
	it != m_data.end();
	++it) {
      Cache::CacheData& data = it->second;
      all.insert(all.end(), 
		 data.getAdapter(id).begin(), data.getAdapter(id).end());
      if (del) {
	 data.getAdapter(id).clear();
      }
   }
}
void OpenSGCache::collectAdapter (u32       id,
				  AdapterVector& all,
				  const NodePtr& node,
				  bool           del)
{
   Entry::iterator it = m_data.find(node);
   if (it != m_data.end()) {
      CacheData& data = it->second;
      all.insert(all.end(), 
		 data.getAdapter(id).begin(), data.getAdapter(id).end());
      if (del) {
	 data.getAdapter(id).clear();
      }
      for (ChildContainer::iterator it2 = data.getChildren().begin();
	   it2 != data.getChildren().end();
	   ++it2) {
	 collectAdapter(id, all, *it2, del);
      }
   }
}


#else

GenvisCachePtr OpenSGCache::s_dummy;

OpenSGCache::CacheData&
OpenSGCache::getShare (const NodePtr& node)
{
   SharesContainer::iterator it = m_shares.find(node->getCore());
   if (it == m_shares.end()) {
      CacheData& data = getNode(node);
      m_shares.insert(std::make_pair(node->getCore(), &data));
   }
   return *(m_shares[node->getCore()]);
}
OpenSGCache::CacheData*
OpenSGCache::findShare (const NodePtr& node)
{
   SharesContainer::iterator it = m_shares.find(node->getCore());
   if (it == m_shares.end()) {
      return NULL;
   }
   return it->second;
}

OSGStaticInput* OpenSGCache::getHierarchy () const
{
   return m_hierarchy;
}

OSG::NodePtr OpenSGCache::getCurrentNode () const
{
   return m_currentNode;
}
void OpenSGCache::setCurrentNode (const OSG::NodePtr& node)
{
   m_currentNode = node;
}

OSG::MaterialPtr OpenSGCache::getCurrentMaterial () const
{
   if (m_materials.empty()) {
      return NullFC;
   }
   return m_materials.top();
}
void OpenSGCache::pushMaterial (const OSG::MaterialPtr& mat)
{
   m_materials.push(mat);
}
void OpenSGCache::popMaterial ()
{
   m_materials.pop();
}
Matrix& OpenSGCache::getCurrentMatrix ()
{
   return m_currentMatrix;
}
const Matrix& OpenSGCache::getCurrentMatrix () const
{
   return m_currentMatrix;
}

OpenSGCache::EntryIterator OpenSGCache::begin ()
{
   return m_all.begin();
}
OpenSGCache::EntryIterator OpenSGCache::end   ()
{
   return m_all.end();
}


void OpenSGCache::clear ()
{
   for (Entry::iterator it=m_all.begin(); 
	it != m_all.end(); 
	++it) {
      GenvisCachePtr nc =
	GenvisCachePtr::dcast(it->first->findAttachment(GenvisCache::getClassType(), 0));
      // clear current node
      beginEditCP(it->first, Node::AttachmentsFieldMask);
      it->first->subAttachment(nc);
      endEditCP(it->first, Node::AttachmentsFieldMask);
      subRefCP(nc);
   }
}

GenvisCachePtr OpenSGCache::findNode (const NodePtr& node) const
{
   assert(node != NullFC);
   GenvisCachePtr nc =
     GenvisCachePtr::dcast(node->findAttachment(GenvisCache::getClassType(), 0));
   if (nc == NullFC) {
      return s_dummy;
   }
   return nc;
}
OpenSGCache::CacheData& OpenSGCache::getNode    (const NodePtr& n) const
{
   GenvisCachePtr nc = findNode(n);
   return *nc;
}
OpenSGCache::CacheData& OpenSGCache::getNode    (const Entry::value_type& n) const
{
   return getNode(n.first);
}
OpenSGCache::CacheData& OpenSGCache::operator[] (const osg::NodePtr& n) const
{
   return getNode(n);
}
OpenSGCache::CacheData& OpenSGCache::operator[] (const Entry::value_type& n) const
{
   return getNode(n.first);
}
OpenSGCache::CacheData& OpenSGCache::getParent (const osg::NodePtr& node, i32 i) const
{
   NodePtr p = node;
   while (i > 0) {
      p = node->getParent();
      --i;
   }
   return getNode(p);
}



void OpenSGCache::setHierarchy (genvis::OSGStaticInput* hier)
{
   m_hierarchy = hier;
   if (m_hierarchy != NULL) {
      m_hierarchy->registerFunctors();
   } else {
      registerFunctors();
   }
}

OpenSGCache::OpenSGCache ()
  : m_hierarchy(NULL)
{
   SLOG << "OpenSGCache (version with Attachments) created!" << std::endl;
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

bool OpenSGCache::callEnterFunctor (const NodePtr& node)
{
   assert(node != NullFC);
   if (node->getCore() == NullFC) {
      return false;
   }
   u32 uiFunctorIndex = node->getCore()->getType().getId();
   CNodePtr cnode(node);
   
   if ( uiFunctorIndex < m_enterFunctors.size() ) {
      return m_enterFunctors[uiFunctorIndex].call(cnode, getHierarchy());
   } 
   // unknown field container
   return defaultFunction(cnode, getHierarchy());
}
bool OpenSGCache::callLeaveFunctor (const NodePtr& node)
{
   assert(node != NullFC);
   if (node->getCore() == NullFC) {
      return false;
   }
   u32 uiFunctorIndex = node->getCore()->getType().getId();
   CNodePtr cnode(node);
   
   if ( uiFunctorIndex < m_leaveFunctors.size() ) {
      return m_leaveFunctors[uiFunctorIndex].call(cnode, getHierarchy());
   }
   // unknown field container
   return defaultFunction(cnode, getHierarchy());
}

void OpenSGCache::addNode    (const NodePtr& node)
{
   GenvisCachePtr nc = findNode(node);
   if (nc == s_dummy) {
      nc = GenvisCache::create();
      addRefCP(nc);
      beginEditCP(node, Node::AttachmentsFieldMask);
      node->addAttachment(nc);
      endEditCP(node, Node::AttachmentsFieldMask);
   }
}

void OpenSGCache::clear (const NodePtr& node)
{
   setCurrentNode(node);
   // remove node from m_all
   Entry::iterator nodeEntry = m_all.find(node);
   m_all.erase(nodeEntry);

   GenvisCachePtr nc = findNode(node);
   // clear current node
   beginEditCP(node, Node::AttachmentsFieldMask);
   node->subAttachment(nc);
   endEditCP(node, Node::AttachmentsFieldMask);

   // iterate over children
   for (ChildContainer::iterator it2 = nc->getChildren().begin();
	it2 != nc->getChildren().end();
	++it2) {
      clear(*it2);
   }
   // nc not used anymore
   subRefCP(nc);
}

void OpenSGCache::apply (const NodePtr& node)
{
  //setStartNode(node);
   getCurrentMatrix().setIdentity();
   addNode(node);
   m_all.insert(Entry::value_type(node,node));

   OSG::TransformPtr trf = OSG::TransformPtr::dcast(node->getCore());
   if (trf == NullFC) {
      setCurrentNode(node);
      if (callEnterFunctor(node)) {
	 CacheData& data = getNode(node);
	 for (ChildContainer::iterator it2 = data.getChildren().begin();
	      it2 != data.getChildren().end();
	      ++it2) {
	    applyIntern(*it2);
	 }
	 setCurrentNode(node);
	 callLeaveFunctor(node);
      }
   } else {
      CacheData& data = getNode(node);
      for (ChildContainer::iterator it2 = data.getChildren().begin();
	   it2 != data.getChildren().end();
	   ++it2) {
	  applyIntern(*it2);
      }
   }

   if (getHierarchy() != NULL) {
      getHierarchy()->process(node);
   }
}

void OpenSGCache::applyIntern (const NodePtr& node)
{
   addNode(node);
   m_all.insert(Entry::value_type(node,node));

   setCurrentNode(node);
   if (callEnterFunctor(node)) {
      CacheData& data = getNode(node);
      for (ChildContainer::iterator it2 = data.getChildren().begin();
	   it2 != data.getChildren().end();
	   ++it2) {
	applyIntern(*it2);
      }
      setCurrentNode(node);
      callLeaveFunctor(node);
   }
}

bool OpenSGCache::defaultFunction(CNodePtr&, OSGStaticInput*)
{
   return true;
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
	   osgTypedFunctionFunctor2CPtrRef<bool, CNodePtr, OSGStaticInput*>
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
	   osgTypedFunctionFunctor2CPtrRef<bool, CNodePtr, OSGStaticInput*>
	   (&Cache::defaultFunction));
	 }
	 m_leaveFunctors[tit->first] = func;
      }
   }
}

#ifndef OSG_NOFUNCTORS
bool OpenSGCache::functorEnterMaterialGroup (CNodePtr& cnode, OSGStaticInput* input)
{
   return staticEnterMaterialGroup(cnode, input); 
}

bool OpenSGCache::functorLeaveMaterialGroup (CNodePtr& cnode, OSGStaticInput* input)
{
   return staticLeaveMaterialGroup(cnode, input); 
}
bool OpenSGCache::functorEnterTransform (CNodePtr& cnode, OSGStaticInput* input)
{
   return staticEnterTransform(cnode, input); 
}

bool OpenSGCache::functorLeaveTransform (CNodePtr& cnode, OSGStaticInput* input)
{
   return staticLeaveTransform(cnode, input); 
}

bool OpenSGCache::functorEnterGeometry (CNodePtr& cnode, OSGStaticInput* input)
{
   return staticEnterGeometry(cnode, input); 
}

bool OpenSGCache::functorEnterSwitch (CNodePtr& cnode, OSGStaticInput* input)
{
   return staticEnterSwitch(cnode, input); 
}
#endif

void    OpenSGCache::registerFunctors (void)
{
#ifndef OSG_NOFUNCTORS
   registerEnterFunctor(MaterialGroup::getClassType(), 
			osgTypedMethodFunctor2ObjPtrCPtrRef<bool,
			Cache, 
			CNodePtr, 
			OSGStaticInput*>
			(this,
			 &Cache::functorEnterMaterialGroup));
   registerLeaveFunctor(MaterialGroup::getClassType(), 
			osgTypedMethodFunctor2ObjPtrCPtrRef<bool,
			Cache, 
			CNodePtr, 
			OSGStaticInput*>
			(this,
			 &Cache::functorLeaveMaterialGroup));
   registerEnterFunctor(Transform::getClassType(), 
			osgTypedMethodFunctor2ObjPtrCPtrRef<bool,
			Cache, 
			CNodePtr, 
			OSGStaticInput*>
			(this,
			 &Cache::functorEnterTransform));
   registerLeaveFunctor(Transform::getClassType(), 
			osgTypedMethodFunctor2ObjPtrCPtrRef<bool,
			Cache, 
			CNodePtr, 
			OSGStaticInput*>
			(this,
			 &Cache::functorLeaveTransform));
   registerEnterFunctor(ComponentTransform::getClassType(), 
			osgTypedMethodFunctor2ObjPtrCPtrRef<bool,
			Cache, 
			CNodePtr, 
			OSGStaticInput*>
			(this,
			 &Cache::functorEnterTransform));
   registerLeaveFunctor(ComponentTransform::getClassType(), 
			osgTypedMethodFunctor2ObjPtrCPtrRef<bool,
			Cache, 
			CNodePtr, 
			OSGStaticInput*>
			(this,
			 &Cache::functorLeaveTransform));
   registerEnterFunctor(Geometry::getClassType(), 
			osgTypedMethodFunctor2ObjPtrCPtrRef<bool,
			Cache, 
			CNodePtr, 
			OSGStaticInput*>
			(this,
			 &Cache::functorEnterGeometry));
   registerEnterFunctor(Switch::getClassType(), 
			osgTypedMethodFunctor2ObjPtrCPtrRef<bool,
			Cache, 
			CNodePtr, 
			OSGStaticInput*>
			(this,
			 &Cache::functorEnterSwitch));
#else
   registerEnterFunction(MaterialGroup::getClassType(),
   cacheFunctionFunctor2(Cache::staticEnterMaterialGroup));
   registerLeaveFunction(MaterialGroup::getClassType(),
   cacheFunctionFunctor2(Cache::staticLeaveMaterialGroup));
   registerEnterFunction(Transform::getClassType(),
   cacheFunctionFunctor2(Cache::staticEnterTransform));
   registerLeaveFunction(Transform::getClassType(),
   cacheFunctionFunctor2(Cache::staticLeaveTransform));
   registerEnterFunction(ComponentTransform::getClassType(),
   cacheFunctionFunctor2(Cache::staticEnterTransform));
   registerLeaveFunction(ComponentTransform::getClassType(),
   cacheFunctionFunctor2(Cache::staticLeaveTransform));
   registerEnterFunction(Geometry::getClassType(),
   cacheFunctionFunctor2(Cache::staticEnterGeometry));
   registerEnterFunction(Switch::getClassType(),
   cacheFunctionFunctor2(Cache::staticEnterSwitch));
#endif
}

bool OpenSGCache::staticEnterGeometry (CNodePtr& cnode, OSGStaticInput*)
{
   if (the().getCurrentMaterial() != NullFC) {
      GeometryPtr arg = GeometryPtr::dcast(cnode);
      arg->setMaterial(the().getCurrentMaterial());
   }
   return true;
}

bool OpenSGCache::staticEnterSwitch (CNodePtr&, OSGStaticInput*)
{
   return false;
}

bool OpenSGCache::staticEnterMaterialGroup (CNodePtr& cnode, OSGStaticInput*)
{
   MaterialGroupPtr arg = MaterialGroupPtr::dcast(cnode);
   the().pushMaterial(arg->getMaterial());
   return true;
}

bool OpenSGCache::staticLeaveMaterialGroup (CNodePtr& cnode, OSGStaticInput*)
{
   the().popMaterial();
   return true;
}
bool OpenSGCache::staticEnterTransform (CNodePtr& cnode, OSGStaticInput*)
{
   TransformPtr arg = TransformPtr::dcast(cnode);
   the().getCurrentMatrix().mult(arg->getMatrix());
   return true;
}

bool OpenSGCache::staticLeaveTransform (CNodePtr& cnode, OSGStaticInput*)
{
   TransformPtr arg = TransformPtr::dcast(cnode);
   Matrix inv; inv.invertFrom(arg->getMatrix());
   the().getCurrentMatrix().mult(inv);
   return true;
}

void OpenSGCache::clearColCache ()
{
   for (Entry::iterator it = begin(); it != end(); ++it) {
      CacheData& data = getNode(*it);
      data.clearColCache();
   }
}
void OpenSGCache::clearAdapter (u32 id, u32 len)
{
   for (Entry::iterator it = begin(); it != end(); ++it) {
      CacheData& data = getNode(*it);
      data.clearAdapter(id, len);
   }
}

void OpenSGCache::collectLeaves (std::vector<CacheData*>& all, const NodePtr& node)
{
   CacheData& data = getNode(node);
   if (data.getNumChildren() > 0) {
      for (ChildContainer::iterator it = data.getChildren().begin();
	   it != data.getChildren().end();
	   ++it) {
	collectLeaves(all, *it);
      }
   } else {
      all.push_back(&data);
   }
}

void OpenSGCache::collectAdapter (u32 id, 
				  AdapterVector& all,
				  bool           del)
{
   for (Entry::iterator it = begin(); it != end(); ++it) {
      CacheData& data = getNode(*it);
      all.insert(all.end(), 
		 data.getAdapter(id).begin(), data.getAdapter(id).end());
      if (del) {
	 data.getAdapter(id).clear();
      }
   }
}
void OpenSGCache::collectAdapter (u32 id, 
				  AdapterVector& all, 
				  const NodePtr& node,
				  bool           del)
{
   CacheData& data = getNode(node);
   all.insert(all.end(), 
	      data.getAdapter(id).begin(), data.getAdapter(id).end());
   if (del) {
      data.getAdapter(id).clear();
   }
   for (ChildContainer::iterator it = data.getChildren().begin();
	it != data.getChildren().end();
	++it) {
      collectAdapter(id, all, *it);
   }
}

#endif

