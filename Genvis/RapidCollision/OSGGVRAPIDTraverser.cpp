//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.2 $
//   $Date: 2003/09/19 21:54:06 $
//                                                                            
//=============================================================================

#include "OSGGVRAPIDTraverser.h"
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
#ifdef GV_WITH_RAPID
   typedef double RAPIDMatrix3[3][3];
   typedef double RAPIDVector3[3];
   typedef double RAPIDScale;
   typedef int    RAPIDResult;

   getDataTyped().Init();

   CacheData& data0 = getCache()[node0];
   CacheData& data1 = getCache()[node1];

   RAPIDMatrix3 r0, r1;
   RAPIDVector3 t0, t1;
   RAPIDScale   s0, s1;

   bool result = false;
   for (typename CacheData::AdapterContainer::const_iterator 
        it0 = data0.getAdapter(ObjectAdapterType::getAdapterId()).begin();
	it0 != data0.getAdapter(ObjectAdapterType::getAdapterId()).end();
	++it0) {
      ObjectAdapterType* rapid0 = static_cast<ObjectAdapterType*>(*it0);

      CacheData&     d0 = getCache()[rapid0->getOriginal()];
      TransformType& m0 = d0.getToWorldMatrix();

      // convert matrix data
      s0 = m0[0].length();
      r0[0][0] = m0[0][0]/s0; r0[0][1] = m0[1][0]/s0; r0[0][2] = m0[2][0]/s0; t0[0] = m0[3][0]/s0;
      r0[1][0] = m0[0][1]/s0; r0[1][1] = m0[1][1]/s0; r0[1][2] = m0[2][1]/s0; t0[1] = m0[3][1]/s0;
      r0[2][0] = m0[0][2]/s0; r0[2][1] = m0[1][2]/s0; r0[2][2] = m0[2][2]/s0; t0[2] = m0[3][2]/s0;

      for (typename CacheData::AdapterContainer::const_iterator 
	   it1=data1.getAdapter(ObjectAdapterType::getAdapterId()).begin();
	   it1 != data1.getAdapter(ObjectAdapterType::getAdapterId()).end();
	   ++it1) {
	 ObjectAdapterType* rapid1 = static_cast<ObjectAdapterType*>(*it1);

	 CacheData&     d1 = getCache()[rapid1->getOriginal()];
	 TransformType& m1 = d1.getToWorldMatrix();

	 // convert matrix data
         s1 = m1[0].length();
	 r1[0][0] = m1[0][0]/s1; r1[0][1] = m1[1][0]/s1; r1[0][2] = m1[2][0]/s1; t1[0] = m1[3][0]/s1;
	 r1[1][0] = m1[0][1]/s1; r1[1][1] = m1[1][1]/s1; r1[1][2] = m1[2][1]/s1; t1[1] = m1[3][1]/s1;
	 r1[2][0] = m1[0][2]/s1; r1[2][1] = m1[1][2]/s1; r1[2][2] = m1[2][2]/s1; t1[2] = m1[3][2]/s1;

	 RAPIDResult thisResult = RAPID_Collide(r0, t0, s0, rapid0->getModel(),
						r1, t1, s1, rapid1->getModel(),
						getDataTyped().getMode());
	 if (thisResult == RAPID_OK) {
	    // copy results
	    for (unsigned i=0; i<RAPID_num_contacts; ++i) {
 	       getDataTyped().getContactFaces().push_back(
	       FaceAdapterType(rapid0, RAPID_contact[i].id1));
 	       getDataTyped().getContactFaces().push_back(
	       FaceAdapterType(rapid1, RAPID_contact[i].id2));
	    }
	    for (FaceContainer::iterator ii = getDataTyped().getContactFaces().end()-(2*RAPID_num_contacts); 
		 ii < getDataTyped().getContactFaces().end(); 
		 ii += 2) {
	       getDataTyped().getContacts().push_back(CollisionPair(ii, ii+1));
	    }

	    getDataTyped().getBVolBVolTests() += RAPID_num_box_tests; 
	    getDataTyped().getPrimPrimTests() += RAPID_num_tri_tests;
	    if (RAPID_num_contacts > 0) {
	       result = true;
	    }
	 }
      }
   }
   return result;
#else
   return false;
#endif
}
