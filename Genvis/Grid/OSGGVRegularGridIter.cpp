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

#include "OSGGVRegularGridIter.h"
USING_GENVIS_NAMESPACE

RegularGridIter::RegularGridIter (RegularGridBase& vs, const Ray& ray)
{
   m_finished = true;   // assume voxel space not hit by ray

   if (vs.getNumVoxels() == 0) {
      return;
   }

   m_voxHitDist = -1;
   // first point in voxelsystem
   PointClass point(ray.getOrigin());    
   if (!vs.testIntersect(point)) { // ray origin not inside voxelsystem
      // intersect ray with bounding box
      if (!vs.testIntersect(ray.getOrigin(), ray.getDirection(), m_voxHitDist)) {
	 m_voxHitDist = -1;
         return;
      }
      // translate point to intersection point
      point += m_voxHitDist * ray.getDirection();
   } else { // ray origin inside voxelsystem
      m_voxHitDist = 0.0;
   }

   static FloatingComparator<Real> comp;
   static const Real maxFloat = 1.0f/comp.getPrecision();

   for (unsigned c=0; c<3; ++c) {
      m_v[c] = stdMin(vs.getNumVoxelsDim()[c]-1,
	       stdMax(0, int((point[c]-vs.minVector()[c]) * vs.getInvLength()[c])));
      if (!comp.zero(ray.getDirection()[c])) {
         Real id = 1.0/ray.getDirection()[c];
         Real vc = vs.getRefCenter()[c] + m_v[c] * vs.getLength()[c];
         m_ta[c] = (vc+vs.getHalfLength()[c]*sign_pm(ray.getDirection()[c])-ray.getOrigin()[c]) * id;
         m_dt[c] = vs.getLength()[c] * stdAbs(id);
      } else {
         m_ta[c] = m_dt[c] = maxFloat;
      }
      m_vStep[c] = sign_pm(ray.getDirection()[c]);
      m_endPos[c] = (m_vStep[c] > 0) ? vs.getNumVoxelsDim()[c] : -1;
   }

   m_arrayPos     = m_v.dot(vs.getStrides());
   m_arrayStep[0] = m_vStep[0];
   m_arrayStep[1] = m_vStep[1] * vs.getStrides()[1];
   m_arrayStep[2] = m_vStep[2] * vs.getStrides()[2];

   m_first = true;
   m_finished = false;
}
RegularGridIter::~RegularGridIter ()
{
}

bool RegularGridIter::operator() ()
{
   if (m_finished) {
      return false;
   }

   if (m_first) {
      m_first = false;
      return true;
   }

   int t = 2;
   if (m_ta[0] < m_ta[1]) {
      if (m_ta[0] < m_ta[2]) t = 0;
   } else {
      if (m_ta[1] < m_ta[2]) t = 1;
   }

   m_v[t] += m_vStep[t];
   if (m_v[t] == m_endPos[t]) {
      m_finished = true;
      return false;
   }
   m_voxHitDist = m_ta[t];
   m_ta[t] += m_dt[t];
   m_arrayPos += m_arrayStep[t];

   return true;
}
