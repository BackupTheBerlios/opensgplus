//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.4 $
//   $Date: 2006/06/08 16:57:46 $
//                                                                            
//=============================================================================

#include <assert.h>
#include "OSGGVRAPIDCollision.h"
#ifdef GV_WITH_RAPID
#include "RAPID.H"
#endif
OSG_USING_NAMESPACE
USING_GENVIS_NAMESPACE

BEGIN_GENVIS_NAMESPACE

template <class BasicTraits>
RAPIDCollision<BasicTraits>::RAPIDCollision ()
  : Inherited()
{
   m_last = 0;
#ifdef GV_WITH_RAPID
   m_mode = RAPID_ALL_CONTACTS;
#endif
}
template <class BasicTraits>
void RAPIDCollision<BasicTraits>::dump (std::ostream& os)
{
   os << "RAPIDCollision";
}

template <class BasicTraits>
void RAPIDCollision<BasicTraits>::setStopFirst (bool flag)
{
   Inherited::setStopFirst(flag);
#ifdef GV_WITH_RAPID
   m_mode = (flag ? RAPID_FIRST_CONTACT : RAPID_ALL_CONTACTS);
#endif
}

template<class BasicTraits>
bool RAPIDCollision<BasicTraits>::Init ()
{
   // clears getContacts()
   bool result = Inherited::Init();
#if 1
   // clear contact containers
   getContactFirst() .reserve(GV_MAX_CONTACTS);
   getContactSecond().reserve(GV_MAX_CONTACTS);
   if (m_last > 0) {
      getContactFirst() .erase(getContactFirst().begin(),  getContactFirst().begin()+(m_last)); 
      getContactSecond().erase(getContactSecond().begin(), getContactSecond().begin()+(m_last)); 
   }
   assert(getContactFirst().size() == getContactSecond().size());
   m_last = getContactFirst().size();
#endif
   return result;
}

template <class BasicTraits>
bool RAPIDCollision<BasicTraits>::InitDouble  
(GroupType* root0, const TransformType& m0, const TransformType& f0, 
 GroupType* root1, const TransformType& m1, const TransformType& f1)
{
#ifdef GV_WITH_RAPID
   // convert matrix data
   m_s0 = m0[0].length();
   m_r0[0][0] = m0[0][0]/m_s0; m_r0[0][1] = m0[1][0]/m_s0; m_r0[0][2] = m0[2][0]/m_s0; m_t0[0] = m0[3][0];
   m_r0[1][0] = m0[0][1]/m_s0; m_r0[1][1] = m0[1][1]/m_s0; m_r0[1][2] = m0[2][1]/m_s0; m_t0[1] = m0[3][1];
   m_r0[2][0] = m0[0][2]/m_s0; m_r0[2][1] = m0[1][2]/m_s0; m_r0[2][2] = m0[2][2]/m_s0; m_t0[2] = m0[3][2];
   // convert matrix data
   m_s1 = m1[0].length();
   m_r1[0][0] = m1[0][0]/m_s1; m_r1[0][1] = m1[1][0]/m_s1; m_r1[0][2] = m1[2][0]/m_s1; m_t1[0] = m1[3][0];
   m_r1[1][0] = m1[0][1]/m_s1; m_r1[1][1] = m1[1][1]/m_s1; m_r1[1][2] = m1[2][1]/m_s1; m_t1[1] = m1[3][1];
   m_r1[2][0] = m1[0][2]/m_s1; m_r1[2][1] = m1[1][2]/m_s1; m_r1[2][2] = m1[2][2]/m_s1; m_t1[2] = m1[3][2];
#endif
   return true;
}

template <class BasicTraits>
bool RAPIDCollision<BasicTraits>::LeaveDouble 
(GroupType* root0, const TransformType& m0, const TransformType& f0, 
 GroupType* root1, const TransformType& m1, const TransformType& f1)
{
   return true;
}

template <class BasicTraits>
typename RAPIDCollision<BasicTraits>::ResultT 
RAPIDCollision<BasicTraits>::BVolBVolCollision (GroupType* node0, GroupType* node1)
{
   bool result = false;
#ifdef GV_WITH_RAPID
   if (!getStopFirst() || getNumContacts() == 0) {
     AdapterType* rapid0 = (AdapterType*)(node0);
     AdapterType* rapid1 = (AdapterType*)(node1);
     IntersectionTest.start();
     int thisResult = RAPID_Collide(m_r0, m_t0, m_s0, rapid0->getModel(),
				    m_r1, m_t1, m_s1, rapid1->getModel(),
				    getMode());
     IntersectionTest.stop();
     getBVolBVolTests() += RAPID_num_box_tests; 
     getPrimPrimTests() += RAPID_num_tri_tests;
     if (thisResult == RAPID_OK) {
        // create contact face information
        i32 num = stdMin(RAPID_num_contacts, i32(GV_MAX_CONTACTS));
#if 1
        for (i32 i=0; i<num; ++i) {
	   getContactFirst().push_back (rapid0->getFaceAdapter(RAPID_contact[i].id1));
	   getContactSecond().push_back(rapid1->getFaceAdapter(RAPID_contact[i].id2));
	}
	typename FaceContainer::iterator it1;
	typename FaceContainer::iterator it2;
	for (it1 = getContactFirst().end()-num,
	       it2 = getContactSecond().end()-num; 
	     it1 != getContactFirst().end(); 
	     ++it1, ++it2) {
	   // create contact pairs
	   getContacts().push_back(CollisionPair(&(*it1), &(*it2)));
	}
#else
        for (i32 i=0; i<num; ++i) {
	   // create contact pairs
	   getContacts().push_back(CollisionPair(&(rapid0->getFaceAdapter(RAPID_contact[i].id1)), 
						 &(rapid1->getFaceAdapter(RAPID_contact[i].id2))));
	}
#endif
	if (RAPID_num_contacts > 0) {
	   result = true;
	}
     }
   }
#endif
   if (result) {
     return DoubleTraverserBase<BasicTraits>::CONTINUE;
   }
   return DoubleTraverserBase<BasicTraits>::QUIT;
}

template <class BasicTraits>
bool RAPIDCollision<BasicTraits>::apply (const GeomObjectType& node0, const GeomObjectType& node1)
{
   CacheData& data0 = Cache::the()[node0];
   CacheData& data1 = Cache::the()[node1];

   GroupType* root0 = (GroupType*)(*data0.getAdapter(AdapterType::getAdapterId()).begin());
   GroupType* root1 = (GroupType*)(*data1.getAdapter(AdapterType::getAdapterId()).begin());
   assert(root0 != NULL);
   assert(root1 != NULL);

   TransformType m0;
   if (data0.getAdapterMatrix(AdapterType::getAdapterId()) == TransformType::identity()) {
      m0.setValue(data0.getToWorldMatrix());
   } else {
      m0.invertFrom(data0.getAdapterMatrix(AdapterType::getAdapterId()));
      m0.multLeft(data0.getToWorldMatrix());
   }
   TransformType m1; 
   if (data1.getAdapterMatrix(AdapterType::getAdapterId()) == TransformType::identity()) {
      m1.setValue(data1.getToWorldMatrix());
   } else {
      m1.invertFrom(data1.getAdapterMatrix(AdapterType::getAdapterId()));
      m1.multLeft(data1.getToWorldMatrix());
   }
   if (InitDouble(root0, m0, data0.getFrameMatrix(), 
		  root1, m1, data1.getFrameMatrix())) {
      BVolBVolCollision (root0, root1);
      return true;
   }
   return false;
}
template <class BasicTraits>
bool RAPIDCollision<BasicTraits>::apply 
(CacheData& parent0, CacheData& data0, BVolAdapterBase* node0, 
 CacheData& parent1, CacheData& data1, BVolAdapterBase* node1, 
 const TransformType& m0, const TransformType& m1)
{
   GroupType* root0 = (GroupType*)(node0);
   GroupType* root1 = (GroupType*)(node1);
   if (InitDouble(root0, m0, parent0.getFrameMatrix(), 
		  root1, m1, parent1.getFrameMatrix())) {
      BVolBVolCollision (root0, root1);
      return true;
   }
   return false;
}


template <class BasicTraits>
typename RAPIDCollisionTraits<BasicTraits>::InitFunctorT 
RAPIDCollisionTraits<BasicTraits>::createInitFunctor (ObjectT* obj) 
{
   typename InitFunctorT::InitMethodT f = &(ObjectT::Init);
   return InitFunctorT(obj, f);
}
template <class BasicTraits>
typename RAPIDCollisionTraits<BasicTraits>::InitDoubleFunctorT 
RAPIDCollisionTraits<BasicTraits>::createInitDoubleFunctor (ObjectT* obj) 
{
   typename InitDoubleFunctorT::InitMethodT f = &(ObjectT::InitDouble);
   return InitDoubleFunctorT(obj, f);
}
template <class BasicTraits>
typename RAPIDCollisionTraits<BasicTraits>::LeaveDoubleFunctorT 
RAPIDCollisionTraits<BasicTraits>::createLeaveDoubleFunctor (ObjectT* obj) 
{
   typename LeaveDoubleFunctorT::InitMethodT f = &(ObjectT::LeaveDouble);
   return LeaveDoubleFunctorT(obj, f);
}
template <class BasicTraits>
typename RAPIDCollisionTraits<BasicTraits>::BVolBVolFunctorT 
RAPIDCollisionTraits<BasicTraits>::createBVolBVolFunctor (ObjectT* obj) 
{
   typename BVolBVolFunctorT::DispatchMethodT f = &(ObjectT::BVolBVolCollision);
   return BVolBVolFunctorT(obj, f);
}

// explicit template instantiations
template class OSG_GENVISLIB_DLLMAPPING RAPIDCollision<OpenSGTraits>;
//template class OSG_GENVISLIB_DLLMAPPING RAPIDCollisionTraits<OpenSGTraits>;

END_GENVIS_NAMESPACE


