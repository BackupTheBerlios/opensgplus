// ---------------------------------------------------------------
// File:    $Id: OSGGVBVolFunctions.h,v 1.2 2004/03/12 13:23:23 fuenfzig Exp $
// Author:  Christoph Fuenfzig, <c.fuenfzig@cg.cs.tu-bs.de>
// ---------------------------------------------------------------
// bounding volume union functions
// bounding volume intersection-test functions

#ifndef OSGGVBVOLFUNCTIONS_H
#define OSGGVBVOLFUNCTIONS_H

#include "OSGGVBase.h"
#include "OSGGVBoundingVolume.h"
#include "OSGGVKDop.h"
#include "OSGGVKDopGeometry.h"
BEGIN_GENVIS_NAMESPACE

template <class REAL>
inline void setValueTransposed (Matrix& m,
				const VectorInterface<REAL, VecStorage3<REAL> >& c0,
				const VectorInterface<REAL, VecStorage3<REAL> >& c1,
				const VectorInterface<REAL, VecStorage3<REAL> >& c2)
{
   m[0][0] = c0[0]; m[0][1] = c1[0]; m[0][2] = c2[0]; 
   m[1][0] = c0[1]; m[1][1] = c1[1]; m[1][2] = c2[1]; 
   m[2][0] = c0[2]; m[2][1] = c1[2]; m[2][2] = c2[2]; 
}

/** (Closed)Interval test q in [a, b]. */
inline bool checkIntersect (float q, float a, float b)
{
   static FloatingComparator<float> comp;
   return (comp.leq(a, q) && comp.leq(q, b));
}
/** (Closed)Interval test q in [a, b]. */
inline bool checkIntersect (double q, double a, double b)
{
   static FloatingComparator<double> comp;
   return (comp.leq(a, q) && comp.leq(q, b));
}

/** Copy dop bvol2 to bvol1 (with possibly different k). */
template <class REAL, int SIZE1, int SIZE2>
inline void copy (KDop<REAL,SIZE1>&       bvol1,
		  const KDop<REAL,SIZE2>& bvol2)
{
   bvol1.minVector()[0] = bvol2.minVector()[0];
   bvol1.minVector()[1] = bvol2.minVector()[1];
   bvol1.minVector()[2] = bvol2.minVector()[2];
   bvol1.maxVector()[0] = bvol2.maxVector()[0];
   bvol1.maxVector()[1] = bvol2.maxVector()[1];
   bvol1.maxVector()[2] = bvol2.maxVector()[2];
   for (int k=3; k<SIZE1; ++k) {
      bvol1.minVector()[k] = bvol1.minVector()[0]*bvol1.direction()[k][0]
	+ bvol1.minVector()[1]*bvol1.direction()[k][1]
	+ bvol1.minVector()[2]*bvol1.direction()[k][2];
      bvol1.maxVector()[k] = bvol1.maxVector()[0]*bvol1.direction()[k][0]
	+ bvol1.maxVector()[1]*bvol1.direction()[k][1]
	+ bvol1.maxVector()[2]*bvol1.direction()[k][2];
   }
   bvol1.setEmpty(bvol2.isEmpty());
   bvol1.update();
}

/** Unify two k-dops (with same k) bvol1 and bvol2. */
template <class REAL, int SIZE>
inline void unify (KDop<REAL,SIZE>&       box1, 
		   const KDop<REAL,SIZE>& box2)
{
   if (box1.isEmpty()) {
      box1 = box2;
      return;
   }
   for (int k=0; k<SIZE; ++k) {
      box1.minVector()[k] = stdMin(box1.minVector()[k], box2.minVector()[k]);
      box1.maxVector()[k] = stdMax(box1.maxVector()[k], box2.maxVector()[k]);
   }
   box1.update();
}

/** Intersection test for k-dops (with same k) bvol1 and dop2. 
    Specialization of 
    inline bool checkIntersect (const KDop<REAL,SIZE1>& box1,
                                const KDop<REAL,SIZE2>& box2).
  */
template <class REAL, int SIZE>
inline bool checkIntersect (const KDop<REAL,SIZE>& dop1,
			    const KDop<REAL,SIZE>& dop2)
{
   if (dop1.isEmpty() || dop2.isEmpty()) { // no intersection
      return false;              
   }
   for (int k=0; k<SIZE; ++k) {
      if (stdMin(dop1.maxVector()[k], dop2.maxVector()[k]) < stdMax(dop1.minVector()[k], dop2.minVector()[k])) {
	 return false;
      }
   }
   return true;
}
/** Intersection test for k-dops (with different k) bvol1 and dop2. */
template <class REAL, int SIZE1, int SIZE2>
inline bool checkIntersect (const KDop<REAL,SIZE1>& box1,
			    const KDop<REAL,SIZE2>& box2)
{
   if (box1.isEmpty() || box2.isEmpty()) {// no intersection
      return false;              
   }
   Polygon3SetIndexed& geom1 = box1.getGeometry().getPolygonSet();
   Polygon3SetIndexed& geom2 = box2.getGeometry().getPolygonSet();
   Polygon3Attribs::Container::iterator it;
   for (it = geom1.attribs.points.begin(); 
	it != geom1.attribs.points.end();
	++it) {
      if (box2.checkIntersect(*it)) {
	 return true;
      }
   }
   for (it = geom2.attribs.points.begin(); 
	it != geom2.attribs.points.end();
	++it) {
      if (box1.checkIntersect(*it)) {
	 return true;
      }
   }
   return false;
}

END_GENVIS_NAMESPACE
#endif
