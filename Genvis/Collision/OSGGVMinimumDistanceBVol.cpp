//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.1 $
//   $Date: 2003/09/11 16:20:30 $
//                                                                            
//=============================================================================

#include "OSGGVMinimumDistanceBVol.h"
#include "OSGGVDoubleTraverserFixed.h"
#include "OSGGVReal.h"
#include "OSGGVGroup.h"
#include "OSGGVGeomFunctions.h"
#include "OSGGVBVolFunctions.h"
#include "OSGGVKDopGeometry.h"
USING_GENVIS_NAMESPACE
OSG_USING_NAMESPACE

// get node depth in hierarchy by counting
#undef  GV_COUNT_DEPTH
// distance computation between realigned k-DOPs 
#undef  GV_MINDIST_REALIGNED
// distance computation between 3-dimensional polygons by triangulation
#define GV_MINDIST_WITH_TRIDIST

// explicit template instantiations
template class OSG_GENVISLIB_DLLMAPPING MinimumDistanceBVol<OpenSGTraits,K6Dop,EuclideanMetric>;
template class OSG_GENVISLIB_DLLMAPPING MinimumDistanceBVol<OpenSGTraits,K12Dop,EuclideanMetric>;
template class OSG_GENVISLIB_DLLMAPPING MinimumDistanceBVol<OpenSGTraits,K20Dop,EuclideanMetric>;
template class OSG_GENVISLIB_DLLMAPPING MinimumDistanceBVol<OpenSGTraits,K14Dop,EuclideanMetric>;
template class OSG_GENVISLIB_DLLMAPPING MinimumDistanceBVol<OpenSGTraits,K18Dop,EuclideanMetric>;
template class OSG_GENVISLIB_DLLMAPPING MinimumDistanceBVol<OpenSGTraits,K26Dop,EuclideanMetric>;
template class OSG_GENVISLIB_DLLMAPPING MinimumDistanceBVolTraits<OpenSGTraits,K6Dop,EuclideanMetric>;
template class OSG_GENVISLIB_DLLMAPPING MinimumDistanceBVolTraits<OpenSGTraits,K12Dop,EuclideanMetric>;
template class OSG_GENVISLIB_DLLMAPPING MinimumDistanceBVolTraits<OpenSGTraits,K20Dop,EuclideanMetric>;
template class OSG_GENVISLIB_DLLMAPPING MinimumDistanceBVolTraits<OpenSGTraits,K14Dop,EuclideanMetric>;
template class OSG_GENVISLIB_DLLMAPPING MinimumDistanceBVolTraits<OpenSGTraits,K18Dop,EuclideanMetric>;
template class OSG_GENVISLIB_DLLMAPPING MinimumDistanceBVolTraits<OpenSGTraits,K26Dop,EuclideanMetric>;

template class OSG_GENVISLIB_DLLMAPPING MinimumDistanceBVol<OpenSGTraits,K6Dop,EuclideanMetricHalfspace>;
template class OSG_GENVISLIB_DLLMAPPING MinimumDistanceBVol<OpenSGTraits,K12Dop,EuclideanMetricHalfspace>;
template class OSG_GENVISLIB_DLLMAPPING MinimumDistanceBVol<OpenSGTraits,K20Dop,EuclideanMetricHalfspace>;
template class OSG_GENVISLIB_DLLMAPPING MinimumDistanceBVol<OpenSGTraits,K14Dop,EuclideanMetricHalfspace>;
template class OSG_GENVISLIB_DLLMAPPING MinimumDistanceBVol<OpenSGTraits,K18Dop,EuclideanMetricHalfspace>;
template class OSG_GENVISLIB_DLLMAPPING MinimumDistanceBVol<OpenSGTraits,K26Dop,EuclideanMetricHalfspace>;
template class OSG_GENVISLIB_DLLMAPPING MinimumDistanceBVolTraits<OpenSGTraits,K6Dop,EuclideanMetricHalfspace>;
template class OSG_GENVISLIB_DLLMAPPING MinimumDistanceBVolTraits<OpenSGTraits,K12Dop,EuclideanMetricHalfspace>;
template class OSG_GENVISLIB_DLLMAPPING MinimumDistanceBVolTraits<OpenSGTraits,K20Dop,EuclideanMetricHalfspace>;
template class OSG_GENVISLIB_DLLMAPPING MinimumDistanceBVolTraits<OpenSGTraits,K14Dop,EuclideanMetricHalfspace>;
template class OSG_GENVISLIB_DLLMAPPING MinimumDistanceBVolTraits<OpenSGTraits,K18Dop,EuclideanMetricHalfspace>;
template class OSG_GENVISLIB_DLLMAPPING MinimumDistanceBVolTraits<OpenSGTraits,K26Dop,EuclideanMetricHalfspace>;


// helper functions
template <class REAL>
static unsigned* insertionSortPositive (unsigned* start, unsigned* end,
					const REAL* value)
{
   typedef REAL Real;

   unsigned* it = NULL;
   for (it = start; 
	it != end; 
	++it) {
      unsigned* maxIt = it;
      Real     maxValue = value[*maxIt];
      for (unsigned* it2 = it+1;
	   it2 != end; 
	   ++it2) {  
	 if (value[*it2] > maxValue) {
	    maxIt = it2;
	    maxValue = value[*maxIt];
	 }
      }
      if (it >= start+5 && maxValue < 0) {
 	 break;
      }
      std::swap(*it, *maxIt);
   }
   return it-1;
}
template <class REAL>
static unsigned* insertionSortNegative (unsigned* start, unsigned* end,
					const REAL* value)
{
   typedef REAL Real;

   unsigned* it = NULL;
   for (it = start; 
        it != end; 
	++it) {
      unsigned* minIt = it;
      Real     minValue = value[*minIt];
      for (unsigned* it2 = it+1;
	   it2 != end; 
	   ++it2) {  
	 if (value[*it2] < minValue) {
	    minIt = it2;
	    minValue = value[*minIt];
	 }
      }
      if (it >= start+5 && minValue > 0) {
 	 break;
      }
      std::swap(*it, *minIt);
   }
   return it-1;
}

struct Dop
{
   unsigned index;
   Real     value[13];
};
struct LessMinComp
{
   unsigned k;
   bool operator() (const Dop& a, const Dop& b) const {
      return a.value[k] < b.value[k];
   } 
};
struct LessMaxComp
{
   unsigned k;
   bool operator() (const Dop& a, const Dop& b) const {
      return a.value[k] > b.value[k];
   } 
};


template <class BasicTraits, class BVOL, class Metric>
MinimumDistanceBVol<BasicTraits,BVOL,Metric>::MinimumDistanceBVol ()
  : Inherited()
{
}

template <class BasicTraits, class BVOL, class Metric>
bool MinimumDistanceBVol<BasicTraits,BVOL,Metric>::Init ()
{
   Inherited::Init();
   m_contacts.push_back(CollisionPair(NULL, NULL));
   m_contacts[0].setData(new Distance());
   setDistance(-1);
   m_intersect = false;

#ifdef GV_WITH_LINEGEOMETRY
   if (getLineGeometry() != NullFC) {
      beginEditCP(getLineGeometry());
      for (unsigned k=0; k<getLineGeometry()->size(); ++k) {
	 getLineGeometry()->setValue(Pnt3f::Null, k);
      }
      endEditCP(getLineGeometry());
   }
#endif
   return true;
}

template<class BasicTraits, class BVOL, class Metric>
unsigned MinimumDistanceBVol<BasicTraits,BVOL,Metric>::getCurrentDepth0
(GroupType* adapter) const
{
#ifdef GV_COUNT_DEPTH
   return adapter->getDepth();
#else
   return ((const DoubleTraverserFixedBase<BasicTraits>*)getTraverser())->getCurrentDepth0();
#endif
}
template<class BasicTraits, class BVOL, class Metric>
unsigned MinimumDistanceBVol<BasicTraits,BVOL,Metric>::getCurrentDepth1
(GroupType* adapter) const
{
#ifdef GV_COUNT_DEPTH
   return adapter->getDepth();
#else
   return ((const DoubleTraverserFixedBase<BasicTraits>*)getTraverser())->getCurrentDepth1();
#endif
}


template<class BasicTraits, class BVOL, class Metric>
bool MinimumDistanceBVol<BasicTraits,BVOL,Metric>::LeaveDouble 
(GroupType*, const TransformType&, 
 GroupType*, const TransformType&)
{
   if (m_thisIntersect) {
      unsigned num = m_root1.size();
      m_numPrimPrimTests += num;
      // calc penetration depth
      const BVOL& dop1 = m_root0->getBVol();

      typedef std::vector<Dop>        Root1Dops;
      Root1Dops root1Min; root1Min.reserve(m_root1.size());
      Root1Dops root1Max; root1Max.reserve(m_root1.size());

      unsigned i, k, kk, mink, maxk;
      Real correct;
      typedef std::vector<GroupType*> Root1Childs;
      for (Root1Childs::iterator it=m_root1.begin(); it != m_root1.end(); ++it) {
	 const BVOL& dop2 = (*it)->getBVol();
	 const Real* center = (*it)->getData().getRotationCenter();

	 root1Min.push_back(Dop());
	 root1Max.push_back(Dop());
	 for (k=0, kk=BVOL::Size; k<BVOL::Size; ++k, ++kk) {
	   Real& min2 = root1Min.back().value[k];
	   Real& max2 = root1Max.back().value[k];

	   maxk = m_perm[k];
	   mink = m_perm[kk];
	   
	   if (maxk < BVOL::Size) {
	     max2 =  dop2.maxVector()[maxk] - center[maxk];
	     min2 =  dop2.minVector()[maxk] - center[maxk];
	   } else {
	     max2 = -dop2.minVector()[mink] + center[mink];
	     min2 = -dop2.maxVector()[mink] + center[mink];
	   }
	   correct = m_proj[k].dot((*it)->getCenter());
#ifdef GV_WITH_SUBCONES
	   min2 = min2*BVOL::unitDopAngleTable((*it)->getData().getOccupancy(mink)[m_submask[kk]], 
					       m_mask[kk], 
					       (*it)->getData().getOuterMost(mink)[m_submask[kk]])
	     + correct;
	   max2 = max2*BVOL::unitDopAngleTable((*it)->getData().getOccupancy(maxk)[m_submask[k]], 
					       m_mask[k], 
					       (*it)->getData().getOuterMost(maxk)[m_submask[k]])
	     + correct;
#else
	   min2 = min2*BVOL::unitDopAngleTable((*it)->getData().getOccupancy(mink)[0], 
					       m_mask[kk], 
					       (*it)->getData().getOuterMost(mink)[0])
	     + correct;
	   max2 = max2*BVOL::unitDopAngleTable((*it)->getData().getOccupancy(maxk)[0], 
					       m_mask[k], 
					       (*it)->getData().getOuterMost(maxk)[0])
	     + correct;
#endif
	 }
      }

      Real value1, value2;
      Real value = getMaxDistance();
      mink = 0;
      LessMinComp comp1;
      LessMaxComp comp2;
      PointClass min0, min1;
      for (k=0; k<BVOL::Size; ++k) {
	 comp1.k = comp2.k = k;
	 std::nth_element(root1Min.begin(), root1Min.begin(), root1Min.end(), comp1);
	 value1 = m_root0->getBVol().maxVector()[k]-root1Min[0].value[k]-m_M1Offset[k];
	 //value1 = m_root0->getBVol().maxVector()[k]-m_root1[0]->getBVol().m_min2[k]-m_M1Offset[k];
	 //value2 = m_root0->getBVol().minVector()[k]-m_root1[0]->getBVol().m_max2[k]-m_M1Offset[k];
	 if (value1 > 0) { // disjoint case: dop1 to the right of dop2
	    // !disjoint case: dop1 to the left of dop2
	    //(value1 <= 0 && value2 > 0), (value1 > 0 && value2 <= 0) is impossible!
 	    if (value > value1) {
 	       value = value1;
	       mink  = k;
	       min1.setValues(0.5f*(root1Min[0].value[0]+root1Max[0].value[0])+m_M1Offset[0],
			      0.5f*(root1Min[0].value[1]+root1Max[0].value[1])+m_M1Offset[1],
			      0.5f*(root1Min[0].value[2]+root1Max[0].value[2])+m_M1Offset[2]);
	       min1 += (0.5f*(root1Min[0].value[k]-root1Max[0].value[k]))*BVOL::getDirection()[k];
	    }
	 }

	 std::nth_element(root1Max.begin(), root1Max.begin(), root1Max.end(), comp2);
	 //value1 = m_root0->getBVol().maxVector()[k]-m_root1[0]->getBVol().m_min2[k]-m_M1Offset[k];
	 //value2 = m_root0->getBVol().minVector()[k]-m_root1[0]->getBVol().m_max2[k]-m_M1Offset[k];
	 value2 = m_root0->getBVol().minVector()[k]-root1Max[0].value[k]-m_M1Offset[k];
	 if (value2 <= 0) { // !disjoint case: dop1 to the right of dop2
	    // disjoint case: dop1 to the left of dop2
	    // (value1 <= 0 && value2 > 0), (value1 > 0 && value2 <= 0) is impossible!
 	    if (value > -value2) {
	       value = -value2;
	       mink  = k+BVOL::Size;
	       min1.setValues(0.5f*(root1Min[0].value[0]+root1Max[0].value[0])+m_M1Offset[0],
			      0.5f*(root1Min[0].value[1]+root1Max[0].value[1])+m_M1Offset[1],
			      0.5f*(root1Min[0].value[2]+root1Max[0].value[2])+m_M1Offset[2]);
	       min1 += (0.5f*(root1Max[0].value[k]-root1Min[0].value[k]))*BVOL::getDirection()[k];
	    }
	 }
      }
      if (value > getDistance()) {
	 setDistance(value);
	 if (mink < BVOL::Size) {
	   // max side
	   min0.setValue(min1 + value*BVOL::getDirection()[mink]);
	 } else {
	   // min side
	   mink -= BVOL::Size;
	   min0.setValue(min1 - value*BVOL::getDirection()[mink]);
	 }
	 CollisionInterface<OpenSGTriangleBase<BasicTraits>,Distance> result(m_contacts[0]);
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
      }
   }
   return true;
}

template<class BasicTraits, class BVOL, class Metric>
bool MinimumDistanceBVol<BasicTraits,BVOL,Metric>::InitDouble (
GroupType* g0, const TransformType& m0, 
GroupType* g1, const TransformType& m1)
{
   // transformation matrix model1 to model0
   // m0^(-1) * m1
   if (!m_M1ToM0.invertFrom(m0)) {
      return false;
   }
   m_M1ToM0.mult(m1);
   // m1^(-1) * m0
   m_M0ToM1.invertFrom(m_M1ToM0);
   m_M0.setValue(m0);

   unsigned k, kk;
   for (k=0, kk=BVOL::Size; k<BVOL::Size; ++k, ++kk) {
      m_M1ToM0.mult(BVOL::getDirection()[k], m_M1Direction[k]);
      m_M1Direction[kk].setValue(-m_M1Direction[k]); 
   }

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

   unsigned i;
   Real value, maxValue, sx, sy;
   for (k=0, kk=BVOL::Size; k<BVOL::Size; ++k, ++kk) {
      // calc remapping m_perm
      m_perm[k] = 0;
      maxValue = BVOL::getDirection()[k].dot(m_M1Direction[0]); 
      for (i=1; i<2*BVOL::Size; ++i) {
	 if ((value=BVOL::getDirection()[k].dot(m_M1Direction[i])) > maxValue) {
	    maxValue  = value;
	    m_perm[k] = i;
	 }
      }
      if (m_perm[k] < BVOL::Size) {
	 m_perm[kk] = m_perm[k]+BVOL::Size;
      } else {
	 m_perm[kk] = m_perm[k]-BVOL::Size;
      }

      // mask
      for (m_mask[k]=0; m_mask[k]<OccTableHighestBit; ++m_mask[k]) {
	 if (maxValue >= BVOL::unitDopAngleTable()[m_mask[k]]) {
            break;
	 }
      }
      m_mask[kk] = m_mask[k];
#ifdef GV_WITH_SUBCONES
      // submask
      dirZ = BVOL::getDirection()[k]-maxValue*m_M1Direction[m_perm[k]];
      m_M1ToM0.mult(BVOL::BVolGeometry::getFrameX()[m_perm[k]], dirX);
      //dirX -= dirX.dot(BVOL::getDirection()[k])*m_M1Direction[m_perm[k]];
      //dirX = BVOL::BVolGeometry::getFrameX()[k];
      m_M1ToM0.mult(BVOL::BVolGeometry::getFrameY()[m_perm[k]], dirY);
      //dirY -= dirY.dot(BVOL::getDirection()[k])*m_M1Direction[m_perm[k]];
      //dirY = BVOL::BVolGeometry::getFrameY()[k];
      sx = dirZ.dot(dirX);
      sy = dirZ.dot(dirY);
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

   // intersecting/non-intersecting
   const BVOL& dop1 = g0->getBVol(); m_root0 = g0;
   const BVOL& dop2 = g1->getBVol(); 
   const Real* center = g1->getData().getRotationCenter();

   unsigned mink, maxk;
   Real     correct;
   Real     min2, max2;
   m_thisIntersect = true;
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
	 m_thisIntersect = false;
	 break;
      }
   }
   if (m_thisIntersect) {
      m_intersect = true;
      m_root1.clear();
      unsigned maxLevel = ((const DoubleTraverserFixedBase<BasicTraits>*)getTraverser())->getDepth1();
      m_root1.reserve(1<<maxLevel);
      m_root1.push_back(g1);

      for (unsigned level=0; level<=maxLevel; ++level) {
	 for (unsigned it=0; it<m_root1.size(); ++it) {
	    GroupType* node = m_root1[it];
	    if (node->getSon(0)->isInner() && node->getSon(1)->isInner()) {
	      m_root1[it] = static_cast<GroupType*>(node->getSon(0));
	      m_root1.insert(m_root1.begin()+it, static_cast<GroupType*>(node->getSon(1)));
	      ++it; 
	    }
	 }
      }
   }
   return true;
}


template<class BasicTraits, class BVOL, class Metric>
bool MinimumDistanceBVol<BasicTraits,BVOL,Metric>::calcIntersectPoint 
(PointClass& p,
 Real di, 
 Real dj, 
 Real dk,
 const VectorClass& ni,
 const VectorClass& nj,
 const VectorClass& nk)
{
   static FloatingComparator<Real> comp(1e-6);

   // ni[0]*p[0] + ni[1]*p[1] + ni[2]*p[2] == di
   // nj[0]*p[0] + nj[1]*p[1] + nj[2]*p[2] == dj
   // nk[0]*p[0] + nk[1]*p[1] + nk[2]*p[2] == dk
   Real det = ni[0]*nj[1]*nk[2]
     +ni[1]*nj[2]*nk[0]
     +ni[2]*nj[0]*nk[1]
     -ni[0]*nj[2]*nk[1]
     -ni[1]*nj[0]*nk[2]
     -ni[2]*nj[1]*nk[0];
   if (comp.zero(det)) { // no solution!
      return false;
   }
   det = 1.0f/det;

   p[0] = det*( di*nj[1]*nk[2]
	       +ni[1]*nj[2]*dk
	       +ni[2]*dj*nk[1]
	       -di*nj[2]*nk[1]
	       -ni[1]*dj*nk[2]
	       -ni[2]*nj[1]*dk);
   p[1] = det*( ni[0]*dj*nk[2]
	       +di*nj[2]*nk[0]
	       +ni[2]*nj[0]*dk
	       -ni[0]*nj[2]*dk
	       -di*nj[0]*nk[2]
	       -ni[2]*dj*nk[0]);
   p[2] = det*( ni[0]*nj[1]*dk
	       +ni[1]*dj*nk[0]
	       +di*nj[0]*nk[1]
	       -ni[0]*dj*nk[1]
	       -ni[1]*nj[0]*dk
	       -di*nj[1]*nk[0]);
   return true;
}

template<class BasicTraits, class BVOL, class Metric>
bool MinimumDistanceBVol<BasicTraits,BVOL,Metric>::intersectProjection 
(unsigned imax, const PointClass* p1, 
 unsigned jmax, const PointClass* p2,
 const VectorClass& dirX, 
 const VectorClass& dirY,
 std::vector<unsigned>& enter, std::vector<unsigned>& leave)
{
   static FloatingComparator<Real> comp(1e-4);
   static FloatingComparator<Real> classifier;

   unsigned i, j;
   PointClass2 pp1[7];
   PointClass2 pp2[7];
   PointClass  p;
   // project pp1 into pp2
   for (i=0; i<imax; ++i) {
      p.setValue(p1[i]-p2[0]);
      pp1[i].setValues(dirX.dot(p), dirY.dot(p));
   }
   for (j=0; j<jmax; ++j) {
      p.setValue(p2[j]-p2[0]);
      pp2[j].setValues(dirX.dot(p), dirY.dot(p));
   }

   unsigned start1, end1;
   unsigned start2, end2;
   start1 = 0, end1 = 0;
   for (i=1; i<imax; ++i) {
      if (pp1[i][1] < pp1[start1][1]) {
	 start1 = i;
      }
      if (pp1[i][1] > pp1[end1][1]) {
	 end1 = i;
      }
   }
   start2 = 0, end2 = 0;
   for (j=1; j<jmax; ++j) {
      if (pp2[j][1] < pp2[start2][1]) {
	 start2 = j;
      }
      if (pp2[j][1] > pp2[end2][1]) {
	 end2 = j;
      }
   }
   // bounding volume test for dimension 1
   if (stdMin(pp1[end1][1], pp2[end2][1]) < stdMax(pp1[start1][1], pp2[start2][1])) {
      return false;
   }
   // find lower and upper chain
   // lower: [start1,..,end1]
   // upper: [end1,  ..,start1]
   start1 = 0, end1 = 0;
   for (i=1; i<imax; ++i) {
      if (pp1[i][0] < pp1[start1][0]) {
	 start1 = i;
      }
      if (pp1[i][0] > pp1[end1][0]) {
	 end1 = i;
      }
   }
   start2 = 0, end2 = 0;
   for (j=1; j<jmax; ++j) {
      if (pp2[j][0] < pp2[start2][0]) {
	 start2 = j;
      }
      if (pp2[j][0] > pp2[end2][0]) {
	 end2 = j;
      }
   }
   // bounding volume test for dimension 0
   if (stdMin(pp1[end1][0], pp2[end2][0]) < stdMax(pp1[start1][0], pp2[start2][0])) {
      return false;
   }
   // sweep over pp1/pp2
   // active edges on pp1: pp1[us1]->pp1[ue1], pp1[ls1]->pp1[le1]
   unsigned ue1 = (start1-1)%imax; // upper end
   unsigned le1 = (start1+1)%imax; // lower end
   unsigned us1 = start1; // upper start
   unsigned ls1 = start1; // lower start
   // active edges on pp2: pp2[us2]->pp2[ue2], pp2[ls2]->pp2[le2]
   unsigned ue2 = (start2-1)%jmax; // upper end
   unsigned le2 = (start2+1)%jmax; // lower end
   unsigned us2 = start2; // upper start
   unsigned ls2 = start2; // lower start
   VectorClass2 diff;
   Real value1, value2;
   while ((us1 != ls1) && (us2 != ls2)) {
#if 0
      std::cout << "active edges: dop1=" << le1 << "/" << end1 << ", " << ue1 << "/" << end1
		<< std::endl
		<< "dop2=" << le2 << "/" << end2 << ", " << ue2 << "/" << end2 
		<< std::endl;
#endif
      // find min of active edges
      // min ue1
      if ((le1 == end1 || comp.leq(pp1[ue1][0], pp1[le1][0]))
	  && comp.leq(pp1[ue1][0], pp2[ue2][0])
	  && comp.leq(pp1[ue1][0], pp2[le2][0])) {
	 //std::cout << "min ue1" << std::endl; 

	 diff.setValue(calcNormal(pp1[us1]-pp1[ue1]));
	 value1 = diff.dot(pp2[us2]-pp1[ue1]);
	 value2 = diff.dot(pp2[ue2]-pp1[ue1]);
	 if (classifier.negative0(value1) && classifier.positive0(value2)) { leave.push_back(ue1); } 
	 if (classifier.positive0(value1) && classifier.negative0(value2)) { enter.push_back(ue1); }

	 value1 = diff.dot(pp2[ls2]-pp1[ue1]);
	 value2 = diff.dot(pp2[le2]-pp1[ue1]);
	 if (classifier.negative0(value1) && classifier.positive0(value2)) { enter.push_back(ue1); } 
	 if (classifier.positive0(value1) && classifier.negative0(value2)) { leave.push_back(ue1); }

 	 us1 = ue1;
	 ue1 = (ue1-1)%imax;
	 continue;
      }
      // min le1
      if ((us1 == end1 || comp.leq(pp1[le1][0], pp1[ue1][0]))
	  && comp.leq(pp1[le1][0], pp2[ue2][0])
	  && comp.leq(pp1[le1][0], pp2[le2][0])) {
	 //std::cout << "min le1" << std::endl; 

	 diff.setValue(calcNormal(pp1[le1]-pp1[ls1]));
	 value1 = diff.dot(pp2[us2]-pp1[ls1]);
	 value2 = diff.dot(pp2[ue2]-pp1[ls1]);
	 if (classifier.negative0(value1) && classifier.positive0(value2)) { enter.push_back(le1); } 
	 if (classifier.positive0(value1) && classifier.negative0(value2)) { leave.push_back(le1); }
	 value1 = diff.dot(pp2[ls2]-pp1[ls1]);
	 value2 = diff.dot(pp2[le2]-pp1[ls1]);
	 if (classifier.negative0(value1) && classifier.positive0(value2)) { leave.push_back(le1); } 
	 if (classifier.positive0(value1) && classifier.negative0(value2)) { enter.push_back(le1); }

 	 ls1 = le1;
	 le1 = (le1+1)%imax;
	 continue;
      }
      // min ue2
      if ((ls2 == end2 || comp.leq(pp2[ue2][0], pp2[le2][0]))
	  && comp.leq(pp2[ue2][0], pp1[ue1][0])
	  && comp.leq(pp2[ue2][0], pp1[le1][0])) {
	 //std::cout << "min ue2" << std::endl; 

	 diff.setValue(calcNormal(pp2[us2]-pp2[ue2]));
	 value1 = diff.dot(pp1[us1]-pp2[ue2]);
	 value2 = diff.dot(pp1[ue1]-pp2[ue2]);
	 if (classifier.negative0(value1) && classifier.positive0(value2)) { enter.push_back(ue1); } 
	 if (classifier.positive0(value1) && classifier.negative0(value2)) { leave.push_back(ue1); }
	 value1 = diff.dot(pp1[ls1]-pp2[ue2]);
	 value2 = diff.dot(pp1[le1]-pp2[ue2]);
	 if (classifier.negative0(value1) && classifier.positive0(value2)) { leave.push_back(ls1); } 
	 if (classifier.positive0(value1) && classifier.negative0(value2)) { enter.push_back(ls1); }

	 us2 = ue2;
	 ue2 = (ue2-1)%jmax;
	 continue;
      }
      // min le2
      if ((us2 == end2 || comp.leq(pp2[le2][0], pp2[ue2][0]))
	  && comp.leq(pp2[le2][0], pp1[ue1][0])
	  && comp.leq(pp2[le2][0], pp1[le1][0])) {
	 //std::cout << "min le2" << std::endl; 

	 diff.setValue(calcNormal(pp2[le2]-pp2[ls2]));
	 value1 = diff.dot(pp1[us1]-pp2[ls2]);
	 value2 = diff.dot(pp1[ue1]-pp2[ls2]);
	 if (classifier.negative0(value1) && classifier.positive0(value2)) { enter.push_back(ue1); } 
	 if (classifier.positive0(value1) && classifier.negative0(value2)) { leave.push_back(ue1); }
	 value1 = diff.dot(pp1[ls1]-pp2[ls2]);
	 value2 = diff.dot(pp1[le1]-pp2[ls2]);
	 if (classifier.negative0(value1) && classifier.positive0(value2)) { leave.push_back(ls1); } 
	 if (classifier.positive0(value1) && classifier.negative0(value2)) { enter.push_back(ls1); }

 	 ls2 = le2;
	 le2 = (le2+1)%jmax;
	 continue;
      }
   }
   return (enter.size() > 0);
}
template <class BasicTraits, class BVOL, class Metric>
bool MinimumDistanceBVol<BasicTraits,BVOL,Metric>::intersectProjection 
(unsigned imax, const PointClass* p1,
 unsigned jmax, const PointClass* p2,
 const VectorClass& dirX,
 const VectorClass& dirY,
 const VectorClass& dirZ,
 Real& result,
 PointClass* store1,
 PointClass* store2)
{
   unsigned i, ii, j, jj;
   Real value;
   std::vector<unsigned> enter, leave;
   if (intersectProjection (imax, p1, jmax, p2, 
			    dirX, dirY, 
			    enter, leave)) {
      //std::cout << "enter:" << enter.size() << "/ leave:" << leave.size() << std::endl;
      ii = (enter[0]+1)%imax;
      for (i=ii; i<=leave[0]; i=(i+1)%imax) {
	 value = getMetric()(dirZ, p1[i]-p2[0]);
	 if (value < result) {
	    result = value;
	    updatePoints(p1[i], p1[i]-value*dirZ, store1, store2);
	 }
      }
      i = enter[0];
      for (j=0; j<jmax; ++j) {
	 jj = (j+1)%jmax;
	 edgeEdgeDistance(p1[i], p1[ii], p2[j], p2[jj], result, store1, store2);
      }
      i  = leave[0];
      ii = (leave[0]+1)%imax;
      for (j=0; j<jmax; ++j) {
	 jj = (j+1)%jmax;
	 edgeEdgeDistance(p1[i], p1[ii], p2[j], p2[jj], result, store1, store2);
      }
      if (enter.size () > 1) {
	ii = (enter[1]+1)%imax;
	for (i=ii; i<=leave[1]; i=(i+1)%imax) {
	  value = getMetric()(dirZ, p1[i]-p2[0]);
	  if (value < result) {
	    result = value;
	    updatePoints(p1[i], p1[i]-value*dirZ, store1, store2);
	  }
	}
	i = enter[1];
	for (j=0; j<jmax; ++j) {
	  jj = (j+1)%jmax;
	  edgeEdgeDistance(p1[i], p1[ii], p2[j], p2[jj], result, store1, store2);
	}
	i  = leave[1];
	ii = (leave[1]+1)%imax;
	for (j=0; j<jmax; ++j) {
	  jj = (j+1)%jmax;
	  edgeEdgeDistance(p1[i], p1[ii], p2[j], p2[jj], result, store1, store2);
	}
      }
      return true;
   }
   return false;
}

template <class BasicTraits, class BVOL, class Metric>
void MinimumDistanceBVol<BasicTraits,BVOL,Metric>::edgeEdgeDistance
(const PointClass& s1, 
 const PointClass& e1, 
 const PointClass& s2, 
 const PointClass& e2,
 Real& result,
 PointClass* store1,
 PointClass* store2)
{
   // calc signed distance d of lines
   VectorClass d1(e1 - s1); 
   VectorClass d2(e2 - s2);
   VectorClass n(d1.cross(d2)); 
   //n /= n.length();
   Real d = n.dot(s1) - n.dot(s2);

   // calc parameters of min distance points
   // s1 + a*d1 + d*n == s2 + b*d2
   // a*d1 - b*d2 == s2-s1-d*n
   Real det = d1[0]*(-d2[1]) - d1[1]*(-d2[0]);
   det = 1.0f/det;
   Real a = det*((s2[0]-s1[0]-d*n[0])*(-d2[1]) - (s2[1]-s1[1]-d*n[1])*(-d2[0]));
   Real b = det*(d1[0]*(s2[1]-s1[1]-d*n[1])    - d1[1]*(s2[0]-s1[0]-d*n[0]));

   // clamp parameters to edge
   if (a < 0.0f) { a = 0.0f; }
   if (a > 1.0f) { a = 1.0f; }
   if (b < 0.0f) { b = 0.0f; }
   if (b > 1.0f) { b = 1.0f; }

   // calc distance of min distance points
   PointClass p1(s1+a*d1);
   PointClass p2(s2+b*d2);
   Real value = getMetric()(p1, p2);
   if (value < result) {
      result = value;
      updatePoints(p1, p2, store1, store2);
   }
}
template <class BasicTraits, class BVOL, class Metric>
void MinimumDistanceBVol<BasicTraits,BVOL,Metric>::allEdgesDistance 
(unsigned imax, const PointClass* p1,
 unsigned jmax, const PointClass* p2,
 Real& result,
 PointClass* store1,
 PointClass* store2)
{
   unsigned i, j, ii, jj;
   for (i=0; i<imax; ++i) {
      ii = (i+1)%imax; 
      for (j=0; j<jmax; ++j) {
	jj = (j+1)%jmax;
	edgeEdgeDistance(p1[i], p1[ii], p2[j], p2[jj], result, store1, store2);
      }
   }
}

template <class BasicTraits, class BVOL, class Metric>
void MinimumDistanceBVol<BasicTraits,BVOL,Metric>::edgePolygonDistance 
(const PointClass* p1,
 unsigned jmax, const PointClass* p2,
 const VectorClass& dirZ,
 Real& result,
 PointClass* store1,
 PointClass* store2)
{
   static FloatingComparator<Real> classifier;

   unsigned j, jj;
   Real value;
   VectorClass diff;
   bool in0, in1;
   in0 = true;
   for (j=0; j<jmax; ++j) {
      jj = (j+1)%jmax;
      diff.setValue(calcNormal(p2[jj]-p2[j], dirZ));
      if (classifier.positive(diff.dot(p1[0]-p2[j]))) {
	 in0 = false;
	 break;
      }
   }
   if (in0) {
     value = dirZ.dot(p1[0]-p2[0]);
     if (value < result) {
       result = value;
       updatePoints(p1[0], p1[0]-value*dirZ, store1, store2);
     }
   }
   in1 = true;
   for (j=0; j<jmax; ++j) {
      jj = (j+1)%jmax;
      diff.setValue(calcNormal(p2[jj]-p2[j], dirZ));
      if (classifier.positive(diff.dot(p1[1]-p2[j]))) {
	 in1 = false;
	 break;
      }
   }
   if (in1) {
     value = dirZ.dot(p1[1]-p2[0]);
     if (value < result) {
       result = value;
       updatePoints(p1[1], p1[1]-value*dirZ, store1, store2);
     }
   }
   if (!in0 || !in1) {  
     for (j=0; j<jmax; ++j) {
       jj = (j+1)%jmax;
       edgeEdgeDistance(p1[0], p1[1], p2[j], p2[jj], result, store1, store2);
     }
   }
}

template <class BasicTraits, class BVOL, class Metric>
Real MinimumDistanceBVol<BasicTraits,BVOL,Metric>::bvolDistance (const BVOL& dop1,
								 const BVOL& dop2,
								 const Data& data1,
								 const Data& data2,
								 PointClass* store1,
								 PointClass* store2)
{
   static FloatingComparator<Real> comp(1e-6);

   unsigned   k, kk, mink, maxk;
   const Real* rot = data2.getRotationCenter();
#ifdef GV_MINDIST_REALIGNED
   Real correct;
   Real min2[BVOL::Size];
   Real max2[BVOL::Size];
   for (k=0, kk=BVOL::Size; k<BVOL::Size; ++k, ++kk) {
      maxk = m_perm[k];
      mink = m_perm[kk];
      
      if (maxk < BVOL::Size) {
	 max2[k] =  dop2.maxVector()[maxk] - rot[maxk];
	 min2[k] =  dop2.minVector()[maxk] - rot[maxk];
      } else {
	 max2[k] = -dop2.minVector()[mink] + rot[mink];
	 min2[k] = -dop2.maxVector()[mink] + rot[mink];
      }
      correct = m_proj[k].dot(dop2.getCenter())+m_M1Offset[k];
#ifdef GV_WITH_SUBCONES
      min2[k] = min2[k]*BVOL::unitDopAngleTable(data2.getOccupancy(mink)[m_submask[kk]], 
						m_mask[kk], 
						data2.getOuterMost(mink)[m_submask[kk]])
	+ correct;
      max2[k] = max2[k]*BVOL::unitDopAngleTable(data2.getOccupancy(maxk)[m_submask[k]], 
						m_mask[k], 
						data2.getOuterMost(maxk)[m_submask[k]])
	+ correct;
#else
      min2[k] = min2[k]*BVOL::unitDopAngleTable(data2.getOccupancy(mink)[0], 
						m_mask[kk], 
						data2.getOuterMost(mink)[0])
	+ correct;
      max2[k] = max2[k]*BVOL::unitDopAngleTable(data2.getOccupancy(maxk)[0], 
						m_mask[k], 
						data2.getOuterMost(maxk)[0])
	+ correct;
#endif
   }
#endif

   PointClass center1(data1.getCenter());
#ifdef GV_MINDIST_REALIGNED
   PointClass center2(0.5f*(min2[0]+max2[0]),
		      0.5f*(min2[1]+max2[1]),
		      0.5f*(min2[2]+max2[2]));
#else
   //m_M1ToM0.mult(data2.getCenter(), center2);
   PointClass center2(m_proj[0].dot(data2.getCenter())+m_M1Offset[0],
		      m_proj[1].dot(data2.getCenter())+m_M1Offset[1],
		      m_proj[2].dot(data2.getCenter())+m_M1Offset[2]);
#endif
   VectorClass diff = center2 - center1;

   // find the plane the minimum distance is lying on
   Real d1[2*BVOL::Size];
   Real d2[2*BVOL::Size];
   Real s1[2*BVOL::Size];
   Real s2[2*BVOL::Size];
   unsigned   i1[2*BVOL::Size];
   unsigned   i2[2*BVOL::Size];
   for (k=0; k<BVOL::Size; ++k) {
      i1[k] = i2[k] = k;
      d1[k] = dop1.maxVector()[k];
#ifdef GV_MINDIST_REALIGNED
      d2[k] = max2[k];
#else
      d2[k] = dop2.maxVector()[k]-rot[k]+m_M1Direction[k].dot(center2); 
#endif
      Real d = BVOL::getDirection()[k].dot(diff);
      if (comp.zero(d)) {
	 d = comp.getPrecision();
      }
#ifdef GV_MINDIST_REALIGNED
      Real dd = BVOL::getDirection()[k].dot(diff);
#else
      Real dd = m_M1Direction[k].dot(diff);
#endif
      if (comp.zero(dd)) {
	 dd = comp.getPrecision();
      }
      s1[k] = (-BVOL::getDirection()[k].dot(center1)+d1[k])/d;
#ifdef GV_MINDIST_REALIGNED
      s2[k] = (-BVOL::getDirection()[k].dot(center2)+d2[k])/dd;
#else
      s2[k] = (-m_M1Direction[k].dot(center2)+d2[k])/dd;
#endif
   }
   for (kk=0; kk<BVOL::Size; ++k, ++kk) {
      i1[k] = i2[k] = k;
      d1[k] = -dop1.minVector()[kk];
#ifdef GV_MINDIST_REALIGNED
      d2[k] = -min2[kk];
#else
      d2[k] = -dop2.minVector()[kk]+rot[kk]+m_M1Direction[k].dot(center2);
#endif
      Real d = BVOL::getDirection()[k].dot(diff);
      if (comp.zero(d)) {
	 d = comp.getPrecision();
      }
#ifdef GV_MINDIST_REALIGNED
      Real dd = BVOL::getDirection()[k].dot(diff);
#else
      Real dd = m_M1Direction[k].dot(diff);
#endif
      if (comp.zero(dd)) {
	 dd = comp.getPrecision();
      }
      s1[k] = (-BVOL::getDirection()[k].dot(center1)+d1[k])/d;
#ifdef GV_MINDIST_REALIGNED
      s2[k] = (-BVOL::getDirection()[k].dot(center2)+d2[k])/dd;
#else
      s2[k] = (-m_M1Direction[k].dot(center2)+d2[k])/dd;
#endif
   }

   unsigned* ii1 = NULL;
   unsigned* ii2 = NULL;
   unsigned* ii1End = NULL;
   unsigned* ii2End = NULL;
   ii1End = insertionSortPositive(i1, i1+2*BVOL::Size, s1);
   ii2End = insertionSortNegative(i2, i2+2*BVOL::Size, s2);

   // find 5 smallest scalars for each hull
   // and compute up to binom(4, 2)=6 points for each face
   PointClass p1[7], p2[7];
   unsigned imax = 0;
   for (ii1=ii1End-4; ii1!=ii1End; ++ii1) {
   for (ii2=ii1+1;    ii2!=ii1End; ++ii2) {
      if (calcIntersectPoint(p1[imax], 
			     d1[*ii1End], d1[*ii1], d1[*ii2],
			     BVOL::getDirection()[*ii1End], 
			     BVOL::getDirection()[*ii1], 
			     BVOL::getDirection()[*ii2])) {
#if 0
   if (getLineGeometry() != NullFC) {
      beginEditCP(getLineGeometry());
      PointClass p;
      m_M0.mult(p1[imax], p);
      getLineGeometry()->setValue(p, imax);
      endEditCP(getLineGeometry());
   }
#endif
	 ++imax;
      }
   }
   }
   unsigned jmax = 0;
   for (ii1=ii2End-4; ii1!=ii2End; ++ii1) {
   for (ii2=ii1+1;    ii2!=ii2End; ++ii2) {
#ifdef GV_MINDIST_REALIGNED
      if (calcIntersectPoint(p2[jmax], 
			     d2[*ii2End], d2[*ii1], d2[*ii2],
			     BVOL::getDirection()[*ii2End], 
			     BVOL::getDirection()[*ii1], 
			     BVOL::getDirection()[*ii2])) {
#else
      if (calcIntersectPoint(p2[jmax], 
			     d2[*ii2End], d2[*ii1], d2[*ii2],
			     m_M1Direction[*ii2End], 
			     m_M1Direction[*ii1], 
			     m_M1Direction[*ii2])) {
#endif
#if 0
   if (getLineGeometry() != NullFC) {
      beginEditCP(getLineGeometry());
      PointClass p;
      m_M0.mult(p2[jmax], p);
      getLineGeometry()->setValue(p, 7+jmax);
      endEditCP(getLineGeometry());
   }
#endif
	 ++jmax;
      }
   }
   }

   // determine minimum distance between the two polygons
   unsigned i, j, ii, jj;
   Real value;
   Real result = getMaxDistance();
#ifdef GV_MINDIST_WITH_TRIDIST
   // subdivide into triangles
   if (imax < 3) {
      edgePolygonDistance(p1, jmax, p2, m_M1Direction[*ii2End], result, store1, store2);
   } else if (jmax < 3) {
      edgePolygonDistance(p2, imax, p1, BVOL::getDirection()[*ii1End], result, store2, store1);
   } else {
     PointClass min0, min1;
     PointClass pp1[3];
     PointClass pp2[3];
     pp1[0] = p1[0];
     pp2[0] = p2[0];
     for (i=1; i<imax; ++i) {
       ii = (i+1)%imax; 
       pp1[1].setValue(p1[i]);
       pp1[2].setValue(p1[ii]);
       for (j=0; j<jmax; ++j) {
	 jj = (j+1)%jmax;
	 pp2[1].setValue(p2[j]);
	 pp2[2].setValue(p2[jj]);
	 Real value = genvis::triTriDistance(min0, min1, pp1, pp2);
	 if (value < result) {
	   result = value;
	   updatePoints(min0, min1, store1, store2);
	 }
       }
     }
   }
#else
   // calc distance on polygons directly
   if (imax < 3) {
      edgePolygonDistance(p1, jmax, p2, m_M1Direction[*ii2End], result, store1, store2);
   } else if (jmax < 3) {
      edgePolygonDistance(p2, imax, p1, BVOL::getDirection()[*ii1End], result, store2, store1);
   } else {
      if (intersectProjection (jmax, p2, imax, p1, 
			       BVOL::BVolGeometry::getFrameX()[*ii1End], 
			       BVOL::BVolGeometry::getFrameY()[*ii1End], 
			       BVOL::getDirection()[*ii1End], 
			       result, store2, store1)) {
	 // projection of p2 into p1 is non-empty
	 //std::cout << "project p2 into p1" << std::endl;
      } else {
	VectorClass dirZ, dirX, dirY;
	dirZ.setValue(m_M1Direction[*ii2End]);
	m_M1ToM0.mult(BVOL::BVolGeometry::getFrameX()[*ii2End], dirX);
	m_M1ToM0.mult(BVOL::BVolGeometry::getFrameY()[*ii2End], dirY);
	if (intersectProjection(imax, p1, jmax, p2, 
				dirX, dirY, dirZ, 
				result, store1, store2)) {
	  // projection of p1 into p2 is non-empty
	  //std::cout << "project p1 into p2" << std::endl;
	} else {
	  //std::cout << "all edges" << std::endl;
	  allEdgesDistance(imax, p1, jmax, p2, result, store1, store2);
	}
      }
   }
#endif
   return result;
}
template <class BasicTraits, class BVOL, class Metric>
Real MinimumDistanceBVol<BasicTraits,BVOL,Metric>::bvolDistanceIntersect 
(const BVOL& dop1,  const BVOL& dop2,
 const Data& data1, const Data& data2,
 PointClass* store1, PointClass* store2)
{
   static FloatingComparator<Real> comp(1e-6);

   unsigned   k, kk, mink, maxk;
   const Real* rot = data2.getRotationCenter();
   Real correct;
   Real min2[BVOL::Size];
   Real max2[BVOL::Size];
   for (k=0, kk=BVOL::Size; k<BVOL::Size; ++k, ++kk) {
      maxk = m_perm[k];
      mink = m_perm[kk];
      
      if (maxk < BVOL::Size) {
	 max2[k] =  dop2.maxVector()[maxk] - rot[maxk];
	 min2[k] =  dop2.minVector()[maxk] - rot[maxk];
      } else {
	 max2[k] = -dop2.minVector()[mink] + rot[mink];
	 min2[k] = -dop2.maxVector()[mink] + rot[mink];
      }
      correct = m_proj[k].dot(dop2.getCenter())+m_M1Offset[k];
#ifdef GV_WITH_SUBCONES
      min2[k] = min2[k]*BVOL::unitDopAngleTable(data2.getOccupancy(mink)[m_submask[kk]], 
						m_mask[kk], 
						data2.getOuterMost(mink)[m_submask[kk]])
	+ correct;
      max2[k] = max2[k]*BVOL::unitDopAngleTable(data2.getOccupancy(maxk)[m_submask[k]], 
						m_mask[k], 
						data2.getOuterMost(maxk)[m_submask[k]])
	+ correct;
#else
      min2[k] = min2[k]*BVOL::unitDopAngleTable(data2.getOccupancy(mink)[0], 
						m_mask[kk], 
						data2.getOuterMost(mink)[0])
	+ correct;
      max2[k] = max2[k]*BVOL::unitDopAngleTable(data2.getOccupancy(maxk)[0], 
						m_mask[k], 
						data2.getOuterMost(maxk)[0])
	+ correct;
#endif
   }

   PointClass center1(data1.getCenter());
   PointClass center2(0.5f*(min2[0]+max2[0]),
		      0.5f*(min2[1]+max2[1]),
		      0.5f*(min2[2]+max2[2]));
   VectorClass diff(center2 - center1);

   // find the plane the minimum distance is lying on
   Real d1[2*BVOL::Size];
   Real d2[2*BVOL::Size];
   Real s1[2*BVOL::Size];
   Real s2[2*BVOL::Size];
   unsigned   i1[2*BVOL::Size];
   unsigned   i2[2*BVOL::Size];
   for (k=0; k<BVOL::Size; ++k) {
      i1[k] = i2[k] = k;
      d1[k] = dop1.maxVector()[k];
      d2[k] = max2[k];
      Real d = BVOL::getDirection()[k].dot(diff);
      if (comp.zero(d)) {
	 d = comp.getPrecision();
      }
      Real dd = BVOL::getDirection()[k].dot(diff);
      if (comp.zero(dd)) {
	 dd = comp.getPrecision();
      }
      s1[k] = (-BVOL::getDirection()[k].dot(center1)+d1[k])/d;
      s2[k] = (-BVOL::getDirection()[k].dot(center2)+d2[k])/dd;
   }
   for (kk=0; kk<BVOL::Size; ++k, ++kk) {
      i1[k] = i2[k] = k;
      d1[k] = -dop1.minVector()[kk];
      d2[k] = -min2[kk];
      Real d = BVOL::getDirection()[k].dot(diff);
      if (comp.zero(d)) {
	 d = comp.getPrecision();
      }
      Real dd = BVOL::getDirection()[k].dot(diff);
      if (comp.zero(dd)) {
	 dd = comp.getPrecision();
      }
      s1[k] = (-BVOL::getDirection()[k].dot(center1)+d1[k])/d;
      s2[k] = (-BVOL::getDirection()[k].dot(center2)+d2[k])/dd;
   }

   unsigned* ii1 = NULL;
   unsigned* ii2 = NULL;
   unsigned* ii1End = NULL;
   unsigned* ii2End = NULL;
   ii1End = insertionSortPositive(i1, i1+2*BVOL::Size, s1);
   ii2End = insertionSortNegative(i2, i2+2*BVOL::Size, s2);

   // find 5 smallest scalars for each hull
   // and compute up to binom(4, 2)=6 points for each face
   PointClass p1[7], p2[7];
   unsigned imax = 0;
   for (ii1=ii1End-4; ii1!=ii1End; ++ii1) {
   for (ii2=ii1+1;    ii2!=ii1End; ++ii2) {
      if (calcIntersectPoint(p1[imax], 
			     d1[*ii1End], d1[*ii1], d1[*ii2],
			     BVOL::getDirection()[*ii1End], 
			     BVOL::getDirection()[*ii1], 
			     BVOL::getDirection()[*ii2])) {
#if 0
   if (getLineGeometry() != NullFC) {
      beginEditCP(getLineGeometry());
      PointClass p;
      m_M0.mult(p1[imax], p);
      getLineGeometry()->setValue(p, imax);
      endEditCP(getLineGeometry());
   }
#endif
	 ++imax;
      }
   }
   }
   unsigned jmax = 0;
   for (ii1=ii2End-4; ii1!=ii2End; ++ii1) {
   for (ii2=ii1+1;    ii2!=ii2End; ++ii2) {
#ifdef GV_MINDIST_REALIGNED
      if (calcIntersectPoint(p2[jmax], 
			     d2[*ii2End], d2[*ii1], d2[*ii2],
			     BVOL::getDirection()[*ii2End], 
			     BVOL::getDirection()[*ii1], 
			     BVOL::getDirection()[*ii2])) {
#else
      if (calcIntersectPoint(p2[jmax], 
			     d2[*ii2End], d2[*ii1], d2[*ii2],
			     m_M1Direction[*ii2End], 
			     m_M1Direction[*ii1], 
			     m_M1Direction[*ii2])) {
#endif
#if 0
   if (getLineGeometry() != NullFC) {
      beginEditCP(getLineGeometry());
      PointClass p;
      m_M0.mult(p2[jmax], p);
      getLineGeometry()->setValue(p, 7+jmax);
      endEditCP(getLineGeometry());
   }
#endif
	 ++jmax;
      }
   }
   }

   // determine minimum between the two pointsets
   unsigned i, j, ii, jj;
   Real value;
   Real result = getMaxDistance();
#ifdef GV_MINDIST_WITH_TRIDIST
   // subdivide into triangles
   if (imax < 3) {
      edgePolygonDistance(p1, jmax, p2, m_M1Direction[*ii2End], result, store1, store2);
   } else if (jmax < 3) {
      edgePolygonDistance(p2, imax, p1, BVOL::getDirection()[*ii1End], result, store2, store1);
   } else {
     PointClass min0, min1;
     PointClass pp1[3];
     PointClass pp2[3];
     pp1[0] = p1[0];
     pp2[0] = p2[0];
     for (i=1; i<imax; ++i) {
       ii = (i+1)%imax; 
       pp1[1].setValue(p1[i]);
       pp1[2].setValue(p1[ii]);
       for (j=0; j<jmax; ++j) {
	 jj = (j+1)%jmax;
	 pp2[1].setValue(p2[j]);
	 pp2[2].setValue(p2[jj]);
	 Real value = genvis::triTriDistance(min0, min1, pp1, pp2);
	 if (value < result) {
	   result = value;
	   updatePoints(min0, min1, store1, store2);
	 }
       }
     }
   }
#else
   // calc distance on polygons directly
   if (imax < 3) {
      edgePolygonDistance(p1, jmax, p2, m_M1Direction[*ii2End], result, store1, store2);
   } else if (jmax < 3) {
      edgePolygonDistance(p2, imax, p1, BVOL::getDirection()[*ii1End], result, store2, store1);
   } else {
      if (intersectProjection (jmax, p2, imax, p1, 
			       BVOL::BVolGeometry::getFrameX()[*ii1End], 
			       BVOL::BVolGeometry::getFrameY()[*ii1End], 
			       BVOL::getDirection()[*ii1End], 
			       result, store2, store1)) {
	 // projection of p2 into p1 is non-empty
	 //std::cout << "project p2 into p1" << std::endl;
      } else {
	VectorClass dirZ, dirX, dirY;
	dirZ.setValue(m_M1Direction[*ii2End]);
	m_M1ToM0.mult(BVOL::BVolGeometry::getFrameX()[*ii2End], dirX);
	m_M1ToM0.mult(BVOL::BVolGeometry::getFrameY()[*ii2End], dirY);
	if (intersectProjection(imax, p1, jmax, p2, 
				dirX, dirY, dirZ, 
				result, store1, store2)) {
	  // projection of p1 into p2 is non-empty
	  //std::cout << "project p1 into p2" << std::endl;
	} else {
	  //std::cout << "all edges" << std::endl;
	  allEdgesDistance(imax, p1, jmax, p2, result, store1, store2);
	}
      }
   }
#endif
   return result;
}

/**
 *	Recursive collision query for normal AABB trees.
 *	\param		b0		[in] collision node from first tree
 *	\param		b1		[in] collision node from second tree
 */
template <class BasicTraits, class BVOL, class Metric>
MinimumDistanceBVol<BasicTraits,BVOL,Metric>::ResultT
MinimumDistanceBVol<BasicTraits,BVOL,Metric>::BVolBVolCollision 
(GroupType* g0, GroupType* g1)
{
   // perform BV-BV overlap test
   const BVOL& dop1 = g0->getBVol();
   const BVOL& dop2 = g1->getBVol();
 
   if (m_thisIntersect || isIntersecting()) {
      return DoubleTraverserBase<BasicTraits>::QUIT;
   } else {
      ++m_numBVolBVolTests;
      const Real* center = g1->getData().getRotationCenter();
      unsigned   k, kk, mink, maxk;
      Real correct;
      Real min2, max2;
      // calc separation distance
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
	    if (getDistance() >= 0 && value2 > getDistance()) {
	       return DoubleTraverserBase<BasicTraits>::QUIT;
	    }
	 } else if (value1 < 0 && value2 < 0) { // disjoint case: dop1 to the left of dop2
	    if (getDistance() >= 0 && -value1 > getDistance()) {
	       return DoubleTraverserBase<BasicTraits>::QUIT;
	    }
	 } //else (value1 >= 0 && value2 < 0), (value1 < 0 && value2 >= 0) is impossible!
      }

      if (((const DoubleTraverserFixedBase<BasicTraits>*)getTraverser())->getDepth0() == getCurrentDepth0(g0)
	  && ((const DoubleTraverserFixedBase<BasicTraits>*)getTraverser())->getDepth1() == getCurrentDepth1(g1)) {
	++m_numPrimPrimTests;
	// pair consists of nodes at max depth
	PointClass min0, min1;
	Real value = bvolDistance (g0->getBVol(), g1->getBVol(), 
				   g0->getData(), g1->getData(),
				   &min0, &min1);
	if ((getDistance() < 0 || value < getDistance())) {
	  setDistance(value);
	  CollisionInterface<OpenSGTriangleBase<BasicTraits>,Distance> result(m_contacts[0]);
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
	  // Keep track of colliding pairs
	  assert(getNumContacts() > 0);
	  //m_contacts[0].setFirst (g0); 
	  //m_contacts[0].setSecond(g1);
	}
      }
   }
   return DoubleTraverserBase<BasicTraits>::CONTINUE;
}

/**
 *	Leaf-leaf test for two primitive indices.
 *	\param		p0		[in] index from first leaf-triangle
 *	\param		p1		[in] index from second leaf-triangle
 */
template <class BasicTraits, class BVOL, class Metric>
MinimumDistanceBVol<BasicTraits,BVOL,Metric>::ResultT
MinimumDistanceBVol<BasicTraits,BVOL,Metric>::PrimPrimCollision 
(AdapterType* p0, AdapterType* p1)
{
   // Transform from model space 1 to model space 0
   PointClass tp1[3];
   m_M1ToM0.mult(p1->getPosition(0), tp1[0]);
   m_M1ToM0.mult(p1->getPosition(1), tp1[1]);
   m_M1ToM0.mult(p1->getPosition(2), tp1[2]);

   // perform triangle-triangle distance calculation
   ++m_numPrimPrimTests;
   PointClass min0, min1;
   Real value = genvis::triTriDistance(min0, min1, p0->getPosition(), tp1);
   if ((getDistance() < 0 || value < getDistance())) {
      setDistance(value);
      CollisionInterface<OpenSGTriangleBase<BasicTraits>,Distance> result(m_contacts[0]);
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
      // Keep track of colliding pairs
      assert(getNumContacts() > 0);
      m_contacts[0].setFirst (p0); 
      m_contacts[0].setSecond(p1);
   }
   return DoubleTraverserBase<BasicTraits>::CONTINUE;
}

template <class BasicTraits, class BVOL, class Metric>
MinimumDistanceBVol<BasicTraits,BVOL,Metric>::ResultT
MinimumDistanceBVol<BasicTraits,BVOL,Metric>::BVolPrimCollision 
(GroupType* g0, AdapterType* p1)
{
   // perform BV-BV overlap test
   const BVOL& dop1 = g0->getBVol();
   const BVOL& dop2 = p1->getBVol();

   ++m_numBVolPrimTests;
   const Real* center = p1->getData().getRotationCenter();
   unsigned   k, kk, mink, maxk;
   Real correct;
   Real min2, max2;
   // calc separation distance
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
     if (value1 >= 0 && value2 >= 0) { // disjoint case: dop1 to the right of dop2
       if (getDistance() >= 0 && value2 > getDistance()) {
	  return DoubleTraverserBase<BasicTraits>::QUIT;
       }
     } else if (value1 < 0 && value2 < 0) { // disjoint case: dop1 to the left of dop2
       if (getDistance() >= 0 && -value1 > getDistance()) {
	  return DoubleTraverserBase<BasicTraits>::QUIT;
       }
     } //else (value1 >= 0 && value2 < 0), (value1 < 0 && value2 >= 0) is impossible!
   }

   if (((const DoubleTraverserFixedBase<BasicTraits>*)getTraverser())->getDepth0() == getCurrentDepth0(g0)) {
     ++m_numPrimPrimTests;
     // pair consists of nodes at max depth
     PointClass min0, min1;
     Real value = bvolDistance (g0->getBVol(), p1->getBVol(), 
				g0->getData(), p1->getData(),
				&min0, &min1);
     if ((getDistance() < 0 || value < getDistance())) {
       setDistance(value);
       CollisionInterface<OpenSGTriangleBase<BasicTraits>,Distance> result(m_contacts[0]);
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
       // Keep track of colliding pairs
       assert(getNumContacts() > 0);
       //m_contacts[0].setFirst (g0); 
       //m_contacts[0].setSecond(p1);
     }
   }
   return DoubleTraverserBase<BasicTraits>::CONTINUE;
}
template <class BasicTraits, class BVOL, class Metric>
MinimumDistanceBVol<BasicTraits,BVOL,Metric>::ResultT
MinimumDistanceBVol<BasicTraits,BVOL,Metric>::PrimBVolCollision 
(AdapterType* p0, GroupType*   g1)
{
   // perform BV-BV overlap test
   const BVOL& dop1 = p0->getBVol();
   const BVOL& dop2 = g1->getBVol();

   ++m_numPrimBVolTests;
   const Real* center = g1->getData().getRotationCenter();
   unsigned   k, kk, mink, maxk;
   Real correct;
   Real min2, max2;
   // calc separation distance
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
       if (getDistance() >= 0 && value2 > getDistance()) {
	  return DoubleTraverserBase<BasicTraits>::QUIT;
       }
     } else if (value1 < 0 && value2 < 0) { // disjoint case: dop1 to the left of dop2
       if (getDistance() >= 0 && -value1 > getDistance()) {
	  return DoubleTraverserBase<BasicTraits>::QUIT;
       }
     } //else (value1 >= 0 && value2 < 0), (value1 < 0 && value2 >= 0) is impossible!
   }

   if (((const DoubleTraverserFixedBase<BasicTraits>*)getTraverser())->getDepth1() == getCurrentDepth1(g1)) {
      ++m_numPrimPrimTests;
      // pair consists of nodes at max depth
      PointClass min0, min1;
      Real value = bvolDistance (p0->getBVol(), g1->getBVol(), 
				 p0->getData(), g1->getData(),
				 &min0, &min1);
      if ((getDistance() < 0 || value < getDistance())) {
	 setDistance(value);
	 CollisionInterface<OpenSGTriangleBase<BasicTraits>,Distance> result(m_contacts[0]);
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
	 // Keep track of colliding pairs
	 assert(getNumContacts() > 0);
	 //m_contacts[0].setFirst (p0); 
	 //m_contacts[0].setSecond(g1);
      }
   }
   return DoubleTraverserBase<BasicTraits>::CONTINUE;
}


template <class BasicTraits, class BVOL, class Metric>
MinimumDistanceBVolTraits<BasicTraits,BVOL,Metric>::InitFunctorT 
MinimumDistanceBVolTraits<BasicTraits,BVOL,Metric>::createInitFunctor (ObjectT* obj) 
{
   InitFunctorT::InitMethodT f = &(ObjectT::Init);
   return InitFunctorT(obj, f);
}
template <class BasicTraits, class BVOL, class Metric>
MinimumDistanceBVolTraits<BasicTraits,BVOL,Metric>::InitDoubleFunctorT 
MinimumDistanceBVolTraits<BasicTraits,BVOL,Metric>::createInitDoubleFunctor (ObjectT* obj) 
{
   InitDoubleFunctorT::InitMethodT f = &(ObjectT::InitDouble);
   return InitDoubleFunctorT(obj, f);
}
template <class BasicTraits, class BVOL, class Metric>
MinimumDistanceBVolTraits<BasicTraits,BVOL,Metric>::LeaveDoubleFunctorT 
MinimumDistanceBVolTraits<BasicTraits,BVOL,Metric>::createLeaveDoubleFunctor (ObjectT* obj) 
{
   LeaveDoubleFunctorT::InitMethodT f = &(ObjectT::LeaveDouble);
   return LeaveDoubleFunctorT(obj, f);
}
template <class BasicTraits, class BVOL, class Metric>
MinimumDistanceBVolTraits<BasicTraits,BVOL,Metric>::BVolBVolFunctorT 
MinimumDistanceBVolTraits<BasicTraits,BVOL,Metric>::createBVolBVolFunctor (ObjectT* obj) 
{
   BVolBVolFunctorT::DispatchMethodT f = &(ObjectT::BVolBVolCollision);
   return BVolBVolFunctorT(obj, f);
}
template <class BasicTraits, class BVOL, class Metric>
MinimumDistanceBVolTraits<BasicTraits,BVOL,Metric>::PrimBVolFunctorT 
MinimumDistanceBVolTraits<BasicTraits,BVOL,Metric>::createPrimBVolFunctor (ObjectT* obj) 
{
   PrimBVolFunctorT::DispatchMethodT f = &(ObjectT::PrimBVolCollision);
   return PrimBVolFunctorT(obj, f);
}
template <class BasicTraits, class BVOL, class Metric>
MinimumDistanceBVolTraits<BasicTraits,BVOL,Metric>::BVolPrimFunctorT 
MinimumDistanceBVolTraits<BasicTraits,BVOL,Metric>::createBVolPrimFunctor (ObjectT* obj) 
{
   BVolPrimFunctorT::DispatchMethodT f = &(ObjectT::BVolPrimCollision);
   return BVolPrimFunctorT(obj, f);
}
template <class BasicTraits, class BVOL, class Metric>
MinimumDistanceBVolTraits<BasicTraits,BVOL,Metric>::PrimPrimFunctorT 
MinimumDistanceBVolTraits<BasicTraits,BVOL,Metric>::createPrimPrimFunctor (ObjectT* obj) 
{
   PrimPrimFunctorT::DispatchMethodT f = &(ObjectT::PrimPrimCollision);
   return PrimPrimFunctorT(obj, f);
}

#if 0
// template instantiations
#include "OSGGVDoubleTraverserFixed.cpp"
template class OSG_GENVISLIB_DLLMAPPING DoubleTraverserFixed<OpenSGTraits,MinimumDistanceBVolTraits<OpenSGTraits,K6Dop,EuclideanMetric>, 6, 6>;
template class OSG_GENVISLIB_DLLMAPPING DoubleTraverserFixed<OpenSGTraits,MinimumDistanceBVolTraits<OpenSGTraits,K14Dop,EuclideanMetric>, 6, 6>;
template class OSG_GENVISLIB_DLLMAPPING DoubleTraverserFixed<OpenSGTraits,MinimumDistanceBVolTraits<OpenSGTraits,K18Dop,EuclideanMetric>, 6, 6>;
template class OSG_GENVISLIB_DLLMAPPING DoubleTraverserFixed<OpenSGTraits,MinimumDistanceBVolTraits<OpenSGTraits,K26Dop,EuclideanMetric>, 6, 6>;
template class OSG_GENVISLIB_DLLMAPPING DoubleTraverserFixed<OpenSGTraits,MinimumDistanceBVolTraits<OpenSGTraits,K12Dop,EuclideanMetric>, 6, 6>;
template class OSG_GENVISLIB_DLLMAPPING DoubleTraverserFixed<OpenSGTraits,MinimumDistanceBVolTraits<OpenSGTraits,K20Dop,EuclideanMetric>, 6, 6>;

template class OSG_GENVISLIB_DLLMAPPING DoubleTraverserFixed<OpenSGTraits,MinimumDistanceBVolTraits<OpenSGTraits,K6Dop,EuclideanMetricHalfspace>, 6, 6>;
template class OSG_GENVISLIB_DLLMAPPING DoubleTraverserFixed<OpenSGTraits,MinimumDistanceBVolTraits<OpenSGTraits,K14Dop,EuclideanMetricHalfspace>, 6, 6>;
template class OSG_GENVISLIB_DLLMAPPING DoubleTraverserFixed<OpenSGTraits,MinimumDistanceBVolTraits<OpenSGTraits,K18Dop,EuclideanMetricHalfspace>, 6, 6>;
template class OSG_GENVISLIB_DLLMAPPING DoubleTraverserFixed<OpenSGTraits,MinimumDistanceBVolTraits<OpenSGTraits,K26Dop,EuclideanMetricHalfspace>, 6, 6>;
template class OSG_GENVISLIB_DLLMAPPING DoubleTraverserFixed<OpenSGTraits,MinimumDistanceBVolTraits<OpenSGTraits,K12Dop,EuclideanMetricHalfspace>, 6, 6>;
template class OSG_GENVISLIB_DLLMAPPING DoubleTraverserFixed<OpenSGTraits,MinimumDistanceBVolTraits<OpenSGTraits,K20Dop,EuclideanMetricHalfspace>, 6, 6>;
#endif

