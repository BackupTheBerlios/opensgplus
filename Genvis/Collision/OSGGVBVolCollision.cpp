//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.2 $
//   $Date: 2004/03/12 13:18:34 $
//                                                                            
//=============================================================================

#include "OSGGVBVolCollision.h"
#include "OSGGVDoubleTraverserFixed.h"
#include "OSGGVReal.h"
#include "OSGGVGroup.h"
#include "OSGGVGeomFunctions.h"
#include "OSGGVBVolFunctions.h"
USING_GENVIS_NAMESPACE
OSG_USING_NAMESPACE

// explicit template instantiations
template class OSG_GENVISLIB_DLLMAPPING BVolCollisionBase<OpenSGTraits>;

template class OSG_GENVISLIB_DLLMAPPING BVolCollision<OpenSGTraits,K6Dop>;
template class OSG_GENVISLIB_DLLMAPPING BVolCollision<OpenSGTraits,K14Dop>;
template class OSG_GENVISLIB_DLLMAPPING BVolCollision<OpenSGTraits,K18Dop>;
template class OSG_GENVISLIB_DLLMAPPING BVolCollision<OpenSGTraits,K26Dop>;
template class OSG_GENVISLIB_DLLMAPPING BVolCollision<OpenSGTraits,K12Dop>;
template class OSG_GENVISLIB_DLLMAPPING BVolCollision<OpenSGTraits,K20Dop>;

template class OSG_GENVISLIB_DLLMAPPING BVolCollisionTraits<OpenSGTraits,K6Dop>;
template class OSG_GENVISLIB_DLLMAPPING BVolCollisionTraits<OpenSGTraits,K14Dop>;
template class OSG_GENVISLIB_DLLMAPPING BVolCollisionTraits<OpenSGTraits,K18Dop>;
template class OSG_GENVISLIB_DLLMAPPING BVolCollisionTraits<OpenSGTraits,K26Dop>;
template class OSG_GENVISLIB_DLLMAPPING BVolCollisionTraits<OpenSGTraits,K12Dop>;
template class OSG_GENVISLIB_DLLMAPPING BVolCollisionTraits<OpenSGTraits,K20Dop>;


template<class BasicTraits, class BVOL>
u32 BVolCollision<BasicTraits,BVOL>::getCurrentDepth0 (const DoubleTraverserFixedBase<BasicTraits>* trav,
						       GroupType* adapter) const
{
#ifdef GV_COUNT_DEPTH
   return adapter->getDepth();
#else
   assert(trav != NULL);
   return trav->getCurrentDepth0();
#endif
}
template<class BasicTraits, class BVOL>
u32 BVolCollision<BasicTraits,BVOL>::getCurrentDepth1 (const DoubleTraverserFixedBase<BasicTraits>* trav,
						       GroupType* adapter) const
{
#ifdef GV_COUNT_DEPTH
   return adapter->getDepth();
#else
   assert(trav != NULL);
   return trav->getCurrentDepth1();
#endif
}

template<class BasicTraits, class BVOL>
bool BVolCollision<BasicTraits,BVOL>::InitDouble (GroupType* g0, const TransformType& m0, const TransformType& f0, 
						  GroupType* g1, const TransformType& m1, const TransformType& f1)
{
   // transformation matrix model1 to model0
   // m0^(-1) * m1
   TransformType WCToM0; 
   if (!WCToM0.invertFrom(m0)) {
      return false;
   }
   m_M1ToM0.setValue(WCToM0);
   m_M1ToM0.mult(m1);

   u32 k, kk;
   // account for scaling and rotation from M1 to M0
   for (k=0; k<BVOL::Size; ++k) {
      // the same as, but more efficient
      //m_M1ToM0.transpose(); m_M1ToM0.mult(BVOL::getDirection()[k], m_M1Direction[k]);
      m_M1Direction[k].setValues(m_M1ToM0[0][0] * BVOL::getDirection()[k][0]
				 + m_M1ToM0[0][1] * BVOL::getDirection()[k][1]
				 + m_M1ToM0[0][2] * BVOL::getDirection()[k][2],
				 m_M1ToM0[1][0] * BVOL::getDirection()[k][0]
				 + m_M1ToM0[1][1] * BVOL::getDirection()[k][1]
				 + m_M1ToM0[1][2] * BVOL::getDirection()[k][2],
				 m_M1ToM0[2][0] * BVOL::getDirection()[k][0]
				 + m_M1ToM0[2][1] * BVOL::getDirection()[k][1]
				 + m_M1ToM0[2][2] * BVOL::getDirection()[k][2]);
      m_M1Direction[k] *= 0.5f;
   }
   // account for translation from M1 to M0
   VectorClass trans(m_M1ToM0[3][0], m_M1ToM0[3][1], m_M1ToM0[3][2]);
#ifdef GV_COLDYNAMIC
   // global translation in world-coordinates
   m_t0.setValues(f0[3][0], f0[3][1], f0[3][2]); WCToM0.mult(m_t0);
   m_t1.setValues(f1[3][0], f1[3][1], f1[3][2]); WCToM0.mult(m_t1);
#endif
   for (k=0, kk=BVOL::Size; k<BVOL::Size; ++k, ++kk) {
      m_proj10[k]   = stdAbs(m_M1Direction[k][0]); //BVOL::getDirection()[k].dot(dir0));
      m_proj11[k]   = stdAbs(m_M1Direction[k][1]); //BVOL::getDirection()[k].dot(dir1));
      m_proj12[k]   = stdAbs(m_M1Direction[k][2]); //BVOL::getDirection()[k].dot(dir2));
      m_M1Offset[k] = BVOL::getDirection()[k].dot(trans);
      //
      m_proj0[k] = BVOL::getDirection()[k].dot(m_t0);
#ifdef GV_COLDYNAMIC
      if (m_proj0[k] >= 0) {
	 m_proj0[kk] = 0;
      } else {
	 m_proj0[kk] = m_proj0[k];
 	 m_proj0[k]  = 0;
      }
      m_proj1[k] = BVOL::getDirection()[k].dot(m_t1);
      if (m_proj1[k] >= 0) {
	 m_proj1[kk] = 0;
      } else {
	 m_proj1[kk] = m_proj1[k];
 	 m_proj1[k]  = 0;
      }
#endif
   }
#ifdef GV_WITH_CLASS2
#ifdef GV_BVOLS_IN_MIXEDTESTS
   // m1^(-1) * m0
   m_M0ToM1.invertFrom(m_M1ToM0);
#endif
   for (k=0; k<BVOL::Size; ++k) {
      // the same as, but more efficient
      //m_M0ToM1.transpose(); m_M0ToM1.mult(BVOL::getDirection()[k], m_M0Direction[k]);
      m_M0Direction[k].setValues(m_M0ToM1[0][0] * BVOL::getDirection()[k][0]
				 + m_M0ToM1[0][1] * BVOL::getDirection()[k][1]
				 + m_M0ToM1[0][2] * BVOL::getDirection()[k][2],
				 m_M0ToM1[1][0] * BVOL::getDirection()[k][0]
				 + m_M0ToM1[1][1] * BVOL::getDirection()[k][1]
				 + m_M0ToM1[1][2] * BVOL::getDirection()[k][2],
				 m_M0ToM1[2][0] * BVOL::getDirection()[k][0]
				 + m_M0ToM1[2][1] * BVOL::getDirection()[k][1]
				 + m_M0ToM1[2][2] * BVOL::getDirection()[k][2]);
      m_M0Direction[k] *= 0.5f;
   }
   trans.setValues(m_M0ToM1[3][0], m_M0ToM1[3][1], m_M0ToM1[3][2]);
   for (k=0; k<BVOL::Size; ++k) {
      m_proj00[k]   = stdAbs(m_M0Direction[k][0]); //BVOL::getDirection()[k].dot(dir0));
      m_proj01[k]   = stdAbs(m_M0Direction[k][1]); //BVOL::getDirection()[k].dot(dir1));
      m_proj02[k]   = stdAbs(m_M0Direction[k][2]); //BVOL::getDirection()[k].dot(dir2));
      m_M0Offset[k] = BVOL::getDirection()[k].dot(trans);
   }
#endif

   static FloatingComparator<Real> comp(1e-4);
   if (comp.positive(m_t0.length()) || comp.positive(m_t1.length())) {
      f_primIntersect = &Self::primIntersectDynamic;
   } else {
      f_primIntersect = &Self::primIntersectStatic;
   }

   return true;
}

template<class BasicTraits, class BVOL>
Real BVolCollision<BasicTraits,BVOL>::bvolIntersect 
(const BVOL& dop0, const BVOL& dop1)
{
   static FloatingComparator<Real> comp;
   // Class I: dop0 directions
   u32  k, kk;
   Real min1, max1, min2, max2;
   Real tmin1 = 0.0f;
   Real tmax1 = +1.0f;
   Real tmin2 = 0.0f;
   Real tmax2 = +1.0f;
   for (k=0, kk=BVOL::Size; k<BVOL::Size; ++k, ++kk) {
      Real projCenter = dop1.addition(0) * m_M1Direction[k][0]
	+ dop1.addition(1) * m_M1Direction[k][1]
	+ dop1.addition(2) * m_M1Direction[k][2];
      Real projRadius = dop1.difference(0) * m_proj10[k]
	+ dop1.difference(1) * m_proj11[k]
	+ dop1.difference(2) * m_proj12[k];

      // static configuration
      min1 = dop0.minVector()[k];
      max1 = dop0.maxVector()[k];
      min2 = projCenter-projRadius+m_M1Offset[k];
      max2 = projCenter+projRadius+m_M1Offset[k];
      Real size1 = max1-min1;
      Real size2 = max2-min2;
      // modify to dynamic configuration
      min1 += m_proj0[kk];
      max1 += m_proj0[k];
      min2 += m_proj1[kk];
      max2 += m_proj1[k];
      bool ezero1 = false;
      Real esize1 = m_proj0[k]-m_proj0[kk];
      if (comp.zero(esize1)) {
	//esize1 = comp.getPrecision();
	ezero1 = true;
      }
      bool ezero2 = false;
      Real esize2 = m_proj1[k]-m_proj1[kk];
      if (comp.zero(esize2)) {
	//esize2 = comp.getPrecision();
	ezero2 = true;
      }

      Real value1 = max1-min2;
      Real value2 = min1-max2;
      //(value1 >= 0 && value2 >= 0) disjoint case: dop1 to the right of dop2
      //(value1 <  0 && value2 <  0) disjoint case: dop1 to the left of dop2
#if defined(GV_COLDYNAMIC)
      if (stdMin(max1, max2) < stdMax(min1, min2)) {
         return -1.0f;     
      } else if (min1 < min2) {// dop1 to the left of dop2
 	 // value1 >= 0, value2 <= 0
	 if (m_proj0[kk] < 0) { // to the left
	    if (m_proj1[kk] < 0) { // to the left
	       if (!ezero1) {
		  Real t1 = 1.0f+value2/esize1;
		  Real t2 = value1/esize1;
		  //if (!(t1 <= t2)) { std::cout << "assert " << __LINE__ << std::endl; }
		  tmin1 = stdMax(tmin1, t1);
		  tmax1 = stdMin(tmax1, t2);
	       }
	       if (!ezero2) {
		  Real t1 = 1.0f-value1/esize2; 
		  Real t2 = 1.0f;
		  //if (!(t1 <= t2)) { std::cout << "assert " << __LINE__ << std::endl; }
		  tmin2 = stdMax(tmin2, t1);
		  tmax2 = stdMin(tmax2, t2);
	       }
	    } else {// to the right
	       if (!ezero1) {
 		  Real t1 = 1.0f+value2/esize1;
		  Real t2 = value1/esize1;
		  //if (!(t1 <= t2)) { std::cout << "assert " << __LINE__ << std::endl; }
		  tmin1 = stdMax(tmin1, t1);
		  tmax1 = stdMin(tmax1, t2);
	       }
	       if (!ezero2) {
		  Real t1 = 0.0f; 
		  Real t2 = value1/esize2;
		  //if (!(t1 <= t2)) { std::cout << "assert " << __LINE__ << std::endl; }
		  tmin2 = stdMax(tmin2, t1);
		  tmax2 = stdMin(tmax2, t2);
	       }
	    }	   
	 } else { // to the right
	    if (m_proj1[kk] < 0) { // to the left
	        if (!ezero1) {
		   Real t1 = 1.0f-value1/esize1;
		   Real t2 = -value2/esize1;
		   //if (!(t1 <= t2)) { std::cout << "assert " << __LINE__ << std::endl; }
		   tmin1 = stdMax(tmin1, t1);
		   tmax1 = stdMin(tmax1, t2);
		}
		if (!ezero2) {
		   Real t1 = 1.0f-value1/esize2;
		   Real t2 = 1.0f;
		   //if (!(t1 <= t2)) { std::cout << "assert " << __LINE__ << std::endl; }
		   tmin2 = stdMax(tmin2, t1);
		   tmax2 = stdMin(tmax2, t2);
		}
	    } else {// to the right
	        if (!ezero1) {
		   Real t1 = 1.0f-value1/esize1;
		   Real t2 = -value2/esize1;
		   //if (!(t1 <= t2)) { std::cout << "assert " << __LINE__ << std::endl; }
		   tmin1 = stdMax(tmin1, t1);
		   tmax1 = stdMin(tmax1, t2);
		}
	        if (!ezero2) {
		   Real t1 = 0.0f;
		   Real t2 = value1/esize2;
		   //if (!(t1 <= t2)) { std::cout << "assert " << __LINE__ << std::endl; }
		   tmin2 = stdMax(tmin2, t1);
		   tmax2 = stdMin(tmax2, t2);
		}
		//std::cout << "1[" << 1.0f-value1/esize1 << ", " << -value2/esize1 << "]"; 
		//std::cout << "2[" << 0 << ", " << value1/esize2 << "]"; 
	    }
         }
      } else { // dop1 to the right of dop2
 	 // value1 >= 0, value2 <= 0
	 if (m_proj0[kk] < 0) { // to the left
	    if (m_proj1[kk] < 0) { // to the left
	        if (!ezero2) {
 		   Real t1 = 1.0f-value1/esize2;
		   Real t2 = -value2/esize2;
		   //if (!(t1 <= t2)) { std::cout << "assert " << __LINE__ << std::endl; }
		   tmin2 = stdMax(tmin2, t1);
		   tmax2 = stdMin(tmax2, t2);
		}
	        if (!ezero1) {
		   Real t1 = 1.0f+value2/esize1;
		   Real t2 = 1.0f;
		   //if (!(t1 <= t2)) { std::cout << "assert " << __LINE__ << std::endl; }
		   tmin1 = stdMax(tmin1, t1);
		   tmax1 = stdMin(tmax1, t2);
		}
	    } else { // to the right
	        if (!ezero2) {
		   Real t1 = 1.0f-value1/esize2;
		   Real t2 = -value2/esize2;
		   //if (!(t1 <= t2)) { std::cout << "assert " << __LINE__ << std::endl; }
		   tmin2 = stdMax(tmin2, t1);
		   tmax2 = stdMin(tmax2, t2);
		}
	        if (!ezero1) {
		   Real t1 = 0.0f;
		   Real t2 = -value2/esize1;
		   //if (!(t1 <= t2)) { std::cout << "assert " << __LINE__ << std::endl; }
		   tmin1 = stdMax(tmin1, t1);
		   tmax1 = stdMin(tmax1, t2);
		}
	    }	   
	 } else { // to the right
	    if (m_proj1[kk] < 0) { // to the left
	        if (!ezero2) {
		   Real t1 = 1.0f+value2/esize2;
		   Real t2 = value1/esize2;
		   //if (!(t1 <= t2)) { std::cout << "assert " << __LINE__ << std::endl; }
		   tmin2 = stdMax(tmin2, t1);
		   tmax2 = stdMin(tmax2, t2);
		}
	        if (!ezero1) {
		   Real t1 = 1.0f+value2/esize1;
		   Real t2 = 1.0f;
		   //if (!(t1 <= t2)) { std::cout << "assert " << __LINE__ << std::endl; }
		   tmin1 = stdMax(tmin1, t1);
		   tmax1 = stdMin(tmax1, t2);
		}
	    } else { // to the right
	        if (!ezero2) {
		   Real t1 = 1.0f+value2/esize2;
		   Real t2 = value1/esize2;
		   //if (!(t1 <= t2)) { std::cout << "assert " << __LINE__ << std::endl; }
		   tmin2 = stdMax(tmin2, t1);
		   tmax2 = stdMin(tmax2, t2);
		}
	        if (!ezero1) {
		   Real t1 = 0.0f;
		   Real t2 = -value2/esize1;
		   //if (!(t1 <= t2)) { std::cout << "assert " << __LINE__ << std::endl; }
		   tmin1 = stdMax(tmin1, t1);
		   tmax1 = stdMin(tmax1, t2);
		}
		//std::cout << "2[" << 1.0f-value2/esize2 << ", " << -value1/esize2 << "]"; 
		//std::cout << "1[" << 0 << ", " << value2/esize1 << "]"; 
	    }
         }
      }
#else
      if (stdMin(max1, max2) < stdMax(min1, min2)) {
         return -1.0f;     
      }
#endif
   }
#if defined(GV_COLDYNAMIC)
   if (stdMin(tmax1, tmax2) < stdMax(tmin1, tmin2)) {
      return -1.0;
   }
   return stdMax(tmin1, tmin2);
#else
   return 0.0f;
#endif

#if 0
   // Class I: dop0 directions
   u32 k, kk;
   Real min2, max2;
   for (k=0, kk=BVOL::Size; k<BVOL::Size; ++k, ++kk) {
      Real projCenter = dop1.addition(0) * m_M1Direction[k][0]
	+ dop1.addition(1) * m_M1Direction[k][1]
	+ dop1.addition(2) * m_M1Direction[k][2];
      Real projRadius = dop1.difference(0) * m_proj10[k]
	+ dop1.difference(1) * m_proj11[k]
	+ dop1.difference(2) * m_proj12[k];
      min2 = projCenter-projRadius;
      max2 = projCenter+projRadius;
      if (stdMin(dop0.maxVector()[k], max2+m_M1Offset[k]) 
	  < stdMax(dop0.minVector()[k], min2+m_M1Offset[k])) { // is separating axis
 	 return false;
      }
   }
   // Class II: dop1 directions
   for (k=0; k<BVOL::Size; ++k) {
      Real projCenter = dop0.addition(0) * m_M0Direction[k][0]
	+ dop0.addition(1) * m_M0Direction[k][1]
	+ dop0.addition(2) * m_M0Direction[k][2]
	+ m_M0Offset[k];
      Real projRadius = dop0.difference(0) * m_proj00[k]
	+ dop0.difference(1) * m_proj01[k]
	+ dop0.difference(2) * m_proj02[k];
      Real center     = dop1.addition(k);
      Real radius     = dop1.difference(k); 
      if (stdAbs(projCenter-center) > projRadius+radius) { 
	 // is separating axis
	 return false;
      }
   }
#endif
}

template<class BasicTraits, class BVOL>
Real BVolCollision<BasicTraits,BVOL>::primIntersectStatic
(AdapterType* p0, AdapterType* p1)
{
   PointClass d0, d1, d2;
   // Transform from model space 1 to model space 0
   m_M1ToM0.mult(p1->getPosition(0), d0);
   m_M1ToM0.mult(p1->getPosition(1), d1);
   m_M1ToM0.mult(p1->getPosition(2), d2);
   // Perform triangle-triangle overlap test
   if (genvis::triTriOverlap(p0->getPosition(0), 
			     p0->getPosition(1), 
			     p0->getPosition(2), 
			     d0, 
			     d1, 
			     d2)) {
     return 0.0f;
   }
   return -1.0f;
}
template<class BasicTraits, class BVOL>
Real BVolCollision<BasicTraits,BVOL>::primIntersectDynamic
(AdapterType* p0, AdapterType* p1)
{
   const BVOL& dop0 = p0->getBVol();
   const BVOL& dop1 = p1->getBVol();
   return bvolIntersect(dop0, dop1);
}

/*! Inner-inner test for two bvol trees.
    \param g0 [in] Node in first tree
    \param g1 [in] Node in second tree
 */
template <class BasicTraits, class BVOL>
typename BVolCollision<BasicTraits,BVOL>::ResultT
BVolCollision<BasicTraits,BVOL>::BVolBVolCollision 
(GroupType* g0, GroupType* g1)
{
   ++m_numBVolBVolTests;
   // perform BV-BV overlap test
   Real t = bvolIntersect(g0->getBVol(), g1->getBVol());
   if (t < 0.0f) {
      return DoubleTraverserBase<BasicTraits>::QUIT;
   }
   const DoubleTraverserFixedBase<BasicTraits>* trav 
     = dynamic_cast<const DoubleTraverserFixedBase<BasicTraits>*>(getTraverser());
   if (trav != NULL 
       && trav->getDepth0() == getCurrentDepth0(trav, g0) 
       && trav->getDepth1() == getCurrentDepth1(trav, g1)) {
      // Keep track of colliding pairs
      // check if first collision already found
      if (getNumContacts() == 0) {
	 m_contacts.push_back(CollisionPair((AdapterType*)g0->findLeaf(), 
					    (AdapterType*)g1->findLeaf()));
	 CollisionInterface<OpenSGTriangleBase<BasicTraits>,Real> 
	   result(m_contacts[m_contacts.size()-1]);
	 result.getData() = t;
      }
      CollisionInterface<OpenSGTriangleBase<BasicTraits>,Real> 
	result(m_contacts[0]);
      if (t < result.getData()) {
	 result.getData() = t;
	 m_contacts[0].setFirst((AdapterType*)g0->findLeaf());
	 m_contacts[0].setSecond((AdapterType*)g1->findLeaf());
      }
   }
   return DoubleTraverserBase<BasicTraits>::CONTINUE;
}

/*! Leaf-leaf test for two bvol trees.
    \param p0 [in] Leaf node in first tree
    \param p1 [in] Leaf node in second tree
 */
template <class BasicTraits, class BVOL>
typename BVolCollision<BasicTraits,BVOL>::ResultT
BVolCollision<BasicTraits,BVOL>::PrimPrimCollision 
(AdapterType* p0, AdapterType* p1)
{
   ++m_numPrimPrimTests;
#if defined(GV_BVOLS_IN_MIXEDTESTS)
   Real t = (this->*f_primIntersect)(p0, p1);
   if (t >= 0.0f) {
      // Keep track of colliding pairs
      // check if first collision already found
      if (!getStopFirst() || getNumContacts() == 0) {
	 m_contacts.push_back(CollisionPair(p0, p1));
	 CollisionInterface<OpenSGTriangleBase<BasicTraits>,Real> 
	   result(m_contacts[m_contacts.size()-1]);
	 result.getData() = t;
      }
      CollisionInterface<OpenSGTriangleBase<BasicTraits>,Real> 
	result(m_contacts[0]);
      if (t < result.getData()) {
	 result.getData() = t;
	 m_contacts[0].setFirst(p0);
	 m_contacts[0].setSecond(p1);
      }
   }
#else
   // Transform from model space 1 to model space 0
   PointClass d0, d1, d2;
   m_M1ToM0.mult(p1->getPosition(0), d0);
   m_M1ToM0.mult(p1->getPosition(1), d1);
   m_M1ToM0.mult(p1->getPosition(2), d2);

   // Perform triangle-triangle overlap test
   if (genvis::triTriOverlap(p0->getPosition(0), 
			     p0->getPosition(1), 
			     p0->getPosition(2), 
			     d0, 
			     d1, 
			     d2)) {
      // Keep track of colliding pairs
      m_contacts.push_back(CollisionPair(p0, p1));
   }
#endif
   return DoubleTraverserBase<BasicTraits>::CONTINUE;
}

/*! Inner-leaf test for two bvol trees.
    \param g0 [in] Inner node in first tree
    \param p1 [in] Leaf node in second tree
 */
template <class BasicTraits, class BVOL>
typename BVolCollision<BasicTraits,BVOL>::ResultT
BVolCollision<BasicTraits,BVOL>::BVolPrimCollision 
(GroupType* g0, AdapterType* p1)
{
   ++m_numBVolPrimTests;
   // perform BV-BV overlap test
#if defined(GV_BVOLS_IN_MIXEDTESTS)
   Real t = bvolIntersect(g0->getBVol(), p1->getBVol());

   if (t < 0.0f) {
      return DoubleTraverserBase<BasicTraits>::QUIT;
   }
   const DoubleTraverserFixedBase<BasicTraits>* trav 
     = dynamic_cast<const DoubleTraverserFixedBase<BasicTraits>*>(getTraverser());
   if (trav != NULL
       && trav->getDepth0() == getCurrentDepth0(trav, g0)) {
      // Keep track of colliding pairs
      // check if first collision already found
      if (getNumContacts() == 0) {
	 m_contacts.push_back(CollisionPair((AdapterType*)g0->findLeaf(), p1));
	 CollisionInterface<OpenSGTriangleBase<BasicTraits>,Real> 
	   result(m_contacts[m_contacts.size()-1]);
	 result.getData() = t;
      }
      CollisionInterface<OpenSGTriangleBase<BasicTraits>,Real> 
	result(m_contacts[0]);
      if (t < result.getData()) {
	 result.getData() = t;
	 m_contacts[0].setFirst((AdapterType*)g0->findLeaf());
	 m_contacts[0].setSecond(p1);
      }
   }
#else
   // Transform from model space 1 to model space 0
   PointClass d0, d1, d2;
   m_M1ToM0.mult(p1->getPosition(0), d0);
   m_M1ToM0.mult(p1->getPosition(1), d1);
   m_M1ToM0.mult(p1->getPosition(2), d2);

   if (!g0->getBVol().checkIntersect(d0, d1, d2)) {
      return DoubleTraverserBase<BasicTraits>::QUIT;
   }
#endif
   return DoubleTraverserBase<BasicTraits>::CONTINUE;
}

/*! Inner-leaf test for two bvol trees.
    \param p0 [in] Leaf node in first tree
    \param g1 [in] Inner node in second tree
 */
template <class BasicTraits, class BVOL>
typename BVolCollision<BasicTraits,BVOL>::ResultT
BVolCollision<BasicTraits,BVOL>::PrimBVolCollision 
(AdapterType* p0, GroupType* g1)
{
   ++m_numPrimBVolTests;
   // perform BV-BV overlap test
#if defined(GV_BVOLS_IN_MIXEDTESTS)
   Real t = bvolIntersect(p0->getBVol(), g1->getBVol());

   if (t < 0.0f) {
      return DoubleTraverserBase<BasicTraits>::QUIT;
   }
   const DoubleTraverserFixedBase<BasicTraits>* trav 
     = dynamic_cast<const DoubleTraverserFixedBase<BasicTraits>*>(getTraverser());
   if (trav != NULL 
       && trav->getDepth1() == getCurrentDepth1(trav, g1)) {
      // Keep track of colliding pairs
      // check if first collision already found
      if (getNumContacts() == 0) {
	 m_contacts.push_back(CollisionPair(p0, (AdapterType*)g1->findLeaf()));
	 CollisionInterface<OpenSGTriangleBase<BasicTraits>,Real> 
	   result(m_contacts[m_contacts.size()-1]);
	 result.getData() = t;
      }
      CollisionInterface<OpenSGTriangleBase<BasicTraits>,Real> 
	result(m_contacts[0]);
      if (t < result.getData()) {
	 result.getData() = t;
	 m_contacts[0].setFirst(p0);
	 m_contacts[0].setSecond((AdapterType*)g1->findLeaf());
      }
   }
#else
   // Transform from model space 0 to model space 1
   PointClass d0, d1, d2;
   m_M0ToM1.mult(p0->getPosition(0), d0);
   m_M0ToM1.mult(p0->getPosition(1), d1);
   m_M0ToM1.mult(p0->getPosition(2), d2);

   if (!g1->getBVol().checkIntersect(d0, d1, d2)) {
      return DoubleTraverserBase<BasicTraits>::QUIT;
   }
#endif
   return DoubleTraverserBase<BasicTraits>::CONTINUE;
}


template <class BasicTraits, class BVOL>
typename BVolCollisionTraits<BasicTraits,BVOL>::InitFunctorT 
BVolCollisionTraits<BasicTraits,BVOL>::createInitFunctor (ObjectT* obj) 
{
   typename InitFunctorT::InitMethodT f = &(ObjectT::Init);
   return InitFunctorT(obj, f);
}
template <class BasicTraits, class BVOL>
typename BVolCollisionTraits<BasicTraits,BVOL>::InitDoubleFunctorT 
BVolCollisionTraits<BasicTraits,BVOL>::createInitDoubleFunctor (ObjectT* obj) 
{
   typename InitDoubleFunctorT::InitMethodT f = &(ObjectT::InitDouble);
   return InitDoubleFunctorT(obj, f);
}
template <class BasicTraits, class BVOL>
typename BVolCollisionTraits<BasicTraits,BVOL>::LeaveDoubleFunctorT 
BVolCollisionTraits<BasicTraits,BVOL>::createLeaveDoubleFunctor (ObjectT* obj) 
{
   typename LeaveDoubleFunctorT::InitMethodT f = &(ObjectT::LeaveDouble);
   return LeaveDoubleFunctorT(obj, f);
}
template <class BasicTraits, class BVOL>
typename BVolCollisionTraits<BasicTraits,BVOL>::BVolBVolFunctorT 
BVolCollisionTraits<BasicTraits,BVOL>::createBVolBVolFunctor (ObjectT* obj) 
{
   typename BVolBVolFunctorT::DispatchMethodT f = &(ObjectT::BVolBVolCollision);
   return BVolBVolFunctorT(obj, f);
}
template <class BasicTraits, class BVOL>
typename BVolCollisionTraits<BasicTraits,BVOL>::PrimBVolFunctorT 
BVolCollisionTraits<BasicTraits,BVOL>::createPrimBVolFunctor (ObjectT* obj) 
{
   typename PrimBVolFunctorT::DispatchMethodT f = &(ObjectT::PrimBVolCollision);
   return PrimBVolFunctorT(obj, f);
}
template <class BasicTraits, class BVOL>
typename BVolCollisionTraits<BasicTraits,BVOL>::BVolPrimFunctorT 
BVolCollisionTraits<BasicTraits,BVOL>::createBVolPrimFunctor (ObjectT* obj) 
{
   typename BVolPrimFunctorT::DispatchMethodT f = &(ObjectT::BVolPrimCollision);
   return BVolPrimFunctorT(obj, f);
}
template <class BasicTraits, class BVOL>
typename BVolCollisionTraits<BasicTraits,BVOL>::PrimPrimFunctorT 
BVolCollisionTraits<BasicTraits,BVOL>::createPrimPrimFunctor (ObjectT* obj) 
{
   typename PrimPrimFunctorT::DispatchMethodT f = &(ObjectT::PrimPrimCollision);
   return PrimPrimFunctorT(obj, f);
}

