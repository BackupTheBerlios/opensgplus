//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.3 $
//   $Date: 2004/12/20 15:58:23 $
//                                                                            
//=============================================================================

#include <sstream>
#include "OSGGL.h"
#include "OSGMatrix.h"
#include "OSGGVKDop.h"
#include "OSGGVKDopGeometry.h"
#include "OSGGVBVolFunctions.h"
#include "OSGGVGeomFunctions.h"
USING_GENVIS_NAMESPACE

// explicit template instantiations
template class OSG_GENVISLIB_DLLMAPPING KDop<Real,3>;
template class OSG_GENVISLIB_DLLMAPPING KDop<Real,7>;
template class OSG_GENVISLIB_DLLMAPPING KDop<Real,9>;
template class OSG_GENVISLIB_DLLMAPPING KDop<Real,13>;
template class OSG_GENVISLIB_DLLMAPPING KDop<Real,10>;
template class OSG_GENVISLIB_DLLMAPPING KDop<Real,6>;

#if 0
#include "OSGGVStatistics.h"
template <class REAL, int SIZE>
Timer KDop<REAL,SIZE>::s_time(100);
#endif

template <class REAL, int SIZE>
void KDop<REAL,SIZE>::setDirection (VectorClass* dir)
{
   for (u32 k=0; k<2*SIZE; ++k) {
      s_direction[k].setValue(dir[k]);
   } 
}
template <class REAL, int SIZE>
const VectorClass* KDop<REAL,SIZE>::getDirection ()
{
   return s_direction;
}

template <class REAL, int SIZE>
REAL KDop<REAL,SIZE>::getMinmaxFraction () const
{
   REAL minDiff, maxDiff;
   minDiff = maxDiff = difference(0);
   for (u32 k=1; k<SIZE; ++k) {
      REAL diff;
      if ((diff = difference(k)) < minDiff) {
	 minDiff = diff;
      }
      if (diff > maxDiff) {
	 maxDiff = diff;
      }
   }
   return minDiff/maxDiff;
}

float KDop<float,3>::getSurfaceArea () const
{
#if 0
   if (GV_verbose) {
      s_time.start();
   }
#endif
   float area = difference(0)*(difference(1)+difference(2)) 
     + difference(1)*difference(2);
#if 0
   if (GV_verbose) {
      s_time.stop();
      Statistics::the()[" 6-DOP NumArea"]  += 1;
      Statistics::the()[" 6-DOP TimeArea"]  = s_time.averageTime();
   }
#endif
   return area;
}

float KDop<float,3>::getVolume      () const
{
#if 0
   if (GV_verbose) {
      s_time.start();
   }
#endif
   float vol = difference(0) * difference(1) * difference(2);   
#if 0
   if (GV_verbose) {
      s_time.stop();
      Statistics::the()[" 6-DOP NumVolume"]  += 1;
      Statistics::the()[" 6-DOP TimeVolume"]  = s_time.averageTime();
   }
#endif
   return vol;
}

template <class REAL, int SIZE>
REAL KDop<REAL,SIZE>::getSurfaceArea () const
{
#if 0
   if (m_geom == NULL) {
      updateGeometry();
   }
#if 0
   if (GV_verbose) {
      s_time.start();
   }
#endif
   REAL area = m_geom->getSurfaceArea();
#if 0
   if (GV_verbose) {
      s_time.stop();
      std::ostringstream type; type << 2*SIZE << "-DOP ";
      Statistics::the()[std::string(type.str())+="NumArea"]  += 1;
      Statistics::the()[std::string(type.str())+="TimeArea"]  = s_time.averageTime();
   }
#endif
#else
   Real area = difference(0)*(difference(1)+difference(2)) 
     + difference(1)*difference(2);
#endif
   return area;
}
template <class REAL, int SIZE>
REAL KDop<REAL,SIZE>::getVolume      () const
{
#if 0
   if (m_geom == NULL) {
      updateGeometry();
   }
#if 0
   if (GV_verbose) {
      s_time.start();
   }
#endif
   REAL vol = m_geom->getVolume();
#if 0
   if (GV_verbose) {
      s_time.stop();
      std::ostringstream type; type << 2*SIZE << "-DOP ";
      Statistics::the()[std::string(type.str())+="NumVolume"]  += 1;
      Statistics::the()[std::string(type.str())+="TimeVolume"]  = s_time.averageTime();
   }
#endif
#else
   Real vol = difference(0) * difference(1) * difference(2);   
#endif
   return vol;
}

template <class REAL, int SIZE>
void KDop<REAL,SIZE>::updateGeometry () const
{
#if 0
   if (GV_verbose) {
      s_time.start();
   }
#endif
   Self* nonconstThis = (Self*)this;
   nonconstThis->m_geom = new KDopGeometry<REAL,SIZE>(*this);
#if 0
   if (GV_verbose) {
      s_time.stop();
      std::ostringstream type; type << 2*SIZE << "-DOP ";
      Statistics::the()[std::string(type.str())+="NumTesselation"]  += 1;
      Statistics::the()[std::string(type.str())+="TimeTesselation"]  = s_time.averageTime();
   }
#endif
}

template <class REAL, int SIZE>
void KDop<REAL,SIZE>::statistics (std::string& str)
{
#if 0
   std::ostringstream type; type << 2*SIZE << "-DOP ";
   std::ostringstream sstr; 
   sstr << "\n"
	<< "   " << Statistics::the()[std::string(type.str())+="NumTesselation"]
	<< " tesselated:   "
	<< Statistics::the()[std::string(type.str())+="TimeTesselation"]/Statistics::the()[std::string(type.str())+="NumTesselation"]
	<< " ms on average/"
	<< 1000.0*Statistics::the()[std::string(type.str())+="NumTesselation"]/Statistics::the()[std::string(type.str())+="TimeTesselation"]
	<< " dops per sec\n"
	<< "   " << Statistics::the()[std::string(type.str())+="NumArea"]
	<< " area-calcs:   "
	<< Statistics::the()[std::string(type.str())+="TimeArea"]/Statistics::the()[std::string(type.str())+="NumArea"]
	<< " ms on average/"
	<< 1000.0*Statistics::the()[std::string(type.str())+="NumArea"]/Statistics::the()[std::string(type.str())+="TimeArea"]
	<< " dops per sec\n"
	<< "   " << Statistics::the()[std::string(type.str())+="NumVolume"]
	<< " volume-calcs: "
	<< Statistics::the()[std::string(type.str())+="TimeVolume"]/Statistics::the()[std::string(type.str())+="NumVolume"]
	<< " ms on average/"
	<< 1000.0*Statistics::the()[std::string(type.str())+="NumVolume"]/Statistics::the()[std::string(type.str())+="TimeVolume"]
	<< " dops per sec\n"
	<< std::flush;
   str += sstr.str(); 
#endif
}

template <class REAL, int SIZE>
VectorClass KDop<REAL,SIZE>::s_direction[2*SIZE];
template <class REAL, int SIZE>
bool        KDop<REAL,SIZE>::s_init = createDirections();

bool KDop<float,3>::createDirections()
{
   s_direction[0].setValues(1, 0, 0);
   s_direction[1].setValues(0, 1, 0);
   s_direction[2].setValues(0, 0, 1);
   s_direction[3].setValue(-s_direction[0]);
   s_direction[4].setValue(-s_direction[1]);
   s_direction[5].setValue(-s_direction[2]);
   return true;
}
bool KDop<float,7>::createDirections()
{
   static float isqrt3 = 1.0f/sqrt(3.0f);
   s_direction[0].setValues(1, 0, 0);
   s_direction[1].setValues(0, 1, 0);
   s_direction[2].setValues(0, 0, 1);
   s_direction[3].setValues(isqrt3,  isqrt3,  isqrt3);
   s_direction[4].setValues(isqrt3, -isqrt3,  isqrt3);
   s_direction[5].setValues(isqrt3,  isqrt3, -isqrt3);
   s_direction[6].setValues(isqrt3, -isqrt3, -isqrt3);

   s_direction[7].setValue(-s_direction[0]);
   s_direction[8].setValue(-s_direction[1]);
   s_direction[9].setValue(-s_direction[2]);
   s_direction[10].setValue(-s_direction[3]);
   s_direction[11].setValue(-s_direction[4]);
   s_direction[12].setValue(-s_direction[5]);
   s_direction[13].setValue(-s_direction[6]);
   return true;
}

// boundary calculation can be done more efficient
// see Crosnier, Rossignac: Tribox bounds for three-dimensional objects
bool KDop<float,9>::createDirections()
{
   static float isqrt2 = 1.0f/sqrt(2.0f);
   s_direction[0].setValues(1, 0, 0);
   s_direction[1].setValues(0, 1, 0);
   s_direction[2].setValues(0, 0, 1);
   s_direction[3].setValues(isqrt2,  isqrt2,  0);
   s_direction[4].setValues(isqrt2,  0,  isqrt2);
   s_direction[5].setValues(0,  isqrt2,  isqrt2);
   s_direction[6].setValues(isqrt2, -isqrt2,  0);
   s_direction[7].setValues(isqrt2,  0, -isqrt2);
   s_direction[8].setValues(0,  isqrt2, -isqrt2);

   s_direction[9].setValue(-s_direction[0]);
   s_direction[10].setValue(-s_direction[1]);
   s_direction[11].setValue(-s_direction[2]);
   s_direction[12].setValue(-s_direction[3]);
   s_direction[13].setValue(-s_direction[4]);
   s_direction[14].setValue(-s_direction[5]);
   s_direction[15].setValue(-s_direction[6]);
   s_direction[16].setValue(-s_direction[7]);
   s_direction[17].setValue(-s_direction[8]);
   return true;
}

bool KDop<float,10>::createDirections()
{
   static float p = 0.5f*(sqrt(5.0f)+1.0f);
   s_direction[0].setValue(calcCenter(VectorClass(+1, 0, -p),VectorClass(+1, 0, +p),VectorClass(+p, -1, 0)));
   s_direction[1].setValue(calcCenter(VectorClass(-p, +1, 0),VectorClass(+p, +1, 0),VectorClass(0, +p, -1)));
   s_direction[2].setValue(calcCenter(VectorClass(0, -p, +1),VectorClass(0, +p, +1),VectorClass(-1, 0, +p)));

   // base neighbour to s_direction[0]
   s_direction[3].setValue(calcCenter(VectorClass(+1, 0, +p),VectorClass(+1, 0, -p),VectorClass(+p, +1, 0)));
   // base neighbour to s_direction[1]
   s_direction[4].setValue(calcCenter(VectorClass(+p, +1, 0),VectorClass(-p, +1, 0),VectorClass(0, +p, +1)));
   // base neighbour to s_direction[2]
   s_direction[5].setValue(calcCenter(VectorClass(0, +p, +1),VectorClass(0, -p, +1),VectorClass(+1, 0, +p)));

   s_direction[6].setValue(calcCenter(VectorClass(+1, 0, +p),VectorClass(+p, +1, 0),VectorClass(0, +p, +1)));
   s_direction[7].setValue(calcCenter(VectorClass(+1, 0, -p),VectorClass(+p, +1, 0),VectorClass(0, +p, -1)));
   s_direction[8].setValue(calcCenter(VectorClass(+1, 0, +p),VectorClass(+p, -1, 0),VectorClass(0, -p, +1)));
   s_direction[9].setValue(calcCenter(VectorClass(+1, 0, -p),VectorClass(+p, -1, 0),VectorClass(0, -p, -1)));

   s_direction[10].setValue(-s_direction[0]);
   s_direction[11].setValue(-s_direction[1]);
   s_direction[12].setValue(-s_direction[2]);
   s_direction[13].setValue(-s_direction[3]);
   s_direction[14].setValue(-s_direction[4]);
   s_direction[15].setValue(-s_direction[5]);
   s_direction[16].setValue(-s_direction[6]);
   s_direction[17].setValue(-s_direction[7]);
   s_direction[18].setValue(-s_direction[8]);
   s_direction[19].setValue(-s_direction[9]);
   return true;
}
bool KDop<float,6>::createDirections()
{
   static float p = 0.5f*(sqrt(5.0f)+1.0f);
   static float i = 0.5f*(sqrt(5.0f)-1.0f);
   s_direction[0].setValue(calcCenter(VectorClass(+p, 0, +i),VectorClass(+p, 0, -i),
				VectorClass(+1, -1, -1),VectorClass(+1, -1, +1),
				VectorClass(+i, -p, 0)));
   s_direction[1].setValue(calcCenter(VectorClass(+i, +p, 0),VectorClass(-i, +p, 0),
				VectorClass(+1, +1, -1),VectorClass(-1, +1, -1),
				VectorClass(0, +i, -p)));
   s_direction[2].setValue(calcCenter(VectorClass(0, +i, +p),VectorClass(0, -i, +p),
				VectorClass(-1, +1, +1),VectorClass(-1, -1, +1),
				VectorClass(-p, 0, +i)));
   s_direction[3].setValue(calcCenter(VectorClass(+p, 0, +i),VectorClass(+p, 0, -i),
				VectorClass(+1, +1, -1),VectorClass(+1, +1, +1),
				VectorClass(+i, +p, 0)));
   s_direction[4].setValue(calcCenter(VectorClass(+i, +p, 0),VectorClass(-i, +p, 0),
				VectorClass(+1, +1, +1),VectorClass(-1, +1, +1),
				VectorClass(0, +i, +p)));
   s_direction[5].setValue(calcCenter(VectorClass(0, +i, +p),VectorClass(0, -i, +p),
				VectorClass(+1, +1, +1),VectorClass(+1, -1, +1),
				VectorClass(+p, 0, +i)));

   s_direction[6].setValue(-s_direction[0]);
   s_direction[7].setValue(-s_direction[1]);
   s_direction[8].setValue(-s_direction[2]);
   s_direction[9].setValue(-s_direction[3]);
   s_direction[10].setValue(-s_direction[4]);
   s_direction[11].setValue(-s_direction[5]);
   return true;
}

bool KDop<float,13>::createDirections()
{
   static float isqrt2 = 1.0f/sqrt(2.0f);
   static float isqrt3 = 1.0f/sqrt(3.0f);
   s_direction[0].setValues(1, 0, 0);
   s_direction[1].setValues(0, 1, 0);
   s_direction[2].setValues(0, 0, 1);
   s_direction[3].setValues(isqrt3,  isqrt3,  isqrt3);
   s_direction[4].setValues(isqrt3, -isqrt3,  isqrt3);
   s_direction[5].setValues(isqrt3,  isqrt3, -isqrt3);
   s_direction[6].setValues(isqrt3, -isqrt3, -isqrt3);
   s_direction[7].setValues(isqrt2,  isqrt2,  0);
   s_direction[8].setValues(isqrt2,  0,  isqrt2);
   s_direction[9].setValues(0,  isqrt2,  isqrt2);
   s_direction[10].setValues(isqrt2, -isqrt2,  0);
   s_direction[11].setValues(isqrt2,  0, -isqrt2);
   s_direction[12].setValues(0,  isqrt2, -isqrt2);

   s_direction[13].setValue(-s_direction[0]);
   s_direction[14].setValue(-s_direction[1]);
   s_direction[15].setValue(-s_direction[2]);
   s_direction[16].setValue(-s_direction[3]);
   s_direction[17].setValue(-s_direction[4]);
   s_direction[18].setValue(-s_direction[5]);
   s_direction[19].setValue(-s_direction[6]);
   s_direction[20].setValue(-s_direction[7]);
   s_direction[21].setValue(-s_direction[8]);
   s_direction[22].setValue(-s_direction[9]);
   s_direction[23].setValue(-s_direction[10]);
   s_direction[24].setValue(-s_direction[11]);
   s_direction[25].setValue(-s_direction[12]);
   return true;
}
template <class REAL, int SIZE>
bool KDop<REAL,SIZE>::createDirections ()
{
   return true;
}

template <class REAL, int SIZE>
void KDop<REAL,SIZE>::init (const BoundingVolume<REAL>& bvol)
{
   const KDop<REAL,3>* k6dop  = NULL;
   if ((k6dop = dynamic_cast<const KDop<REAL,3>*>(&bvol))) {
      genvis::copy(*this, *k6dop);
      return;
   }
   const KDop<REAL,7>* k14dop  = NULL;
   if ((k14dop = dynamic_cast<const KDop<REAL,7>*>(&bvol))) {
      genvis::copy(*this, *k14dop);
      return;
   }
   const KDop<REAL,9>* k18dop  = NULL;
   if ((k18dop = dynamic_cast<const KDop<REAL,9>*>(&bvol))) {
      genvis::copy(*this, *k18dop);
      return;
   }
   const KDop<REAL,13>* k26dop  = NULL;
   if ((k26dop = dynamic_cast<const KDop<REAL,13>*>(&bvol))) {
      genvis::copy(*this, *k26dop);
      return;
   }
}
template <class REAL, int SIZE>
KDop<REAL,SIZE>::KDop<REAL,SIZE> (const BoundingVolume<REAL>& bvol)
  : Inherited(),
  m_geom(NULL)
{
   init(bvol);
}

template <class REAL, int SIZE>
const KDop<REAL,SIZE>& KDop<REAL,SIZE>::operator= (const KDop<REAL,SIZE>& dop)
{
   for (u32 k=0; k<SIZE; ++k) {
      m_min[k] = dop.m_min[k];
      m_max[k] = dop.m_max[k];
   }
   setEmpty(dop.isEmpty());
   update();
   return *this;
}
template <class REAL, int SIZE>
KDop<REAL,SIZE>::KDop (const KDop<REAL,SIZE>& dop)
  : Inherited(),
  m_geom(NULL)
{
   operator=(dop);
}

template <class REAL, int SIZE>
void KDop<REAL,SIZE>::init (const PointClass& center, 
			    REAL hx, REAL hy, REAL hz)
{
   m_min[0] = center[0]-hx;
   m_min[1] = center[1]-hy;
   m_min[2] = center[2]-hz;
   m_max[0] = center[0]+hx;
   m_max[1] = center[1]+hy;
   m_max[2] = center[2]+hz;
   for (u32 k=3; k<SIZE; ++k) {
      m_min[k] = m_min[0]*direction()[k][0]
	+ m_min[1]*direction()[k][1]
	+ m_min[2]*direction()[k][2];
      m_max[k] = m_max[0]*direction()[k][0]
	+ m_max[1]*direction()[k][1]
	+ m_max[2]*direction()[k][2];
   }
   static FloatingComparator<REAL> comp;
   setEmpty(comp.zero(hx) || comp.zero(hy) || comp.zero(hz));
   update();
}
template <class REAL, int SIZE>
KDop<REAL,SIZE>::KDop (const PointClass& center, 
		       REAL hx, REAL hy, REAL hz)
  : Inherited(),
  m_geom(NULL)
{
   init(center, hx, hy, hz);
}

template <class REAL, int SIZE>
KDop<REAL,SIZE>::KDop ()
  : Inherited(),
  m_geom(NULL)
{
}

template <class REAL, int SIZE>
KDop<REAL,SIZE>::KDop (const REAL* minVector, 
		       const REAL* maxVector)
  : Inherited(),
  m_geom(NULL)
{
   setEmpty(false);
   static FloatingComparator<REAL> comp;
   for (u32 k=0; k<SIZE; ++k) {
      m_min[k] = minVector[k];
      m_max[k] = maxVector[k];
      setEmpty(isEmpty() || comp.equal(m_min[k], m_max[k]));
   }
}

template <class REAL, int SIZE>
KDop<REAL,SIZE>::~KDop ()
{
   delete m_geom;
}

template <class REAL, int SIZE>
void KDop<REAL,SIZE>::update ()
{
   delete m_geom;
   m_geom = NULL;
}

PointClass KDop<float,6>::getCenter () const
{
   if (isEmpty()) {
      return PointClass();
   }
   VectorClass dir1(getDirection()[1]
		    -getDirection()[0].dot(getDirection()[1])*getDirection()[0]);
   VectorClass dir2(getDirection()[0].cross(dir1));
   PointClass center(average(0)*getDirection()[0]);
   center += average(1)*dir1;
   center += average(2)*dir2;
   return center;
}
PointClass KDop<float,10>::getCenter () const
{
   if (isEmpty()) {
      return PointClass();
   }
   VectorClass dir1(getDirection()[1]
		    -getDirection()[0].dot(getDirection()[1])*getDirection()[0]);
   VectorClass dir2(getDirection()[0].cross(dir1));
   PointClass center(average(0)*getDirection()[0]);
   center += average(1)*dir1;
   center += average(2)*dir2;
   return center;
}

template <class REAL, int SIZE>
PointClass KDop<REAL,SIZE>::getCenter () const
{
   if (isEmpty()) {
      return PointClass();
   }
   return PointClass(average(0), average(1), average(2));
}

bool KDop<float,3>::calcIntersect (const PointClass&  origin, 
				   const VectorClass& dir,
				   float& dist) const
{
   static FloatComparator comp;
   static i32 indices[] = {1, 2, 0, 2, 1, 0};

   if (isEmpty()) {
      return false;
   }

   VectorClass v(getCenter() - origin);
   VectorClass half(0.5f*difference(0),
		    0.5f*difference(1),
		    0.5f*difference(2));

   bool result = false;

   i32 pos = 0;
   for (u32 c=0; c<3; ++c, pos += 2) {
      if (!comp.zero(dir[c])) {
         float id = 1.0f/dir[c];
         float d = (v[c]+half[c])*id;
         //if (comp.positive(d) && (comp.negative(dist) || comp.less(d,dist)) && comp.less(stdAbs(v[indices[pos]]-d*dir[indices[pos]]),     half[indices[pos]]) && comp.less(stdAbs(v[indices[pos+1]]-d*dir[indices[pos+1]]), half[indices[pos+1]]) ) {
         if (d > 0 && (dist < 0 || d < dist) 
	     && stdAbs(v[indices[pos]]-d*dir[indices[pos]]) < half[indices[pos]]
             && stdAbs(v[indices[pos+1]]-d*dir[indices[pos+1]]) < half[indices[pos+1]]) {
            dist = d; result = true;
         }
         d = (v[c]-half[c])*id;
         //if (comp.positive(d) && (comp.negative(dist) || comp.less(d,dist)) && comp.less(stdAbs(v[indices[pos]]-d*dir[indices[pos]]),     half[indices[pos]]) && comp.less(stdAbs(v[indices[pos+1]]-d*dir[indices[pos+1]]), half[indices[pos+1]]) ) {
	 if (d > 0 && (dist < 0 || d < dist) 
	     && stdAbs(v[indices[pos]]-d*dir[indices[pos]]) < half[indices[pos]]
	     && stdAbs(v[indices[pos+1]]-d*dir[indices[pos+1]]) < half[indices[pos+1]]) {
            dist = d; result = true;
         }
      }
   }
   return result;
}

template <class REAL,int SIZE>
bool KDop<REAL,SIZE>::checkIntersect (const PointClass&  origin, 
				      const VectorClass& dir,
				      const REAL& dist) const
{
   static FloatingComparator<REAL> c;

   if (isEmpty()) {
      return false;
   }

   REAL cen[SIZE];
   REAL ray[SIZE];
   u32 k, kk;
   for (k=0; k<SIZE; ++k) {
      cen[k]  = direction()[k].dot(origin); 
      ray[k]  = direction()[k].dot(dir);
   }

   for (k=0; k<SIZE; ++k) {
      if (!c.zero(ray[k])) {
         REAL id = 1.0f/ray[k];
         REAL d  = (m_min[k]-cen[k])*id;
         if (c.positive(d) && (c.negative(dist) || c.less(d, dist)) ) {
	    bool thisResult = true;
	    for (kk=0; kk<SIZE; ++kk) {
	       if (kk == k) {
		  continue;
	       }
	       REAL proj = cen[kk]+d*ray[kk];
	       if (c.less(proj, m_min[kk]) || c.greater(proj, m_max[kk])) {
		  thisResult = false;
	       }
	    }
	    if (thisResult) {
	       return true;
	    }
	 }
         d = (m_max[k]-cen[k])*id;
         if (c.positive(d) && (c.negative(dist) || c.less(d,dist)) ) {
	    bool thisResult = true;
	    for (kk=0; kk<SIZE; ++kk) {
	       if (kk == k) {
		  continue;
	       }
	       REAL proj = cen[kk]+d*ray[kk];
	       if (c.less(proj, m_min[kk]) || c.greater(proj, m_max[kk])) {
		  thisResult = false;
	       }
	    }
	    if (thisResult) {
	       return true;
	    }
	 }
      }
   }
   return false;
}
template <class REAL,int SIZE>
bool KDop<REAL,SIZE>::calcIntersect (const PointClass&  origin, 
				     const VectorClass& dir,
				     REAL& dist) const
{
   static FloatingComparator<REAL> c;

   if (isEmpty()) {
      return false;
   }

   REAL cen[SIZE];
   REAL ray[SIZE];
   u32 k, kk;
   for (k=0; k<SIZE; ++k) {
      cen[k]  = direction()[k].dot(origin); 
      ray[k]  = direction()[k].dot(dir);
   }

   bool result = false;
   for (k=0; k<SIZE; ++k) {
      if (!c.zero(ray[k])) {
         REAL id = 1.0f/ray[k];
         REAL d  = (m_min[k]-cen[k])*id;
         if (c.positive(d) && (c.negative(dist) || c.less(d, dist)) ) {
	    bool thisResult = true;
	    for (kk=0; kk<SIZE; ++kk) {
	       if (kk == k) {
		  continue;
	       }
	       REAL proj = cen[kk]+d*ray[kk];
	       if (c.less(proj, m_min[kk]) || c.greater(proj, m_max[kk])) {
		  thisResult = false;
	       }
	    }
	    if (thisResult) {
	       dist = d; 
	       result = true;
	    }
	 }
         d = (m_max[k]-cen[k])*id;
         if (c.positive(d) && (c.negative(dist) || c.less(d,dist)) ) {
	    bool thisResult = true;
	    for (kk=0; kk<SIZE; ++kk) {
	       if (kk == k) {
		  continue;
	       }
	       REAL proj = cen[kk]+d*ray[kk];
	       if (c.less(proj, m_min[kk]) || c.greater(proj, m_max[kk])) {
		  thisResult = false;
	       }
	    }
	    if (thisResult) {
	       dist = d; 
	       result = true;
	    }
	 }
      }
   }
   return result;
}

template <class REAL, int SIZE>
bool KDop<REAL,SIZE>::checkIntersect (const PointClass& point) const
{
   for (u32 k=0; k<SIZE; ++k) {
      REAL proj = direction()[k].dot(point);
      if (proj < minVector()[k] || maxVector()[k] < proj) {
	//if (!genvis::checkIntersect(proj, minVector()[k], maxVector()[k])) {
	 return false;
      }
   }
   return true;
}

bool KDop<float, 3>::checkIntersect (const PointClass& p0,
				    const PointClass& p1,
				    const PointClass& p2) const
{
   static FloatingComparator<float> comp;
   float min0 = stdMin(stdMin(p0[0], p1[0]), p2[0]);
   float max0 = stdMax(stdMax(p0[0], p1[0]), p2[0]);
   if (   comp.less   (max0, minVector()[0]) 
       || comp.greater(min0, maxVector()[0])) {
      return false;
   }
   float min1 = stdMin(stdMin(p0[1], p1[1]), p2[1]);
   float max1 = stdMax(stdMax(p0[1], p1[1]), p2[1]);
   if (   comp.less   (max1, minVector()[1]) 
       || comp.greater(min1, maxVector()[1])) {
      return false;
   }
   float min2 = stdMin(stdMin(p0[2], p1[2]), p2[2]);
   float max2 = stdMax(stdMax(p0[2], p1[2]), p2[2]);
   if (   comp.less   (max2, minVector()[2]) 
       || comp.greater(min2, maxVector()[2])) {
      return false;
   }
   return true;
}

template <class REAL, int SIZE>
bool KDop<REAL, SIZE>::checkIntersect (const PointClass& p0,
				      const PointClass& p1,
				      const PointClass& p2) const
{
   static FloatingComparator<REAL> comp;
   for (u32 k=0; k<SIZE; ++k) {
      REAL proj0 = direction()[k].dot(p0);
      REAL proj1 = direction()[k].dot(p1);
      REAL proj2 = direction()[k].dot(p2);
      REAL mink = stdMin(stdMin(proj0, proj1), proj2);
      REAL maxk = stdMax(stdMax(proj0, proj1), proj2);
      if (   comp.less   (maxk, minVector()[k]) 
	  || comp.greater(mink, maxVector()[k])) {
	 return false;
      }
   }
   return true;
}

template <class REAL, int SIZE>
typename KDop<REAL,SIZE>::BVolGeometry& 
KDop<REAL,SIZE>::getGeometry () const
{
   if (m_geom == NULL) {
      updateGeometry();
   }
   return *m_geom;
}

template <class REAL, int SIZE>
bool KDop<REAL,SIZE>::checkIntersect (const BoundingVolume<REAL>& box2) const
{
   const KDop<REAL,3>*    aabb = NULL;
   const KDop<REAL,SIZE>* dop  = NULL;
   if ((aabb = dynamic_cast<const KDop<REAL,3>*>(&box2))) {
      return genvis::checkIntersect(*this, *aabb);
   }
   if ((dop = dynamic_cast<const KDop<REAL,SIZE>*>(&box2))) {
      return genvis::checkIntersect(*this, *dop);
   }
   return false;
}

template <class REAL, int SIZE>
bool KDop<REAL,SIZE>::checkIntersect (const KDop<REAL,SIZE>& dop2) const
{
   return genvis::checkIntersect(*this, dop2);
}

template <class REAL, int SIZE>
bool KDop<REAL,SIZE>::checkIntersect (const KDop<REAL,SIZE>& dop2,
				      REAL*                  offset) const
{
   if (isEmpty() || dop2.isEmpty()) {// one box is empty => no intersection
      return false;              
   }
   static FloatingComparator<REAL> comp;
   for (u32 k=0; k<SIZE; ++k) {
      if (comp.less(stdMin(maxVector()[k], dop2.maxVector()[k]+offset[k]),
		    stdMax(minVector()[k], dop2.minVector()[k]+offset[k]))) {
	 return false;
      }
   }
   return true;
}

template <class REAL, int SIZE>
void KDop<REAL,SIZE>::unify (const KDop<REAL,SIZE>& dop)
{
   if (isEmpty()) {
      operator=(dop);
   } else {
      for (u32 k=0; k<SIZE; ++k) {
         minVector()[k] = stdMin(minVector()[k], dop.minVector()[k]);
         maxVector()[k] = stdMax(maxVector()[k], dop.maxVector()[k]);
      }
      update();
   }
}
template <class REAL, int SIZE>
void KDop<REAL,SIZE>::unify (const KDop<REAL,SIZE>& dop,
			     REAL*                  offset)
{
   if (isEmpty()) {
      for (u32 k=0; k<SIZE; ++k) {
         minVector()[k] = dop.minVector()[k]+offset[k];
         maxVector()[k] = dop.maxVector()[k]+offset[k];
      }
      setEmpty(dop.isEmpty());
      update();
   } else {
      for (u32 k=0; k<SIZE; ++k) {
         minVector()[k] = stdMin(minVector()[k], dop.minVector()[k]+offset[k]);
         maxVector()[k] = stdMax(maxVector()[k], dop.maxVector()[k]+offset[k]);
      }
      update();
   }
}

template <class REAL, int SIZE>
void KDop<REAL,SIZE>::unify (float size)
{
   for (u32 k=0; k<BVol::Size; ++k) {
      minVector()[k] -= size;
      maxVector()[k] += size;
   }
}
template <class REAL, int SIZE>
void KDop<REAL,SIZE>::unify (const BoundingVolume<REAL>& bvol)
{
   const KDop<REAL,SIZE>* dop  = NULL;
   if ((dop = dynamic_cast<const KDop<REAL,SIZE>*>(&bvol))) {
      genvis::unify(*this, *dop);
   }
}
template <class REAL, int SIZE>
void KDop<REAL,SIZE>::unify (const PointClass& point)
{
   if (isEmpty()) {
      for (u32 k=0; k<SIZE; ++k) {
	 REAL proj = direction()[k].dot(point);
	 minVector()[k] = proj;
	 maxVector()[k] = proj;
      }
   } else {
      for (u32 k=0; k<SIZE; ++k) {
	 REAL proj = direction()[k].dot(point);
         minVector()[k] = stdMin(minVector()[k], proj);
         maxVector()[k] = stdMax(maxVector()[k], proj);
      }
   }
   setEmpty(false);
   update();
}

void KDop<float,3>::draw ()          const
{
     glBegin(GL_QUADS);
     glNormal3f(0,0,-1);
     glVertex3f(minVector()[0], minVector()[1], minVector()[2]);
     glVertex3f(minVector()[0], maxVector()[1], minVector()[2]);
     glVertex3f(maxVector()[0], maxVector()[1], minVector()[2]);
     glVertex3f(maxVector()[0], minVector()[1], minVector()[2]);

     glNormal3f(0,0,+1);
     glVertex3f(maxVector()[0], minVector()[1], maxVector()[2]);
     glVertex3f(maxVector()[0], maxVector()[1], maxVector()[2]);
     glVertex3f(minVector()[0], maxVector()[1], maxVector()[2]);
     glVertex3f(minVector()[0], minVector()[1], maxVector()[2]);

     glNormal3f(0,0,-1);
     glVertex3f(minVector()[0], minVector()[1], minVector()[2]);
     glVertex3f(minVector()[0], minVector()[1], maxVector()[2]);
     glVertex3f(minVector()[0], maxVector()[1], maxVector()[2]);  
     glVertex3f(minVector()[0], maxVector()[1], minVector()[2]);

     glNormal3f(0,0,+1);
     glVertex3f(maxVector()[0], maxVector()[1], minVector()[2]);
     glVertex3f(maxVector()[0], maxVector()[1], maxVector()[2]);
     glVertex3f(maxVector()[0], minVector()[1], maxVector()[2]);
     glVertex3f(maxVector()[0], minVector()[1], minVector()[2]);

     glNormal3f(0,0,-1);
     glVertex3f(minVector()[0], minVector()[1], minVector()[2]);
     glVertex3f(maxVector()[0], minVector()[1], minVector()[2]);
     glVertex3f(maxVector()[0], minVector()[1], maxVector()[2]);
     glVertex3f(minVector()[0], minVector()[1], maxVector()[2]);
     
     glNormal3f(0,0,+1);
     glVertex3f(minVector()[0], maxVector()[1], maxVector()[2]);
     glVertex3f(maxVector()[0], maxVector()[1], maxVector()[2]);
     glVertex3f(maxVector()[0], maxVector()[1], minVector()[2]);
     glVertex3f(minVector()[0], maxVector()[1], minVector()[2]);
     glEnd();
}
void KDop<float,3>::drawWireframe ()          const
{
     glBegin(GL_LINE_LOOP);
     glVertex3f(minVector()[0], minVector()[1], minVector()[2]);
     glVertex3f(minVector()[0], maxVector()[1], minVector()[2]);
     glVertex3f(maxVector()[0], maxVector()[1], minVector()[2]);
     glVertex3f(maxVector()[0], minVector()[1], minVector()[2]);
     glEnd();

     glBegin(GL_LINE_LOOP);
     glVertex3f(maxVector()[0], minVector()[1], maxVector()[2]);
     glVertex3f(maxVector()[0], maxVector()[1], maxVector()[2]);
     glVertex3f(minVector()[0], maxVector()[1], maxVector()[2]);
     glVertex3f(minVector()[0], minVector()[1], maxVector()[2]);
     glEnd();

     glBegin(GL_LINES);
     glVertex3f(minVector()[0], minVector()[1], minVector()[2]);
     glVertex3f(minVector()[0], minVector()[1], maxVector()[2]);
     glVertex3f(minVector()[0], maxVector()[1], minVector()[2]);
     glVertex3f(minVector()[0], maxVector()[1], maxVector()[2]);
     glVertex3f(maxVector()[0], maxVector()[1], minVector()[2]);
     glVertex3f(maxVector()[0], maxVector()[1], maxVector()[2]);
     glVertex3f(maxVector()[0], minVector()[1], minVector()[2]);
     glVertex3f(maxVector()[0], minVector()[1], maxVector()[2]);
     glEnd();
}

template <class REAL, int SIZE>
void KDop<REAL,SIZE>::draw ()          const
{
   if (m_geom == NULL) {
      updateGeometry();
   }
   m_geom->draw();
}
template <class REAL, int SIZE>
void KDop<REAL,SIZE>::drawWireframe ()          const
{
   if (m_geom == NULL) {
      updateGeometry();
   }
   m_geom->drawWireframe();
}

template <class REAL, int SIZE>
std::ostream& KDop<REAL,SIZE>::dumpInternal (std::ostream& os) const
{
   os << "k-DOP(k=" << SIZE << ", direction" << std::flush; 
   u32 k;
   for (k=0; k<SIZE; ++k) {
      os << " " << direction()[k];
   } 
   os << ", min" << std::flush;
   for (k=0; k<SIZE; ++k) {
      os << " " << m_min[k];
   } 
   os << ", max" << std::flush;
   for (k=0; k<SIZE; ++k) {
      os << " " << m_max[k];
   } 
   os << ")" << std::flush;
   return os;
}     

template <class REAL, int SIZE>
const KDop<REAL,SIZE>& KDop<REAL,SIZE>::unitDop ()
{
   static REAL unitMin[] =
   {-1, -1, -1, -1,  -1, -1, -1, -1,  -1, -1, -1, -1, -1};
   static REAL unitMax[] =
   {+1, +1, +1, +1,  +1, +1, +1, +1,  +1, +1, +1, +1, +1};
   static BVol unit(unitMin, unitMax);
   return unit;
}

template <class REAL, int SIZE>
REAL KDop<REAL,SIZE>::unitDopRadius ()
{
   static REAL radius = -1;
   if (radius < 0) {
      Polygon3SetIndexed& geom = unitDop().getGeometry().getPolygonSet();
      for (u32 k=0; k<BVol::Size; ++k) {
	 PointClass           center(getDirection()[k]);
	 Polygon3Indexed*     itF    = geom.getFace(k);
	 Polygon3EdgeIndexed* e      = itF->firstEdge();
	 do {
	    radius = stdMax(radius, center.dist2(e->getPoint()));
	    e = e->getNext();
	 } while (e != itF->firstEdge());
      }
      radius = sqrt(radius);
   }
   return radius;
}

template <class REAL, int SIZE>
REAL KDop<REAL,SIZE>::unitDopAngle ()
{
   static REAL angle = -1;
   if (angle < 0) {
      // length of hypothenusis sqrt(1+unitDopRadius()*unitDopRadius()) (phythagorean theorem)
      // angle = ancathete/hypothenusis
      angle = 1.0f/sqrt(1+unitDopRadius()*unitDopRadius());
      GV_stream << "unitDopRadius(" << SIZE << "-DOP: " << unitDopRadius() << ")" << std::endl;
      GV_stream << "unitDopAngle (" << SIZE << "-DOP: " << angle << ")" << std::endl;
   }
   return angle;
}

#ifdef GV_PRECOMPUTEDREALIGN_32BIT

// tab[-1] = 1
// tab[30] = unitDopAngle()

#if 0
// subdividing the total angle uniformly 
template <class REAL, int SIZE>
const REAL* KDop<REAL,SIZE>::unitDopAngleTable ()
{
   static REAL tab[OccTableBits] = { 
     -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
     -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
     -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
     -1.0, -1.0
   };
   if (tab[0] < 0) {
      REAL step = acosf(unitDopAngle()) / OccTableHighestBit;
      for (i32 i=0; i<OccTableBits; ++i) {
	 tab[i] = cosf(i*step);
      }
   }
   return tab+1;
}
#else
// subdividing the cosine of the total angle uniformly 
template <class REAL, int SIZE>
const REAL* KDop<REAL,SIZE>::unitDopAngleTable ()
{
   static REAL tab[OccTableBits] = { 
     -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
     -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
     -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
     -1.0, -1.0
   };
   if (tab[0] < 0) {
      tab[0] = 1;
      REAL step = (1 - unitDopAngle()) / OccTableHighestBit;
      for (i32 i=1; i<OccTableBits; ++i) {
	 tab[i] = tab[i-1] - step;
      }
   }
   return tab+1;
}
#endif
template <class REAL, int SIZE>
typename KDop<REAL,SIZE>::DopTableType* 
KDop<REAL,SIZE>::unitDopTable ()
{
   static const REAL* tab = unitDopAngleTable();
   static REAL  tab2[OccTableBits][OccTableBits] = {
   { 
     -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
     -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
     -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
     -1.0, -1.0 
   }
   };
   if (tab2[0][0] < 0) {
      for (i32 ring=0; ring<OccTableBits; ++ring) {
	 for (i32 angle=0; angle<OccTableBits; ++angle) {
	    tab2[ring][angle] = tab[angle-1]/tab[ring];
	 }
      }
   }
   return (DopTableType*)(&(tab2[0][0])+1);
}
#else
// subdividing the cosine of the total angle uniformly 
template <class REAL, int SIZE>
const REAL* KDop<REAL,SIZE>::unitDopAngleTable ()
{
   static REAL tab[OccTableBits] = { 
     -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
     -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
     -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
     -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
     -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
     -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
     -1.0, -1.0, -1.0, -1.0
   };
   if (tab[0] < 0) {
      tab[0] = 1;
      REAL step = (1 - unitDopAngle()) / OccTableHighestBit;
      for (i32 i=1; i<OccTableBits; ++i) {
	 tab[i] = tab[i-1] - step;
      }
   }
   return tab+1;
}
template <class REAL, int SIZE>
KDop<REAL,SIZE>::DopTableType* KDop<REAL,SIZE>::unitDopTable ()
{
   static const REAL* tab = unitDopAngleTable();
   static REAL  tab2[OccTableBits][OccTableBits] = {
   { 
     -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
     -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
     -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
     -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
     -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
     -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
     -1.0, -1.0, -1.0, -1.0
   }
   };
   if (tab2[0][0] < 0) {
      for (i32 ring=0; ring<OccTableBits; ++ring) {
	 for (i32 angle=0; angle<OccTableBits; ++angle) {
	    tab2[ring][angle] = tab[angle-1]/tab[ring];
	 }
      }
   }
   return (DopTableType*)(&(tab2[0][0])+1);
}
#endif

template <class REAL, int SIZE>
REAL KDop<REAL,SIZE>::unitDopAngleTable (const OccTableType& mask, 
					 int                 p,
					 REAL                out)
{
   assert(0 <= p && p <= OccTableBits);
#if 1
   static DopTableType* tab = unitDopTable();

   register OccTableType check = OccTableType(1) << OccTableHighestBit;
   register i32          angle = OccTable2HighestBit - p;
   // -1 <= angle <= OccTable2HighestBit
   register REAL maxValue = 1;
   if (mask & check) {
      //tab[angle-1]/tab[0]
      maxValue = tab[0][angle]*out;
   }
   //std::cout << ">" << maxValue;
   // sequence v+_i = tab[i]/tab[p+i] >= 1
   // and monotonously increasing
   check >>= 1;
   assert(check == (OccTableType(1)<<OccTable2HighestBit));
   --angle;
   register i32 ring = OccTable2HighestBit;
   // 0 <= ring <= OccTable2HighestBit
   while (angle >= 0) {
     if (mask & check) { // (1<<(p+i))
       if (tab[ring][angle] > maxValue) {
	  maxValue = tab[ring][angle];
       }
       //std::cout << " >>" << maxValue << std::endl;
       break;
     }
     check>>=1, --angle, --ring;
   }
   // sequence v-_i = tab[i-1]/tab[p-i] > 1
   // and monotonously decreasing
   check = (1<<p);
   assert(check == (1<<p));
   ring  = p;
   assert(ring  == p);
   //++angle;
   angle = 0;
   while (ring >= 0) {
      if (mask & check) { // (1<<(p-i))
	 if (tab[ring][angle] > maxValue) {
	    maxValue = tab[ring][angle];
	 }
	 //std::cout << " >>>" << maxValue << std::endl;
	 break;
      }
      check>>=1, ++angle; --ring;
   }
   //std::cout << std::endl;
   return maxValue;
#endif

#if 0
   static REAL* tab = unitDopAngleTable();
   REAL maxValue = 1;
   register REAL value;
   register u32 check = (OccTableType(1) << OccTableHighestBit);
   //std::cout << "p=" << p << ":";
   if (mask & check) {
      maxValue = tab[OccTable2HighestBit-p]*out;
   }
   //std::cout << " " << (mask & check ? 1 : 0) << ">" << maxValue;
   // sequence v+_i = tab[i]/tab[p+i] >= 1
   // and monotonously increasing
   check >>= 1;
   register i32 i;
   for (i=OccTable2HighestBit-p; i>0; --i, check>>=1) {
     if (mask & check) { // (1<<(p+i))
	 if ((value=tab[i-1]/tab[p+i]) > maxValue) {
	    maxValue = value;
	 }
	 //std::cout << " " << i << ">>1" << ":" << value;
	 break;
      }
      //std::cout << " " << i << ">>0";
   }
   // i ==  0, angle == -1, ring == p
   // i == -1, angle == -2, ring == p-1 
   // sequence v-_i = tab[i-1]/tab[p-i] > 1
   // and monotonously decreasing
   check = (OccTableType(1) << p);
   for (i=0; i<=p; ++i, check>>=1) {
      if (mask & check) { // (1<<(p-i))
	 if ((value=tab[i-1]/tab[p-i]) > maxValue) {
	    maxValue = value;
	 }
	 //std::cout << " " << i << ">>>1" << ":" << value;
	 break;
      }
      //std::cout << " " << i << " >>>0";
   }
   //std::cout << std::endl;
   return maxValue;
#endif

#if 0
   static REAL* tab = unitDopAngleTable();
   register REAL maxValue = 1;
   register REAL value;
   register OccTableType check = OccTableType(1) << OccTableHighestBit;
   register i32          angle = OccTable2HighestBit - p;
   if (mask & check) {
      maxValue = tab[angle]*out;
   }
   //std::cout << ">>" << maxValue;
   // sequence v+_i = tab[i]/tab[p+i] >= 1
   // and monotonously increasing
   check >>= 1;
   assert(check == (OccTableType(1)<<OccTable2HighestBit));
   --angle;
   register i32 ring = OccTable2HighestBit;
   while (angle >= 0) {
     if (mask & check) { // (1<<(p+i))
	 if ((value=tab[angle]/tab[ring]) > maxValue) {
	    maxValue = value;
	    //std::cout << " >" << i << ":" << value;
	 }
	 return maxValue;
      }
      check>>=1, --angle, --ring;
   }
   // sequence v-_i = tab[i-1]/tab[p-i] > 1
   // and monotonously decreasing
   assert(check == (1<<p));
   assert(ring  == p);
   ++angle;
   while (ring >= 0) {
      if (mask & check) { // (1<<(p-i))
	 if ((value=tab[angle]/tab[ring]) > maxValue) {
	    maxValue = value;
	    //std::cout << " <" << i << ":" << value;
	 }
	 return maxValue;
      }
      check>>=1, ++angle; --ring;
   }
   return maxValue;
#endif
}

