//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.3 $
//   $Date: 2004/12/20 15:59:52 $
//                                                                            
//=============================================================================

#include "OSGGVSpatialize.h"
OSG_USING_NAMESPACE
USING_GENVIS_NAMESPACE

// explicit template instantiations
template class OSG_GENVISLIB_DLLMAPPING Spatialize<OpenSGTraits>;
template class OSG_GENVISLIB_DLLMAPPING SpatializeTraits<OpenSGTraits>;


template <class BasicTraits>
Spatialize<BasicTraits>::Spatialize ()
  : m_depth(0)
{
   if (getGroupCore() == NullFC) {
      setGroupCore(Group::create());
   }
}
template <class BasicTraits>
Spatialize<BasicTraits>::~Spatialize ()
{
}

template <class BasicTraits>
GroupPtr Spatialize<BasicTraits>::s_group;

template <class BasicTraits>
GroupPtr Spatialize<BasicTraits>::getGroupCore ()
{
   return s_group;
}
template <class BasicTraits>
void          Spatialize<BasicTraits>::setGroupCore (const GroupPtr& group)
{
   setRefdCP(s_group, group);
}

template <class BasicTraits>
NodePtr Spatialize<BasicTraits>::getRoot () const
{ 
   return m_root;
}
template <class BasicTraits>
void         Spatialize<BasicTraits>::setDepth (unsigned depth)
{
   m_depth = depth;
}
template <class BasicTraits>
unsigned     Spatialize<BasicTraits>::getDepth () const
{
   return m_depth;
}

template <class BasicTraits>
bool     Spatialize<BasicTraits>::InitEnter  (GeneralType*, 
					      const TransformType&)
{
   m_current = 0;
   return true;
}
template <class BasicTraits>
bool     Spatialize<BasicTraits>::InitLeave  (GeneralType*, 
					      const TransformType&)
{
   assert(m_rootStack.size() == 0);
   return true;
}
template <class BasicTraits>
typename Spatialize<BasicTraits>::ResultT  
Spatialize<BasicTraits>::InnerEnter (GroupType*)
{
  if (m_current <= m_depth) {
     NodePtr root = Node::create();
     if (m_current > 0) {
        m_rootLevel->addChild(root);
     } else {
        m_root = root;
     }
     m_rootLevel = root;
     m_rootStack.push_back(root);
     beginEditCP(root);
     root->setCore(getGroupCore());
  }
  ++m_current;
  return SingleTraverserBase<BasicTraits>::CONTINUE;
}
template <class BasicTraits>
typename Spatialize<BasicTraits>::ResultT  
Spatialize<BasicTraits>::InnerLeave (GroupType*)
{
  --m_current;
  if (m_current <= m_depth) {
     endEditCP(m_rootLevel);
     m_rootStack.pop_back();
     if (m_rootStack.size() > 0) {
	m_rootLevel = m_rootStack.back();
     }
  }
  return SingleTraverserBase<BasicTraits>::CONTINUE;
}
template <class BasicTraits>
typename Spatialize<BasicTraits>::ResultT  
Spatialize<BasicTraits>::LeafEnter  (AdapterType* node)
{
   // create new node with transformed positions and normals
   NodePtr     newNode = Node::create();
   GeometryPtr oldCore = GeometryPtr::dcast(node->getOriginal()->getCore());
   GeometryPtr newCore = oldCore->clone();
   beginEditCP(newNode);
   newNode->setCore(newCore);
   endEditCP(newNode);
   beginEditCP(newCore);
   newCore->setPositions(node->getPositions());
   newCore->setNormals  (node->getNormals());
   endEditCP(newCore);

   m_rootLevel->addChild(newNode);
   return SingleTraverserBase<BasicTraits>::CONTINUE;
}


template <class BasicTraits>
typename SpatializeTraits<BasicTraits>::InitFunctorT  
SpatializeTraits<BasicTraits>::createInitFunctor  (ObjectT* obj)
{
   typename InitFunctorT::InitMethodT f = &(ObjectT::InitEnter);
   return InitFunctorT(obj, f);
}
template <class BasicTraits>
typename SpatializeTraits<BasicTraits>::InitFunctorT  
SpatializeTraits<BasicTraits>::createInitLeaveFunctor  (ObjectT* obj)
{
   typename InitFunctorT::InitMethodT f = &(ObjectT::InitLeave);
   return InitFunctorT(obj, f);
}
template <class BasicTraits>
typename SpatializeTraits<BasicTraits>::InnerFunctorT 
SpatializeTraits<BasicTraits>::createInnerFunctor (ObjectT* obj)
{
   typename InnerFunctorT::DispatchMethodT f = &(ObjectT::InnerEnter);
   return InnerFunctorT(obj, f);
}
template <class BasicTraits>
typename SpatializeTraits<BasicTraits>::InnerFunctorT 
SpatializeTraits<BasicTraits>::createInnerLeaveFunctor (ObjectT* obj)
{
   typename InnerFunctorT::DispatchMethodT f = &(ObjectT::InnerLeave);
   return InnerFunctorT(obj, f);
}
template <class BasicTraits>
typename SpatializeTraits<BasicTraits>::LeafFunctorT  
SpatializeTraits<BasicTraits>::createLeafFunctor  (ObjectT* obj)
{
   typename LeafFunctorT::DispatchMethodT f = &(ObjectT::LeafEnter);
   return LeafFunctorT(obj, f);
}



