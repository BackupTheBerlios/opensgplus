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

#include "OSGGVBVolCollision.h"
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
bool BVolCollision<BasicTraits,BVOL>::InitDouble (
GroupType*, const TransformType& m0, 
GroupType*, const TransformType& m1)
{
   // transformation matrix model1 to model0
   // m0^(-1) * m1
   if (!m_M1ToM0.invertFrom(m0)) {
      return false;
   }
   m_M1ToM0.mult(m1);
   // m1^(-1) * m0
#ifndef GV_BVOLS_IN_MIXEDTESTS
   m_M0ToM1.invertFrom(m_M1ToM0);
#endif

   unsigned k;
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
   }
   VectorClass trans (m_M1ToM0[3][0], m_M1ToM0[3][1], m_M1ToM0[3][2]); trans *= 2;
   VectorClass dir0  (m_M1ToM0[0][0], m_M1ToM0[0][1], m_M1ToM0[0][2]);
   VectorClass dir1  (m_M1ToM0[1][0], m_M1ToM0[1][1], m_M1ToM0[1][2]);
   VectorClass dir2  (m_M1ToM0[2][0], m_M1ToM0[2][1], m_M1ToM0[2][2]);
   for (k=0; k<BVOL::Size; ++k) {
      m_proj10[k]   = stdAbs(BVOL::getDirection()[k].dot(dir0));
      m_proj11[k]   = stdAbs(BVOL::getDirection()[k].dot(dir1));
      m_proj12[k]   = stdAbs(BVOL::getDirection()[k].dot(dir2));
      m_M1Offset[k] = BVOL::getDirection()[k].dot(trans);
   }
#ifdef GV_WITH_CLASS2
#ifdef GV_BVOLS_IN_MIXEDTESTS
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
   }
   trans.setValues(m_M0ToM1[3][0], m_M0ToM1[3][1], m_M0ToM1[3][2]); trans *= 2;
   dir0.setValues(m_M0ToM1[0][0], m_M0ToM1[0][1], m_M0ToM1[0][2]);
   dir1.setValues(m_M0ToM1[1][0], m_M0ToM1[1][1], m_M0ToM1[1][2]);
   dir2.setValues(m_M0ToM1[2][0], m_M0ToM1[2][1], m_M0ToM1[2][2]);
   for (k=0; k<BVOL::Size; ++k) {
      m_proj00[k]   = stdAbs(BVOL::getDirection()[k].dot(dir0));
      m_proj01[k]   = stdAbs(BVOL::getDirection()[k].dot(dir1));
      m_proj02[k]   = stdAbs(BVOL::getDirection()[k].dot(dir2));
      m_M0Offset[k] = BVOL::getDirection()[k].dot(trans);
   }
#endif

#if 0
   if (getStream() != NULL) {
      const BVOL& dop0 = g0->getBVol();
      const BVOL& dop1 = g1->getBVol();
      for (k=0; k<BVOL::Size; ++k) {
	 *getStream() << " " 
		      << dop1.difference(0)*stdAbs(m_proj10[k])
	   + dop1.difference(1)*stdAbs(m_proj11[k])
	   + dop1.difference(2)*stdAbs(m_proj12[k])
		      << std::flush;
      }
      *getStream() << std::endl;
   }
#endif

   return true;
}

template<class BasicTraits, class BVOL>
bool BVolCollision<BasicTraits,BVOL>::testIntersect 
(const BVOL& dop0, const BVOL& dop1)
{
   // Class I: dop0 directions
   unsigned k;
   Real min2, max2;
   for (k=0; k<BVOL::Size; ++k) {
      Real projCenter = dop1.addition(0) * m_M1Direction[k][0]
	+ dop1.addition(1) * m_M1Direction[k][1]
	+ dop1.addition(2) * m_M1Direction[k][2];
      Real projRadius = dop1.difference(0) * m_proj10[k]
	+ dop1.difference(1) * m_proj11[k]
	+ dop1.difference(2) * m_proj12[k];
      Real center     = dop0.addition(k);
      Real radius     = dop0.difference(k); 
      min2 = projCenter-projRadius;
      min2 *= 0.5f;
      max2 = projCenter+projRadius;
      max2 *= 0.5f;
      if (stdAbs(projCenter+m_M1Offset[k] - center) > projRadius+radius) { 
	 // is separating axis
	 return false;
      }
   }
#ifdef GV_WITH_CLASS2
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
   return true;
}

/**
 *	Recursive collision query for normal AABB trees.
 *	\param		b0		[in] collision node from first tree
 *	\param		b1		[in] collision node from second tree
 */
template <class BasicTraits, class BVOL>
BVolCollision<BasicTraits,BVOL>::ResultT
BVolCollision<BasicTraits,BVOL>::BVolBVolCollision (GroupType* g0, 
						    GroupType* g1)
{
   // check if first collision already found
   if (getStopFirst() && getNumContacts()>0) {
      return DoubleTraverserBase<BasicTraits>::QUIT;
   }
   ++m_numBVolBVolTests;
   // perform BV-BV overlap test
   if (!testIntersect(g0->getBVol(), g1->getBVol())) {
      return DoubleTraverserBase<BasicTraits>::QUIT;
   }
   return DoubleTraverserBase<BasicTraits>::CONTINUE;
}

/**
 *	Leaf-leaf test for two primitive indices.
 *	\param		p0		[in] index from first leaf-triangle
 *	\param		p1		[in] index from second leaf-triangle
 */
template <class BasicTraits, class BVOL>
BVolCollision<BasicTraits,BVOL>::ResultT
BVolCollision<BasicTraits,BVOL>::PrimPrimCollision (AdapterType* p0, 
						    AdapterType* p1)
{
   // Request vertices from the app   
   // Transform from model space 1 to model space 0
   PointClass d0, d1, d2;
   m_M1ToM0.mult(p1->getPosition(0), d0);
   m_M1ToM0.mult(p1->getPosition(1), d1);
   m_M1ToM0.mult(p1->getPosition(2), d2);

   // Perform triangle-triangle overlap test
   ++m_numPrimPrimTests;
   if (genvis::triTriOverlap(p0->getPosition(0), 
			     p0->getPosition(1), 
			     p0->getPosition(2), 
			     d0, 
			     d1, 
			     d2)) {
      // Keep track of colliding pairs
      m_contacts.push_back(CollisionPair(p0, p1));
   }
   return DoubleTraverserBase<BasicTraits>::CONTINUE;
}

template <class BasicTraits, class BVOL>
BVolCollision<BasicTraits,BVOL>::ResultT
BVolCollision<BasicTraits,BVOL>::BVolPrimCollision (GroupType*   g0, 
						    AdapterType* p1)
{
   // check if first collision already found
   if (getStopFirst() && getNumContacts()>0) {
      return DoubleTraverserBase<BasicTraits>::QUIT;
   }
   ++m_numBVolPrimTests;
   // perform BV-BV overlap test
#ifdef GV_BVOLS_IN_MIXEDTESTS
   if (!testIntersect(g0->getBVol(), p1->getBVol())) {
      return DoubleTraverserBase<BasicTraits>::QUIT;
   }
#else
   // Transform from model space 1 to model space 0
   PointClass d0, d1, d2;
   m_M1ToM0.mult(p1->getPosition(0), d0);
   m_M1ToM0.mult(p1->getPosition(1), d1);
   m_M1ToM0.mult(p1->getPosition(2), d2);

   if (!g0->getBVol().testIntersect(d0, d1, d2)) {
      return DoubleTraverserBase<BasicTraits>::QUIT;
   }
#endif
   return DoubleTraverserBase<BasicTraits>::CONTINUE;
}
template <class BasicTraits, class BVOL>
BVolCollision<BasicTraits,BVOL>::ResultT
BVolCollision<BasicTraits,BVOL>::PrimBVolCollision (AdapterType* p0, 
						    GroupType*   g1)
{
   // check if first collision already found
   if (getStopFirst() && getNumContacts()>0) {
      return DoubleTraverserBase<BasicTraits>::QUIT;
   }
   ++m_numPrimBVolTests;
   // perform BV-BV overlap test
#ifdef GV_BVOLS_IN_MIXEDTESTS
   if (!testIntersect(p0->getBVol(), g1->getBVol())) {
      return DoubleTraverserBase<BasicTraits>::QUIT;
   }
#else
   // Transform from model space 0 to model space 1
   PointClass d0, d1, d2;
   m_M0ToM1.mult(p0->getPosition(0), d0);
   m_M0ToM1.mult(p0->getPosition(1), d1);
   m_M0ToM1.mult(p0->getPosition(2), d2);

   if (!g1->getBVol().testIntersect(d0, d1, d2)) {
      return DoubleTraverserBase<BasicTraits>::QUIT;
   }
#endif
   return DoubleTraverserBase<BasicTraits>::CONTINUE;
}


template <class BasicTraits, class BVOL>
BVolCollisionTraits<BasicTraits,BVOL>::InitFunctorT 
BVolCollisionTraits<BasicTraits,BVOL>::createInitFunctor (ObjectT* obj) 
{
   InitFunctorT::InitMethodT f = &(ObjectT::Init);
   return InitFunctorT(obj, f);
}
template <class BasicTraits, class BVOL>
BVolCollisionTraits<BasicTraits,BVOL>::InitDoubleFunctorT 
BVolCollisionTraits<BasicTraits,BVOL>::createInitDoubleFunctor (ObjectT* obj) 
{
   InitDoubleFunctorT::InitMethodT f = &(ObjectT::InitDouble);
   return InitDoubleFunctorT(obj, f);
}
template <class BasicTraits, class BVOL>
BVolCollisionTraits<BasicTraits,BVOL>::LeaveDoubleFunctorT 
BVolCollisionTraits<BasicTraits,BVOL>::createLeaveDoubleFunctor (ObjectT* obj) 
{
   LeaveDoubleFunctorT::InitMethodT f = &(ObjectT::LeaveDouble);
   return LeaveDoubleFunctorT(obj, f);
}
template <class BasicTraits, class BVOL>
BVolCollisionTraits<BasicTraits,BVOL>::BVolBVolFunctorT 
BVolCollisionTraits<BasicTraits,BVOL>::createBVolBVolFunctor (ObjectT* obj) 
{
   BVolBVolFunctorT::DispatchMethodT f = &(ObjectT::BVolBVolCollision);
   return BVolBVolFunctorT(obj, f);
}
template <class BasicTraits, class BVOL>
BVolCollisionTraits<BasicTraits,BVOL>::PrimBVolFunctorT 
BVolCollisionTraits<BasicTraits,BVOL>::createPrimBVolFunctor (ObjectT* obj) 
{
   PrimBVolFunctorT::DispatchMethodT f = &(ObjectT::PrimBVolCollision);
   return PrimBVolFunctorT(obj, f);
}
template <class BasicTraits, class BVOL>
BVolCollisionTraits<BasicTraits,BVOL>::BVolPrimFunctorT 
BVolCollisionTraits<BasicTraits,BVOL>::createBVolPrimFunctor (ObjectT* obj) 
{
   BVolPrimFunctorT::DispatchMethodT f = &(ObjectT::BVolPrimCollision);
   return BVolPrimFunctorT(obj, f);
}
template <class BasicTraits, class BVOL>
BVolCollisionTraits<BasicTraits,BVOL>::PrimPrimFunctorT 
BVolCollisionTraits<BasicTraits,BVOL>::createPrimPrimFunctor (ObjectT* obj) 
{
   PrimPrimFunctorT::DispatchMethodT f = &(ObjectT::PrimPrimCollision);
   return PrimPrimFunctorT(obj, f);
}

#if 0
#include "OSGGVDoubleTraverser.cpp"

template class OSG_GENVISLIB_DLLMAPPING DoubleTraverser<OpenSGTraits,BVolCollisionTraits<OpenSGTraits,K6Dop> >;
template class OSG_GENVISLIB_DLLMAPPING DoubleTraverser<OpenSGTraits,BVolCollisionTraits<OpenSGTraits,K14Dop> >;
template class OSG_GENVISLIB_DLLMAPPING DoubleTraverser<OpenSGTraits,BVolCollisionTraits<OpenSGTraits,K18Dop> >;
template class OSG_GENVISLIB_DLLMAPPING DoubleTraverser<OpenSGTraits,BVolCollisionTraits<OpenSGTraits,K26Dop> >;
template class OSG_GENVISLIB_DLLMAPPING DoubleTraverser<OpenSGTraits,BVolCollisionTraits<OpenSGTraits,K12Dop> >;
template class OSG_GENVISLIB_DLLMAPPING DoubleTraverser<OpenSGTraits,BVolCollisionTraits<OpenSGTraits,K20Dop> >;

#include "OSGGVDoubleTraverserBinary.cpp"

template class OSG_GENVISLIB_DLLMAPPING DoubleTraverserBinary<OpenSGTraits,BVolCollisionTraits<OpenSGTraits,K6Dop> >;
template class OSG_GENVISLIB_DLLMAPPING DoubleTraverserBinary<OpenSGTraits,BVolCollisionTraits<OpenSGTraits,K14Dop> >;
template class OSG_GENVISLIB_DLLMAPPING DoubleTraverserBinary<OpenSGTraits,BVolCollisionTraits<OpenSGTraits,K18Dop> >;
template class OSG_GENVISLIB_DLLMAPPING DoubleTraverserBinary<OpenSGTraits,BVolCollisionTraits<OpenSGTraits,K26Dop> >;
template class OSG_GENVISLIB_DLLMAPPING DoubleTraverserBinary<OpenSGTraits,BVolCollisionTraits<OpenSGTraits,K12Dop> >;
template class OSG_GENVISLIB_DLLMAPPING DoubleTraverserBinary<OpenSGTraits,BVolCollisionTraits<OpenSGTraits,K20Dop> >;
#endif
