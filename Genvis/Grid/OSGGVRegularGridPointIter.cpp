//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.3 $
//   $Date: 2004/12/20 15:55:46 $
//                                                                            
//=============================================================================

#include "OSGGVRegularGridPointIter.h"
USING_GENVIS_NAMESPACE

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
   if (!getGrid().checkIntersect(point)) { // point not inside voxelsystem
      getGrid().toVoxel(point);
      m_start.setValues(u32(stdClamp(0u, point[0], getGrid().getNumVoxelsDim()[0]-1)), 
			u32(stdClamp(0u, point[1], getGrid().getNumVoxelsDim()[1]-1)), 
			u32(stdClamp(0u, point[2], getGrid().getNumVoxelsDim()[2]-1)));
      m_coord.setValue(m_start);
      m_index = m_coord.dot(getGrid().getStrides());
   } else { // point inside voxelsystem
      getGrid().toVoxel(point);
      m_start.setValues(u32(point[0]), 
			u32(point[1]), 
			u32(point[2]));
      m_coord.setValue(m_start);
      m_index = m_coord.dot(getGrid().getStrides());
   }
}

i32 RegularGridPointIter::operator() ()
{
   if (m_first) {
      m_first = false;
      return m_dmax-m_d;
   }

   // For d=m_d there are
   // 2*sum_0^(d-1) 2^(i+1) + 2^(d+1) - 2 = d^(d+2) + 2^(d+1) - 6 cells to traverse
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
     coord.setValue(m_start); coord += m_diff;
     //SLOG << m_d << ": " << m_diff[0] << ", " << m_diff[1] << ", " << m_diff[2] << "=" << coord << std::endl;
   } while (   coord[0] < 0 || getGrid().getNumVoxelsDim()[0] <= u32(coord[0])
	    || coord[1] < 0 || getGrid().getNumVoxelsDim()[1] <= u32(coord[1])
	    || coord[2] < 0 || getGrid().getNumVoxelsDim()[2] <= u32(coord[2]));

   m_coord.setValue(coord);
   m_index = m_coord.dot(getGrid().getStrides());
   return m_dmax-m_d;
}
