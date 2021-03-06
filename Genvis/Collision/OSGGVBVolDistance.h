//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//                                License                                     
//                                                                            
//   This library is free software; you can redistribute it and/or modify it 
//   under the terms of the GNU Library General Public License as published  
//   by the Free Software Foundation, version 2.                             
//                                                                             
//   This library is distributed in the hope that it will be useful, but       
//   WITHOUT ANY WARRANTY; without even the implied warranty of                
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         
//   Library General Public License for more details.                          
//                                                                            
//   You should have received a copy of the GNU Library General Public         
//   License along with this library; if not, write to the Free Software       
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.3 $
//   $Date: 2004/12/20 15:57:44 $
//                                                                            
//=============================================================================

#ifndef OSGGVBVOLDISTANCE_H
#define OSGGVBVOLDISTANCE_H

#include <assert.h>
#include "OSGGVBVolCollision.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief TO BE DOCUMENTED
 */
struct Distance
{
   Real       distance;
   PointClass p1; 
   PointClass p2; 
};

/*! \brief Euclidean distance between two points.
 */
struct EuclideanMetric
{
   Real operator () (const VectorClass& d) const {
      return d.length();
   }
   Real operator () (const PointClass& a, const PointClass& b) const {
      return a.dist(b);
   }
};
/*! \brief Euclidean distance in a plane.
 */
struct EuclideanMetricPlane
{
   EuclideanMetricPlane ()
   {
      setNormal(Vec3f(0,1,0));
   }
   const VectorClass& getNormal () const {
      return m_normal;
   }
   void               setNormal (const VectorClass& normal) {
      m_normal = normal;
   }

   bool isInPlane (const VectorClass& v) const {
      static FloatingComparator<Real> comp(0.1);
      return comp.zero(m_normal.dot(v));
   }
   Real operator () (const VectorClass& v) const {
#if 0
      if (isInPlane(v)) {
	 return v.length();
      }
      return infinity;
#else
      static const Real s_penalty = 1000.0f;
      Real proj = m_normal.dot(v);
      return (v - proj*m_normal).length() + pow(stdAbs(proj)/v.length(), 2)*s_penalty;
#endif
   }
   Real operator () (const PointClass& a, const PointClass& b) const {
      VectorClass d = b-a;
      return operator()(d);
   }

protected:
   VectorClass m_normal;
};
/*! \brief Euclidean distance in a halfspace.
 */
struct EuclideanMetricHalfspace : public EuclideanMetricPlane
{
   EuclideanMetricHalfspace ()
     : EuclideanMetricPlane()
   {}

   bool isInPlane (const VectorClass& v) const {
      static FloatingComparator<Real> comp(0.1);
      return comp.positive0(m_normal.dot(v));
   }
   Real operator () (const VectorClass& v) const {
      if (isInPlane(v)) {
	 return v.length();
      }
      return infinity;
   }
   Real operator () (const PointClass& a, const PointClass& b) const {
      VectorClass d = b-a;
      return operator()(d);
   }
};


/*! \brief DataBase for pairwise distance calculation, defining the interface for
           computing and storing a distance value.
 */
template <class BasicTraits, 
          class Metric = EuclideanMetric>
class OSG_GENVISLIB_DLLMAPPING BVolDistanceBase 
: public BVolCollisionBase<BasicTraits>
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef BVolCollisionBase<BasicTraits>         Inherited;
   typedef typename Inherited::Cache              Cache;
   typedef typename Inherited::CacheData          CacheData;
   typedef typename Inherited::DoubleTraverser    DoubleTraverser;

   typedef typename Inherited::TransformType      TransformType;
   typedef typename Inherited::AdapterType        AdapterType;
   typedef typename Inherited::CollisionPair      CollisionPair;
   typedef typename Inherited::CollisionContainer CollisionContainer;

   typedef Metric                                 MetricType; 
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   inline BVolDistanceBase ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Member.                                                      */
   /*! \{                                                                 */   
   inline bool       isIntersecting  () const;
   inline void       setIntersecting (bool value=true);
   inline Real      getDistance () const;
   inline void       setDistance (Real dist);
   inline MetricType&       getMetric ();
   inline const MetricType& getMetric () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   static inline Real getMaxDistance ();

protected:
   static Real       s_maxDistance;
   static MetricType s_metric;

   bool              m_intersect;
   Real              m_dist;
};
typedef BVolDistanceBase<OpenSGTraits> OSGBVolDistance;


template <class BasicTraits, class Metric>
inline Real BVolDistanceBase<BasicTraits,Metric>::getMaxDistance ()
{
   return s_maxDistance;
}

template <class BasicTraits, class Metric>
inline Real BVolDistanceBase<BasicTraits,Metric>::getDistance () const
{
   return m_dist;
}
template <class BasicTraits, class Metric>
inline void BVolDistanceBase<BasicTraits,Metric>::setDistance (Real dist)
{
   m_dist = dist;
}

template <class BasicTraits, class Metric>
inline BVolDistanceBase<BasicTraits,Metric>::BVolDistanceBase ()
  : Inherited(), m_intersect(false)
{
}

template <class BasicTraits, class Metric>
inline bool BVolDistanceBase<BasicTraits,Metric>::isIntersecting  () const
{
   return m_intersect;
}
template <class BasicTraits, class Metric>
inline void BVolDistanceBase<BasicTraits,Metric>::setIntersecting (bool value)
{
   m_intersect = value;
}

template <class BasicTraits, class Metric>
inline typename BVolDistanceBase<BasicTraits,Metric>::MetricType& 
BVolDistanceBase<BasicTraits,Metric>::getMetric ()
{
   return s_metric;
}
template <class BasicTraits, class Metric>
inline const typename BVolDistanceBase<BasicTraits,Metric>::MetricType& 
BVolDistanceBase<BasicTraits,Metric>::getMetric () const
{
   return s_metric;
}

END_GENVIS_NAMESPACE

#endif



