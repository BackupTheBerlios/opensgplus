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

#include "OSGGVDynAlignCollision.h"
#include "OSGGVReal.h"
#include "OSGGVGroup.h"
#include "OSGGVGeomFunctions.h"
#include "OSGGVBVolFunctions.h"
USING_GENVIS_NAMESPACE
OSG_USING_NAMESPACE

// explicit template instantiations
template class OSG_GENVISLIB_DLLMAPPING DynamicAlignCollision<OpenSGTraits,K6Dop>;
template class OSG_GENVISLIB_DLLMAPPING DynamicAlignCollision<OpenSGTraits,K14Dop>;
template class OSG_GENVISLIB_DLLMAPPING DynamicAlignCollision<OpenSGTraits,K18Dop>;
template class OSG_GENVISLIB_DLLMAPPING DynamicAlignCollision<OpenSGTraits,K26Dop>;
template class OSG_GENVISLIB_DLLMAPPING DynamicAlignCollision<OpenSGTraits,K12Dop>;
template class OSG_GENVISLIB_DLLMAPPING DynamicAlignCollision<OpenSGTraits,K20Dop>;

template class OSG_GENVISLIB_DLLMAPPING DynamicAlignCollisionTraits<OpenSGTraits,K6Dop>;
template class OSG_GENVISLIB_DLLMAPPING DynamicAlignCollisionTraits<OpenSGTraits,K14Dop>;
template class OSG_GENVISLIB_DLLMAPPING DynamicAlignCollisionTraits<OpenSGTraits,K18Dop>;
template class OSG_GENVISLIB_DLLMAPPING DynamicAlignCollisionTraits<OpenSGTraits,K26Dop>;
template class OSG_GENVISLIB_DLLMAPPING DynamicAlignCollisionTraits<OpenSGTraits,K12Dop>;
template class OSG_GENVISLIB_DLLMAPPING DynamicAlignCollisionTraits<OpenSGTraits,K20Dop>;


template <class BasicTraits, class BVOL>
DynamicAlignCollision<BasicTraits,BVOL>::DynamicAlignCollision ()
  : Inherited()
{
   for (u32 i=0; i<BVOL::Size; ++i) {
      m_support[i][0] = m_support[i][1] = m_support[i][2] = i;
      m_support[i+BVOL::Size][0] = m_support[i+BVOL::Size][1] = m_support[i+BVOL::Size][2] = i+BVOL::Size;
   }
}

template<class BasicTraits, class BVOL>
bool DynamicAlignCollision<BasicTraits,BVOL>::InitDouble (GroupType* g0, const TransformType& m0, const TransformType& f0,  
							  GroupType* g1, const TransformType& m1, const TransformType& f1)
{
   const BVOL& dop2 = g1->getBVol();
   // transformation matrix model1 to model0
   // m0^(-1) * m1
   if (!m_M1ToM0.invertFrom(m0)) {
      return false;
   }
   m_M1ToM0.mult(m1);
   // m1^(-1) * m0
   m_M0ToM1.invertFrom(m_M1ToM0);

   u32 k, kk;
   VectorClass dir[2*BVOL::Size];
   for (k=0, kk=BVOL::Size; k<BVOL::Size; ++k, ++kk) {
      m_M1ToM0.mult(BVOL::getDirection()[k], m_M1Direction[k]);
      m_M1Direction[kk] = -m_M1Direction[k]; 

      m_M0ToM1.mult(BVOL::getDirection()[k], dir[k]);
      dir[kk] = -dir[k]; 
   }
   VectorClass trans(m_M1ToM0[3][0], m_M1ToM0[3][1], m_M1ToM0[3][2]);
   for (k=0, kk=BVOL::Size; k<BVOL::Size; ++k, ++kk) {
      m_M1Translate[k]  = BVOL::getDirection()[k].dot(trans);
      m_M1Translate[kk] = -m_M1Translate[k];
   }

   // do realigning only if sufficiently rotated
   if (   m_M1Direction[0] == BVOL::getDirection()[0]
       && m_M1Direction[1] == BVOL::getDirection()[1]
       && m_M1Direction[2] == BVOL::getDirection()[2]) {
      m_realign = false;
      return true;
   }

   m_realign = true;
   // precomputation for realigning
   // transform directions BVOL::getDirection()[k] with m_M0ToM1 instead
   Polygon3SetIndexed& geom = BVOL::unitDop().getGeometry().getPolygonSet();
   for (k=0; k<geom.size(); ++k) {
      Polygon3EdgeIndexed* maxEdge = NULL;
      float maxValue = -1e10;
      for (Polygon3SetIndexed::Container::iterator itF=geom.faces.begin(); 
	   itF != geom.faces.end(); 
	   ++itF) {
	 Polygon3EdgeIndexed* edge = itF->firstEdge();
	 Polygon3EdgeIndexed* startEdge = edge;
	 float current;
	 for (edge = edge->getNext();
	      edge != startEdge;
	      edge = edge->getNext()) {
	    if ((current = dir[k].dot(edge->getPoint())) > maxValue) {
	       maxValue = current;
	       maxEdge  = edge;
	    }
	 }
      }
      // maxEdge found: walk cw around maxEdge->getPoint()
      m_support[k][0] = maxEdge->face->index;
      maxEdge = maxEdge->getCW();
      m_support[k][1] = maxEdge->face->index;
      maxEdge = maxEdge->getCW();
      m_support[k][2] = maxEdge->face->index;
#ifdef GV_DEBUG
      std::cout << "support " << k << ": (" 
		<< m_support[k][0] << ", "
		<< m_support[k][1] << ", "
		<< m_support[k][2] << "), degree "
		<< maxEdge->size()
		<< std::endl;
#endif
      TransformType dinv(m_M1Direction[m_support[k][0]][0], m_M1Direction[m_support[k][0]][1], m_M1Direction[m_support[k][0]][2], 0,
				  m_M1Direction[m_support[k][1]][0], m_M1Direction[m_support[k][1]][1], m_M1Direction[m_support[k][1]][2], 0,
				  m_M1Direction[m_support[k][2]][0], m_M1Direction[m_support[k][2]][1], m_M1Direction[m_support[k][2]][2], 0,
				  0, 0, 0, 1); 
      //std::cout << "det=" << dinv.det3() << std::endl;
      bool isInvertable = dinv.invert3(); assert(isInvertable);
      dinv.transpose();
      //std::cout << "inv det=" << dinv.det3() << std::endl;
      //std::cout << "det m_M1ToM0=" << m_M1ToM0.det3() << std::endl;
#if 0
      m_direction[k][0] = dinv[0][0] * BVOL::getDirection()[k][0]
	+ dinv[0][1] * BVOL::getDirection()[k][1]
	+ dinv[0][2] * BVOL::getDirection()[k][2];
      m_direction[k][1] = dinv[1][0] * BVOL::getDirection()[k][0]
	+ dinv[1][1] * BVOL::getDirection()[k][1]
	+ dinv[1][2] * BVOL::getDirection()[k][2];
      m_direction[k][2] = dinv[2][0] * BVOL::getDirection()[k][0]
	+ dinv[2][1] * BVOL::getDirection()[k][1]
	+ dinv[2][2] * BVOL::getDirection()[k][2];
#else
      dinv.mult(BVOL::getDirection()[k], m_direction[k]);
#endif
   }

#if 0
   if (getStream() != NULL) {
      const BVOL& dop1 = g0->getBVol();
      const BVOL& dop2 = g1->getBVol();

      VectorClass s;
      u32 k, kk;
      Real min2, max2;
      for (k=0, kk=BVOL::Size; k<BVOL::Size; ++k, ++kk) {
 	 s.setValues(dop2.getScalar(m_support[kk][0]),
		     dop2.getScalar(m_support[kk][1]),
		     dop2.getScalar(m_support[kk][2]));
	 min2 = -m_direction[kk].dot(s)-m_M1Translate[kk];

	 s.setValues(dop2.getScalar(m_support[k][0]),
		     dop2.getScalar(m_support[k][1]),
		     dop2.getScalar(m_support[k][2]));
	 max2 = m_direction[k].dot(s)+m_M1Translate[k];
	 *getStream() << " " << max2-min2 << std::flush;
      }
      *getStream() << std::endl;
   }
#endif

   return true;
}

/* Test for intersection with an Eulerian-transformed kdop
   (see Zachmann). */
template<class BasicTraits, class BVOL>
bool DynamicAlignCollision<BasicTraits,BVOL>::testIntersect (const BVOL& dop1, const BVOL& dop2)
{
   VectorClass s;
   u32 k, kk;
   Real min2, max2;
   for (k=0, kk=BVOL::Size; k<BVOL::Size; ++k, ++kk) {
      s.setValues(dop2.getScalar(m_support[kk][0]),
		  dop2.getScalar(m_support[kk][1]),
		  dop2.getScalar(m_support[kk][2]));
      min2 = -m_direction[kk].dot(s);

      s.setValues(dop2.getScalar(m_support[k][0]),
		  dop2.getScalar(m_support[k][1]),
		  dop2.getScalar(m_support[k][2]));
      max2 = m_direction[k].dot(s);

      if (stdMin(dop1.maxVector()[k], max2+m_M1Translate[k]) 
	  < stdMax(dop1.minVector()[k], min2-m_M1Translate[kk])) {
	 return false;
      }
   }
   return true;
}

template <class BasicTraits, class BVOL>
typename DynamicAlignCollision<BasicTraits,BVOL>::ResultT
DynamicAlignCollision<BasicTraits,BVOL>::BVolBVolCollision (GroupType* g0, GroupType* g1)
{
   // check if first collision already found
   if (getStopFirst() && getNumContacts()>0) {
      return DoubleTraverserBase<BasicTraits>::QUIT;
   }
   ++m_numBVolBVolTests;
   // perform BV-BV overlap test
   if (!m_realign) {
      if (!g0->getBVol().checkIntersect(
		    g1->getBVol(), 
		    m_M1Translate)) {
	 return DoubleTraverserBase<BasicTraits>::QUIT;
      }
      return DoubleTraverserBase<BasicTraits>::CONTINUE;
   }
   if (!testIntersect(g0->getBVol(), g1->getBVol())) {
      return DoubleTraverserBase<BasicTraits>::QUIT;
   }
   return DoubleTraverserBase<BasicTraits>::CONTINUE;
}

template <class BasicTraits, class BVOL>
typename DynamicAlignCollision<BasicTraits,BVOL>::ResultT
DynamicAlignCollision<BasicTraits,BVOL>::PrimPrimCollision (AdapterType* p0, AdapterType* p1)
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
typename DynamicAlignCollision<BasicTraits,BVOL>::ResultT
DynamicAlignCollision<BasicTraits,BVOL>::BVolPrimCollision (GroupType* g0, AdapterType* p1)
{
   // check if first collision already found
   if (getStopFirst() && getNumContacts()>0) {
      return DoubleTraverserBase<BasicTraits>::QUIT;
   }
   ++m_numBVolPrimTests;
   // Transform from model space 1 to model space 0
   OSG::Pnt3f d0, d1, d2;
   m_M1ToM0.mult(p1->getPosition(0), d0);
   m_M1ToM0.mult(p1->getPosition(1), d1);
   m_M1ToM0.mult(p1->getPosition(2), d2);

   if (!g0->getBVol().checkIntersect(d0, d1, d2)) {
      return DoubleTraverserBase<BasicTraits>::QUIT;
   }
   return DoubleTraverserBase<BasicTraits>::CONTINUE;
}
template <class BasicTraits, class BVOL>
typename DynamicAlignCollision<BasicTraits,BVOL>::ResultT
DynamicAlignCollision<BasicTraits,BVOL>::PrimBVolCollision (AdapterType* p0, GroupType*   g1)
{
   // check if first collision already found
   if (getStopFirst() && getNumContacts()>0) {
      return DoubleTraverserBase<BasicTraits>::QUIT;
   }
   ++m_numPrimBVolTests;
   // Transform from model space 0 to model space 1
   OSG::Pnt3f d0, d1, d2;
   m_M0ToM1.mult(p0->getPosition(0), d0);
   m_M0ToM1.mult(p0->getPosition(1), d1);
   m_M0ToM1.mult(p0->getPosition(2), d2);

   if (!g1->getBVol().checkIntersect(d0, d1, d2)) {
      return DoubleTraverserBase<BasicTraits>::QUIT;
   }
   return DoubleTraverserBase<BasicTraits>::CONTINUE;
}

template <class BasicTraits, class BVOL>
typename DynamicAlignCollisionTraits<BasicTraits,BVOL>::InitFunctorT 
DynamicAlignCollisionTraits<BasicTraits,BVOL>::createInitFunctor (ObjectT* obj) 
{
   typename InitFunctorT::InitMethodT f = &(ObjectT::Init);
   return InitFunctorT(obj, f);
}
template <class BasicTraits, class BVOL>
typename DynamicAlignCollisionTraits<BasicTraits,BVOL>::InitDoubleFunctorT 
DynamicAlignCollisionTraits<BasicTraits,BVOL>::createInitDoubleFunctor (ObjectT* obj) 
{
   typename InitDoubleFunctorT::InitMethodT f = &(ObjectT::InitDouble);
   return InitDoubleFunctorT(obj, f);
}
template <class BasicTraits, class BVOL>
typename DynamicAlignCollisionTraits<BasicTraits,BVOL>::LeaveDoubleFunctorT 
DynamicAlignCollisionTraits<BasicTraits,BVOL>::createLeaveDoubleFunctor (ObjectT* obj) 
{
   typename LeaveDoubleFunctorT::InitMethodT f = &(ObjectT::LeaveDouble);
   return LeaveDoubleFunctorT(obj, f);
}
template <class BasicTraits, class BVOL>
typename DynamicAlignCollisionTraits<BasicTraits,BVOL>::BVolBVolFunctorT 
DynamicAlignCollisionTraits<BasicTraits,BVOL>::createBVolBVolFunctor (ObjectT* obj) 
{
   typename BVolBVolFunctorT::DispatchMethodT f = &(ObjectT::BVolBVolCollision);
   return BVolBVolFunctorT(obj, f);
}
template <class BasicTraits, class BVOL>
typename DynamicAlignCollisionTraits<BasicTraits,BVOL>::PrimBVolFunctorT 
DynamicAlignCollisionTraits<BasicTraits,BVOL>::createPrimBVolFunctor (ObjectT* obj) 
{
   typename PrimBVolFunctorT::DispatchMethodT f = &(ObjectT::PrimBVolCollision);
   return PrimBVolFunctorT(obj, f);
}
template <class BasicTraits, class BVOL>
typename DynamicAlignCollisionTraits<BasicTraits,BVOL>::BVolPrimFunctorT 
DynamicAlignCollisionTraits<BasicTraits,BVOL>::createBVolPrimFunctor (ObjectT* obj) 
{
   typename BVolPrimFunctorT::DispatchMethodT f = &(ObjectT::BVolPrimCollision);
   return BVolPrimFunctorT(obj, f);
}
template <class BasicTraits, class BVOL>
typename DynamicAlignCollisionTraits<BasicTraits,BVOL>::PrimPrimFunctorT 
DynamicAlignCollisionTraits<BasicTraits,BVOL>::createPrimPrimFunctor (ObjectT* obj) 
{
   typename PrimPrimFunctorT::DispatchMethodT f = &(ObjectT::PrimPrimCollision);
   return PrimPrimFunctorT(obj, f);
}

