//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.2 $
//   $Date: 2003/09/19 21:33:57 $
//                                                                            
//=============================================================================

#include "OSGGVTriangleAdapterAligned.h"

#include "OSGVector.h"
#include "OSGFieldContainerPtr.h"
#include "OSGNode.h"
#include "OSGNodeCore.h"
#include "OSGBoxVolume.h"
#include "OSGMFFieldContainerTypes.h"
#include "OSGQuaternion.h"
#include "OSGMatrix.h"
#include "OSGGVGeomFunctions.h"
#include "OSGGVBVolFunctions.h"
#include "OSGGVPolygon.h"
USING_GENVIS_NAMESPACE
OSG_USING_NAMESPACE

// explicit template instantiations
template class OpenSGTriangleAligned<OpenSGTraits, K6Dop>;
template class OpenSGTriangleAligned<OpenSGTraits, K14Dop>;
template class OpenSGTriangleAligned<OpenSGTraits, K18Dop>;
template class OpenSGTriangleAligned<OpenSGTraits, K26Dop>;
template class OpenSGTriangleAligned<OpenSGTraits, K12Dop>;
template class OpenSGTriangleAligned<OpenSGTraits, K20Dop>;

template class OSG_GENVISLIB_DLLMAPPING BVolGroupAligned<K6Dop>;
template class OSG_GENVISLIB_DLLMAPPING BVolGroupAligned<K14Dop>;
template class OSG_GENVISLIB_DLLMAPPING BVolGroupAligned<K18Dop>;
template class OSG_GENVISLIB_DLLMAPPING BVolGroupAligned<K26Dop>;
template class OSG_GENVISLIB_DLLMAPPING BVolGroupAligned<K12Dop>;
template class OSG_GENVISLIB_DLLMAPPING BVolGroupAligned<K20Dop>;


template <class BasicTraits, class BVOL>
std::ostream& OpenSGTriangleAligned<BasicTraits,BVOL>::dump (std::ostream& os) const
{
   os << "OpenSGTriangleAligned<BasicTraits,BVOL>(";
   BVolAdapter<BVOL>::dump(os);
   os << ")";
   return os;
}

template <class BasicTraits, class BVOL>
OpenSGTriangleAligned<BasicTraits,BVOL>::OpenSGTriangleAligned ()
{
}
template <class BasicTraits, class BVOL>
OpenSGTriangleAligned<BasicTraits,BVOL>::OpenSGTriangleAligned (const GeomTriangleType& obj)
{
   init(obj);
}
template <class BasicTraits, class BVOL>
OpenSGTriangleAligned<BasicTraits,BVOL>::OpenSGTriangleAligned (const TransformType&    m2w,
								const GeomTriangleType& obj)
{
   init(m2w, obj);
}

template <class BasicTraits, class BVOL>
void OpenSGTriangleAligned<BasicTraits,BVOL>::setPoint (unsigned k,
							Real  len,
						        VectorClass local)
{
   static FloatingComparator<Real> comp(0.01f);

   Real s  = local.dot(BVOL::getDirection()[k]);
   local -= s*BVOL::getDirection()[k];
   Real sx = local.dot(BVOL::BVolGeometry::getFrameX()[k]);
   Real sy = local.dot(BVOL::BVolGeometry::getFrameY()[k]);
#if 1
   // 45-degrees rotated
   unsigned si = (sy < -sx ? 2 : 0) 
     + (sy < -sx && sy >= sx ? 1 : 0) 
     + (sy >= -sx && sy < sx ? 1 : 0);
#else
   // axes-aligned
   unsigned si = (sx < 0 ? 2 : 0) 
     + (sx < 0 && sy >= 0 ? 1 : 0) 
     + (sx >= 0 && sy < 0 ? 1 : 0);
#endif
   assert(si < 4);
   unsigned sii = (si == 0 ? 3 : si-1);
   assert(sii < 4);
   unsigned i;
   if (s >= 0) {
	 if (comp.greater(len, m_bvol.maxVector()[k])) {
	    for (i=0; i<OccTableHighestBit; ++i) {
	       if (s >= BVOL::unitDopAngleTable()[i]) {
		  break;
	       }
	    }
#ifdef GV_WITH_SUBCONES
	    getData().getOccupancyInternal()[k][sii] |= (1<<i);
	    getData().getOccupancyInternal()[k][si]  |= (1<<i);
#else
	    getData().getOccupancyInternal()[k][0] |= (1<<i);
#endif
	    if (i == OccTableHighestBit) {
	      s = (s*BVOL::unitDopAngle() + cos2sin(s)*cos2sin(BVOL::unitDopAngle()))*len;
#ifdef GV_WITH_SUBCONES
	      if (s > getData().getOuterMostInternal()[k][sii]) {
		 getData().getOuterMostInternal()[k][sii] = s;
	      }
	      if (s > getData().getOuterMostInternal()[k][si]) {
		 getData().getOuterMostInternal()[k][si]  = s;
	      }
#else
	      if (s > getData().getOuterMostInternal()[k][0]) {
		 getData().getOuterMostInternal()[k][0]  = s;
	      }
#endif
	    }
	 }
   } else {
      if (comp.greater(len, -m_bvol.minVector()[k])) {
	 unsigned   kk = k + BVOL::Size;
	 s = -s;
#if 1
	 if (si <  2) si  += 2;
	 else si  -= 2;
	 if (sii < 2) sii += 2;
	 else sii -= 2;
	 assert(si  < 4);
	 assert(sii < 4);
#endif
	 for (i=0; i<OccTableHighestBit; ++i) {
	   if (s >= BVOL::unitDopAngleTable()[i]) {
	     break;
	   }
	 }
#ifdef GV_WITH_SUBCONES
	 getData().getOccupancyInternal()[kk][sii] |= (1<<i);
	 getData().getOccupancyInternal()[kk][si]  |= (1<<i);
#else
	 getData().getOccupancyInternal()[kk][0]  |= (1<<i);
#endif
	 if (i == OccTableHighestBit) {
	   s = (s*BVOL::unitDopAngle() + cos2sin(s)*cos2sin(BVOL::unitDopAngle()))*len;
#ifdef GV_WITH_SUBCONES
	   if (s > getData().getOuterMostInternal()[kk][sii]) {
	     getData().getOuterMostInternal()[kk][sii] = s;
	   }
	   if (s > getData().getOuterMostInternal()[kk][si]) {
	     getData().getOuterMostInternal()[kk][si]  = s;
	   }
#else
	   if (s > getData().getOuterMostInternal()[kk][0]) {
	     getData().getOuterMostInternal()[kk][0]  = s;
	   }
#endif
	 }
      }
   }
}
template <class BasicTraits, class BVOL>
void  OpenSGTriangleAligned<BasicTraits,BVOL>::init (const PointClass& p0,
						     const PointClass& p1,
						     const PointClass& p2)
{
   // center point
   PointClass center(0.33f*(VectorClass(p0)+VectorClass(p1)+VectorClass(p2)));

   unsigned    k, kk, i;
   // first round: calc k-dop as usual
   VectorClass local(p0 - center); 
   m_bvol.unify(local);
   local.setValue(p1 - center);
   m_bvol.unify(local);
   local.setValue(p2 - center);
   m_bvol.unify(local);
   // add safety distance
   static FloatingComparator<Real> comp;
   m_bvol.unify(comp.getPrecision());

   // second round: calc occupancy map
   local.setValue(p0 - center); 
   Real len = local.length(); 
   local /= len;
   for (k=0, kk=BVOL::Size; k<BVOL::Size; ++k, ++kk) {
      getData().getOuterMostInternal()[k][0] = getData().getOuterMostInternal()[kk][0] = 0;
      getData().getOuterMostInternal()[k][1] = getData().getOuterMostInternal()[kk][1] = 0;
      getData().getOuterMostInternal()[k][2] = getData().getOuterMostInternal()[kk][2] = 0;
      getData().getOuterMostInternal()[k][3] = getData().getOuterMostInternal()[kk][3] = 0;
      getData().getOccupancyInternal()[k][0] = getData().getOccupancyInternal()[kk][0] = 0;
      getData().getOccupancyInternal()[k][1] = getData().getOccupancyInternal()[kk][1] = 0;
      getData().getOccupancyInternal()[k][2] = getData().getOccupancyInternal()[kk][2] = 0;
      getData().getOccupancyInternal()[k][3] = getData().getOccupancyInternal()[kk][3] = 0;
      setPoint(k, len, local);
   }
   local.setValue(p1 - center); 
   len = local.length(); 
   local /= len;
   for (k=0; k<BVOL::Size; ++k) {
      setPoint(k, len, local);
   }
   local.setValue(p2 - center); 
   len = local.length(); 
   local /= len;
   for (k=0; k<BVOL::Size; ++k) {
      setPoint(k, len, local);
   }

   getData().getCenterInternal()[0] = center[0];
   getData().getCenterInternal()[1] = center[1];
   getData().getCenterInternal()[2] = center[2];
   for (k=0, kk=BVOL::Size; k<BVOL::Size; ++k, ++kk) {
      getData().getCenterInternal()[k+3] = BVOL::getDirection()[k].dot(center);

      getData().getOuterMostInternal()[k][0]  /= m_bvol.maxVector()[k];
      getData().getOuterMostInternal()[k][1]  /= m_bvol.maxVector()[k];
      getData().getOuterMostInternal()[k][2]  /= m_bvol.maxVector()[k];
      getData().getOuterMostInternal()[k][3]  /= m_bvol.maxVector()[k];
      getData().getOuterMostInternal()[kk][0] /= -m_bvol.minVector()[k];
      getData().getOuterMostInternal()[kk][1] /= -m_bvol.minVector()[k];
      getData().getOuterMostInternal()[kk][2] /= -m_bvol.minVector()[k];
      getData().getOuterMostInternal()[kk][3] /= -m_bvol.minVector()[k];

      m_bvol.maxVector()[k] += getData().getCenterInternal()[k+3];
      m_bvol.minVector()[k] += getData().getCenterInternal()[k+3];
      assert(m_bvol.minVector()[k] <= m_bvol.maxVector()[k]);
   }

   m_bvol.setEmpty(false);
   m_bvol.update();
}

template <class BasicTraits, class BVOL>
void  OpenSGTriangleAligned<BasicTraits,BVOL>::init (const GeomTriangleType& obj)
{
   m_originalId = obj.getIndex();
   m_point[0] = obj.getPosition(0);
   m_point[1] = obj.getPosition(1);
   m_point[2] = obj.getPosition(2);

   init(m_point[0], m_point[1],	m_point[2]);
}
template <class BasicTraits, class BVOL>
void  OpenSGTriangleAligned<BasicTraits,BVOL>::init (const TransformType&    m2w,
						     const GeomTriangleType& obj)
{
   m_originalId = obj.getIndex();

   m2w.mult(obj.getPosition(0), m_point[0]);
   m2w.mult(obj.getPosition(1), m_point[1]);
   m2w.mult(obj.getPosition(2), m_point[2]);
   init(m_point[0], m_point[1], m_point[2]);
}

template <class BasicTraits, class BVOL>
void OpenSGTriangleAligned<BasicTraits,BVOL>::draw ()
{
   const PointClass* coord = getPosition(); 
   glBegin(GL_TRIANGLES);
   glVertex3fv(coord[0].getValues());
   glVertex3fv(coord[1].getValues());
   glVertex3fv(coord[2].getValues());
   glEnd();
}
template <class BasicTraits, class BVOL>
void OpenSGTriangleAligned<BasicTraits,BVOL>::drawWireframe ()
{
   GLfloat current[4];
   glGetFloatv(GL_CURRENT_COLOR, current);
   glColor3f(0.8f, 0.8f, 0.8f);
   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   draw();
   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   glColor3fv(current);
}
template <class BasicTraits, class BVOL>
bool OpenSGTriangleAligned<BasicTraits,BVOL>::calcIntersect (Intersection& hit)
{
   Real t;
   Vec3f normal;
   //GeomTriangleType it(getOriginal());
   if (hit.getRay().calcIntersect(getPosition(0), getPosition(1), getPosition(2), 
				  t, normal)) { // intersection case
      if (hit.updateDist(t)) {
	 hit.setTo(this);
	 // additional intersection data
	 IntersectionNormal* data = new IntersectionNormal();
	 data->setSurfaceNormal(normal);
	 hit.setData(data);
	 return true;
      }
   }
   return false;
}


template <class BVOL>
void BVolGroupAligned<BVOL>::init (const BVolGroupAligned<BVOL>& src)
{
   *this = src;
}

template <class BVOL>
void BVolGroupAligned<BVOL>::init (unsigned first,
				   unsigned last,
				   const std::vector< std::vector<unsigned> >& index,
				   const std::vector<Adapter*>&                nodes)
{
   updateBoundingVolume(first, last, index, nodes);
   setValid();
}


template <class BVOL>
const BoundingVolume<Real>& 
BVolGroupAligned<BVOL>::getBoundingVolume () const
{
   return m_bvol;
}

template <class BVOL>
void BVolGroupAligned<BVOL>::setPoint (unsigned k,
				       Real  len,
				       VectorClass local)
{
   static FloatingComparator<Real> comp(0.01f);

   Real s  = local.dot(BVOL::getDirection()[k]);
   local -= s*BVOL::getDirection()[k];
   Real sx = local.dot(BVOL::BVolGeometry::getFrameX()[k]);
   Real sy = local.dot(BVOL::BVolGeometry::getFrameY()[k]);
#if 1
   // 45-degrees rotated
   unsigned si = (sy < -sx ? 2 : 0) 
     + (sy < -sx && sy >= sx ? 1 : 0) 
     + (sy >= -sx && sy < sx ? 1 : 0);
#else
   // axes-aligned
   unsigned si = (sx < 0 ? 2 : 0) 
     + (sx < 0 && sy >= 0 ? 1 : 0) 
     + (sx >= 0 && sy < 0 ? 1 : 0);
#endif
   assert(si  < 4);
   unsigned sii = (si == 0 ? 3 : si-1);
   assert(sii < 4);
   unsigned i;
   if (s >= 0) {
	 if (comp.greater(len, m_bvol.maxVector()[k])) {
	    for (i=0; i<OccTableHighestBit; ++i) {
	       if (s >= BVOL::unitDopAngleTable()[i]) {
		  break;
	       }
	    }
#ifdef GV_WITH_SUBCONES
	    getData().getOccupancyInternal()[k][sii] |= (1<<i);
	    getData().getOccupancyInternal()[k][si]  |= (1<<i);
#else
	    getData().getOccupancyInternal()[k][0]  |= (1<<i);
#endif
	    if (i == OccTableHighestBit) {
	      s = (s*BVOL::unitDopAngle() + cos2sin(s)*cos2sin(BVOL::unitDopAngle()))*len;
#ifdef GV_WITH_SUBCONES
	      if (s > getData().getOuterMostInternal()[k][sii]) {
		 getData().getOuterMostInternal()[k][sii] = s;
	      }
	      if (s > getData().getOuterMostInternal()[k][si]) {
		 getData().getOuterMostInternal()[k][si]  = s;
	      }
#else
	      if (s > getData().getOuterMostInternal()[k][0]) {
		 getData().getOuterMostInternal()[k][0]  = s;
	      }
#endif
	    }
	 }
   } else {
      if (comp.greater(len, -m_bvol.minVector()[k])) {
	 unsigned   kk = k + BVOL::Size;
	 s = -s;
#if 1
	    if (si <  2) si  += 2;
	    else si  -= 2;
	    if (sii < 2) sii += 2;
	    else sii -= 2;
	    assert(si  < 4);
	    assert(sii < 4);
#endif
	    for (i=0; i<OccTableHighestBit; ++i) {
	       if (s >= BVOL::unitDopAngleTable()[i]) {
		  break;
	       }
	    }
#ifdef GV_WITH_SUBCONES
	    getData().getOccupancyInternal()[kk][sii] |= (1<<i);
	    getData().getOccupancyInternal()[kk][si]  |= (1<<i);
#else
	    getData().getOccupancyInternal()[kk][0]  |= (1<<i);
#endif
	    if (i == OccTableHighestBit) {
	       s = (s*BVOL::unitDopAngle() + cos2sin(s)*cos2sin(BVOL::unitDopAngle()))*len;
#ifdef GV_WITH_SUBCONES
	       if (s > getData().getOuterMostInternal()[kk][sii]) {
		 getData().getOuterMostInternal()[kk][sii] = s;
	       }
	       if (s > getData().getOuterMostInternal()[kk][si]) {
		 getData().getOuterMostInternal()[kk][si]  = s;
	       }
#else
	       if (s > getData().getOuterMostInternal()[kk][0]) {
		 getData().getOuterMostInternal()[kk][0]  = s;
	       }
#endif
	    }
	 }
   }
}
template <class BVOL>
void BVolGroupAligned<BVOL>::updateBoundingVolume (unsigned first,
						   unsigned last,
						   const std::vector< std::vector<unsigned> >& index,
						   const std::vector<Adapter*>&                nodes)
{  
   unsigned i, k, kk;

   std::vector<unsigned>::const_iterator iter; 
   std::vector<unsigned>::const_iterator end = index[0].begin()+last; 
   // calc center
   PointClass  center;
   for (iter = index[0].begin()+first; 
	iter != end;
	++iter) {
      OpenSGTriangleAligned<OpenSGTraits,BVOL>* leaf = 
	static_cast<OpenSGTriangleAligned<OpenSGTraits,BVOL>*>(nodes[*iter]);
      center += VectorClass(leaf->getPosition(0));
      center += VectorClass(leaf->getPosition(1));
      center += VectorClass(leaf->getPosition(2));
   }
   center /= 3*Real(last-first);

   VectorClass local;
   Real  len;
   // first round
   m_bvol.setEmpty();
   iter = index[0].begin()+first; 
   {
      OpenSGTriangleAligned<OpenSGTraits,BVOL>* leaf = 
	static_cast<OpenSGTriangleAligned<OpenSGTraits,BVOL>*>(nodes[*iter]);
      local.setValue(leaf->getPosition(0) - center);
      m_bvol.unify(local);
      local.setValue(leaf->getPosition(1) - center);
      m_bvol.unify(local);
      local.setValue(leaf->getPosition(2) - center);
      m_bvol.unify(local);
   }
   for (++iter; 
	iter != end; 
	++iter) {
      OpenSGTriangleAligned<OpenSGTraits,BVOL>* leaf = 
	static_cast<OpenSGTriangleAligned<OpenSGTraits,BVOL>*>(nodes[*iter]);
      local.setValue(leaf->getPosition(0) - center);
      m_bvol.unify(local);
      local.setValue(leaf->getPosition(1) - center);
      m_bvol.unify(local);
      local.setValue(leaf->getPosition(2) - center);
      m_bvol.unify(local);
   }
   // add safety distance
   static FloatingComparator<Real> comp;
   m_bvol.unify(comp.getPrecision());

   // second round
   iter = index[0].begin()+first; 
   {
      OpenSGTriangleAligned<OpenSGTraits,BVOL>* leaf = 
	static_cast<OpenSGTriangleAligned<OpenSGTraits,BVOL>*>(nodes[*iter]);
      local.setValue(leaf->getPosition(0) - center); len = local.length(); local /= len;
      for (k=0, kk=BVOL::Size; k<BVOL::Size; ++k, ++kk) {
	 getData().getOuterMostInternal()[k][0] = getData().getOuterMostInternal()[kk][0] = 0;
	 getData().getOuterMostInternal()[k][1] = getData().getOuterMostInternal()[kk][1] = 0;
	 getData().getOuterMostInternal()[k][2] = getData().getOuterMostInternal()[kk][2] = 0;
	 getData().getOuterMostInternal()[k][3] = getData().getOuterMostInternal()[kk][3] = 0;
	 getData().getOccupancyInternal()[k][0] = getData().getOccupancyInternal()[kk][0] = 0;
	 getData().getOccupancyInternal()[k][1] = getData().getOccupancyInternal()[kk][1] = 0;
	 getData().getOccupancyInternal()[k][2] = getData().getOccupancyInternal()[kk][2] = 0;
	 getData().getOccupancyInternal()[k][3] = getData().getOccupancyInternal()[kk][3] = 0;
	 setPoint(k, len, local);
      }
      local.setValue(leaf->getPosition(1) - center); len = local.length(); local /= len;
      for (k=0; k<BVOL::Size; ++k) {
	 setPoint(k, len, local);
      }
      local.setValue(leaf->getPosition(2) - center); len = local.length(); local /= len;
      for (k=0; k<BVOL::Size; ++k) {
	 setPoint(k, len, local);
      }
   }
   for (++iter; 
	iter != end; 
	++iter) {
      OpenSGTriangleAligned<OpenSGTraits,BVOL>* leaf = 
	static_cast<OpenSGTriangleAligned<OpenSGTraits,BVOL>*>(nodes[*iter]);
      local.setValue(leaf->getPosition(0) - center); len = local.length(); local /= len;
      for (k=0; k<BVOL::Size; ++k) {
	 setPoint(k, len, local);
      }
      local.setValue(leaf->getPosition(1) - center); len = local.length(); local /= len;
      for (k=0; k<BVOL::Size; ++k) {
	 setPoint(k, len, local);
      }
      local.setValue(leaf->getPosition(2) - center); len = local.length(); local /= len;
      for (k=0; k<BVOL::Size; ++k) {
	 setPoint(k, len, local);
      }
   }

   getData().getCenterInternal()[0] = center[0];
   getData().getCenterInternal()[1] = center[1];
   getData().getCenterInternal()[2] = center[2];
   for (k=0, kk=BVOL::Size; k<BVOL::Size; ++k, ++kk) {
      getData().getCenterInternal()[k+3] = BVOL::getDirection()[k].dot(center);

      getData().getOuterMostInternal()[k][0]  /= m_bvol.maxVector()[k];
      getData().getOuterMostInternal()[k][1]  /= m_bvol.maxVector()[k];
      getData().getOuterMostInternal()[k][2]  /= m_bvol.maxVector()[k];
      getData().getOuterMostInternal()[k][3]  /= m_bvol.maxVector()[k];
      getData().getOuterMostInternal()[kk][0] /= -m_bvol.minVector()[k];
      getData().getOuterMostInternal()[kk][1] /= -m_bvol.minVector()[k];
      getData().getOuterMostInternal()[kk][2] /= -m_bvol.minVector()[k];
      getData().getOuterMostInternal()[kk][3] /= -m_bvol.minVector()[k];

      m_bvol.maxVector()[k] += getData().getCenterInternal()[k+3];
      m_bvol.minVector()[k] += getData().getCenterInternal()[k+3];
      assert(m_bvol.minVector()[k] <= m_bvol.maxVector()[k]);
   }

   m_bvol.setEmpty(false);
   m_bvol.update();
}

template <class BVOL>
u8* BVolGroupAligned<BVOL>::texTable (const OccTableType& mask)
{
   static const Real*    tab = BVOL::unitDopAngleTable();
   static const unsigned res = 2*BVOL::OccTableBits;
   static u8 tex[2*res] = { (u8)-1, (u8)-1 };
   static const u8 zero(0);
   static const u8 one((u8)-1);
   static const u8 half(one/2);
   static const u8 quarter(one/4);

   unsigned i, j;
   if (tex[0] == one && tex[1] == one) {
      for (i=1; i<2*res; i+=2) {
	 tex[i] = quarter;
      }
   }
   unsigned angle = 0;
   u8       value = ((mask & (1<<angle)) ? one : half);
   for (i=0; i<2*res; i+=4) {
      tex[i]   = value;
      tex[i+2] = zero;
      ++angle;
      value = ((mask & (1<<angle)) ? one : half);
   }
   return tex;
}

#define GV_DIST1(p)       1.0
//((p)[0])*((p)[0])+((p)[1])*((p)[1])+((p)[2])*((p)[2])
#define GV_DIST2(x, y)    (x)*(x)+(y)*(y)
#define GV_DIST3(x, y, z) 1.0
//(x)*(x)+(y)*(y)+(z)*(z)
void BVolGroupAligned<AABox>::draw ()
{
   static GLfloat denom  = acos(BVol::unitDopAngle());
   static GLfloat factor = 1.25f*RAND_MAX;

   glEnable(GL_TEXTURE_1D);
   glEnable(GL_BLEND);
   glColor3f(GLfloat(rand())/factor + 0.2f, 
	     GLfloat(rand())/factor + 0.2f, 
	     GLfloat(rand())/factor + 0.2f);
   glTexImage1D(GL_TEXTURE_1D, 0, GL_LUMINANCE_ALPHA, 2*OccTableBits, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, 
		texTable(getData().getOccupancy(0)[0]));
   GLfloat m1, m2, tm1, tm2, m, radius;   
   glBegin(GL_TRIANGLE_FAN);
   m1 = 0.5f*(getBVol().maxVector()[0]-getBVol().minVector()[0]);
   m2 = 0.5f*(getBVol().maxVector()[1]-getBVol().minVector()[1]);
   radius = 0.5f*(getBVol().maxVector()[2]-getBVol().minVector()[2]);
   tm1 = atanf(m2/radius)/denom; tm2 = atanf(m1/radius)/denom;
   m  = sqrt(tm1*tm1+tm2*tm2); 
   glNormal3f(0,0,-1);
   glTexCoord1f(0.0f);
   glVertex3f(getBVol().minVector()[0]+m1, getBVol().minVector()[1]+m2, getBVol().minVector()[2]);
   glTexCoord1f(m);
   glVertex3f(getBVol().minVector()[0], getBVol().minVector()[1], getBVol().minVector()[2]);
   glTexCoord1f(tm2);
   glVertex3f(getBVol().minVector()[0], getBVol().minVector()[1]+m2, getBVol().minVector()[2]);
   glTexCoord1f(m);
   glVertex3f(getBVol().minVector()[0], getBVol().maxVector()[1], getBVol().minVector()[2]);
   glTexCoord1f(tm1);
   glVertex3f(getBVol().minVector()[0]+m1, getBVol().maxVector()[1], getBVol().minVector()[2]);
   glTexCoord1f(m);
   glVertex3f(getBVol().maxVector()[0], getBVol().maxVector()[1], getBVol().minVector()[2]);
   glTexCoord1f(tm2);
   glVertex3f(getBVol().maxVector()[0], getBVol().minVector()[1]+m2, getBVol().minVector()[2]);
   glTexCoord1f(m);
   glVertex3f(getBVol().maxVector()[0], getBVol().minVector()[1], getBVol().minVector()[2]);
   glTexCoord1f(tm1);
   glVertex3f(getBVol().minVector()[0]+m1, getBVol().minVector()[1], getBVol().minVector()[2]);
   glTexCoord1f(m);
   glVertex3f(getBVol().minVector()[0], getBVol().minVector()[1], getBVol().minVector()[2]);
   glEnd();

   glTexImage1D(GL_TEXTURE_1D, 0, GL_LUMINANCE_ALPHA, 2*OccTableBits, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, 
		texTable(getData().getOccupancy(1)[0]));
   glBegin(GL_TRIANGLE_FAN);
   glNormal3f(0,0,+1);
   glTexCoord1f(0.0f);
   glVertex3f(getBVol().minVector()[0]+m1, getBVol().minVector()[1]+m2, getBVol().maxVector()[2]);
   glTexCoord1f(m);
   glVertex3f(getBVol().maxVector()[0], getBVol().minVector()[1], getBVol().maxVector()[2]);
   glTexCoord1f(tm2);
   glVertex3f(getBVol().maxVector()[0], getBVol().minVector()[1]+m2, getBVol().maxVector()[2]);
   glTexCoord1f(m);
   glVertex3f(getBVol().maxVector()[0], getBVol().maxVector()[1], getBVol().maxVector()[2]);
   glTexCoord1f(tm1);
   glVertex3f(getBVol().minVector()[0]+m1, getBVol().maxVector()[1], getBVol().maxVector()[2]);
   glTexCoord1f(m);
   glVertex3f(getBVol().minVector()[0], getBVol().maxVector()[1], getBVol().maxVector()[2]);
   glTexCoord1f(tm2);
   glVertex3f(getBVol().minVector()[0], getBVol().minVector()[1]+m2, getBVol().maxVector()[2]);
   glTexCoord1f(m);
   glVertex3f(getBVol().minVector()[0], getBVol().minVector()[1], getBVol().maxVector()[2]);
   glTexCoord1f(tm1);
   glVertex3f(getBVol().minVector()[0]+m1, getBVol().minVector()[1], getBVol().maxVector()[2]);
   glTexCoord1f(m);
   glVertex3f(getBVol().maxVector()[0], getBVol().minVector()[1], getBVol().maxVector()[2]);
   glEnd();

   glTexImage1D(GL_TEXTURE_1D, 0, GL_LUMINANCE_ALPHA, 2*OccTableBits, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, 
		texTable(getData().getOccupancy(2)[0]));
   glBegin(GL_TRIANGLE_FAN);
   m1 = 0.5f*(getBVol().maxVector()[1]-getBVol().minVector()[1]);
   m2 = 0.5f*(getBVol().maxVector()[2]-getBVol().minVector()[2]);
   radius = 0.5f*(getBVol().maxVector()[0]-getBVol().minVector()[0]);
   tm1 = atanf(m2/radius)/denom; tm2 = atanf(m1/radius)/denom;
   m  = sqrt(tm1*tm1+tm2*tm2); 
   glNormal3f(0,0,-1);
   glTexCoord1f(0.0f);
   glVertex3f(getBVol().minVector()[0], getBVol().minVector()[1]+m1, getBVol().minVector()[2]+m2);
   glTexCoord1f(m);
   glVertex3f(getBVol().minVector()[0], getBVol().minVector()[1], getBVol().minVector()[2]);
   glTexCoord1f(tm2);
   glVertex3f(getBVol().minVector()[0], getBVol().minVector()[1], getBVol().minVector()[2]+m2);
   glTexCoord1f(m);
   glVertex3f(getBVol().minVector()[0], getBVol().minVector()[1], getBVol().maxVector()[2]);
   glTexCoord1f(tm1);
   glVertex3f(getBVol().minVector()[0], getBVol().minVector()[1]+m1, getBVol().maxVector()[2]);
   glTexCoord1f(m);
   glVertex3f(getBVol().minVector()[0], getBVol().maxVector()[1], getBVol().maxVector()[2]);  
   glTexCoord1f(tm2);
   glVertex3f(getBVol().minVector()[0], getBVol().maxVector()[1], getBVol().minVector()[2]+m2);  
   glTexCoord1f(m);
   glVertex3f(getBVol().minVector()[0], getBVol().maxVector()[1], getBVol().minVector()[2]);
   glTexCoord1f(tm1);
   glVertex3f(getBVol().minVector()[0], getBVol().minVector()[1]+m1, getBVol().minVector()[2]);
   glTexCoord1f(m);
   glVertex3f(getBVol().minVector()[0], getBVol().minVector()[1], getBVol().minVector()[2]);
   glEnd();

   glTexImage1D(GL_TEXTURE_1D, 0, GL_LUMINANCE_ALPHA, 2*OccTableBits, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, 
		texTable(getData().getOccupancy(3)[0]));
   glBegin(GL_TRIANGLE_FAN);
   glNormal3f(0,0,+1);
   glTexCoord1f(0.0f);
   glVertex3f(getBVol().maxVector()[0], getBVol().minVector()[1]+m1, getBVol().minVector()[2]+m2); 
   glTexCoord1f(m);
   glVertex3f(getBVol().maxVector()[0], getBVol().maxVector()[1], getBVol().minVector()[2]);
   glTexCoord1f(tm2);
   glVertex3f(getBVol().maxVector()[0], getBVol().maxVector()[1], getBVol().minVector()[2]+m2);
   glTexCoord1f(m);
   glVertex3f(getBVol().maxVector()[0], getBVol().maxVector()[1], getBVol().maxVector()[2]);
   glTexCoord1f(tm1);
   glVertex3f(getBVol().maxVector()[0], getBVol().minVector()[1]+m1, getBVol().maxVector()[2]);
   glTexCoord1f(m);
   glVertex3f(getBVol().maxVector()[0], getBVol().minVector()[1], getBVol().maxVector()[2]);
   glTexCoord1f(tm2);
   glVertex3f(getBVol().maxVector()[0], getBVol().minVector()[1], getBVol().minVector()[2]+m2);
   glTexCoord1f(m);
   glVertex3f(getBVol().maxVector()[0], getBVol().minVector()[1], getBVol().minVector()[2]);
   glTexCoord1f(tm1);
   glVertex3f(getBVol().maxVector()[0], getBVol().minVector()[1]+m1, getBVol().minVector()[2]);
   glTexCoord1f(m);
   glVertex3f(getBVol().maxVector()[0], getBVol().maxVector()[1], getBVol().minVector()[2]);
   glEnd();

   glTexImage1D(GL_TEXTURE_1D, 0, GL_LUMINANCE_ALPHA, 2*OccTableBits, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, 
		texTable(getData().getOccupancy(4)[0]));
   glBegin(GL_TRIANGLE_FAN);
   m1 = 0.5f*(getBVol().maxVector()[0]-getBVol().minVector()[0]);
   m2 = 0.5f*(getBVol().maxVector()[2]-getBVol().minVector()[2]);
   //radius = 0.5f*(getBVol().maxVector()[1]-getBVol().minVector()[1])/BVol::unitDopRadius();
   //tm1 = radius*m2; tm2 = radius*m1;
   radius = 0.5f*(getBVol().maxVector()[1]-getBVol().minVector()[1]);
   tm1 = atanf(m2/radius)/denom; tm2 = atanf(m1/radius)/denom;
   m  = sqrt(tm1*tm1+tm2*tm2); 
   glNormal3f(0,0,-1);
   glTexCoord1f(0.0f);
   glVertex3f(getBVol().minVector()[0]+m1, getBVol().minVector()[1], getBVol().minVector()[2]+m2);
   glTexCoord1f(m);
   glVertex3f(getBVol().minVector()[0], getBVol().minVector()[1], getBVol().minVector()[2]);
   glTexCoord1f(tm1);
   glVertex3f(getBVol().minVector()[0]+m1, getBVol().minVector()[1], getBVol().minVector()[2]);
   glTexCoord1f(m);
   glVertex3f(getBVol().maxVector()[0], getBVol().minVector()[1], getBVol().minVector()[2]);
   glTexCoord1f(tm2);
   glVertex3f(getBVol().maxVector()[0], getBVol().minVector()[1], getBVol().minVector()[2]+m2);
   glTexCoord1f(m);
   glVertex3f(getBVol().maxVector()[0], getBVol().minVector()[1], getBVol().maxVector()[2]);
   glTexCoord1f(tm1);
   glVertex3f(getBVol().minVector()[0]+m1, getBVol().minVector()[1], getBVol().maxVector()[2]);
   glTexCoord1f(m);
   glVertex3f(getBVol().minVector()[0], getBVol().minVector()[1], getBVol().maxVector()[2]);
   glTexCoord1f(tm2);
   glVertex3f(getBVol().minVector()[0], getBVol().minVector()[1], getBVol().minVector()[2]+m2);
   glTexCoord1f(m);
   glVertex3f(getBVol().minVector()[0], getBVol().minVector()[1], getBVol().minVector()[2]);
   glEnd();

   glTexImage1D(GL_TEXTURE_1D, 0, GL_LUMINANCE_ALPHA, 2*OccTableBits, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, 
		texTable(getData().getOccupancy(5)[0]));
   glBegin(GL_TRIANGLE_FAN);
   glNormal3f(0,0,+1);
   glTexCoord1f(0.0f);
   glVertex3f(getBVol().minVector()[0]+m1, getBVol().maxVector()[1], getBVol().minVector()[2]+m2);
   glTexCoord1f(m);
   glVertex3f(getBVol().minVector()[0], getBVol().maxVector()[1], getBVol().maxVector()[2]);
   glTexCoord1f(tm1);
   glVertex3f(getBVol().minVector()[0]+m1, getBVol().maxVector()[1], getBVol().maxVector()[2]);
   glTexCoord1f(m);
   glVertex3f(getBVol().maxVector()[0], getBVol().maxVector()[1], getBVol().maxVector()[2]);
   glTexCoord1f(tm2);
   glVertex3f(getBVol().maxVector()[0], getBVol().maxVector()[1], getBVol().minVector()[2]+m2);
   glTexCoord1f(m);
   glVertex3f(getBVol().maxVector()[0], getBVol().maxVector()[1], getBVol().minVector()[2]);
   glTexCoord1f(tm1);
   glVertex3f(getBVol().minVector()[2]+m1, getBVol().maxVector()[1], getBVol().minVector()[2]);
   glTexCoord1f(m);
   glVertex3f(getBVol().minVector()[0], getBVol().maxVector()[1], getBVol().minVector()[2]);
   glTexCoord1f(tm2);
   glVertex3f(getBVol().minVector()[0], getBVol().maxVector()[1], getBVol().minVector()[2]+m2);
   glTexCoord1f(m);
   glVertex3f(getBVol().minVector()[0], getBVol().maxVector()[1], getBVol().maxVector()[2]);
   glEnd();
   glDisable(GL_TEXTURE_1D);
   glDisable(GL_BLEND);
}
void BVolGroupAligned<AABox>::drawWireframe ()
{
   Inherited::drawWireframe();
}
template <class BVOL>
void BVolGroupAligned<BVOL>::draw ()
{
   static GLfloat sqrt2 = sqrt(2.0f);
   static GLfloat factor = 1.25f*RAND_MAX;

   glEnable(GL_TEXTURE_1D);
   glEnable(GL_BLEND);
   glColor3f(GLfloat(rand())/factor + 0.2f, 
	     GLfloat(rand())/factor + 0.2f, 
	     GLfloat(rand())/factor + 0.2f);
   unsigned f;
   Polygon3SetIndexed::Container::iterator itF;
   getBVol().updateGeometry();
   Polygon3SetIndexed& geom = getBVol().getGeometry().getPolygonSet();
   for (itF=geom.faces.begin(), f=0; 
	itF != geom.faces.end(); 
	++itF, ++f) { 
      glTexImage1D(GL_TEXTURE_1D, 0, GL_LUMINANCE_ALPHA, OccTableBits, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, 
		   texTable(getData().getOccupancy(f)[0]));
      glBegin(GL_POLYGON);
      glNormal3fv(getBVol().getNormal(f).getValues());
      for (Polygon3Indexed::Container::iterator itE=itF->edges.begin(); 
	   itE != itF->edges.end();
	   ++itE) {
	 glTexCoord1f(GV_DIST1(itE->getPoint()));
	 glVertex3fv(itE->getPoint().getValues());
      }
      glEnd();
   }
   glDisable(GL_TEXTURE_1D);
   glDisable(GL_BLEND);
}
template <class BVOL>
void BVolGroupAligned<BVOL>::drawWireframe ()
{
   Inherited::drawWireframe();
}

// explicit template instantiations
#include "OSGGVOptimizer.cpp"
template class OSG_GENVISLIB_DLLMAPPING Optimizer<BVolGroupAligned<K6Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Optimizer<BVolGroupAligned<K14Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Optimizer<BVolGroupAligned<K18Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Optimizer<BVolGroupAligned<K26Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Optimizer<BVolGroupAligned<K12Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Optimizer<BVolGroupAligned<K20Dop> >;



