//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.3 $
//   $Date: 2004/12/20 15:57:44 $
//                                                                            
//=============================================================================

#include "OSGGVPrecompAlignCollision.h"
#include "OSGGVReal.h"
#include "OSGGVGroup.h"
#include "OSGGVGeomFunctions.h"
#include "OSGGVBVolFunctions.h"
USING_GENVIS_NAMESPACE
OSG_USING_NAMESPACE

// explicit template instantiation
template class OSG_GENVISLIB_DLLMAPPING PrecomputedAlignCollision<OpenSGTraits,K6Dop>;
template class OSG_GENVISLIB_DLLMAPPING PrecomputedAlignCollision<OpenSGTraits,K12Dop>;
template class OSG_GENVISLIB_DLLMAPPING PrecomputedAlignCollision<OpenSGTraits,K20Dop>;
template class OSG_GENVISLIB_DLLMAPPING PrecomputedAlignCollision<OpenSGTraits,K14Dop>;
template class OSG_GENVISLIB_DLLMAPPING PrecomputedAlignCollision<OpenSGTraits,K18Dop>;
template class OSG_GENVISLIB_DLLMAPPING PrecomputedAlignCollision<OpenSGTraits,K26Dop>;

template class OSG_GENVISLIB_DLLMAPPING PrecomputedAlignCollisionTraits<OpenSGTraits,K6Dop>;
template class OSG_GENVISLIB_DLLMAPPING PrecomputedAlignCollisionTraits<OpenSGTraits,K12Dop>;
template class OSG_GENVISLIB_DLLMAPPING PrecomputedAlignCollisionTraits<OpenSGTraits,K20Dop>;
template class OSG_GENVISLIB_DLLMAPPING PrecomputedAlignCollisionTraits<OpenSGTraits,K14Dop>;
template class OSG_GENVISLIB_DLLMAPPING PrecomputedAlignCollisionTraits<OpenSGTraits,K18Dop>;
template class OSG_GENVISLIB_DLLMAPPING PrecomputedAlignCollisionTraits<OpenSGTraits,K26Dop>;


template <class BasicTraits, class BVOL>
PrecomputedAlignCollision<BasicTraits,BVOL>::PrecomputedAlignCollision ()
  : Inherited()
{
}

template<class BasicTraits, class BVOL>
bool PrecomputedAlignCollision<BasicTraits,BVOL>::InitDouble 
(GroupType* g0, const TransformType& m0, const TransformType& f0, 
 GroupType* g1, const TransformType& m1, const TransformType& f1)
{
   // transformation matrix model1 to model0
   // m0^(-1) * m1
   if (!m_M1ToM0.invertFrom(m0)) {
      return false;
   }
   m_M1ToM0.mult(m1);
   // m1^(-1) * m0
   m_M0ToM1.invertFrom(m_M1ToM0);

   u32 k, kk;
   VectorClass M1Direction[2*BVOL::Size];
   // precompute m_proj[k] = m_M1ToM0.transposed().mult(BVOL::getDirection()[k])
   VectorClass trans(m_M1ToM0[3][0], m_M1ToM0[3][1], m_M1ToM0[3][2]);
   VectorClass dirX (m_M1ToM0[0][0], m_M1ToM0[0][1], m_M1ToM0[0][2]);
   VectorClass dirY (m_M1ToM0[1][0], m_M1ToM0[1][1], m_M1ToM0[1][2]);
   VectorClass dirZ (m_M1ToM0[2][0], m_M1ToM0[2][1], m_M1ToM0[2][2]);
   for (k=0; k<BVOL::Size; ++k) {
      m_proj[k][0]  = BVOL::getDirection()[k].dot(dirX);
      m_proj[k][1]  = BVOL::getDirection()[k].dot(dirY);
      m_proj[k][2]  = BVOL::getDirection()[k].dot(dirZ);
      m_M1Offset[k] = BVOL::getDirection()[k].dot(trans);
   }

#if 0
   for (k=0, kk=BVOL::Size; k<BVOL::Size; ++k, ++kk) {
      m_M1ToM0.mult(BVOL::getDirection()[k], M1Direction[k]);
      M1Direction[kk] = -M1Direction[k]; 
   }
#endif
   m_scale = m_proj[0].length();

   u32 i;
   Real value, maxValue, sx, sy;
   for (k=0, kk=BVOL::Size; k<BVOL::Size; ++k, ++kk) {
      // calc remapping m_perm[k],m_perm[kk]
#if 0
      m_perm[k] = 0;
      maxValue = BVOL::getDirection()[k].dot(M1Direction[0]); 
      for (i=1; i<2*BVOL::Size; ++i) {
	 if ((value=BVOL::getDirection()[k].dot(M1Direction[i])) > maxValue) {
	    maxValue  = value;
	    m_perm[k] = i;
	 }
      }
#else
      m_perm[k] = 0;
      maxValue = m_proj[k].dot(BVOL::getDirection()[0]); 
      for (i=1; i<2*BVOL::Size; ++i) {
	 if ((value=m_proj[k].dot(BVOL::getDirection()[i])) > maxValue) {
	    maxValue  = value;
	    m_perm[k] = i;
	 }
      }
#endif
      if (m_perm[k] < BVOL::Size) {
	 m_perm[kk] = m_perm[k]+BVOL::Size;
      } else {
	 m_perm[kk] = m_perm[k]-BVOL::Size;
      }

      maxValue /= m_scale;
      // calc ring index m_mask[k],m_mask[kk]
      for (m_mask[k]=0; m_mask[k]<OccTableHighestBit; ++m_mask[k]) {
	 if (maxValue >= BVOL::unitDopAngleTable()[m_mask[k]]) {
            break;
	 }
      }
      m_mask[kk] = m_mask[k];
#ifdef GV_WITH_SUBCONES
      // calc ring section index m_submask[k],m_submask[kk]
      dirZ = BVOL::getDirection()[m_perm[k]]-(maxValue*m_scale)*m_proj[k];
      m_M1ToM0.mult(BVOL::BVolGeometry::getFrameX()[m_perm[k]], dirX);
      //dirX -= dirX.dot(BVOL::getDirection()[k])*M1Direction[m_perm[k]];
      //dirX = BVOL::BVolGeometry::getFrameX()[k];
      m_M1ToM0.mult(BVOL::BVolGeometry::getFrameY()[m_perm[k]], dirY);
      //dirY -= dirY.dot(BVOL::getDirection()[k])*M1Direction[m_perm[k]];
      //dirY = BVOL::BVolGeometry::getFrameY()[k];
      sx = dirZ.dot(dirX)/(dirZ.length()*dirX.length());
      sy = dirZ.dot(dirY)/(dirZ.length()*dirY.length());
      //std::cout << "k=" << k << ": (" << sx << ", " << sy << ")" << std::endl;
      m_submask[k] = (sx < 0 ? 2 : 0) 
	+ (sx <  0 && sy >= 0 ? 1 : 0) 
	+ (sx >= 0 && sy <  0 ? 1 : 0);
      assert(m_submask[k] < 4);
      if (m_submask[k] < 2) {
         m_submask[kk] = m_submask[k]+2;
      } else {
         m_submask[kk] = m_submask[k]-2;
      }
      assert(m_submask[kk] < 4);
#endif
   }

#ifdef GV_DEBUG
   GV_stream << "permutation("
   for (i=0; i<2*BVOL::Size; ++i) {
      GV_stream << " " << k << "->" << m_perm[k]
		<< "(" << m_mask[k] << ")";
   }
   GV_stream << ")" << std::endl;
#endif

#if 0
   // statistics
   if (getStream() != NULL) {
      const BVOL& dop2 = g1->getBVol();
      const Real* center = g1->getData().getRotationCenter();

      u32   mink, maxk;
      Real min2, max2, correct;
      for (k=0, kk=BVOL::Size; k<BVOL::Size; ++k, ++kk) {
         maxk = m_perm[k];
         mink = m_perm[kk];

         if (maxk < BVOL::Size) {
	   max2 =  dop2.maxVector()[maxk] - center[maxk];
	   min2 =  dop2.minVector()[maxk] - center[maxk];
	 } else {
	   max2 = -dop2.minVector()[mink] + center[mink];
	   min2 = -dop2.maxVector()[mink] + center[mink];
	 }
	 correct = m_proj[k].dot(g1->getCenter())+m_M1Offset[k];
#ifdef GV_WITH_SUBCONES
	 min2 *= BVOL::unitDopAngleTable(g1->getData().getOccupancy(mink)[m_submask[kk]], 
					 m_mask[kk], 
					 g1->getData().getOuterMost(mink)[m_submask[kk]]);
	 min2 += correct;
	 max2 *= BVOL::unitDopAngleTable(g1->getData().getOccupancy(maxk)[m_submask[k]], 
					 m_mask[k], 
					 g1->getData().getOuterMost(maxk)[m_submask[k]]);
	 max2 += correct;
#else
	 min2 *= BVOL::unitDopAngleTable(g1->getData().getOccupancy(mink)[0], 
					 m_mask[kk], 
					 g1->getData().getOuterMost(mink)[0]);
	 min2 += correct;
	 max2 *= BVOL::unitDopAngleTable(g1->getData().getOccupancy(maxk)[0], 
					 m_mask[k], 
					 g1->getData().getOuterMost(maxk)[0]);
	 min2 += correct;
#endif
	 *getStream() << " " << max2-min2 << std::flush;
      }
      *getStream() << std::endl;
   }
#endif
   return true;
}

template <class BasicTraits, class BVOL>
typename PrecomputedAlignCollision<BasicTraits,BVOL>::ResultT
PrecomputedAlignCollision<BasicTraits,BVOL>::BVolBVolCollision (GroupType* g0, GroupType* g1)
{
   // check if first collision already found
   if (getStopFirst() && getNumContacts()>0) {
      return DoubleTraverserBase<BasicTraits>::QUIT;
   }
   ++m_numBVolBVolTests;
   // perform BV-BV overlap test
   const BVOL& dop1 = g0->getBVol();
   const BVOL& dop2 = g1->getBVol();
   const Real* center = g1->getData().getRotationCenter();

   u32 k, kk, mink, maxk;
   Real     min2, max2, correct;
   for (k=0, kk=BVOL::Size; k<BVOL::Size; ++k, ++kk) {
      maxk = m_perm[k];
      mink = m_perm[kk];

      if (maxk < BVOL::Size) {
	 max2 =  dop2.maxVector()[maxk] - center[maxk];
	 min2 =  dop2.minVector()[maxk] - center[maxk];
      } else {
	 max2 = -dop2.minVector()[mink] + center[mink];
	 min2 = -dop2.maxVector()[mink] + center[mink];
      }
      min2 *= m_scale;
      max2 *= m_scale;
      correct = m_proj[k].dot(g1->getCenter());
#ifdef GV_WITH_SUBCONES
      min2 = min2*BVOL::unitDopAngleTable(g1->getData().getOccupancy(mink)[m_submask[kk]], 
					  m_mask[kk], 
					  g1->getData().getOuterMost(mink)[m_submask[kk]])
	+ correct;
      max2 = max2*BVOL::unitDopAngleTable(g1->getData().getOccupancy(maxk)[m_submask[k]], 
					  m_mask[k], 
					  g1->getData().getOuterMost(maxk)[m_submask[k]])
	+ correct;
#else
      min2 = min2*BVOL::unitDopAngleTable(g1->getData().getOccupancy(mink)[0], 
					  m_mask[kk], 
					  g1->getData().getOuterMost(mink)[0])
	+ correct;
      max2 = max2*BVOL::unitDopAngleTable(g1->getData().getOccupancy(maxk)[0], 
					  m_mask[k], 
					  g1->getData().getOuterMost(maxk)[0])
	+ correct;
#endif
      if (stdMin(dop1.maxVector()[k], max2+m_M1Offset[k]) < stdMax(dop1.minVector()[k], min2+m_M1Offset[k])) {
	 return DoubleTraverserBase<BasicTraits>::QUIT;
      }
   }
   return DoubleTraverserBase<BasicTraits>::CONTINUE;
}

template <class BasicTraits, class BVOL>
typename PrecomputedAlignCollision<BasicTraits,BVOL>::ResultT
PrecomputedAlignCollision<BasicTraits,BVOL>::PrimPrimCollision (AdapterType* p0, AdapterType* p1)
{
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
typename PrecomputedAlignCollision<BasicTraits,BVOL>::ResultT
PrecomputedAlignCollision<BasicTraits,BVOL>::BVolPrimCollision (GroupType* g0, AdapterType* p1)
{
   // check if first collision already found
   if (getStopFirst() && getNumContacts()>0) {
      return DoubleTraverserBase<BasicTraits>::QUIT;
   }
   ++m_numBVolPrimTests;
#if 0
   // perform BV-BV overlap test
   const BVOL& dop1 = g0->getBVol();
   const BVOL& dop2 = p1->getBVol();
   const Real* center = p1->getData().getRotationCenter();

   u32   k, kk, mink, maxk;
   Real       min2, max2, correct;
   for (k=0, kk=BVOL::Size; k<BVOL::Size; ++k, ++kk) {
      maxk = m_perm[k];
      mink = m_perm[kk];

      if (maxk < BVOL::Size) {
	 max2 =  dop2.maxVector()[maxk] - center[maxk];
	 min2 =  dop2.minVector()[maxk] - center[maxk];
      } else {
	 max2 = -dop2.minVector()[mink] + center[mink];
	 min2 = -dop2.maxVector()[mink] + center[mink];
      }
      min2 *= m_scale;
      max2 *= m_scale;
      correct = m_proj[k].dot(p1->getCenter());
#ifdef GV_WITH_SUBCONES
      min2 = min2*BVOL::unitDopAngleTable(p1->getData().getOccupancy(mink)[m_submask[kk]], 
					  m_mask[kk], 
					  p1->getData().getOuterMost(mink)[m_submask[kk]])
	+ correct;
      max2 = max2*BVOL::unitDopAngleTable(p1->getData().getOccupancy(maxk)[m_submask[k]], 
					  m_mask[k], 
					  p1->getData().getOuterMost(maxk)[m_submask[k]])
	+ correct;
#else
      min2 = min2*BVOL::unitDopAngleTable(p1->getData().getOccupancy(mink)[0], 
					  m_mask[kk], 
					  p1->getData().getOuterMost(mink)[0])
	+ correct;
      max2 = max2*BVOL::unitDopAngleTable(p1->getData().getOccupancy(maxk)[0], 
					  m_mask[k], 
					  p1->getData().getOuterMost(maxk)[0])
	+ correct;
#endif
      if (stdMin(dop1.maxVector()[k], max2+m_M1Offset[k]) < stdMax(dop1.minVector()[k], min2+m_M1Offset[k])) {
	 return DoubleTraverserBase<BasicTraits>::QUIT;
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
template <class BasicTraits, class BVOL>
typename PrecomputedAlignCollision<BasicTraits,BVOL>::ResultT
PrecomputedAlignCollision<BasicTraits,BVOL>::PrimBVolCollision (AdapterType* p0, GroupType* g1)
{
   // check if first collision already found
   if (getStopFirst() && getNumContacts()>0) {
      return DoubleTraverserBase<BasicTraits>::QUIT;
   }
   ++m_numPrimBVolTests;
#if 0
   // perform BV-BV overlap test
   const BVOL& dop1 = p0->getBVol();
   const BVOL& dop2 = g1->getBVol();
   const Real* center = g1->getData().getRotationCenter();

   u32 k, kk, mink, maxk;
   Real     min2, max2, correct;
   for (k=0, kk=BVOL::Size; k<BVOL::Size; ++k, ++kk) {
      maxk = m_perm[k];
      mink = m_perm[kk];

      if (maxk < BVOL::Size) {
	 max2 =  dop2.maxVector()[maxk] - center[maxk];
	 min2 =  dop2.minVector()[maxk] - center[maxk];
      } else {
	 max2 = -dop2.minVector()[mink] + center[mink];
	 min2 = -dop2.maxVector()[mink] + center[mink];
      }
      min2 *= m_scale;
      max2 *= m_scale;
      correct = m_proj[k].dot(g1->getCenter());
#ifdef GV_WITH_SUBCONES
      min2 = min2*BVOL::unitDopAngleTable(g1->getData().getOccupancy(mink)[m_submask[kk]], 
					  m_mask[kk], 
					  g1->getData().getOuterMost(mink)[m_submask[kk]])
	+ correct;
      max2 = max2*BVOL::unitDopAngleTable(g1->getData().getOccupancy(maxk)[m_submask[k]], 
					  m_mask[k], 
					  g1->getData().getOuterMost(maxk)[m_submask[k]])
	+ correct;
#else
      min2 = min2*BVOL::unitDopAngleTable(g1->getData().getOccupancy(mink)[0], 
					  m_mask[kk], 
					  g1->getData().getOuterMost(mink)[0])
	+ correct;
      max2 = max2*BVOL::unitDopAngleTable(g1->getData().getOccupancy(maxk)[0], 
					  m_mask[k], 
					  g1->getData().getOuterMost(maxk)[0])
	+ correct;
#endif
      if (stdMin(dop1.maxVector()[k], max2+m_M1Offset[k]) < stdMax(dop1.minVector()[k], min2+m_M1Offset[k])) {
	 return DoubleTraverserBase<BasicTraits>::QUIT;
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
typename PrecomputedAlignCollisionTraits<BasicTraits,BVOL>::InitFunctorT 
PrecomputedAlignCollisionTraits<BasicTraits,BVOL>::createInitFunctor (ObjectT* obj) 
{
   typename InitFunctorT::InitMethodT f = &(ObjectT::Init);
   return InitFunctorT(obj, f);
}
template <class BasicTraits, class BVOL>
typename PrecomputedAlignCollisionTraits<BasicTraits,BVOL>::InitDoubleFunctorT 
PrecomputedAlignCollisionTraits<BasicTraits,BVOL>::createInitDoubleFunctor (ObjectT* obj) 
{
   typename InitDoubleFunctorT::InitMethodT f = &(ObjectT::InitDouble);
   return InitDoubleFunctorT(obj, f);
}
template <class BasicTraits, class BVOL>
typename PrecomputedAlignCollisionTraits<BasicTraits,BVOL>::LeaveDoubleFunctorT 
PrecomputedAlignCollisionTraits<BasicTraits,BVOL>::createLeaveDoubleFunctor (ObjectT* obj) 
{
   typename LeaveDoubleFunctorT::InitMethodT f = &(ObjectT::LeaveDouble);
   return LeaveDoubleFunctorT(obj, f);
}
template <class BasicTraits, class BVOL>
typename PrecomputedAlignCollisionTraits<BasicTraits,BVOL>::BVolBVolFunctorT 
PrecomputedAlignCollisionTraits<BasicTraits,BVOL>::createBVolBVolFunctor (ObjectT* obj) 
{
   typename BVolBVolFunctorT::DispatchMethodT f = &(ObjectT::BVolBVolCollision);
   return BVolBVolFunctorT(obj, f);
}
template <class BasicTraits, class BVOL>
typename PrecomputedAlignCollisionTraits<BasicTraits,BVOL>::PrimBVolFunctorT 
PrecomputedAlignCollisionTraits<BasicTraits,BVOL>::createPrimBVolFunctor (ObjectT* obj) 
{
   typename PrimBVolFunctorT::DispatchMethodT f = &(ObjectT::PrimBVolCollision);
   return PrimBVolFunctorT(obj, f);
}
template <class BasicTraits, class BVOL>
typename PrecomputedAlignCollisionTraits<BasicTraits,BVOL>::BVolPrimFunctorT 
PrecomputedAlignCollisionTraits<BasicTraits,BVOL>::createBVolPrimFunctor (ObjectT* obj) 
{
   typename BVolPrimFunctorT::DispatchMethodT f = &(ObjectT::BVolPrimCollision);
   return BVolPrimFunctorT(obj, f);
}
template <class BasicTraits, class BVOL>
typename PrecomputedAlignCollisionTraits<BasicTraits,BVOL>::PrimPrimFunctorT 
PrecomputedAlignCollisionTraits<BasicTraits,BVOL>::createPrimPrimFunctor (ObjectT* obj) 
{
   typename PrimPrimFunctorT::DispatchMethodT f = &(ObjectT::PrimPrimCollision);
   return PrimPrimFunctorT(obj, f);
}

