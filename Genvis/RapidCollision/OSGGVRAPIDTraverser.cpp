//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.3 $
//   $Date: 2004/03/12 13:28:31 $
//                                                                            
//=============================================================================

#include "OSGGVRAPIDTraverser.h"
#include "OSGGVProfiler.h"
#ifdef GV_WITH_RAPID
# include "RAPID.H"
#endif
OSG_USING_NAMESPACE
USING_GENVIS_NAMESPACE

// explicit template instantiations 
template class OSG_GENVISLIB_DLLMAPPING RAPIDAllTraverser<OpenSGTraits>;

#if 0
template <class BasicTraits>
double& RAPIDAllTraverser<BasicTraits>::getInitTime     ()
{
   return RAPID_time_init;
}
template <class BasicTraits>
double& RAPIDAllTraverser<BasicTraits>::getBVolTestTime ()
{
   return RAPID_time_box_tests;
}
#endif

template <class BasicTraits>
bool RAPIDAllTraverser<BasicTraits>::apply   (const OSG::NodePtr& node0, const OSG::NodePtr& node1)
{
   Profiler::the().StartProfile("RAPIDAllTraverser.Init");
   getDataTyped().Init();
   Profiler::the().EndProfile("RAPIDAllTraverser.Init");

#ifdef GV_WITH_RAPID
   typedef double RAPIDMatrix3[3][3];
   typedef double RAPIDVector3[3];
   typedef double RAPIDScale;
   typedef int    RAPIDResult;

   CacheData& data0 = Cache::the()[node0];
   CacheData& data1 = Cache::the()[node1];

   RAPIDMatrix3 r0, r1;
   RAPIDVector3 t0, t1;
   RAPIDScale   s0, s1;

   Profiler::the().StartProfile("RAPIDAllTraverser.Loop");
   typename CacheData::AdapterContainer::const_iterator it0;
   typename CacheData::AdapterContainer::const_iterator it1;
   // compute to world matrices once
   std::vector<TransformType> all1; all1.reserve(data1.getAdapter(ObjectAdapterType::getAdapterId()).size());
   for (it1 = data1.getAdapter(ObjectAdapterType::getAdapterId()).begin();
	it1 != data1.getAdapter(ObjectAdapterType::getAdapterId()).end();
	++it1) {
	 ObjectAdapterType* rapid1 = static_cast<ObjectAdapterType*>(*it1);
	 CacheData&   d1 = rapid1->getObjectAdapter();
	 all1.push_back(d1.getToWorldMatrix());
   }
   // collision loop
   u32 i;
   bool result = false;
   for (it0 = data0.getAdapter(ObjectAdapterType::getAdapterId()).begin();
	it0 != data0.getAdapter(ObjectAdapterType::getAdapterId()).end();
	++it0) {
      Profiler::the().StartProfile("RAPIDAllTraverser.Prepare");
      Profiler::the().StartProfile("RAPIDAllTraverser.GetCacheData");
      ObjectAdapterType* rapid0 = static_cast<ObjectAdapterType*>(*it0);
      CacheData&    d0 = rapid0->getObjectAdapter();
      Profiler::the().EndProfile("RAPIDAllTraverser.GetCacheData");
      Profiler::the().StartProfile("RAPIDAllTraverser.WorldMatrix");
      TransformType m0 = d0.getToWorldMatrix();
      Profiler::the().EndProfile("RAPIDAllTraverser.WorldMatrix");
      Profiler::the().EndProfile("RAPIDAllTraverser.Prepare");

      // convert matrix data
      s0 = m0[0].length();
      r0[0][0] = m0[0][0]/s0; r0[0][1] = m0[1][0]/s0; r0[0][2] = m0[2][0]/s0; t0[0] = m0[3][0];
      r0[1][0] = m0[0][1]/s0; r0[1][1] = m0[1][1]/s0; r0[1][2] = m0[2][1]/s0; t0[1] = m0[3][1];
      r0[2][0] = m0[0][2]/s0; r0[2][1] = m0[1][2]/s0; r0[2][2] = m0[2][2]/s0; t0[2] = m0[3][2];

      for (it1 = data1.getAdapter(ObjectAdapterType::getAdapterId()).begin(), i=0;
	   it1 != data1.getAdapter(ObjectAdapterType::getAdapterId()).end();
	   ++it1, ++i) {
	 Profiler::the().StartProfile("RAPIDAllTraverser.Prepare");
	 Profiler::the().StartProfile("RAPIDAllTraverser.GetCacheData");
	 ObjectAdapterType* rapid1 = static_cast<ObjectAdapterType*>(*it1);
	 CacheData&    d1 = rapid1->getObjectAdapter();
	 Profiler::the().EndProfile("RAPIDAllTraverser.GetCacheData");
	 Profiler::the().StartProfile("RAPIDAllTraverser.WorldMatrix");
	 //TransformType m1 = d1.getToWorldMatrix();
	 const TransformType& m1 = all1[i];
	 Profiler::the().EndProfile("RAPIDAllTraverser.WorldMatrix");
	 Profiler::the().EndProfile("RAPIDAllTraverser.Prepare");

	 Profiler::the().StartProfile("RAPIDAllTraverser.TestSameNode");
	 bool same = (d0.getNode() == d1.getNode());
	 Profiler::the().EndProfile("RAPIDAllTraverser.TestSameNode");
	 if (same) {
	    continue;
	 }

	 Profiler::the().StartProfile("RAPIDAllTraverser.RapidCollide");
	 // convert matrix data
         s1 = m1[0].length();
	 r1[0][0] = m1[0][0]/s1; r1[0][1] = m1[1][0]/s1; r1[0][2] = m1[2][0]/s1; t1[0] = m1[3][0];
	 r1[1][0] = m1[0][1]/s1; r1[1][1] = m1[1][1]/s1; r1[1][2] = m1[2][1]/s1; t1[1] = m1[3][1];
	 r1[2][0] = m1[0][2]/s1; r1[2][1] = m1[1][2]/s1; r1[2][2] = m1[2][2]/s1; t1[2] = m1[3][2];
	 RAPIDResult thisResult = RAPID_Collide(r0, t0, s0, rapid0->getModel(),
						r1, t1, s1, rapid1->getModel(),
						getDataTyped().getMode());
	 getDataTyped().getBVolBVolTests() += RAPID_num_box_tests; 
	 getDataTyped().getPrimPrimTests() += RAPID_num_tri_tests;
	 if (thisResult == RAPID_OK) {
	    // create contact face information
	    for (i32 i=0; i<RAPID_num_contacts; ++i) {
 	       getDataTyped().getContactFirst().push_back(
	       FaceAdapterType(rapid0, RAPID_contact[i].id1));
 	       getDataTyped().getContactSecond().push_back(
	       FaceAdapterType(rapid1, RAPID_contact[i].id2));
	    }
	    typename FaceContainer::iterator it1;
	    typename FaceContainer::iterator it2;
	    for (it1 = getDataTyped().getContactFirst().end()-(RAPID_num_contacts),
		   it2 = getDataTyped().getContactSecond().end()-(RAPID_num_contacts); 
		 it1 != getDataTyped().getContactFirst().end(); 
		 ++it1, ++it2) {
	       // create contact pairs
	       getDataTyped().getContacts().push_back(CollisionPair(&(*it1), &(*it2)));
	    }
	    if (RAPID_num_contacts > 0) {
	       result = true;
	    }
	 }
	 Profiler::the().EndProfile("RAPIDAllTraverser.RapidCollide");   
      }
   }
   Profiler::the().EndProfile("RAPIDAllTraverser.Loop");
   return result;
#else
   return false;
#endif
}
