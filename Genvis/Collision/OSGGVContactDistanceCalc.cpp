//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.3 $
//   $Date: 2004/03/12 13:18:34 $
//                                                                            
//=============================================================================

#include "OSGGVContactDistanceCalc.h"
#include "OSGGVReal.h"
#include "OSGGVGroup.h"
#include "OSGGVGeomFunctions.h"
#include "OSGGVBVolFunctions.h"
USING_GENVIS_NAMESPACE
OSG_USING_NAMESPACE

// explicit template instantiations
template class OSG_GENVISLIB_DLLMAPPING ContactDistanceCalc<OpenSGTraits,K6Dop,EuclideanMetric>;
template class OSG_GENVISLIB_DLLMAPPING ContactDistanceCalc<OpenSGTraits,K12Dop,EuclideanMetric>;
template class OSG_GENVISLIB_DLLMAPPING ContactDistanceCalc<OpenSGTraits,K20Dop,EuclideanMetric>;
template class OSG_GENVISLIB_DLLMAPPING ContactDistanceCalc<OpenSGTraits,K14Dop,EuclideanMetric>;
template class OSG_GENVISLIB_DLLMAPPING ContactDistanceCalc<OpenSGTraits,K18Dop,EuclideanMetric>;
template class OSG_GENVISLIB_DLLMAPPING ContactDistanceCalc<OpenSGTraits,K26Dop,EuclideanMetric>;

template class OSG_GENVISLIB_DLLMAPPING ContactDistanceCalcTraits<OpenSGTraits,K6Dop,EuclideanMetric>;
template class OSG_GENVISLIB_DLLMAPPING ContactDistanceCalcTraits<OpenSGTraits,K12Dop,EuclideanMetric>;
template class OSG_GENVISLIB_DLLMAPPING ContactDistanceCalcTraits<OpenSGTraits,K20Dop,EuclideanMetric>;
template class OSG_GENVISLIB_DLLMAPPING ContactDistanceCalcTraits<OpenSGTraits,K14Dop,EuclideanMetric>;
template class OSG_GENVISLIB_DLLMAPPING ContactDistanceCalcTraits<OpenSGTraits,K18Dop,EuclideanMetric>;
template class OSG_GENVISLIB_DLLMAPPING ContactDistanceCalcTraits<OpenSGTraits,K26Dop,EuclideanMetric>;


template <class BasicTraits, class BVOL, class Metric>
ContactDistanceCalc<BasicTraits,BVOL,Metric>::ContactDistanceCalc ()
  : Inherited(), m_tolerance(0.2f)
{
}

template <class BasicTraits, class BVOL, class Metric>
bool ContactDistanceCalc<BasicTraits,BVOL,Metric>::Init ()
{
   Inherited::Init();
   setDistance(getMaxDistance());
#ifdef GV_WITH_LINEGEOMETRY
   if (getLineGeometry() != NullFC) {
      beginEditCP(getLineGeometry());
      for (u32 k=0; k<getLineGeometry()->size(); ++k) {
	 getLineGeometry()->setValue(Pnt3f::Null, k);
      }
      endEditCP(getLineGeometry());
   }
#endif
   return true;
}

template<class BasicTraits, class BVOL, class Metric>
bool ContactDistanceCalc<BasicTraits,BVOL,Metric>::InitDouble 
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
   m_M0 = m0;

   u32 k, kk;
   VectorClass trans(m_M1ToM0[3][0], m_M1ToM0[3][1], m_M1ToM0[3][2]);
   VectorClass dirX(m_M1ToM0[0][0], m_M1ToM0[0][1], m_M1ToM0[0][2]);
   VectorClass dirY(m_M1ToM0[1][0], m_M1ToM0[1][1], m_M1ToM0[1][2]);
   VectorClass dirZ(m_M1ToM0[2][0], m_M1ToM0[2][1], m_M1ToM0[2][2]);
   for (k=0; k<BVOL::Size; ++k) {
      m_proj[k][0]  = BVOL::getDirection()[k].dot(dirX);
      m_proj[k][1]  = BVOL::getDirection()[k].dot(dirY);
      m_proj[k][2]  = BVOL::getDirection()[k].dot(dirZ);
      m_M1Offset[k] = BVOL::getDirection()[k].dot(trans);
   }

#if 0
   for (k=0, kk=BVOL::Size; k<BVOL::Size; ++k, ++kk) {
      m_M1ToM0.mult(BVOL::getDirection()[k], m_M1Direction[k]);
      m_M1Direction[kk] = -m_M1Direction[k]; 
   }
#endif
   m_scale = m_proj[0].length();

   u32 i;
   Real value, maxValue, sx, sy;
   for (k=0, kk=BVOL::Size; k<BVOL::Size; ++k, ++kk) {
      // calc remapping m_perm
      m_perm[k] = 0;
      maxValue = m_proj[k].dot(BVOL::getDirection()[0]); 
      for (i=1; i<2*BVOL::Size; ++i) {
	 if ((value=m_proj[k].dot(BVOL::getDirection()[i])) > maxValue) {
	    maxValue  = value;
	    m_perm[k] = i;
	 }
      }
      if (m_perm[k] < BVOL::Size) {
	 m_perm[kk] = m_perm[k]+BVOL::Size;
      } else {
	 m_perm[kk] = m_perm[k]-BVOL::Size;
      }

      maxValue /= m_scale;
      // mask
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
   m_intersect = false;

   return true;
}

template <class BasicTraits, class BVOL, class Metric>
typename ContactDistanceCalc<BasicTraits,BVOL,Metric>::ResultT
ContactDistanceCalc<BasicTraits,BVOL,Metric>::BVolBVolCollision (GroupType* g0, 
								 GroupType* g1)
{
   ++m_numBVolBVolTests;
   // perform BV-BV overlap test
   const BVOL& dop1 = g0->getBVol();
   const BVOL& dop2 = g1->getBVol();
   const Real* center = g1->getData().getRotationCenter();

   u32   k, kk, mink, maxk;
   Real correct;
   Real min2, max2;
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
      Real value1 = dop1.maxVector()[k] - min2 - m_M1Offset[k];
      Real value2 = dop1.minVector()[k] - max2 - m_M1Offset[k];
      if (value1 >= 0 && value2 >= 0) { // disjoint case: dop1 to the right of dop2
	 if (value2 > getTolerance()) {
	    return DoubleTraverserBase<BasicTraits>::QUIT;
	 }
      } else if (value1 < 0 && value2 < 0) { // disjoint case: dop1 to the left of dop2
	 if (-value1 > getTolerance()) {
	    return DoubleTraverserBase<BasicTraits>::QUIT;
	 }
      } //(value1 >= 0 && value2 < 0), (value1 < 0 && value2 >= 0) is impossible!
   }
   return DoubleTraverserBase<BasicTraits>::CONTINUE;
}

template <class BasicTraits, class BVOL, class Metric>
typename ContactDistanceCalc<BasicTraits,BVOL,Metric>::ResultT
ContactDistanceCalc<BasicTraits,BVOL,Metric>::PrimPrimCollision (AdapterType* p0, 
								 AdapterType* p1)
{
   // Transform from model space 1 to model space 0
   PointClass tp1[3];
   m_M1ToM0.mult(p1->getPosition(0), tp1[0]);
   m_M1ToM0.mult(p1->getPosition(1), tp1[1]);
   m_M1ToM0.mult(p1->getPosition(2), tp1[2]);

   // perform triangle-triangle distance calculation
   ++m_numPrimPrimTests;
   PointClass min0, min1;
   Real dist = genvis::triTriDistance(min0, min1, p0->getPosition(), tp1);
   if (dist <= getTolerance()) {
      // update minimum distance
      if (dist < getDistance()) {
	setDistance(dist);
	// keep track of contact pairs
	m_contacts.insert(m_contacts.begin(), CollisionPair(p0, p1));
	//m_contacts[0].setData(new Distance());
	CollisionInterface<OpenSGTriangleBase<BasicTraits>,Distance> result(m_contacts[0]);
	result.getData().distance = dist;
	m_M0.mult(min0, result.getData().p1);
	m_M0.mult(min1, result.getData().p2);
#ifdef GV_WITH_LINEGEOMETRY
	if (getLineGeometry() != NullFC) {
	   beginEditCP(getLineGeometry());
	   getLineGeometry()->setValue(result.getData().p1, 1);
	   getLineGeometry()->setValue(result.getData().p2, 0);
	   endEditCP(getLineGeometry());
	}
#endif
      } else {
	// keep track of contact pairs
	m_contacts.push_back(CollisionPair(p0, p1));
	//m_contacts[m_contacts.size()-1].setData(new Distance());
	CollisionInterface<OpenSGTriangleBase<BasicTraits>,Distance> result(m_contacts[m_contacts.size()-1]);
	result.getData().distance = dist;
      }
   }
   return DoubleTraverserBase<BasicTraits>::CONTINUE;
}

template <class BasicTraits, class BVOL, class Metric>
typename ContactDistanceCalc<BasicTraits,BVOL,Metric>::ResultT
ContactDistanceCalc<BasicTraits,BVOL,Metric>::BVolPrimCollision (GroupType*   g0, 
								 AdapterType* p1)
{
   ++m_numBVolPrimTests;
   // perform BV-BV overlap test
   const BVOL& dop1 = g0->getBVol();
   const BVOL& dop2 = p1->getBVol();
   const Real* center = p1->getData().getRotationCenter();

   u32   k, kk, mink, maxk;
   Real correct;
   Real min2, max2;
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
      Real value1 = dop1.maxVector()[k] - min2 - m_M1Offset[k];
      Real value2 = dop1.minVector()[k] - max2 - m_M1Offset[k];
      if (value1 >= 0 && value2 >= 0) { // disjoint case
	 if (value2 > getTolerance()) {
	    return DoubleTraverserBase<BasicTraits>::QUIT;
	 }
      } else if (value1 < 0 && value2 < 0) { // disjoint case
	 if (-value1 > getTolerance()) {
	    return DoubleTraverserBase<BasicTraits>::QUIT;
	 }
      } //(value1 >= 0 && value2 < 0), (value1 < 0 && value2 >= 0) is impossible!
   }
   return DoubleTraverserBase<BasicTraits>::CONTINUE;
}
template <class BasicTraits, class BVOL, class Metric>
typename ContactDistanceCalc<BasicTraits,BVOL,Metric>::ResultT
ContactDistanceCalc<BasicTraits,BVOL,Metric>::PrimBVolCollision (AdapterType* p0, 
								 GroupType*   g1)
{
   ++m_numPrimBVolTests;
   // perform BV-BV overlap test
   const BVOL& dop1 = p0->getBVol();
   const BVOL& dop2 = g1->getBVol();
   const Real* center = g1->getData().getRotationCenter();

   u32   k, kk, mink, maxk;
   Real correct;
   Real min2, max2;
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
      Real value1 = dop1.maxVector()[k] - min2 - m_M1Offset[k];
      Real value2 = dop1.minVector()[k] - max2 - m_M1Offset[k];
      if (value1 >= 0 && value2 >= 0) { // disjoint case
	 if (value2 > getTolerance()) {
	    return DoubleTraverserBase<BasicTraits>::QUIT;
	 }
      } else if (value1 < 0 && value2 < 0) { // disjoint case
	 if (-value1 > getTolerance()) {
	    return DoubleTraverserBase<BasicTraits>::QUIT;
	 }
      } //(value1 >= 0 && value2 < 0), (value1 < 0 && value2 >= 0) is impossible!
   }
   return DoubleTraverserBase<BasicTraits>::CONTINUE;
}


template <class BasicTraits, class BVOL, class Metric>
typename ContactDistanceCalcTraits<BasicTraits,BVOL,Metric>::InitFunctorT 
ContactDistanceCalcTraits<BasicTraits,BVOL,Metric>::createInitFunctor (ObjectT* obj) 
{
   typename InitFunctorT::InitMethodT f = &(ObjectT::Init);
   return InitFunctorT(obj, f);
}
template <class BasicTraits, class BVOL, class Metric>
typename ContactDistanceCalcTraits<BasicTraits,BVOL,Metric>::InitDoubleFunctorT 
ContactDistanceCalcTraits<BasicTraits,BVOL,Metric>::createInitDoubleFunctor (ObjectT* obj) 
{
   typename InitDoubleFunctorT::InitMethodT f = &(ObjectT::InitDouble);
   return InitDoubleFunctorT(obj, f);
}
template <class BasicTraits, class BVOL, class Metric>
typename ContactDistanceCalcTraits<BasicTraits,BVOL,Metric>::LeaveDoubleFunctorT 
ContactDistanceCalcTraits<BasicTraits,BVOL,Metric>::createLeaveDoubleFunctor (ObjectT* obj) 
{
   typename LeaveDoubleFunctorT::InitMethodT f = &(ObjectT::LeaveDouble);
   return LeaveDoubleFunctorT(obj, f);
}
template <class BasicTraits, class BVOL, class Metric>
typename ContactDistanceCalcTraits<BasicTraits,BVOL,Metric>::BVolBVolFunctorT 
ContactDistanceCalcTraits<BasicTraits,BVOL,Metric>::createBVolBVolFunctor (ObjectT* obj) 
{
   typename BVolBVolFunctorT::DispatchMethodT f = &(ObjectT::BVolBVolCollision);
   return BVolBVolFunctorT(obj, f);
}
template <class BasicTraits, class BVOL, class Metric>
typename ContactDistanceCalcTraits<BasicTraits,BVOL,Metric>::PrimBVolFunctorT 
ContactDistanceCalcTraits<BasicTraits,BVOL,Metric>::createPrimBVolFunctor (ObjectT* obj) 
{
   typename PrimBVolFunctorT::DispatchMethodT f = &(ObjectT::PrimBVolCollision);
   return PrimBVolFunctorT(obj, f);
}
template <class BasicTraits, class BVOL, class Metric>
typename ContactDistanceCalcTraits<BasicTraits,BVOL,Metric>::BVolPrimFunctorT 
ContactDistanceCalcTraits<BasicTraits,BVOL,Metric>::createBVolPrimFunctor (ObjectT* obj) 
{
   typename BVolPrimFunctorT::DispatchMethodT f = &(ObjectT::BVolPrimCollision);
   return BVolPrimFunctorT(obj, f);
}
template <class BasicTraits, class BVOL, class Metric>
typename ContactDistanceCalcTraits<BasicTraits,BVOL,Metric>::PrimPrimFunctorT 
ContactDistanceCalcTraits<BasicTraits,BVOL,Metric>::createPrimPrimFunctor (ObjectT* obj) 
{
   typename PrimPrimFunctorT::DispatchMethodT f = &(ObjectT::PrimPrimCollision);
   return PrimPrimFunctorT(obj, f);
}

