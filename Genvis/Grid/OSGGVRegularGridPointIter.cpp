//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.1 $
//   $Date: 2003/09/19 21:46:39 $
//                                                                            
//=============================================================================

#include "OSGGVRegularGridPointIter.h"
USING_GENVIS_NAMESPACE

template <class TYPE, class TYPE2>
inline TYPE2 stdClamp (const TYPE& l, const TYPE2& value, const TYPE& h)
{
   if (value < l) {
      return TYPE2(l);
   }
   if (h < value) {
      return TYPE2(h);
   }
   return value;
}

RegularGridPointIter::RegularGridPointIter (RegularGridBase& grid, PointClass point)
  : m_grid(&grid)
{
   m_first = true;
   if (getGrid().getNumVoxels() == 0) {
      m_diff[2] = m_diff[1] = m_diff[0] = m_dmax = m_d = 0;
      return;
   }

   // init
   m_d = m_diff[2] = m_diff[1] = m_diff[0] = 0;
   m_dmax = stdMax(stdMax(getGrid().getNumVoxelsDim()[0], getGrid().getNumVoxelsDim()[1]), getGrid().getNumVoxelsDim()[2]);
   // first point in voxelsystem
   if (!getGrid().testIntersect(point)) { // point not inside voxelsystem
      // CF to be checked
      getGrid().toVoxel(point);
      m_coord.setValues(stdClamp(0, point[0], getGrid().getNumVoxelsDim()[0]-1), 
			stdClamp(0, point[1], getGrid().getNumVoxelsDim()[1]-1), 
			stdClamp(0, point[2], getGrid().getNumVoxelsDim()[2]-1));
      m_index = m_coord.dot(getGrid().getStrides());
   } else { // point inside voxelsystem
      getGrid().toVoxel(point);
      m_coord.setValues(point[0], point[1], point[2]);
      m_index = m_coord.dot(getGrid().getStrides());
   }
}

i32 RegularGridPointIter::operator() ()
{
   if (m_first) {
      m_first = false;
      return m_dmax-m_d;
   }

   VectorClass3i coord;
   do {
     if (m_diff[2] < 0) { // flip m_diff[2]
       m_diff[2] = -m_diff[2];
     } else { // next m_diff[1]
       i32 dydz = m_d - stdAbs(m_diff[0]);
       ++m_diff[1];
       if (m_diff[1] <= dydz) {// init m_diff[2]
	 m_diff[2] = -(dydz - stdAbs(m_diff[1])); 
       } else { // next m_diff[0]
 	 ++m_diff[0];
	 if (m_diff[0] <= m_d) { // init m_diff[1], m_diff[2]
	   m_diff[1] = -(m_d - stdAbs(m_diff[0])); 
	   m_diff[2] = 0; 
	 } else { // next m_d
	   ++m_d;
	   if (m_d < m_dmax) { // init m_diff[0], m_diff[1], m_diff[2]
	     m_diff[0] = -m_d;
	     m_diff[1] = m_diff[2] = 0;
	   } else { // grid processed completely
	     break;
	   }
	 }
       }
     }
     coord.setValue(m_coord); coord += m_diff;
   } while (   coord[0] < 0 || getGrid().getNumVoxelsDim()[0] <= coord[0]
	    || coord[1] < 0 || getGrid().getNumVoxelsDim()[1] <= coord[1]
	    || coord[2] < 0 || getGrid().getNumVoxelsDim()[2] <= coord[2]);

   m_index = coord.dot(getGrid().getStrides());
   return m_dmax-m_d;
}